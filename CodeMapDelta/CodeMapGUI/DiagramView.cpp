#include "DiagramView.h"

#include <QGridLayout>
#include <QGraphicsScene>

#include <qmath.h>
#include <QMouseEvent>

#include "MainWindow.h"


DiagramView::DiagramView(QWidget *parent) : QWidget(parent)
{
    m_idLabel = new QLabel("TEST_Diagram");
    setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    m_layout = new QGridLayout(this);
    m_layout->setMargin(0);
    m_layout->setSpacing(0);

    QToolBar* toolbar = createToolbar();
    m_layout->addWidget(toolbar, 0, 0, 1, -1);

    m_view = new DiagramGraphicsView(this);
    m_layout->addWidget(m_view, 1, 0, 1, -1);

    m_view->viewport()->installEventFilter(this);
    m_view->setDragMode(QGraphicsView::DragMode::ScrollHandDrag);

    m_scene = new QGraphicsScene(this);
    m_scene->setBackgroundBrush(Qt::gray);
    m_view->setScene(m_scene);
}

void DiagramView::setId(const QString id)
{
    m_id = id;
    m_idLabel->setText(m_id);
}

QToolBar* DiagramView::createToolbar()
{
    QToolBar* toolbar = new QToolBar("DiagramView", this);
    toolbar->addWidget(m_idLabel);
    QWidget *spacerWidget = new QWidget(this);
    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    toolbar->addWidget(spacerWidget);
    toolbar->addAction("X", [this]() {
        MainWindow::instance()->getDocumentManager()->closeDiagramView(this);
    });

    toolbar->setFixedHeight(20);

    return toolbar;
}

bool DiagramView::eventFilter(QObject *object, QEvent *ev)
{
    auto type = ev->type();
    if(ev->type() == QEvent::MouseMove) {
        QMouseEvent* mouse_event = static_cast<QMouseEvent*>(ev);
        QPointF delta = m_targetViewportPos - mouse_event->pos();
        if(qAbs(delta.x()) > 5 || qAbs(delta.y()) > 5) {
            m_targetViewportPos = mouse_event->pos();
            m_targetScenePos = m_view->mapToScene(mouse_event->pos());
        }
    }
    else if(type == QEvent::Wheel)
    {
        QWheelEvent* wheel_event = static_cast<QWheelEvent*>(ev);
        if(wheel_event->orientation() == Qt::Vertical) {
            double angle = wheel_event->angleDelta().y();
            double factor = qPow(1.0015, angle);
            //gentle_zoom(factor);

            m_view->scale(factor, factor);
            m_view->centerOn(m_targetScenePos);
            QPointF delta_viewport_pos = m_targetViewportPos - QPointF(m_view->viewport()->width() / 2.0,
                m_view->viewport()->height() / 2.0);
            QPointF viewport_center = m_view->mapFromScene(m_targetScenePos) - delta_viewport_pos;
            m_view->centerOn(m_view->mapToScene(viewport_center.toPoint()));

            return true;
        }
    }
    return false;
}

void DiagramGraphicsView::enterEvent(QEvent *event)
{
    QGraphicsView::enterEvent(event);
    viewport()->setCursor(Qt::ArrowCursor);
}

void DiagramGraphicsView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
    viewport()->setCursor(Qt::ArrowCursor);
}

void DiagramGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
    viewport()->setCursor(Qt::ArrowCursor);
}