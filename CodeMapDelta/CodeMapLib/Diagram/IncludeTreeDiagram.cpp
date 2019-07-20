#include "IncludeTreeDiagram.h"

BoxBuilder& BoxBuilder::operator=(const BoxBuilder & other)
{
    m_box = other.m_box;
    return *this;
}

bool BoxBuilder::operator==(const BoxBuilder& o)
{
    return m_box == o.m_box;
}

void IncludeTreeDiagram::addBoxesToLevel(int levelIndex, const IncludeDiagramBuilderLevel& level) 
{
    Q_ASSERT(size() >= levelIndex);
    if (size() == levelIndex)
    {
        if (level.size() > 0)
            addLevel(level);
    }
    else
    {
        auto& requestedLevel = (*this)[levelIndex];
        requestedLevel.insertGroup(level);
    }
}

void IncludeTreeDiagram::addLevel(const IncludeDiagramBuilderLevel& level)
{
    emplace_back(level);
}

std::pair<bool, BoxBuilder*> IncludeTreeDiagram::tryGetBoxWithPath(std::string path)
{
    QString fullPath = QString::fromStdString(path);
    for (auto& level : *this)
    {
        for (auto& item : level)
        {
            if (item.m_box->getFullName() == fullPath)
                return std::make_pair(true, &item);
        }
    }
    return std::make_pair(false, nullptr);
}