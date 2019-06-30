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
    void build(IncludeDiagramView& diagram);

private:
    BoxBuilder addRootBox(IncludeDiagramView& diagram, QGraphicsScene& scene, cm::IncludeTree& tree);
    void recursiveBuildIncludeTreeLevel(IncludeDiagramView& diagram, QGraphicsScene& scene, const BoxBuilder& current, int currentLevel);

};

#endif // !INCLUDETREEDIAGRAMBUILDER_H
