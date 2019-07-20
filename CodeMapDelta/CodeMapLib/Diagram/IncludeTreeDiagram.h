#ifndef INCLUDETREEDIAGRAM_H
#define INCLUDETREEDIAGRAM_H

#include "DiagramItems.h"
#include "CodeParser.h"

struct BoxBuilder
{
    BoxDGI* m_box;

    BoxBuilder& operator=(const BoxBuilder & other);
    bool operator==(const BoxBuilder& o);
};

class IncludeDiagramBuilderLevel
{
    std::vector<IncludeDiagramBuilderLevel> m_groups;
    std::vector<BoxBuilder> m_items;
    BoxBuilder m_parent;
public:
    IncludeDiagramBuilderLevel(const BoxBuilder& parent) : m_parent(parent) {}

    void emplace_back(BoxBuilder box)
    {
        m_items.emplace_back(box);
    }

    size_t size() const noexcept { return m_items.size(); }
    BoxBuilder operator[](size_t index) { return m_items[index]; }
    BoxBuilder operator[](size_t index) const { return m_items[index]; }
    BoxBuilder front() { return m_items.front(); }
    BoxBuilder back() { return m_items.back(); }
    std::vector<BoxBuilder>::iterator begin() noexcept { return m_items.begin(); }
    std::vector<BoxBuilder>::const_iterator begin() const noexcept { return m_items.cbegin(); }
    std::vector<BoxBuilder>::iterator end() noexcept { return m_items.end(); }
    std::vector<BoxBuilder>::const_iterator end() const noexcept { return m_items.cend(); }
    std::reverse_iterator<std::vector<BoxBuilder>::iterator> rend() noexcept { return m_items.rend(); }

    const std::vector<IncludeDiagramBuilderLevel>& groups()
    {
        return m_groups;
    }

    void insertGroup(const IncludeDiagramBuilderLevel& other) noexcept
    {
        m_groups.push_back(other);
        m_items.insert(this->end(), other.begin(), other.end());
    }

    const BoxBuilder& parent()
    {
        return m_parent;
    }
};

class IncludeTreeDiagram : public std::vector<IncludeDiagramBuilderLevel>
{
public:

    void addBoxesToLevel(int levelIndex, const IncludeDiagramBuilderLevel& level);
    void addLevel(const IncludeDiagramBuilderLevel& level);

    /// Returns (true, box*) if the box is added to the diagram with this path, otherwise false.
    std::pair<bool, BoxBuilder*> tryGetBoxWithPath(std::string path);
};


#endif // INCLUDETREEDIAGRAM_H