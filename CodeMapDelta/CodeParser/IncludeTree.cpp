#include "IncludeTree.h"


namespace cm
{

/**************** IncludeNode *****************/

const std::string IncludeNode::name() const
{
    return this->m_name;
}

const std::string IncludeNode::path() const
{
    return this->m_path;
}

std::vector<IncludeNode>& IncludeNode::includes()
{
    return this->m_includes;
}

void IncludeNode::setFullInclude(bool fullInclude)
{
    this->m_fullInclude = fullInclude;
}

bool IncludeNode::isFullInclude() const
{
    return m_fullInclude;
}

void IncludeNode::addInclude(std::string name, std::string path)
{
    this->m_includes.emplace_back(name, path);
}

/**************** IncludeTree *****************/

IncludeNode& IncludeTree::root()
{
    return m_root;
}

}