#ifndef INCLUDETREEDIAGRAM_H
#define INCLUDETREEDIAGRAM_H

#include "DiagramItems.h"
#include "CodeParser.h"

struct BoxBuilder
{
    cm::IncludeNode& m_node;
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

    size_t size() noexcept { return m_items.size(); }
    BoxBuilder operator[](size_t index) { return m_items[index]; }
    BoxBuilder front() { return m_items.front(); }
    BoxBuilder back() { return m_items.front(); }
    std::vector<BoxBuilder>::iterator begin() noexcept { return m_items.begin(); }
    std::vector<BoxBuilder>::iterator end() noexcept { return m_items.end(); }
    std::reverse_iterator<std::vector<BoxBuilder>::iterator> rend() noexcept { return m_items.rend(); }

    const std::vector<IncludeDiagramBuilderLevel>& groups()
    {
        return m_groups;
    }

    void insertGroup(IncludeDiagramBuilderLevel& other) noexcept
    {
        m_groups.push_back(other);
        m_items.insert(this->end(), other.begin(), other.end());
    }

    const BoxBuilder& parent()
    {
        return m_parent;
    }
};

class IncludeTreeDiagram
{
    using LevelList = std::vector<IncludeDiagramBuilderLevel>;
    LevelList m_levels;
public:

    void addLevel(const IncludeDiagramBuilderLevel& level) { m_levels.emplace_back(level); }
    IncludeDiagramBuilderLevel& operator[](size_t index) { return m_levels[index]; }
    size_t size() const noexcept{ return m_levels.size(); }
    LevelList::iterator begin() noexcept { return m_levels.begin(); }
    LevelList::iterator end() noexcept { return m_levels.end(); }
};


#endif // INCLUDETREEDIAGRAM_H