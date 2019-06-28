#include "DiagramSerializer.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

struct IncludeDiagramNode
{
    int m_id;
    QString m_name;
    QString m_path;
};

QString DiagramSerializer::serialize(const IncludeTreeDiagram& diagram)
{
    QJsonObject jsonRoot{ {"type", "IncludeDiagram"}, {"version", "1.0"} };
    QJsonObject diagramJson;
    QJsonArray nodesJson;

    std::map<QString, IncludeDiagramNode> nodes;
    int index = 0;
    for(auto& level : diagram)
    {
        for(auto& node : level)
        {
            IncludeDiagramNode idn{index, node.m_box->getDisplayName(), node.m_box->getFullName() };
            nodesJson.append(QJsonObject{ {"id", index}, {"name", idn.m_name}, {"path", idn.m_path} });
            nodes.insert_or_assign(idn.m_path, idn);
            ++index;
        }
    }
    diagramJson.insert("nodes", nodesJson);

    QJsonObject edgesJson;
    index = 0;
    for (auto& level : diagram)
    {
        for (auto& node : level)
        {
            auto& idn = nodes[node.m_box->getFullName()];
            QJsonArray edgeList;
            auto& children = node.m_box->getChildren();
            if (children.size() > 0)
            {
                for (auto& child : children)
                {
                    edgeList.append(nodes[child->getFullName()].m_id);
                }
                edgesJson.insert(QString::number(idn.m_id), edgeList);
            }
            ++index;
        }
    }
    diagramJson.insert("edges", edgesJson);

    jsonRoot.insert("diagram", diagramJson);

    QJsonDocument doc(jsonRoot);
    
    const QString serialized = doc.toJson();
    return serialized;
}