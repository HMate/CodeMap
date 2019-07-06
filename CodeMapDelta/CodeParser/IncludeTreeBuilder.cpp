#include "IncludeTreeBuilder.h"

#include "llvm/Support/Format.h"
#include "llvm/Support/FormatVariadic.h"
#include "llvm/Support/Path.h"

#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"

#include "Utils.h"
#include "ParserError.h"
#include "CodeParser.h"

namespace cm
{
/**************** IncludeTreeBuilder *****************/

IncludeTreeBuilder::IncludeTreeBuilder(IncludeTree& tree) : 
    m_tree(tree), m_currentNode(nullptr) {}

IncludeNode& IncludeTreeBuilder::getRoot() const
{
    return m_tree.root();
}

IncludeNode& IncludeTreeBuilder::currentNode()
{
    return *m_currentNode;
}

void IncludeTreeBuilder::setRoot(std::string name, std::string path)
{
    m_tree.m_root.setNameAndPath(name, path);
    m_tree.m_root.includes().clear();
    m_tree.m_root.setFullInclude(true);
    m_currentNode = &(m_tree.m_root);
}

IncludeNode& IncludeTreeBuilder::addNode(std::string name, std::string path)
{
    assert(m_currentNode != nullptr);
    if(m_currentNode != nullptr)
    {
        return m_currentNode->addInclude(m_nextId++, name, path);
    }
    return *m_currentNode;
}

bool IncludeTreeBuilder::selectNode(std::string path)
{
    for(auto& node : m_currentNode->includes())
    {
        if(node.path() == path)
        {
            m_selectionStack.push(m_currentNode);
            m_currentNode = &node;
            return true;
        }
    }
    return false;
}

void IncludeTreeBuilder::selectPreviousNode()
{
    if(!isRootSelected())
    {
        m_currentNode = m_selectionStack.top();
        m_selectionStack.pop();
    }
}

bool IncludeTreeBuilder::isRootSelected()
{
    return m_currentNode == &m_tree.root();
}

/**************** IncludeCollectorCallback *****************/

class IncludeCollectorCallback : public clang::PPCallbacks
{
    IncludeTreeBuilder m_builder;
    clang::SourceManager& m_sm;
    bool m_rootInited = false;

public:
    IncludeCollectorCallback(clang::SourceManager& sm, IncludeTree& tree) : m_sm(sm), m_builder(tree) {}

    /// Called when the parser found an include directive
    virtual void InclusionDirective(clang::SourceLocation HashLoc,
        const clang::Token &IncludeTok,
        StringRef FileName,
        bool IsAngled,
        clang::CharSourceRange FilenameRange,
        const clang::FileEntry *File,
        StringRef SearchPath,
        StringRef RelativePath,
        const clang::Module *Imported) override
    {
        if(File != nullptr)
        {
            m_builder.addNode(FileName.str(), File->getName().str());
        }
        else
        {
            m_builder.addNode(FileName.str(), FileName.str());
        }
        
        //clang::PresumedLoc PLoc = m_sm.getPresumedLoc(HashLoc);
        //llvm::outs() << llvm::formatv("InclusionDirective {0} {1} {2} {3}\n", FileName, File->getName(), PLoc.getLine(), PLoc.getColumn());
        //llvm::outs().flush();
    }

    /// Called when the parser actually steps into an included file.
    /// For example this wont be called if the included file has a header guard, and was already parsed once
    void FileChanged(clang::SourceLocation Loc, FileChangeReason Reason,
        clang::SrcMgr::CharacteristicKind FileType,
        clang::FileID PrevFID = clang::FileID()) override
    {
        clang::PresumedLoc PLoc = m_sm.getPresumedLoc(Loc);
        if(Reason == FileChangeReason::EnterFile)
        {
            if(!m_rootInited)
            {
                // We assume here that the first EnterFile will be to the main file.
                m_builder.setRoot(llvm::sys::path::filename(PLoc.getFilename()), PLoc.getFilename());
                m_rootInited = true;
            }
            else
            {
                if(m_builder.selectNode(PLoc.getFilename()))
                {
                    m_builder.currentNode().setFullInclude(true);
                }
            }
        }
        if(Reason == FileChangeReason::ExitFile)
        {
            m_builder.selectPreviousNode();
        }
        //llvm::outs() << llvm::formatv("FileChanged {0} {1} {2} {3}\n", PLoc.getFilename(), PLoc.getLine(), PLoc.getColumn(), (int)Reason);
        //llvm::outs().flush();
    }
};

class IncludeCollectorFrontendAction : public clang::PreprocessOnlyAction
{
    IncludeTree& m_tree;
public:

    IncludeCollectorFrontendAction(IncludeTree& tree) : m_tree(tree) {}

    bool BeginSourceFileAction(clang::CompilerInstance &CI) override
    {
        CI.getPreprocessor().addPPCallbacks(std::make_unique<IncludeCollectorCallback>(CI.getSourceManager(), m_tree));
        return true;
    }

    void EndSourceFileAction() override 
    {

    }
};

class IncludeCollectorFrontendActionFactory : public clang::tooling::FrontendActionFactory
{
    IncludeTree& m_tree;
public:
    IncludeCollectorFrontendActionFactory(IncludeTree& tree) : m_tree(tree) {}

    clang::FrontendAction *create() override
    {
        return new IncludeCollectorFrontendAction(m_tree);
    }
};

std::unique_ptr<IncludeTree> getIncludeTree(const QString& srcPath, const std::vector<QString>& includeDirs)
{
    clang::tooling::FixedCompilationDatabase cdb = createCompilationDatabase(srcPath, includeDirs);
    std::vector<std::string> src{ srcPath.toStdString() };
    clang::tooling::ClangTool tool(cdb, src);

    ParserErrorCollector errorCollector;
    tool.setDiagnosticConsumer(&errorCollector);

    std::unique_ptr<IncludeTree> tree = std::make_unique<IncludeTree>();
    std::unique_ptr<clang::tooling::FrontendActionFactory> actionFactory = std::make_unique<IncludeCollectorFrontendActionFactory>(*tree);
    tool.run(actionFactory.get());
    // TODO: Include errors in result

    if (errorCollector.getNumErrors() > 0)
    {
        llvm::outs() << llvm::formatv("Got errors while collecting inlcudes for {0}:\n", srcPath.toStdString());
        for (auto& error : errorCollector.GetErrorsList())
        {
            llvm::outs() << llvm::formatv(" - {0}\n", error);
        }
        llvm::outs().flush();
    }

    return tree;
}

} // namespace cm