#include "diagramview.h"

#include <QGridLayout>
#include <QGraphicsView>
#include <QGraphicsScene>

#include "mainwindow.h"


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