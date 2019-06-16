#ifndef INCLUDETREEDIAGRAMBUILDER_H
#define INCLUDETREEDIAGRAMBUILDER_H

#include <QGraphicsScene>

#include "DiagramItems.h"
#include "IncludeTreeDiagram.h"

class IncludeDiagramView;

void buildIncludeTreeDiagram(IncludeDiagramView& diagram, cm::IncludeTree& tree);

class IncludeDiagramBuilder
{
    QPointF m_pos = QPointF(0, 0);
    std::unique_ptr<IncludeTreeDiagram> m_levels;

public:
    IncludeDiagramBuilder() {}
    void build(IncludeDiagramView& diagram, cm::IncludeTree& tree);

private:
    BoxBuilder addRootBox(IncludeDiagramView& diagram, QGraphicsScene& scene, cm::IncludeTree& tree);
    void recursiveBuildIncludeTreeLevel(IncludeDiagramView& diagram, QGraphicsScene& scene, const BoxBuilder& current, int currentLevel);

    /// Align the boxes of levels to not overlap, and to be centered relative to the prev level.
    void alignBoxesToCenter(IncludeTreeDiagram& levels);

    /// Align the boxes of levels to not overlap.
    /// Do this by making groups out of boxes who have the same parent. These groups are centered to their parent.
    /// But the space between group elements is based on how many child they have.
    void alignBoxesToGroups(IncludeTreeDiagram& levels);
    void moveBoxesToRightRecursively(IncludeTreeDiagram& levels, size_t levelIndex, size_t from, qreal moveBy);
    std::vector<QSizeF> calculateLevelSizes(IncludeTreeDiagram& levels);
};

#endif // !INCLUDETREEDIAGRAMBUILDER_H
