#include "IncludeTree.h"


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

}