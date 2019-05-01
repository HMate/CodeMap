#include "IncludeTreeDiagramBuilder.h"

#include <QGraphicsScene>

const QSizeF margin = QSizeF(10, 15);

typedef std::vector<BoxDGI*> IncludeDiagramTreeLevel;
typedef std::vector <IncludeDiagramTreeLevel> IncludeDiagramTree;

struct BoxBuilder
{
    cm::IncludeNode& m_node;
    BoxDGI* m_box;
};

class IncludeDiagramBuilder
{
    QPointF m_pos = QPointF(0, 0);
    IncludeDiagramTree m_levels;

public:

    IncludeDiagramBuilder(){}

    void build(IncludeDiagramView& diagram, cm::IncludeTree& tree)
    {
        // lefttop is x-y-, middle is x0y0, rightbot is x+y+
        QGraphicsScene& scene = *diagram.getScene();
        m_pos = QPointF(0, 0);
        m_levels = IncludeDiagramTree();

        auto& box = addRootBox(diagram, scene, tree);
        
        diagram.setUpdatesEnabled(false);
        // TODO: Add class that coordinates the coloring of nodes and knows about which boxes are for the same include
        // TODO: Have to add arrows too from parent to child, and align those too
        recursiveBuildIncludeTreeLevel(diagram, scene, box.m_node, box.m_box, 1);

        alignBoxesToCenter();

        diagram.setUpdatesEnabled(true);
        diagram.update();
    }

private:
    BoxBuilder addRootBox(IncludeDiagramView& diagram, QGraphicsScene& scene, cm::IncludeTree& tree)
    {
        auto& current = tree.root();
        auto levelBoxes = IncludeDiagramTreeLevel();

        auto box = new BoxDGI(diagram, current.name(), current.path());
        box->setPos(m_pos);
        scene.addItem(box);
        levelBoxes.emplace_back(box);
        auto rect = box->boundingRect();
        m_pos = m_pos + QPointF(0, rect.height() + margin.height());

        m_levels.emplace_back(levelBoxes);
        return BoxBuilder{ current, box };
    }

    void buildTreeLevel(IncludeDiagramView& diagram, QGraphicsScene& scene, const cm::IncludeNode& current, BoxDGI* currentBox, int currentLevel)
    {

    }

    void recursiveBuildIncludeTreeLevel(IncludeDiagramView& diagram, QGraphicsScene& scene, const cm::IncludeNode& current, BoxDGI* currentBox, int currentLevel)
    {
        auto pos2 = m_pos;
        IncludeDiagramTreeLevel levelBoxes;

        qreal h = 0;
        auto& includes = current.includes();
        for(auto& inc : includes)
        {
            auto box = new BoxDGI(diagram, inc.name(), inc.path());
            box->setPos(pos2);
            scene.addItem(box);
            levelBoxes.emplace_back(box);

            scene.addItem(new ArrowDGI(currentBox, box));

            pos2 = pos2 + QPointF(box->boundingRect().width() + margin.width(), 0);
            h = box->boundingRect().height();
        }

        Q_ASSERT(m_levels.size() >= currentLevel);
        if(m_levels.size() == currentLevel)
        {
            if(levelBoxes.size() > 0)
                m_levels.emplace_back(levelBoxes);
        }
        else
        {
            auto& level = m_levels[currentLevel];
            level.insert(level.end(), levelBoxes.begin(), levelBoxes.end());
        }

        // set next levels y coordinate
        m_pos = m_pos + QPointF(0, h + margin.height());

        Q_ASSERT(includes.size() == levelBoxes.size());
        for(auto i = 0; i < includes.size(); i++)
        {
            auto& inc = includes[i];
            auto box = levelBoxes[i];
            recursiveBuildIncludeTreeLevel(diagram, scene, inc, box, currentLevel + 1);
        }

        // set back this levels y coordinate and x coordinate for next boxes on this level.
        m_pos = QPointF(pos2.x(), m_pos.y() - h - margin.height());
    }

    /// Align the boxes of levels to be centered relative to the prev level.
    void alignBoxesToCenter()
    {
        // TODO: I need somehow to align boxes under theirparent locally not just to level center
        // This becomes interesting when two neighbour parents have many children, which will overlap
        // The pretty solution is to move the parents further apart, but the questions is by how much?
        // use std::vector<std::vector<std::vector<BoxDGI*>>> for this?
        // basically std::vector<Level<BoxGroup>>
        qreal prevLevelCenter = 0;
        for(auto& level : m_levels)
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
};

void buildIncludeTreeDiagram(IncludeDiagramView& diagram, cm::IncludeTree& tree)
{
    IncludeDiagramBuilder builder;
    builder.build(diagram, tree);
}

