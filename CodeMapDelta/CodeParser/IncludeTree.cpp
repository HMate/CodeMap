#include "IncludeTree.h"


namespace cm
{

/**************** IncludeNode *****************/

IncludeNode::IncludeNode(int32_t id, std::string name, std::string path, bool fullInclude)
    : m_id(id), m_name(name), m_path(path), m_fullInclude(fullInclude) {}

const int32_t IncludeNode::id() const
{
    return m_id;
}

const std::string IncludeNode::name() const
{
    return m_name;
}

const std::string IncludeNode::path() const
{
    return m_path;
}

std::vector<IncludeNode>& IncludeNode::includes()
{
    return m_includes;
}

const std::vector<IncludeNode>& IncludeNode::includes() const
{
    return m_includes;
}

void IncludeNode::setNameAndPath(std::string name, std::string path)
{
    m_name = name;
    m_path = path;
}

void IncludeNode::setFullInclude(bool fullInclude)
{
    m_fullInclude = fullInclude;
}

bool IncludeNode::isFullInclude() const
{
    return m_fullInclude;
}

IncludeNode& IncludeNode::addInclude(int32_t id, std::string name, std::string path)
{
    m_includes.emplace_back(id, name, path);
    return m_includes.back();
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