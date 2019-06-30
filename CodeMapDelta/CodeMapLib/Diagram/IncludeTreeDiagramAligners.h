#ifndef INCLUDETREEDIAGRAMALIGNERS_H
#define INCLUDETREEDIAGRAMALIGNERS_H

#include "IncludeTreeDiagram.h"

/// Align the boxes of levels to not overlap, and to be centered relative to the prev level.
class CenterDiagramAligner
{
public:
    static void alignDiagram(IncludeDiagramView& diagram);
private:
    static std::vector<QSizeF> calculateLevelSizes(IncludeTreeDiagram& levels);
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
};

#endif //INCLUDETREEDIAGRAMALIGNERS_H