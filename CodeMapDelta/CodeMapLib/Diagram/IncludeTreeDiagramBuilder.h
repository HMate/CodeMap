#ifndef INCLUDETREEDIAGRAMBUILDER_H
#define INCLUDETREEDIAGRAMBUILDER_H

#include <QGraphicsScene>

#include "DiagramItems.h"
#include "IncludeTreeDiagram.h"

class IncludeDiagramView;

void buildIncludeTreeDiagram(IncludeDiagramView& diagram, std::shared_ptr<cm::IncludeTree> tree);


class IncludeDiagramBuilder
{
    QPointF m_pos = QPointF(0, 0);
    std::unique_ptr<IncludeTreeDiagram> m_levels;

public:
    IncludeDiagramBuilder() {}

    /// Build an include tree, where inlcudes of the same files appear as separate nodes.
    void buildTree(IncludeDiagramView& diagram);

    /// Build an include graph, where multiple includes of the same file point to the same node.
    void buildGraph(IncludeDiagramView& diagram);

private:
    BoxBuilder addRootBox(IncludeDiagramView& diagram, QGraphicsScene& scene, cm::IncludeTree& tree);
    void recursiveBuildIncludeTreeLevel(IncludeDiagramView& diagram, QGraphicsScene& scene, const BoxBuilder& current, int currentLevel);
    void recursiveBuildIncludeGraphLevel(IncludeDiagramView& diagram, QGraphicsScene& scene, const BoxBuilder& current, int currentLevel);
};

#endif // !INCLUDETREEDIAGRAMBUILDER_H
