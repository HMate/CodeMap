#include "includetreediagrambuilder.h"


#include "diagramview.h"


const qreal xMargin = 10;
const qreal yMargin = 15;

void recursiveBuildIncludeTreeLevel(QGraphicsScene& scene, const cm::IncludeNodeRef& current, BoxDGI* currentBox, QPointF& pos,
    std::vector<std::vector<BoxDGI*>>& levels, int currentLevel);

void buildIncludeTreeDiagram(QGraphicsScene& scene, cm::IncludeTree& tree)
{
    // lefttop is x-y-, middle is x0y0, rightbot is x+y+

    auto pos = QPointF(0, 0);
    std::vector<BoxDGI*> levelBoxes;

    auto current = tree.root();
    auto box = new BoxDGI(current.name());
    box->setPos(pos);
    scene.addItem(box); 
    levelBoxes.emplace_back(box);
    auto rect = box->boundingRect();
    pos = pos + QPointF(0, rect.height() + yMargin);

    std::vector < std::vector<BoxDGI*>> levels;
    levels.emplace_back(levelBoxes);
    
    // TODO: Add class that coordinates the coloring of nodes and knows about which boxes are for the same include
    // TODO: Have to add arrows too from parent to child, and align those too
    recursiveBuildIncludeTreeLevel(scene, current, box, pos, levels, 1);

    // Align the boxes of levels to be centered relative to the prev level.
    // TODO: I need somehow to align boxes under theirparent locally not just to level center
    // This becomes interesting when two neighbour parents have many children, which will overlap
    // The pretty solution is to move the parents further apart, but the questions is by how much?
    // use std::vector<std::vector<std::vector<BoxDGI*>>> for this?
    // basically std::vector<Level<BoxGroup>>
    qreal prevLevelCenter = 0;
    for(auto& level : levels)
    {
        Q_ASSERT(level.size() > 0);
        qreal x0 = level.front()->x();
        auto& last = level.back();
        qreal x1 = last->x() + last->boundingRect().width();

        qreal levelCenter = (x1 - x0) / 2.0;
        qreal cDiff = prevLevelCenter - levelCenter;
        for(auto& box : level)
        {
            box->setX(box->x() + cDiff);
        }
        levelCenter += cDiff;
        prevLevelCenter = levelCenter;
    }
}

void recursiveBuildIncludeTreeLevel(QGraphicsScene& scene, const cm::IncludeNodeRef& current, BoxDGI* currentBox, QPointF& pos,
    std::vector<std::vector<BoxDGI*>>& levels, int currentLevel)
{
    auto pos2 = pos;
    std::vector<BoxDGI*> levelBoxes;

    qreal h = 0;
    auto& includes = current.includes();
    for(auto& inc : includes)
    {
        auto box = new BoxDGI(inc.name());
        box->setPos(pos2);
        scene.addItem(box);
        levelBoxes.emplace_back(box);

        scene.addItem(new ArrowDGI(currentBox, box));

        pos2 = pos2 + QPointF(box->boundingRect().width() + xMargin, 0);
        h = box->boundingRect().height();
    }

    Q_ASSERT(levels.size() >= currentLevel);
    if(levels.size() == currentLevel)
    {
        if(levelBoxes.size() > 0)
            levels.emplace_back(levelBoxes);
    }
    else
    {
        auto& level = levels[currentLevel];
        level.insert(level.end(), levelBoxes.begin(), levelBoxes.end());
    }

    // set next levels y coordinate
    pos = pos + QPointF(0, h + yMargin);

    Q_ASSERT(includes.size() == levelBoxes.size());
    for(auto i = 0; i < includes.size(); i++)
    {
        auto& inc = includes[i];
        auto box = levelBoxes[i];
        recursiveBuildIncludeTreeLevel(scene, inc, box, pos, levels, currentLevel+1);
    }

    // set back this levels y coordinate and x coordinate for next boxes on this level.
    pos = QPointF(pos2.x(), pos.y() - h - yMargin);
}