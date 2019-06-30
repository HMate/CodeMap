#include "IncludeTreeDiagramBuilder.h"

#include "IncludeTree.h"
#include "IncludeDiagramView.h"


void IncludeDiagramBuilder::build(IncludeDiagramView& diagram)
{
    cm::IncludeTree& tree = diagram.getIncludeTree();

    // lefttop is x-y-, middle is x0y0, rightbot is x+y+
    QGraphicsScene& scene = *diagram.getScene();
    m_levels = std::make_unique<IncludeTreeDiagram>();

    diagram.setUpdatesEnabled(false);

    scene.clear();
    auto& box = addRootBox(diagram, scene, tree);
    recursiveBuildIncludeTreeLevel(diagram, scene, box, 1);

    diagram.setDiagram(m_levels);
    diagram.setUpdatesEnabled(true);
    diagram.update();
}

BoxBuilder IncludeDiagramBuilder::addRootBox(IncludeDiagramView& diagram, QGraphicsScene& scene, cm::IncludeTree& tree)
{
    auto& current = tree.root();

    auto box = new BoxDGI(diagram, current);
    scene.addItem(box);

    BoxBuilder bb{ current, box };
    IncludeDiagramBuilderLevel levelBoxes(bb);
    levelBoxes.emplace_back(bb);
    auto rect = box->boundingRect();

    m_levels->addLevel(levelBoxes);
    return bb;
}
    
void IncludeDiagramBuilder::recursiveBuildIncludeTreeLevel(IncludeDiagramView& diagram, QGraphicsScene& scene, const BoxBuilder& current, int currentLevel)
{
    IncludeDiagramBuilderLevel levelBoxes(current);

    auto& includes = current.m_node.includes();
    for(auto& inc : includes)
    {
        auto box = new BoxDGI(diagram, inc);
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

void buildIncludeTreeDiagram(IncludeDiagramView& diagram, std::shared_ptr<cm::IncludeTree> tree)
{
    IncludeDiagramBuilder builder;
    diagram.setIncludeTree(tree);
    builder.build(diagram);
}
