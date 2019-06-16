#include "DiagramSerializer.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

QString DiagramSerializer::serialize(const IncludeTreeDiagram& diagram)
{
    QJsonObject jsonRoot{ {"type", "IncludeDiagram"}, {"version", "1.0"} };
    QJsonObject diagramJson;
    QJsonArray nodes;

    int index = 0;
    for(auto& level : diagram)
    {
        for(auto& node : level)
        {
            nodes.append(QJsonObject{ {"id", index}, {"name", node.m_box->getDisplayName()}, {"path", node.m_box->getFullName()} });
            ++index;
        }
    }
    diagramJson.insert("nodes", nodes);
    diagramJson.insert("edges", QJsonObject());

    jsonRoot.insert("diagram", diagramJson);

    QJsonDocument doc(jsonRoot);
    
    const QString serialized = doc.toJson();
    return serialized;
}