#include "IncludeDiagramView.h"

#include <QTime>
#include <QDebug>

#include <QVBoxLayout>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QStyleOptionGraphicsItem>
#include <Diagram\IncludeTreeDiagramBuilder.h>

IncludeDiagramView::IncludeDiagramView(QWidget* parent) : DiagramView(parent)
{
    m_box = new QGroupBox("", this);
    m_box->setObjectName("LegendUi");
    QVBoxLayout *boxLayout = new QVBoxLayout();
    m_box->setLayout(boxLayout);

    m_btnSwitchGroupSelection = new QCheckBox(QStringLiteral("Ctrl - group select"), this);
    connect(m_btnSwitchGroupSelection, &QCheckBox::toggled, this, &IncludeDiagramView::setBoxSelectionMode);
    boxLayout->addWidget(m_btnSwitchGroupSelection);

    m_btnChangeBoxAlignment = new QCheckBox(QStringLiteral("Center aligned boxes"), this);
    connect(m_btnChangeBoxAlignment, &QCheckBox::toggled, this, &IncludeDiagramView::toggleDiagramAlign);
    boxLayout->addWidget(m_btnChangeBoxAlignment);

    m_btnShowDuplicateNodes = new QCheckBox(QStringLiteral("Showing tree graph"), this);
    connect(m_btnShowDuplicateNodes, &QCheckBox::toggled, this, &IncludeDiagramView::toggleDiagramType);
    boxLayout->addWidget(m_btnShowDuplicateNodes);

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
        setDiagramAlignment(DiagramAlignment::Grouped);
    }
    else
    {
        setDiagramAlignment(DiagramAlignment::Center);
    }
}

void IncludeDiagramView::setDiagramAlignment(DiagramAlignment alignment)
{
    m_boxAlignment = alignment;
    if (m_boxAlignment == DiagramAlignment::Grouped)
    {
        GroupDiagramAligner aligner;
        aligner.alignDiagram(*this);
        m_btnChangeBoxAlignment->setText(QStringLiteral("Group aligned boxes"));
    }
    else if (m_boxAlignment == DiagramAlignment::Center)
    {
        CenterDiagramAligner aligner;
        aligner.alignDiagram(*this);
        m_btnChangeBoxAlignment->setText(QStringLiteral("Center aligned boxes"));
    }
    else if (m_boxAlignment == DiagramAlignment::Graph)
    {
        GraphDiagramAligner aligner;
        aligner.alignDiagram(*this);
        m_btnChangeBoxAlignment->setText(QStringLiteral("Graph aligned boxes"));
    }
}

void IncludeDiagramView::toggleDiagramType(bool showSinglesOnly)
{
    // TODO: This needs:
    // - new group alignment, current wont work i think
    // - figure out how to handle the levels for circular includes
    if (showSinglesOnly)
    {
        setDiagramType(IncludeDiagramType::UniqueNodes);
    }
    else
    {
        setDiagramType(IncludeDiagramType::DuplicatedNodes);
    }
}

void IncludeDiagramView::setDiagramType(IncludeDiagramType diagramType)
{
    m_diagramType = diagramType;
    if (m_diagramType == IncludeDiagramType::DuplicatedNodes)
    {
        IncludeDiagramBuilder builder;
        builder.buildTree(*this);
        setDiagramAlignment(DiagramAlignment::Center);
        m_btnChangeBoxAlignment->setDisabled(false);
    }
    else
    {
        IncludeDiagramBuilder builder;
        builder.buildGraph(*this);
        setDiagramAlignment(DiagramAlignment::Center);
        m_btnChangeBoxAlignment->setDisabled(true);
    }
}

DiagramAlignment IncludeDiagramView::getDiagramAlignment()
{
    return m_boxAlignment;
}

IncludeDiagramType IncludeDiagramView::getDiagramType()
{
    return m_diagramType;
}

void IncludeDiagramView::addSkirtToScene()
{
    qreal skirtSize = 1000.0;
    auto& rect = getScene()->sceneRect();
    rect.adjust(-skirtSize, -skirtSize, skirtSize, skirtSize);
    getScene()->setSceneRect(rect);
}
