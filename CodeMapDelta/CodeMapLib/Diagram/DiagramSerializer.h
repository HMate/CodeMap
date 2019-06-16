#ifndef DIAGRAMSERIALIZER_H
#define DIAGRAMSERIALIZER_H

#include <QString>
#include "IncludeTreeDiagram.h"

class DiagramSerializer
{
public:
    static QString serialize(const IncludeTreeDiagram& diagram);
};

#endif // !DIAGRAMSERIALIZER_H
