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
    std::vector<IncludeDiagramBuilderLevel> m_groups;
    std::vector<BoxBuilder> m_items;
    BoxBuilder m_parent;
public:
    IncludeDiagramBuilderLevel(const BoxBuilder& parent) : m_parent(parent) {}

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

    const std::vector<IncludeDiagramBuilderLevel>& groups()
    {
        return m_groups;
    }

    void insertGroup(IncludeDiagramBuilderLevel& other) noexcept
    {
        m_groups.push_back(other);
        m_items.insert(this->end(), other.begin(), other.end());
    }

    const BoxBuilder& parent()
    {
        return m_parent;
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

        diagram.setUpdatesEnabled(false);

        auto& box = addRootBox(diagram, scene, tree);
        recursiveBuildIncludeTreeLevel(diagram, scene, box, 1);
        //alignBoxesToCenter(m_levels);
        alignBoxesToGroups(m_levels);

        diagram.setUpdatesEnabled(true);
        diagram.update();
    }

private:
    BoxBuilder addRootBox(IncludeDiagramView& diagram, QGraphicsScene& scene, cm::IncludeTree& tree)
    {
        auto& current = tree.root();

        auto box = new BoxDGI(diagram, current.name(), current.path());
        box->setFullInclude(current.isFullInclude());
        scene.addItem(box);

        BoxBuilder bb{ current, box };
        auto levelBoxes = IncludeDiagramBuilderLevel(bb);
        levelBoxes.emplace_back(bb);
        auto rect = box->boundingRect();
        m_pos = m_pos + QPointF(0, rect.height() + margin.height());

        m_levels.emplace_back(levelBoxes);
        return bb;
    }
    
    void recursiveBuildIncludeTreeLevel(IncludeDiagramView& diagram, QGraphicsScene& scene, const BoxBuilder& current, int currentLevel)
    {
        IncludeDiagramBuilderLevel levelBoxes(current);

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
            level.insertGroup(levelBoxes);
        }

        Q_ASSERT(includes.size() == levelBoxes.size());
        for(auto i = 0; i < includes.size(); i++)
        {
            auto box = levelBoxes[i];
            recursiveBuildIncludeTreeLevel(diagram, scene, box, currentLevel + 1);
        }
    }

    /// Align the boxes of levels to not overlap, and to be centered relative to the prev level.
    void alignBoxesToCenter(IncludeDiagramTree& levels)
    {
        std::vector<QSizeF> levelSizes = calculateLevelSizes(levels);

        qreal y = 0;
        for(size_t i = 0; i < levels.size(); i++)
        {
            auto& level = levels[i];
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

    /// Align the boxes of levels to not overlap.
    /// Do this by making groups out of boxes who have the same parent. These groups are centered to their parent.
    /// But the space between group elements is based on how many child they have.
    void alignBoxesToGroups(IncludeDiagramTree& levels)
    {
        std::vector<QSizeF> levelSizes = calculateLevelSizes(levels);

        // TODO: I need somehow to align boxes under theirparent locally not just to level center.
        // This becomes interesting when two neighbour parents have many children, which will overlap.
        // The pretty solution is to move the parents further apart, but the questions is by how much?
        // use std::vector<std::vector<std::vector<BoxDGI*>>> for this?
        // basically std::vector<Level<BoxGroup>>

        qreal y = 0;
        for(size_t i = 0; i < levels.size(); i++)
        {
            auto& level = levels[i];
            auto size = levelSizes[i];

            for(auto& box : level)
            {
                box.m_box->setY(y);
            }
            y += size.height() + margin.height();
        }

        auto& widestLevel = std::max_element(levelSizes.begin(), levelSizes.end(), [](const QSizeF& s1, const QSizeF& s2)
        {
            return s2.width() > s1.width();
        });

        size_t widestIndex = std::distance(levelSizes.begin(), widestLevel);

        auto& level = levels[widestIndex];
        auto size = levelSizes[widestIndex];
        qreal levelCenter = size.width() / 2.0;
        qreal x = -levelCenter;
        for(auto& box : level)
        {
            box.m_box->setX(x);
            x += box.m_box->boundingRect().width() + margin.width();
        }

        for(size_t i = 0; i < levels.size(); i++)
        {
            auto& level = levels[i];
            auto size = levelSizes[i];

            qreal levelCenter = size.width() / 2.0;
            qreal x = -levelCenter;
            for(auto& box : level)
            {
                box.m_box->setX(x);
                x += box.m_box->boundingRect().width() + margin.width();
            }
        }
    }

    std::vector<QSizeF> calculateLevelSizes(IncludeDiagramTree levels)
    {
        std::vector<QSizeF> levelSizes;
        for(auto& level : levels)
        {
            Q_ASSERT(level.size() > 0);
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
        Q_ASSERT(levelSizes.size() == levels.size());
        return levelSizes;
    }
};

void buildIncludeTreeDiagram(IncludeDiagramView& diagram, cm::IncludeTree& tree)
{
    IncludeDiagramBuilder builder;
    builder.build(diagram, tree);
}

