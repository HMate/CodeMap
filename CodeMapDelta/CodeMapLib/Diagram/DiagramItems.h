#ifndef DIAGRAMITEMS_H
#define DIAGRAMITEMS_H

#include <QGraphicsItem>
#include <QFont>

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
    std::vector<BoxDGI*> m_children;
    QString m_displayName;
    QString m_fullName;
    QFont m_font;
    bool m_fullInclude = false;

public:
    explicit BoxDGI(IncludeDiagramView& parentView, const std::string& displayName, const std::string& fullName, QGraphicsItem* parent = nullptr);
    BoxDGI(IncludeDiagramView& parentView, const QString& displayName, const QString& fullName, QGraphicsItem* parent = nullptr);
    void setFullInclude(bool fullInclude) { m_fullInclude = fullInclude; }
    QRectF boundingRect() const override;

    void addChild(BoxDGI* child) { m_children.emplace_back(child); }
    const std::vector<BoxDGI*>& getChildren() { return m_children; }
    QString getFullName() { return m_fullName; }

    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};


#endif // DIAGRAMITEMS_H
