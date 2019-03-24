#pragma once

#include <vector>

namespace cm
{

struct IncludeTree;

class IncludeNodeRef
{
    IncludeTree& tree;
    bool fullInclude;
public:
    size_t index;

    IncludeNodeRef(IncludeTree& tree, size_t index, bool fullInclude);
    IncludeNodeRef& operator=(const IncludeNodeRef& o);
    void addInclude(IncludeTree& tree, size_t index, bool fullInclude);

    const std::string name() const;
    const std::string path() const;
    const std::vector<IncludeNodeRef>& includes() const;
    // Returns false if the include is skipped due to header guard
    void setFullInclude(bool fullInclude);
    bool isFullInclude() const;
    bool isRecursive() const;
};

struct IncludeNode
{
    const std::string name;
    const std::string path;
    std::vector<IncludeNodeRef> includes;

    IncludeNode(std::string name, std::string path) : name(name), path(path) {}
};

struct IncludeTree
{
    std::vector<IncludeNode> nodes;

    IncludeNodeRef root();
    IncludeNode& node(const IncludeNodeRef& ref);
    const IncludeNode& node(const IncludeNodeRef& ref) const;
};

}
