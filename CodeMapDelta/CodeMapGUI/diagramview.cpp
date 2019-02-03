#include "diagramview.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>

#include "mainwindow.h"

class BoxDGI : public QGraphicsItem
{
    QString m_name;
    QFont m_font;

public:
    BoxDGI(const QString& name, QGraphicsItem* parent = nullptr) : QGraphicsItem(parent), m_name(name) 
    {
        setFlags(QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
        m_font = QFont("Helvetica");
    }

protected:
    void paint(QPainter *painter, 
        const QStyleOptionGraphicsItem *option, 
        QWidget *widget = nullptr) override
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
        painter->drawText(textMargin, margin+h, m_name);
    }

    QRectF boundingRect() const override
    {
        QFontMetrics fmetric(m_font);
        const int margin = 10;
        const int w = fmetric.width(m_name);
        const int h = fmetric.height();
        return QRectF(0, 0, 2 * margin + w, 2 * margin + h);
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