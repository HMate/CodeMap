#include "IncludeTreeDiagramBuilder.h"

#include <QGraphicsScene>

const QSizeF margin = QSizeF(10, 15);

struct BoxBuilder
{
    cm::IncludeNode& m_node;
    BoxDGI* m_box;

    BoxBuilder& BoxBuilder::operator=(const BoxBuilder & other)
    {
        m_node = other.m_node;
        m_box = other.m_box;
        return *this;
    }
};

class IncludeDiagramBuilderLevel
{
    std::vector<BoxBuilder> m_items;
public:

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
    void insert(IncludeDiagramBuilderLevel& other) noexcept
    {
        m_items.insert(this->end(), other.begin(), other.end());
    }
};

typedef std::vector<IncludeDiagramBuilderLevel> IncludeDiagramTree;

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

        recursiveBuildIncludeTreeLevel(diagram, scene, box, 1);

        alignBoxesToCenter();

        diagram.setUpdatesEnabled(true);
        diagram.update();
    }

private:
    BoxBuilder addRootBox(IncludeDiagramView& diagram, QGraphicsScene& scene, cm::IncludeTree& tree)
    {
        auto& current = tree.root();
        auto levelBoxes = IncludeDiagramBuilderLevel();

        auto box = new BoxDGI(diagram, current.name(), current.path());
        box->setPos(m_pos);
        scene.addItem(box);

        BoxBuilder bb{ current, box };
        levelBoxes.emplace_back(bb);
        auto rect = box->boundingRect();
        m_pos = m_pos + QPointF(0, rect.height() + margin.height());

        m_levels.emplace_back(levelBoxes);
        return bb;
    }

    void buildTreeLevel(IncludeDiagramView& diagram, QGraphicsScene& scene, const cm::IncludeNode& current, BoxDGI* currentBox, int currentLevel)
    {
        auto pos2 = m_pos;
        IncludeDiagramBuilderLevel levelBoxes;
    }

    void recursiveBuildIncludeTreeLevel(IncludeDiagramView& diagram, QGraphicsScene& scene, const BoxBuilder& current, int currentLevel)
    {
        auto pos2 = m_pos;
        IncludeDiagramBuilderLevel levelBoxes;

        qreal h = 0;
        auto& includes = current.m_node.includes();
        for(auto& inc : includes)
        {
            auto box = new BoxDGI(diagram, inc.name(), inc.path());
            box->setPos(pos2);
            scene.addItem(box);
            BoxBuilder bb{ inc, box };
            levelBoxes.emplace_back(bb);

            scene.addItem(new ArrowDGI(current.m_box, box));

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
            level.insert(levelBoxes);
        }

        // set next levels y coordinate
        m_pos = m_pos + QPointF(0, h + margin.height());

        Q_ASSERT(includes.size() == levelBoxes.size());
        for(auto i = 0; i < includes.size(); i++)
        {
            auto box = levelBoxes[i];
            recursiveBuildIncludeTreeLevel(diagram, scene, box, currentLevel + 1);
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
            qreal x0 = level.front().m_box->x();
            auto last = level.back();
            qreal x1 = last.m_box->x() + last.m_box->boundingRect().width();

            qreal levelCenter = (x1 - x0) / 2.0;
            qreal cDiff = prevLevelCenter - levelCenter;
            for(auto& box : level)
            {
                box.m_box->setX(box.m_box->x() + cDiff);
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

