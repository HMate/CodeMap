#include "includecollector.h"

#include "llvm/Support/Format.h"
#include "llvm/Support/FormatVariadic.h"
#include "llvm/Support/Path.h"

#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"

//#include "clang/Basic/SourceLocation.h"
//#include "clang/Basic/SourceManager.h"

#include "utils.h"

namespace cm
{

/**************** IncludeNodeRef *****************/

IncludeNodeRef& IncludeNodeRef::operator=(const IncludeNodeRef& o)
{
    if(this == &o)
        return *this;
    this->index = o.index;
}

void IncludeNodeRef::setIndex(size_t index)
{
    this->index = index;
}

const std::string IncludeNodeRef::name() const
{
    return tree.node(*this).name;
}

const std::string IncludeNodeRef::path() const
{
    return tree.node(*this).path;
}

std::vector<IncludeNodeRef>& IncludeNodeRef::includes()
{
    return tree.node(*this).includes;
}

/**************** IncludeTree *****************/

IncludeNodeRef IncludeTree::root()
{
    return IncludeNodeRef(*this, 0);
}

IncludeNode& IncludeTree::node(const IncludeNodeRef& ref)
{
    return nodes[ref.index];
}

/**************** IncludeTreeBuilder *****************/

IncludeTreeBuilder::IncludeTreeBuilder(IncludeTree& tree) : m_tree(tree), m_currentNode(tree, 0) {}

IncludeNodeRef IncludeTreeBuilder::getRoot() const
{
    return m_tree.root();
}

IncludeNodeRef IncludeTreeBuilder::currentNode()
{
    return m_currentNode;
}

void IncludeTreeBuilder::setRoot(std::string name, std::string path)
{
    assert(m_tree.nodes.empty());
    if(m_tree.nodes.empty())
        m_tree.nodes.emplace_back(name, path);
}

void IncludeTreeBuilder::addNode(std::string name, std::string path)
{
    bool found = false;
    size_t index;
    for(auto i = 0; i < m_tree.nodes.size(); i++)
    {
        if(m_tree.nodes[i].path == path)
        {
            found = true;
            index = i;
        }
    }

    if(!found)
    {
        index = m_tree.nodes.size();
        m_tree.nodes.emplace_back(name, path);
    }
    m_currentNode.includes().emplace_back(m_tree, index);
}

bool IncludeTreeBuilder::selectNode(std::string path)
{
    for(auto& node : m_currentNode.includes())
    {
        if(node.path() == path)
        {
            m_selectionStack.push(m_currentNode);
            m_currentNode.setIndex(node.index);
            return true;
        }
    }
    return false;
}

void IncludeTreeBuilder::selectParent()
{
    if(!isRootSelected())
    {
        m_currentNode = m_selectionStack.top();
        m_selectionStack.pop();
    }
}

bool IncludeTreeBuilder::isRootSelected()
{
    return m_currentNode.index == 0;
}

/**************** IncludeCollectorCallback *****************/

class IncludeCollectorCallback : public clang::PPCallbacks
{
    IncludeTreeBuilder m_builder;
    clang::SourceManager& m_sm;
    bool m_rootInited = false;

public:
    IncludeCollectorCallback(clang::SourceManager& sm, IncludeTree& tree) : m_sm(sm), m_builder(tree) {}

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
        m_builder.addNode(FileName.str(), File->getName().str());
        
        //clang::PresumedLoc PLoc = m_sm.getPresumedLoc(HashLoc);
        //llvm::outs() << llvm::formatv("InclusionDirective {0} {1} {2} {3}\n", FileName, File->getName(), PLoc.getLine(), PLoc.getColumn());
        //llvm::outs().flush();
    }

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
                m_builder.selectNode(PLoc.getFilename());
            }
        }
        if(Reason == FileChangeReason::ExitFile)
        {
            m_builder.selectParent();
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

std::unique_ptr<IncludeTree> CodeParser::getIncludeTree(const QString& srcPath, const std::vector<QString>& includeDirs)
{
    clang::tooling::FixedCompilationDatabase cdb = createCompilationDatabase(srcPath, includeDirs);
    std::vector<std::string> src{ srcPath.toStdString() };
    clang::tooling::ClangTool tool(cdb, src);

    std::unique_ptr<IncludeTree> tree = std::make_unique<IncludeTree>();
    std::unique_ptr<clang::tooling::FrontendActionFactory> actionFactory = std::make_unique<IncludeCollectorFrontendActionFactory>(*tree);
    tool.run(actionFactory.get());
    return tree;
}

} // namespace cm