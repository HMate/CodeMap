#include "DiagramItems.h"

#include <QDebug>
#include <QPainter>

#include "IncludeDiagramView.h"
#include <QGraphicsSceneHoverEvent>

ArrowDGI::ArrowDGI(QGraphicsItem* startItem, QGraphicsItem* endItem, QGraphicsItem* parent)
    : m_startItem(startItem), m_endItem(endItem), QGraphicsItem(parent) {}

void ArrowDGI::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setBrush(Qt::black);
    painter->drawLine(startPoint(), endPoint());
}

QRectF ArrowDGI::boundingRect() const
{
    // Adjust rect so width is positive, and x() is left side, y() is top side
    // otherwise the arrow dissappears in some circumstances
    QRectF r = QRectF(startPoint(), endPoint());
    if(r.width() < 0)
    {
        r.setWidth(-r.width());
        r.setX(r.x() - r.width());
    }
    return r;
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

BoxDGI::BoxDGI(IncludeDiagramView& parentView, const std::string& displayName, const std::string& fullName, QGraphicsItem* parent)
    : BoxDGI(parentView, QString(displayName.c_str()), QString(fullName.c_str()), parent) {}

BoxDGI::BoxDGI(IncludeDiagramView& parentView, const QString& displayName, const QString& fullName, QGraphicsItem* parent)
    : QGraphicsItem(parent), m_parentView(parentView), m_displayName(displayName), m_fullName(fullName)
{
    setFlags(QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsMovable |
        QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsScenePositionChanges);
    m_font = QFont("Helvetica");
    setAcceptHoverEvents(true);
}

void BoxDGI::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setBrush(QBrush(QColor(200, 200, 250)));
    if(isSelected())
    {
        painter->setBrush(QBrush(QColor(100, 250, 250)));
    }
    else if(m_parentView.isBoxSelectedWithID(m_fullName))
    {
        painter->setBrush(QBrush(QColor(200, 250, 250)));
    }
    else if(m_fullInclude)
    {
        painter->setBrush(QBrush(QColor(160, 160, 250)));
    }

    auto rectSize = boundingRect();
    painter->drawRect(rectSize);

    // TODO: The actually rendered font is not Helvetica. Find out what happens.
    painter->setFont(m_font);

    auto fmetric = painter->fontMetrics();
    const int w = fmetric.width(m_displayName);
    const int h = fmetric.height();
    auto textMargin = (rectSize.width() - w) / 2.0;
    const int margin = 10;
    painter->drawText(textMargin, margin + h, m_displayName);
}

QRectF BoxDGI::boundingRect() const
{
    QFontMetrics fmetric(m_font);
    const int margin = 10;
    const int w = fmetric.width(m_displayName);
    const int h = fmetric.height();
    return QRectF(0, 0, 2 * margin + w, 2 * margin + h);
}

QVariant BoxDGI::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{

    if(change == GraphicsItemChange::ItemSelectedHasChanged)
    {
        if(isSelected())
            m_parentView.setSelectedID(m_fullName);
        else
            m_parentView.clearSelectedID(m_fullName);
    }

    QVariant result = QGraphicsItem::itemChange(change, value);

    // update for 2 purpose:
    // 1) if item position change, have to update arrows
    // 2) if selection changed, have to update boxes, because their color could have changed
    if(scene() != nullptr && m_parentView.updatesEnabled() &&
        (change == GraphicsItemChange::ItemSelectedHasChanged ||
            change == GraphicsItemChange::ItemPositionHasChanged))
    {
        scene()->update();
    }
    return result;
}

void BoxDGI::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    if(event->type() == QEvent::GraphicsSceneHoverEnter || event->type() == QEvent::GraphicsSceneHoverMove)
    {
#if !defined(CM_DEBUG)
        setToolTip(m_fullName);
#else
        setToolTip(m_fullName + QStringLiteral(" (x=%1, y=%2)").arg(this->pos().x()).arg(this->pos().y()));
#endif
    }
}