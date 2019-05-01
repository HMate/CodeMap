#include "IncludeTreeDiagramBuilder.h"

#include <QGraphicsScene>

const QSizeF margin = QSizeF(10, 15);

typedef std::vector<BoxDGI*> IncludeDiagramTreeLevel;
typedef std::vector <IncludeDiagramTreeLevel> IncludeDiagramTree;

class IncludeDiagramBuilder
{
    QPointF m_pos = QPointF(0, 0);
    IncludeDiagramTree m_levels;
    BoxDGI* m_currentBox = nullptr;

public:

    IncludeDiagramBuilder(){}

    void build(QGraphicsScene& scene, cm::IncludeTree& tree)
    {
        m_pos = QPointF(0, 0);
        m_levels = IncludeDiagramTree();
        m_currentBox = nullptr;

        auto current = addRootBox(scene, tree);
        
        // TODO: Add class that coordinates the coloring of nodes and knows about which boxes are for the same include
        // TODO: Have to add arrows too from parent to child, and align those too
        recursiveBuildIncludeTreeLevel(scene, current, 1);

        alignBoxesToCenter();
    }

private:
    cm::IncludeNode& addRootBox(QGraphicsScene& scene, cm::IncludeTree& tree)
    {
        auto current = tree.root();
        auto levelBoxes = IncludeDiagramTreeLevel();

        auto m_currentBox = new BoxDGI(current.name());
        m_currentBox->setPos(m_pos);
        scene.addItem(m_currentBox);
        levelBoxes.emplace_back(m_currentBox);
        auto rect = m_currentBox->boundingRect();
        m_pos = m_pos + QPointF(0, rect.height() + margin.height());

        m_levels.emplace_back(levelBoxes);
    }

    void recursiveBuildIncludeTreeLevel(QGraphicsScene& scene, const cm::IncludeNode& current, int currentLevel)
    {
        auto pos2 = m_pos;
        IncludeDiagramTreeLevel levelBoxes;

        qreal h = 0;
        auto& includes = current.includes();
        for(auto& inc : includes)
        {
            auto box = new BoxDGI(inc.name());
            box->setPos(pos2);
            scene.addItem(box);
            levelBoxes.emplace_back(box);

            scene.addItem(new ArrowDGI(m_currentBox, box));

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
            recursiveBuildIncludeTreeLevel(scene, inc, currentLevel + 1);
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
    // lefttop is x-y-, middle is x0y0, rightbot is x+y+
    QGraphicsScene& scene = *diagram.getScene();

    IncludeDiagramBuilder builder;
    builder.build(scene, tree);
}

