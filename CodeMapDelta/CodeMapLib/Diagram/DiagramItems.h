#ifndef DIAGRAMITEMS_H
#define DIAGRAMITEMS_H

#include <QGraphicsItem>
#include <QFont>
#include "IncludeTree.h"

class IncludeDiagramView;

class ArrowDGI : public QGraphicsItem
{
    QGraphicsItem* m_startItem;
    QGraphicsItem* m_endItem;

public:
    ArrowDGI(QGraphicsItem* startItem, QGraphicsItem* endItem, QGraphicsItem* parent = nullptr);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QRectF boundingRect() const override;

    QPointF startPoint() const;
    QPointF endPoint() const;
};

// BoxDiagramGraphicsItem
class BoxDGI : public QGraphicsItem
{
    IncludeDiagramView& m_parentView;
    const cm::IncludeNode& m_node;
    std::vector<BoxDGI*> m_children;
    QFont m_font;

public:
    explicit BoxDGI(IncludeDiagramView& parentView, const cm::IncludeNode& node, QGraphicsItem* parent = nullptr);
    QRectF boundingRect() const override;

    void addChild(BoxDGI* child) { m_children.emplace_back(child); }
    const std::vector<BoxDGI*>& getChildren() { return m_children; }
    int32_t getNodeId() const { return m_node.id(); }
    QString getDisplayName() const { return QString::fromStdString(m_node.name()); }
    QString getFullName() const { return QString::fromStdString(m_node.path()); }
    const std::vector<cm::IncludeNode>& getChildrenNodes() const { return m_node.includes(); }
    bool isFullInclude() const { return m_node.isFullInclude(); }

    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};


#endif // DIAGRAMITEMS_H
