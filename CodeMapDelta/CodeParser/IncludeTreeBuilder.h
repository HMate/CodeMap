#ifndef INCLUDECOLLECTOR_H
#define INCLUDECOLLECTOR_H

#include "IncludeTree.h"

#include <memory>
#include <stack>
#include <QString>

namespace cm
{

std::unique_ptr<IncludeTree> getIncludeTree(const QString& srcPath, const std::vector<QString>& includeDirs);

class IncludeTreeBuilder
{
    IncludeTree& m_tree;
    IncludeNode* m_currentNode;
    std::stack<IncludeNode*> m_selectionStack;
    int32_t m_nextId = 1;
public:
    IncludeTreeBuilder(IncludeTree& tree);
    IncludeNode& getRoot() const;
    IncludeNode& currentNode();
    void setRoot(std::string name, std::string path);
    IncludeNode& addNode(std::string name, std::string path);

    /** Selects the first child node with the given path as the currentNode. 
    * The node must be a child node of the current selected node
    * The mathod fails if there is no child node of the current node with this path*/
    bool selectNode(std::string path);
    void selectPreviousNode();
    bool isRootSelected();
};

}
#endif //INCLUDECOLLECTOR_H