#ifndef INCLUDETREEDIAGRAMALIGNERS_H
#define INCLUDETREEDIAGRAMALIGNERS_H

#include "IncludeTreeDiagram.h"

enum class DiagramAlignment
{
    Center,
    Grouped,
    Graph
};

/// Align the boxes of levels to not overlap, and to be centered along a common axis.
class CenterDiagramAligner
{
public:
    static void alignDiagram(IncludeDiagramView& diagram);
private:
    static std::vector<QSizeF> calculateLevelSizes(IncludeTreeDiagram& levels);
    static void placeBoxes(IncludeTreeDiagram& levels, const std::vector<QSizeF>& levelSizes);
};

/// Align the boxes of levels to not overlap.
/// Do this by making groups out of boxes who have the same parent. These groups are centered to their parent.
/// But the space between group elements is based on how many child they have.
class GroupDiagramAligner
{
public:
    static void alignDiagram(IncludeDiagramView& diagram);
private:
    static void placeBoxes(IncludeTreeDiagram& levels);
    static void moveBoxesToRightRecursively(IncludeTreeDiagram& levels, size_t levelIndex, size_t from, qreal moveBy);
    static void moveBoxesToScreen(IncludeTreeDiagram& levels);
};

/// Align the boxes as a directed graph with cycles.
/// A node will be placed on a level where every parent is above it.
class GraphDiagramAligner
{
public:
    static void alignDiagram(IncludeDiagramView& diagram);
    static void placeBoxes(IncludeTreeDiagram& levels);
};

#endif //INCLUDETREEDIAGRAMALIGNERS_H