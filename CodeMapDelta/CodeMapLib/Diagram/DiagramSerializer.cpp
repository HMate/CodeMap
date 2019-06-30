#include "DiagramSerializer.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

QString DiagramSerializer::serialize(const IncludeTreeDiagram& diagram)
{
    QJsonObject jsonRoot{ {"type", "IncludeDiagram"}, {"version", "1.0"} };
    QJsonObject diagramJson;
    QJsonArray nodesJson;

    for(auto& level : diagram)
    {
        for(auto& node : level)
        {
            auto& box = node.m_box;
            nodesJson.append(QJsonObject{ 
                {"id", static_cast<int>(box->getNodeId())},
                {"name", box->getDisplayName()},
                {"path", box->getFullName()},
                {"fullInclude", box->isFullInclude()},
                {"pos", QStringLiteral("%1;%2").arg(box->pos().x()).arg(box->pos().y()) }
            });
        }
    }
    diagramJson.insert("nodes", nodesJson);

    QJsonObject edgesJson;
    QJsonObject positionsJson;
    for (auto& level : diagram)
    {
        for (auto& node : level)
        {
            QJsonArray edgeList;
            auto& children = node.m_box->getChildren();
            if (children.size() > 0)
            {
                for (auto& child : children)
                {
                    edgeList.append(static_cast<int>(child->getNodeId()));
                }
                edgesJson.insert(QString::number(node.m_box->getNodeId()), edgeList);
            }
        }
    }
    diagramJson.insert("edges", edgesJson);

    jsonRoot.insert("diagram", diagramJson);

    QJsonDocument doc(jsonRoot);
    
    const QString serialized = doc.toJson();
    return serialized;
}