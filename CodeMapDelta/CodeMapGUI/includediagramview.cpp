#include "IncludeDiagramView.h"

#include <QPainter>
#include <QGraphicsScene>

ArrowDGI::ArrowDGI(QGraphicsItem* startItem, QGraphicsItem* endItem, QGraphicsItem* parent)
    : m_startItem(startItem), m_endItem(endItem), QGraphicsItem(parent) {}

void ArrowDGI::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setBrush(Qt::black);

    painter->drawLine(startPoint(), endPoint());
}

QRectF ArrowDGI::boundingRect() const
{
    return QRectF(startPoint(), endPoint());
}

QPointF ArrowDGI::startPoint() const
{
    Q_ASSERT(m_startItem != nullptr);
    auto& rect = m_startItem->boundingRect();
    return QPointF(m_startItem->x() + (rect.width() / 2.0), m_startItem->y() + rect.height());
}

QPointF ArrowDGI::endPoint() const
{
    Q_ASSERT(m_endItem != nullptr);
    auto& rect = m_endItem->boundingRect();
    return QPointF(m_endItem->x() + (rect.width() / 2.0), m_endItem->y());
}

BoxDGI::BoxDGI(const std::string& name, QGraphicsItem* parent) :
    BoxDGI(QString(name.c_str()), parent) {}

BoxDGI::BoxDGI(const QString& name, QGraphicsItem* parent) : QGraphicsItem(parent), m_name(name)
{
    setFlags(QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsMovable | 
        QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsScenePositionChanges);
    m_font = QFont("Helvetica");
}

void BoxDGI::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // TODO: The actually rendered font is not Helvetica. Find out what happens.
    painter->setFont(m_font);
    auto fmetric = painter->fontMetrics();

    painter->setBrush(QBrush(QColor(200, 200, 250)));
    if(isSelected())
        painter->setBrush(QBrush(QColor(200, 250, 250)));

    const int margin = 10;
    const int w = fmetric.width(m_name);
    const int h = fmetric.height();

    auto rectSize = boundingRect();
    painter->drawRect(rectSize);

    auto textMargin = (rectSize.width() - w) / 2.0;
    painter->drawText(textMargin, margin + h, m_name);
}

QRectF BoxDGI::boundingRect() const
{
    QFontMetrics fmetric(m_font);
    const int margin = 10;
    const int w = fmetric.width(m_name);
    const int h = fmetric.height();
    return QRectF(0, 0, 2 * margin + w, 2 * margin + h);
}

QVariant BoxDGI::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if(scene() != nullptr && 
        change == GraphicsItemChange::ItemPositionHasChanged)
        scene()->update(scene()->sceneRect());
    return QGraphicsItem::itemChange(change, value);
}

IncludeDiagramView::IncludeDiagramView(QWidget* parent) : DiagramView(parent)
{

}