#pragma once

#include <QGraphicsItem>

#include "DiagramView.h"

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
    QString m_displayName;
    QString m_fullName;
    QFont m_font;

public:
    explicit BoxDGI(const std::string& displayName, const std::string& fullName, QGraphicsItem* parent = nullptr);
    BoxDGI(const QString& displayName, const QString& fullName, QGraphicsItem* parent = nullptr);
    QRectF boundingRect() const override;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};


class IncludeDiagramView : public DiagramView
{
    Q_OBJECT
public:
    explicit IncludeDiagramView(QWidget *parent = nullptr);

};
