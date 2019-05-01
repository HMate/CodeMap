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

        alignBoxes();

        diagram.setUpdatesEnabled(true);
        diagram.update();
    }

private:
    BoxBuilder addRootBox(IncludeDiagramView& diagram, QGraphicsScene& scene, cm::IncludeTree& tree)
    {
        auto& current = tree.root();
        auto levelBoxes = IncludeDiagramBuilderLevel();

        auto box = new BoxDGI(diagram, current.name(), current.path());
        box->setFullInclude(current.isFullInclude());
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
        IncludeDiagramBuilderLevel levelBoxes;

        auto& includes = current.m_node.includes();
        for(auto& inc : includes)
        {
            auto box = new BoxDGI(diagram, inc.name(), inc.path());
            box->setFullInclude(inc.isFullInclude());
            scene.addItem(box);
            BoxBuilder bb{ inc, box };
            levelBoxes.emplace_back(bb);

            scene.addItem(new ArrowDGI(current.m_box, box));
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

        Q_ASSERT(includes.size() == levelBoxes.size());
        for(auto i = 0; i < includes.size(); i++)
        {
            auto box = levelBoxes[i];
            recursiveBuildIncludeTreeLevel(diagram, scene, box, currentLevel + 1);
        }
    }

    /// Align the boxes of levels to not overlap, and to be centered relative to the prev level.
    void alignBoxes()
    {
        std::vector<QSizeF> levelSizes;
        for(auto& level : m_levels)
        {
            QSizeF size(0, 0);
            for(auto& box : level)
            {
                auto rect = box.m_box->boundingRect();
                if(rect.height() > size.height())
                {
                    size.setHeight(rect.height());
                }
                size.setWidth(size.width() + rect.width());
            }

            size.setWidth(size.width() + level.size()*margin.width());
            levelSizes.emplace_back(size);
        }

        // TODO: I need somehow to align boxes under theirparent locally not just to level center
        // This becomes interesting when two neighbour parents have many children, which will overlap
        // The pretty solution is to move the parents further apart, but the questions is by how much?
        // use std::vector<std::vector<std::vector<BoxDGI*>>> for this?
        // basically std::vector<Level<BoxGroup>>
        Q_ASSERT(levelSizes.size() == m_levels.size());

        qreal y = 0;
        for(size_t i = 0; i < m_levels.size(); i++)
        {
            auto& level = m_levels[i];
            Q_ASSERT(level.size() > 0);

            auto size = levelSizes[i];

            qreal levelCenter = size.width() / 2.0;
            qreal x = -levelCenter;
            for(auto& box : level)
            {
                box.m_box->setPos(x, y);
                x += box.m_box->boundingRect().width() + margin.width();
            }
            y += size.height() + margin.height();
        }
    }
};

void buildIncludeTreeDiagram(IncludeDiagramView& diagram, cm::IncludeTree& tree)
{
    IncludeDiagramBuilder builder;
    builder.build(diagram, tree);
}

