#include "codeparser.h"

#include "llvm/Support/Format.h"
#include "llvm/Support/FormatVariadic.h"

#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"

//#include "clang/Basic/SourceLocation.h"
//#include "clang/Basic/SourceManager.h"

#include "utils.h"

namespace cm
{

class IncludeCollectorCallback : public clang::PPCallbacks
{
    IncludeTree& m_tree;
    IncludeNode m_currentNode;
    clang::SourceManager& m_sm;

    bool m_isRootInited = false;

public:
    IncludeCollectorCallback(clang::SourceManager& sm, IncludeTree& tree) : m_sm(sm), m_tree(tree)
    {
    }

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
        IncludeNode node = IncludeNode();
        node.name = FileName.str();
        node.id = File->getName().str();

        m_currentNode.includes.push_back(node);

        clang::PresumedLoc PLoc = m_sm.getPresumedLoc(HashLoc);
        llvm::outs() << llvm::formatv("InclusionDirective {0} {1} {2} {3}\n", FileName, File->getName(), PLoc.getLine(), PLoc.getColumn());
    }

    void FileChanged(clang::SourceLocation Loc, FileChangeReason Reason,
        clang::SrcMgr::CharacteristicKind FileType,
        clang::FileID PrevFID = clang::FileID()) override
    {
        clang::PresumedLoc PLoc = m_sm.getPresumedLoc(Loc);
        if(Reason == FileChangeReason::EnterFile)
        {
            if(!m_isRootInited)
            {
                m_tree.root.id = PLoc.getFilename();
                m_isRootInited = true;
            }
            else
            {
                // search for node in includes, and make that the current node
                for(auto& n : m_currentNode.includes)
                {
                    if(n.id == PLoc.getFilename())
                    {
                        m_currentNode = n;
                        break;
                    }
                }
            }
        }
        if(Reason == FileChangeReason::ExitFile)
        {
            // change current to parent TODO
        }
        llvm::outs() << llvm::formatv("FileChanged {0} {1} {2} {3}\n", PLoc.getFilename(), PLoc.getLine(), PLoc.getColumn(), (int)Reason);
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

IncludeTree CodeParser::getIncludeTree(const QString& srcPath, const std::vector<QString>& includeDirs)
{
    clang::tooling::FixedCompilationDatabase cdb = createCompilationDatabase(srcPath, includeDirs);
    std::vector<std::string> src{ srcPath.toStdString() };
    clang::tooling::ClangTool tool(cdb, src);

    IncludeTree tree;
    std::unique_ptr<clang::tooling::FrontendActionFactory> actionFactory = std::make_unique<IncludeCollectorFrontendActionFactory>(tree);
    tool.run(actionFactory.get());
    return tree;
}

} // namespace cm