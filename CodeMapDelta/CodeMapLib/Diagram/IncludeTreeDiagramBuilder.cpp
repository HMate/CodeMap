#include "IncludeTreeDiagramBuilder.h"

#include "IncludeTree.h"
#include "IncludeDiagramView.h"


void IncludeDiagramBuilder::buildTree(IncludeDiagramView& diagram)
{
    cm::IncludeTree& tree = diagram.getIncludeTree();

    QGraphicsScene& scene = *diagram.getScene();
    scene.clear();
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

    m_levels->addBoxesToLevel(currentLevel, levelBoxes);

    Q_ASSERT(includes.size() == levelBoxes.size());
    for (auto& box : levelBoxes)
    {
        recursiveBuildIncludeTreeLevel(diagram, scene, box, currentLevel + 1);
    }
}

void IncludeDiagramBuilder::buildGraph(IncludeDiagramView& diagram)
{
    cm::IncludeTree& tree = diagram.getIncludeTree();
    QGraphicsScene& scene = *diagram.getScene();

    m_levels = std::make_unique<IncludeTreeDiagram>();

    diagram.setUpdatesEnabled(false);

    scene.clear();
    auto& box = addRootBox(diagram, scene, tree);
    recursiveBuildIncludeGraphLevel(diagram, scene, box, 1);

    diagram.setDiagram(m_levels);
    diagram.setUpdatesEnabled(true);
    diagram.update();
}

void IncludeDiagramBuilder::recursiveBuildIncludeGraphLevel(IncludeDiagramView& diagram, QGraphicsScene& scene, const BoxBuilder& current, int currentLevel)
{
    IncludeDiagramBuilderLevel levelBoxes(current);

    auto& includes = current.m_node.includes();
    for (auto& inc : includes)
    {
        auto& hasBox = m_levels->tryGetBoxWithPath(inc.path());
        if (hasBox.first)
        {
            BoxBuilder* box = hasBox.second;
            current.m_box->addChild(box->m_box);
            scene.addItem(new ArrowDGI(current.m_box, box->m_box));
            continue;
        }

        auto box = new BoxDGI(diagram, inc);
        scene.addItem(box);
        current.m_box->addChild(box);
        BoxBuilder bb{ inc, box };
        levelBoxes.emplace_back(bb);

        scene.addItem(new ArrowDGI(current.m_box, box));
    }

    m_levels->addBoxesToLevel(currentLevel, levelBoxes);

    for (auto& box : levelBoxes)
    {
        recursiveBuildIncludeGraphLevel(diagram, scene, box, currentLevel + 1);
    }
}

void buildIncludeTreeDiagram(IncludeDiagramView& diagram, std::shared_ptr<cm::IncludeTree> tree)
{
    IncludeDiagramBuilder builder;
    diagram.setIncludeTree(tree);
    builder.buildTree(diagram);
}
