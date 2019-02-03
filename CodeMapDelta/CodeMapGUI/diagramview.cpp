#include "diagramview.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>

#include "mainwindow.h"

class BoxDGI : public QGraphicsItem
{
    QString m_name;
public:
    BoxDGI(const QString& name, QGraphicsItem* parent = nullptr) : QGraphicsItem(parent), m_name(name) 
    {
        setFlags(QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    }

protected:
    void paint(QPainter *painter, 
        const QStyleOptionGraphicsItem *option, 
        QWidget *widget = nullptr) override
    {
        painter->setBrush(QBrush(QColor(200, 200, 250)));
        if(isSelected())
            painter->setBrush(QBrush(QColor(200, 250, 250)));
        painter->drawRoundedRect(0, 0, 50, 50, 10, 10);
        painter->drawText(QRect(20, 10, 30, 40), m_name);
    }

    QRectF boundingRect() const override
    {
        return QRectF(0, 0, 50, 50);
    }
};

DiagramView::DiagramView(QWidget *parent) : QWidget(parent)
{
    m_idLabel = new QLabel("TEST_Diagram");
    setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    m_layout = new QGridLayout(this);
    m_layout->setMargin(0);
    m_layout->setSpacing(0);

    QToolBar* toolbar = createToolbar();
    m_layout->addWidget(toolbar, 0, 0, 1, -1);

    m_view = new QGraphicsView(this);
    m_layout->addWidget(m_view, 1, 0, 1, -1);

    m_scene = new QGraphicsScene(this);
    m_scene->setBackgroundBrush(Qt::gray);
    m_view->setScene(m_scene);

    // lefttop is x-y-, middle is x0y0, rightbot is x+y+
    m_scene->addItem(new BoxDGI("asd"));
    m_scene->addItem(new BoxDGI("<valami.h>"));

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