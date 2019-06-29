#include "DiagramSerializer.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

struct IncludeDiagramNode
{
    int m_id;
    QString m_name;
    QString m_path;
    bool m_fullInclude;
};

QString DiagramSerializer::serialize(const IncludeTreeDiagram& diagram)
{
    QJsonObject jsonRoot{ {"type", "IncludeDiagram"}, {"version", "1.0"} };
    QJsonObject diagramJson;
    QJsonArray nodesJson;

    // TODO: using a pointer as a map key is gross, figure out a better way
    std::map<BoxDGI*, IncludeDiagramNode> nodes;
    int index = 0;
    for(auto& level : diagram)
    {
        for(auto& node : level)
        {
            auto& box = node.m_box;
            IncludeDiagramNode idn{index, box->getDisplayName(), box->getFullName(), box->isFullInclude() };
            nodesJson.append(QJsonObject{ 
                {"id", index}, 
                {"name", idn.m_name}, 
                {"path", idn.m_path}, 
                {"fullInclude", idn.m_fullInclude} });
            nodes.insert_or_assign(node.m_box, idn);
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
            auto& idn = nodes[node.m_box];
            QJsonArray edgeList;
            auto& children = node.m_box->getChildren();
            if (children.size() > 0)
            {
                for (auto& child : children)
                {
                    edgeList.append(nodes[child].m_id);
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