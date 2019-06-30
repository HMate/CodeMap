#include "IncludeDiagramView.h"

#include <QTime>
#include <QDebug>

#include <QVBoxLayout>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QStyleOptionGraphicsItem>
#include "Diagram/IncludeTreeDiagramAligners.h"

IncludeDiagramView::IncludeDiagramView(QWidget* parent) : DiagramView(parent)
{
    m_box = new QGroupBox("", this);
    m_box->setObjectName("LegendUi");
    QVBoxLayout *boxLayout = new QVBoxLayout();
    m_box->setLayout(boxLayout);

    m_label = new QCheckBox(QStringLiteral("Ctrl - group select"), this);
    connect(m_label, &QCheckBox::toggled, this, &IncludeDiagramView::setBoxSelectionMode);
    boxLayout->addWidget(m_label);

    m_check = new QCheckBox(QStringLiteral("Center aligned boxes"), this);
    connect(m_check, &QCheckBox::toggled, this, &IncludeDiagramView::toggleDiagramAlign);
    boxLayout->addWidget(m_check);

    m_box->setStyleSheet("QGroupBox#LegendUi{ background-color:rgba(150, 100, 100, 230); border: 2px solid rgba(0,0,0,255); }");

    getView()->registerLegendUiWidget(m_box);
}


cm::IncludeTree& IncludeDiagramView::getIncludeTree()
{
    return *m_tree;
}

void IncludeDiagramView::setIncludeTree(const std::shared_ptr<cm::IncludeTree>& tree)
{
    m_tree = tree;
}

IncludeTreeDiagram& IncludeDiagramView::getDiagram()
{
    return *m_diagram;
}

void IncludeDiagramView::setDiagram(std::unique_ptr<IncludeTreeDiagram>& diagram)
{
    m_diagram = std::move(diagram);
}

void IncludeDiagramView::clearSelectedID(const QString& id)
{
    m_selectedIDs[id]--;
}

void IncludeDiagramView::setSelectedID(const QString& id)
{
    m_selectedIDs[id]++;
}

bool IncludeDiagramView::isBoxSelectedWithID(const QString& id)
{
    return m_selectedIDs[id] > 0;
}

void IncludeDiagramView::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control && !getBoxSelectionMode())
    {
        getView()->setDragMode(QGraphicsView::DragMode::RubberBandDrag);
    }
}

void IncludeDiagramView::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control && !getBoxSelectionMode())
    {
        getView()->setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
    }
}

bool IncludeDiagramView::getBoxSelectionMode() const
{
    return m_boxSelectionMode;
}

void IncludeDiagramView::setBoxSelectionMode(bool toggleOn)
{
    m_boxSelectionMode = toggleOn;
    if(toggleOn)
    {
        getView()->setDragMode(QGraphicsView::DragMode::RubberBandDrag);
    }
    else
    {
        getView()->setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
    }
}

void IncludeDiagramView::toggleDiagramAlign(bool setGroupedAlignment)
{
    if (setGroupedAlignment)
    {
        GroupDiagramAligner aligner;
        aligner.alignDiagram(*this);
        m_check->setText(QStringLiteral("Group aligned boxes"));
    }
    else
    {
        CenterDiagramAligner aligner;
        aligner.alignDiagram(*this);
        m_check->setText(QStringLiteral("Center aligned boxes"));
    }
}


