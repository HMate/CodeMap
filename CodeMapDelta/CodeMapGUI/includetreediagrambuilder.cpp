#include "includetreediagrambuilder.h"


#include "diagramview.h"

void recursiveBuildIncludeTreeLevel(QGraphicsScene& scene, const cm::IncludeNodeRef& current, QPointF& pos);

void buildIncludeTreeDiagram(QGraphicsScene& scene, cm::IncludeTree& tree)
{
    // lefttop is x-y-, middle is x0y0, rightbot is x+y+

    auto pos = QPointF(0, 0);

    auto current = tree.root();
    auto box = new BoxDGI(current.name());
    box->setPos(pos);
    scene.addItem(box);
    pos = pos + QPointF(0, box->boundingRect().height());

    recursiveBuildIncludeTreeLevel(scene, current, pos);
}

void recursiveBuildIncludeTreeLevel(QGraphicsScene& scene, const cm::IncludeNodeRef& current, QPointF& pos)
{
    auto pos2 = pos;
    int h = 0;
    for(auto& inc : current.includes())
    {
        auto box = new BoxDGI(inc.name());
        box->setPos(pos2);
        scene.addItem(box);
        pos2 = pos2 + QPointF(box->boundingRect().width(), 0);
        h = box->boundingRect().height();
    }
    pos = pos + QPointF(0, h);

    for(auto& inc : current.includes())
    {
        recursiveBuildIncludeTreeLevel(scene, inc, pos);
    }

    pos = QPointF(pos2.x(), pos.y() - h);
}