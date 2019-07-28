#include "IncludeTreeDiagramBuilder.h"

#include <set>

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

    BoxBuilder bb{ box };
    IncludeDiagramBuilderLevel levelBoxes;
    levelBoxes.emplace_back(bb);
    auto rect = box->boundingRect();

    m_levels->addLevel(levelBoxes);
    return bb;
}
    
void IncludeDiagramBuilder::recursiveBuildIncludeTreeLevel(IncludeDiagramView& diagram, QGraphicsScene& scene, const BoxBuilder& current, int currentLevel)
{
    IncludeDiagramBuilderLevel levelBoxes;

    auto& includes = current.m_box->getChildrenNodes();
    for(auto& inc : includes)
    {
        auto box = new BoxDGI(diagram, inc);
        scene.addItem(box);
        current.m_box->addChild(box);
        BoxBuilder bb{ box };
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

class GraphBuilder
{
    std::set<cm::IncludeNode*> m_nodes;
    std::set<cm::IncludeNode*> m_missingNodes;
public:
    GraphBuilder() {}
    void collectEveryNode(cm::IncludeTree& tree)
    {
        cm::IncludeNode& current = tree.root();
        m_nodes.emplace(&current);
        m_missingNodes.emplace(&current);

        collectChildNodes(current);
    }

    void collectChildNodes(cm::IncludeNode& node)
    {
        auto& includes = node.includes();
        for (auto& inc : includes)
        {
            // ugh this deep first search relies on the fact that that the first occurence of 
            // a node it will find is usually the fullInclude. But this is a very fragile assumption.
            if (!nodesContain(inc))
            {
                m_nodes.emplace(&inc);
                m_missingNodes.emplace(&inc);
                collectChildNodes(inc);
            }
        }
    }

    bool nodesContain(cm::IncludeNode& node)
    {
        return m_nodes.find(&node) != m_nodes.end();
    }

    void placeBoxesOnLevels(IncludeDiagramView& diagram, QGraphicsScene& scene, IncludeTreeDiagram& levels)
    {
        while (m_missingNodes.size() != 0)
        {
            IncludeDiagramBuilderLevel levelBoxes;
            for (auto& node : m_missingNodes)
            {
                int parents = countParents(*node);
                if (parents == 0)
                {
                    auto box = new BoxDGI(diagram, *node);
                    scene.addItem(box);
                    BoxBuilder bb{ box };
                    levelBoxes.emplace_back(bb);
                }
            }

            if (levelBoxes.size() == 0 && m_missingNodes.size() != 0)
            {
                // we have a circle on this level, have to find a node with the least dependencies
                int minParents = -1;
                cm::IncludeNode* minNode = *m_missingNodes.begin();
                for (auto& node : m_missingNodes)
                {
                    int parents = countParents(*node);
                    if (minParents == -1 || minParents > parents)
                    {
                        minParents = parents;
                        minNode = node;
                    }
                }

                auto box = new BoxDGI(diagram, *minNode);
                scene.addItem(box);
                BoxBuilder bb{ box };
                levelBoxes.emplace_back(bb);
            }

            for (auto& box : levelBoxes)
            {
                auto& elem = std::find(m_missingNodes.begin(), m_missingNodes.end(), &box.m_box->getNode());
                m_missingNodes.erase(elem);
            }

            levels.addLevel(levelBoxes);
        }
    }

    int countParents(const cm::IncludeNode& node)
    {
        int parents = 0;
        for (auto& other : m_missingNodes)
        {
            if (node == *other)
                continue;
            for (auto& child : other->includes())
            {
                if (child == node)
                {
                    parents++;
                }
            }
        }
        return parents;
    }

    void placeArrows(IncludeDiagramView& diagram, QGraphicsScene& scene, IncludeTreeDiagram& levels)
    {
        for (auto& level : levels)
        {
            for (auto& node : level)
            {
                const std::vector<cm::IncludeNode>& includes = node.m_box->getChildrenNodes();
                for (auto& inc : includes)
                {
                    auto& hasBox = levels.tryGetBoxWithPath(inc.path());
                    Q_ASSERT(hasBox.first);
                    
                    BoxBuilder* box = hasBox.second;
                    node.m_box->addChild(box->m_box);
                    scene.addItem(new ArrowDGI(node.m_box, box->m_box));
                    
                }
            }
        }
    }
};

void IncludeDiagramBuilder::buildGraph(IncludeDiagramView& diagram)
{
    cm::IncludeTree& tree = diagram.getIncludeTree();
    QGraphicsScene& scene = *diagram.getScene();
    m_levels = std::make_unique<IncludeTreeDiagram>();
    diagram.setUpdatesEnabled(false);
    scene.clear();

    GraphBuilder graphBuilder;
    graphBuilder.collectEveryNode(tree);
    graphBuilder.placeBoxesOnLevels(diagram, scene, *m_levels);
    graphBuilder.placeArrows(diagram, scene, *m_levels);

    diagram.setDiagram(m_levels);
    diagram.setUpdatesEnabled(true);
    diagram.update();
}

void buildIncludeTreeDiagram(IncludeDiagramView& diagram, std::shared_ptr<cm::IncludeTree> tree)
{
    IncludeDiagramBuilder builder;
    diagram.setIncludeTree(tree);
    builder.buildTree(diagram);
}
