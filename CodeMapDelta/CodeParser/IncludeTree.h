#pragma once

#include <vector>
#include <string>

namespace cm
{

class IncludeNode
{
    std::string m_name;
    std::string m_path;
    std::vector<IncludeNode> m_includes;
    int32_t m_id;
    bool m_fullInclude = false;

public:
    IncludeNode(int32_t m_id, std::string name, std::string path, bool fullInclude = false);
    
    const int32_t id() const;
    const std::string name() const;
    const std::string path() const;
    
    std::vector<IncludeNode>& includes();
    const std::vector<IncludeNode>& includes() const;

    IncludeNode& addInclude(int32_t id, std::string name, std::string path);
    void setNameAndPath(std::string name, std::string path);
    void setFullInclude(bool fullInclude);
    bool isFullInclude() const;

    bool operator==(const IncludeNode& other) const;
};

struct IncludeTree
{
    IncludeNode m_root;

    IncludeTree() : m_root(0, "", "") {}
    IncludeNode& root();
    const IncludeNode& root() const;
};

}
