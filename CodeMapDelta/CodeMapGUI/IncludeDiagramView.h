#pragma once

#include <QGraphicsItem>

#include "DiagramView.h"

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
    QString m_displayName;
    QString m_fullName;
    QFont m_font;
    bool m_fullInclude = false;

public:
    explicit BoxDGI(IncludeDiagramView& parentView, const std::string& displayName, const std::string& fullName, QGraphicsItem* parent = nullptr);
    BoxDGI(IncludeDiagramView& parentView, const QString& displayName, const QString& fullName, QGraphicsItem* parent = nullptr);
    void setFullInclude(bool fullInclude) { m_fullInclude = fullInclude; }
    QRectF boundingRect() const override;
    QString getFullName() { return m_fullName; }

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};


class IncludeDiagramView : public DiagramView
{
    QString m_selectedID;
    Q_OBJECT
public:
    explicit IncludeDiagramView(QWidget *parent = nullptr);

    void clearSelectedID();
    void setSelectedID(const QString& id);
    bool isBoxSelectedWithID(QString& id);
};
