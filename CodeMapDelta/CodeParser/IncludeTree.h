#pragma once

#include <vector>
#include <string>

namespace cm
{

struct IncludeNode
{
    std::string m_name;
    std::string m_path;
    std::vector<IncludeNode> m_includes;
    bool m_fullInclude = false;

    IncludeNode(std::string name, std::string path, bool fullInclude = false);
    
    const std::string name() const;
    const std::string path() const;
    
    std::vector<IncludeNode>& includes();
    const std::vector<IncludeNode>& includes() const;

    IncludeNode& addInclude(std::string name, std::string path);
    void setFullInclude(bool fullInclude);
    bool isFullInclude() const;
};

struct IncludeTree
{
    IncludeNode m_root;

    IncludeTree() : m_root("", "") {}
    IncludeNode& root();
    const IncludeNode& root() const;
};

}
