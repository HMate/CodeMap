#pragma once

#include <QGraphicsItem>

#include "diagramview.h"

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
    QString m_name;
    QFont m_font;

public:
    BoxDGI(const QString& name, QGraphicsItem* parent = nullptr);
    explicit BoxDGI(const std::string& name, QGraphicsItem* parent = nullptr);
    QRectF boundingRect() const override;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
};


class IncludeDiagramView : public DiagramView
{
    Q_OBJECT
public:
    explicit IncludeDiagramView(QWidget *parent = nullptr);

};
