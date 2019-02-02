#ifndef INCLUDECOLLECTOR_H
#define INCLUDECOLLECTOR_H

#include "codeparser.h"

#include <stack>

namespace cm
{

class IncludeTreeBuilder
{
    IncludeTree& m_tree;
    IncludeNodeRef m_currentNode;
    std::stack<IncludeNodeRef> m_selectionStack;
public:
    IncludeTreeBuilder(IncludeTree& tree);
    IncludeNodeRef getRoot() const;
    IncludeNodeRef currentNode();
    void setRoot(std::string name, std::string path);
    void addNode(std::string name, std::string path);

    /** Selects the first child node with the given path as the currentNode. 
    * The node must be a child node of the current selected node
    * The mathod fails if there is no child node of the current node with this path*/
    bool selectNode(std::string path);
    void selectParent();
    bool isRootSelected();
};

}
#endif //INCLUDECOLLECTOR_H