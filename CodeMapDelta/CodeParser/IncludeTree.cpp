#include "IncludeTree.h"


namespace cm
{

/**************** IncludeNode *****************/

IncludeNode::IncludeNode(std::string name, std::string path, bool fullInclude)
    : m_name(name), m_path(path), m_fullInclude(fullInclude) {}

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

const std::vector<IncludeNode>& IncludeNode::includes() const
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

IncludeNode& IncludeNode::addInclude(std::string name, std::string path)
{
    this->m_includes.emplace_back(name, path);
    return this->m_includes.back();
}

/**************** IncludeTree *****************/

IncludeNode& IncludeTree::root()
{
    return m_root;
}

const IncludeNode& IncludeTree::root() const
{
    return m_root;
}

}