#include "IncludeTreeDiagramBuilder.h"

#include "IncludeTree.h"
#include "IncludeTreeDiagram.h"
#include "IncludeDiagramView.h"

const QSizeF margin = QSizeF(10, 15);


void IncludeDiagramBuilder::build(IncludeDiagramView& diagram, cm::IncludeTree& tree)
{
    // lefttop is x-y-, middle is x0y0, rightbot is x+y+
    QGraphicsScene& scene = *diagram.getScene();
    m_pos = QPointF(0, 0);
    m_levels = std::make_unique<IncludeTreeDiagram>();

    diagram.setUpdatesEnabled(false);

    scene.clear();
    auto& box = addRootBox(diagram, scene, tree);
    recursiveBuildIncludeTreeLevel(diagram, scene, box, 1);
    //alignBoxesToCenter(m_levels);
    alignBoxesToGroups(*m_levels);

    diagram.setDiagram(m_levels);
    diagram.setUpdatesEnabled(true);
    diagram.update();
}

BoxBuilder IncludeDiagramBuilder::addRootBox(IncludeDiagramView& diagram, QGraphicsScene& scene, cm::IncludeTree& tree)
{
    auto& current = tree.root();

    auto box = new BoxDGI(diagram, current.name(), current.path());
    box->setFullInclude(current.isFullInclude());
    scene.addItem(box);

    BoxBuilder bb{ current, box };
    IncludeDiagramBuilderLevel levelBoxes(bb);
    levelBoxes.emplace_back(bb);
    auto rect = box->boundingRect();
    m_pos = m_pos + QPointF(0, rect.height() + margin.height());

    m_levels->addLevel(levelBoxes);
    return bb;
}
    
void IncludeDiagramBuilder::recursiveBuildIncludeTreeLevel(IncludeDiagramView& diagram, QGraphicsScene& scene, const BoxBuilder& current, int currentLevel)
{
    IncludeDiagramBuilderLevel levelBoxes(current);

    auto& includes = current.m_node.includes();
    for(auto& inc : includes)
    {
        auto box = new BoxDGI(diagram, inc.name(), inc.path());
        box->setFullInclude(inc.isFullInclude());
        scene.addItem(box);
        current.m_box->addChild(box);
        BoxBuilder bb{ inc, box };
        levelBoxes.emplace_back(bb);

        scene.addItem(new ArrowDGI(current.m_box, box));
    }

    Q_ASSERT(m_levels->size() >= currentLevel);
    if(m_levels->size() == currentLevel)
    {
        if(levelBoxes.size() > 0)
            m_levels->addLevel(levelBoxes);
    }
    else
    {
        auto& level = (*m_levels)[currentLevel];
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
void IncludeDiagramBuilder::alignBoxesToCenter(IncludeTreeDiagram& levels)
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

void IncludeDiagramBuilder::alignBoxesToGroups(IncludeTreeDiagram& levels)
{
    qreal y = 0;
    for(size_t i = 0; i < levels.size(); i++)
    {
        auto& level = levels[i];
            
        qreal levelHeight = 0;
        for(auto& box : level)
        {
            box.m_box->setY(y);
            levelHeight = std::max(box.m_box->boundingRect().height(), levelHeight);
        }
        y += levelHeight + margin.height();
    }
        
    // Place boxes bottom-up, to deal with overlaping boxes
    for(int i = levels.size()-1; i >= 0; i--)
    {
        auto& level = levels[i];

        // First place boxes with children, then fill up space with remaining boxes.

        std::vector<BoxBuilder> parents;
        std::copy_if(level.begin(), level.end(), std::back_inserter(parents), [](const BoxBuilder& box) 
        {
            return box.m_box->getChildren().size() > 0;
        });

        if(parents.size() == 0)
        {
            // this is the bottom row, nobody has children, just place them next to each other
            qreal x = 0;
            for(auto& box : level)
            {
                box.m_box->setX(x);
                x += box.m_box->boundingRect().width() + margin.width();
            }
        }
        else 
        {
            // Place parents to average of childs position
            for(auto& box : parents)
            {
                Q_ASSERT(box.m_box->getChildren().size() > 0);

                auto& children = box.m_box->getChildren();
                const qreal xStart = children.front()->pos().x();
                const qreal xEnd = children.back()->pos().x() + children.back()->boundingRect().width();
                const qreal width = box.m_box->boundingRect().width();
                box.m_box->setX((xStart + xEnd - width) / 2.0);
            }

            // Left neighbours of first parent: just place them next to each other from right to left
            auto& firstParentIt = std::find(level.begin(), level.end(), parents.front());
            qreal leftX = firstParentIt->m_box->pos().x();
            auto& beforeFirst = std::reverse_iterator(firstParentIt);
            for(; beforeFirst != level.rend(); beforeFirst++)
            {
                auto& neighbour = beforeFirst->m_box;
                auto moveBy = neighbour->boundingRect().width() + margin.width();
                leftX = leftX - moveBy;
                neighbour->setX(leftX);
            }

            // Now place every box without children.
            // We want to place these boxes evenly between box with children.
            // So in 1 cycle of this loop, we place the boxes between 2 parents
            auto secondParentIt = firstParentIt;

            while(firstParentIt != level.end())
            {
                while(secondParentIt != level.end())
                {
                    secondParentIt++;
                    if(secondParentIt != level.end() &&
                        secondParentIt->m_box->getChildren().size() > 0)
                        break;
                }

                if(secondParentIt != level.end())
                {
                    auto holeCount = std::distance(firstParentIt, secondParentIt);
                    const qreal minMarginWidth = margin.width() * holeCount;
                    qreal boxWidth = 0;
                    for(auto boxIt = std::next(firstParentIt); boxIt != secondParentIt; boxIt++)
                    {
                        boxWidth += boxIt->m_box->boundingRect().width();
                    }

                    qreal xStart = firstParentIt->m_box->pos().x() + firstParentIt->m_box->boundingRect().width();
                    if(secondParentIt->m_box->pos().x() < (xStart + boxWidth + minMarginWidth))
                    {
                        auto moveBy = (xStart + boxWidth + minMarginWidth) - secondParentIt->m_box->pos().x();
                            
                        size_t boxIndex = std::distance(level.begin(), secondParentIt);
                        moveBoxesToRightRecursively(levels, i, boxIndex, moveBy);
                    }

                    const qreal actualSpace = secondParentIt->m_box->pos().x() - xStart;
                    const qreal totalMarginWidth = actualSpace - boxWidth;
                    const qreal marginW = totalMarginWidth / holeCount;
                    qreal x = xStart + marginW;
                    for(auto boxIt = std::next(firstParentIt); boxIt != secondParentIt; boxIt++)
                    {
                        BoxDGI* box = boxIt->m_box;
                        box->setX(x);
                        x += box->boundingRect().width() + marginW;
                    }
                }
                else
                {
                    // First parent box is the last parent in this row, so just place boxes to the right of firstParent
                    qreal x = firstParentIt->m_box->pos().x() + firstParentIt->m_box->boundingRect().width() + margin.width();
                    firstParentIt++;
                    for(; firstParentIt != level.end(); firstParentIt++)
                    {
                        auto& box = firstParentIt->m_box;
                        box->setX(x);
                        x += box->boundingRect().width() + margin.width();
                    }
                }
                firstParentIt = secondParentIt;
            }
        }
    }
}

void IncludeDiagramBuilder::moveBoxesToRightRecursively(IncludeTreeDiagram& levels, size_t levelIndex, size_t from, qreal moveBy)
{
    auto& level = levels[levelIndex];
    auto nextLevelIndex = levelIndex + 1;
    bool hasMoreLevels = levels.size() > nextLevelIndex;
    int firstChildIndex = -1;
    for(auto i = from; i < level.size(); i++)
    {
        BoxDGI* b = level[i].m_box;
        b->setX(b->x() + moveBy);

        if(hasMoreLevels && firstChildIndex < 0 && b->getChildren().size() > 0)
        {
            auto& firstChild = b->getChildren().begin();
                
            auto& nextLevel = levels[nextLevelIndex];
            for(size_t index = 0; index < nextLevel.size(); index++)
            {
                auto& bb = nextLevel[index];
                if(bb.m_box == *firstChild)
                {
                    firstChildIndex = index;
                    break;
                }
            }
        }
    }

    if(firstChildIndex >= 0)
    {
        moveBoxesToRightRecursively(levels, nextLevelIndex, firstChildIndex, moveBy);
    }
}

std::vector<QSizeF> IncludeDiagramBuilder::calculateLevelSizes(IncludeTreeDiagram& levels)
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

void buildIncludeTreeDiagram(IncludeDiagramView& diagram, cm::IncludeTree& tree)
{
    IncludeDiagramBuilder builder;
    builder.build(diagram, tree);
}
