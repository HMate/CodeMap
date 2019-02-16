#ifndef INCLUDETREEDIAGRAMBUILDER_H
#define INCLUDETREEDIAGRAMBUILDER_H

#include <QGraphicsScene>

#include "includediagramview.h"

#include "codeparser.h"

void buildIncludeTreeDiagram(QGraphicsScene& scene, cm::IncludeTree& tree);

#endif // !INCLUDETREEDIAGRAMBUILDER_H