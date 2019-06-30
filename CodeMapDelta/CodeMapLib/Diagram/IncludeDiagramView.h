#ifndef INCLUDEDIAGRAMVIEW_H
#define INCLUDEDIAGRAMVIEW_H

#include <QHash>
#include <QGroupBox>
#include <QCheckBox>

#include "DiagramView.h"
#include "DiagramItems.h"
#include "IncludeTreeDiagram.h"


// TODO:
// -Save, open diagram
// -Open code file on double click / right click menu
// -Gui: 
//  - show/hide include guarded files
//  - Filter boxes by path
//  - color legend
//  - statistics: how many includes total / unique includes / child of selected / tree of selected
class IncludeDiagramView : public DiagramView
{
    Q_OBJECT

    QHash<QString, int> m_selectedIDs;
    std::shared_ptr<cm::IncludeTree> m_tree;
    std::unique_ptr<IncludeTreeDiagram> m_diagram = nullptr;

    QGroupBox* m_box;
    QCheckBox* m_label;
    QCheckBox* m_check;

    // can select mltiple boxes without holding ctrl
    bool m_boxSelectionMode = false;
public:
    explicit IncludeDiagramView(QWidget *parent = nullptr);

    cm::IncludeTree& getIncludeTree();
    void setIncludeTree(const std::shared_ptr<cm::IncludeTree>& tree);

    IncludeTreeDiagram& getDiagram();
    void setDiagram(std::unique_ptr<IncludeTreeDiagram>& diagram);

    void clearSelectedID(const QString& id);
    void setSelectedID(const QString& id);
    bool isBoxSelectedWithID(const QString& id);

    bool getBoxSelectionMode() const;

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

public slots:
    void setBoxSelectionMode(bool toggleOn);
};

#endif // INCLUDEDIAGRAMVIEW_H