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
/* TODO: Remove IncludeNodeRef and build the tree with IncludeNodes 
    which represents the actually entered nodes.
    Build a special reference net in the view to highlight equal nodes.
*/

IncludeNodeRef::IncludeNodeRef(IncludeTree& tree, size_t index, bool fullInclude) 
    : tree(tree), index(index), fullInclude(fullInclude) 
{

}

IncludeNodeRef& IncludeNodeRef::operator=(const IncludeNodeRef& o)
{
    if(this == &o)
        return *this;
    this->index = o.index;
    this->fullInclude = o.fullInclude;
}

const std::string IncludeNodeRef::name() const
{
    return tree.node(*this).name;
}

const std::string IncludeNodeRef::path() const
{
    return tree.node(*this).path;
}

const std::vector<IncludeNodeRef>& IncludeNodeRef::includes() const
{
    return tree.node(*this).includes;
}

void IncludeNodeRef::setFullInclude(bool fullInclude)
{
    this->fullInclude = fullInclude;
}

bool IncludeNodeRef::isFullInclude() const
{
    return fullInclude;
}

bool IncludeNodeRef::isRecursive() const
{
    return false;
}

void IncludeNodeRef::addInclude(IncludeTree& tree, size_t index, bool fullInclude)
{
    tree.node(*this).includes.emplace_back(tree, index, fullInclude);
}

/**************** IncludeTree *****************/

IncludeNodeRef IncludeTree::root()
{
    return IncludeNodeRef(*this, 0, true);
}

IncludeNode& IncludeTree::node(const IncludeNodeRef& ref)
{
    return nodes.at(ref.index);
}

const IncludeNode& IncludeTree::node(const IncludeNodeRef& ref) const
{
    return nodes.at(ref.index);
}

/**************** IncludeTreeBuilder *****************/

IncludeTreeBuilder::IncludeTreeBuilder(IncludeTree& tree) : m_tree(tree), m_currentNode(tree, 0, true) {}

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
    // TODO: handle unguarded recursive includes
    m_currentNode.addInclude(m_tree, index, false);
}

bool IncludeTreeBuilder::selectNode(std::string path)
{
    for(auto& node : m_currentNode.includes())
    {
        if(node.path() == path)
        {
            m_selectionStack.push(m_currentNode);
            m_currentNode = node;
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
                if(m_builder.selectNode(PLoc.getFilename()))
                {
                    m_builder.currentNode().setFullInclude(true);
                }
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