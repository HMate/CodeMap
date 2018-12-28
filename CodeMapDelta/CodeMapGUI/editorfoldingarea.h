#ifndef EDITORFOLDINGAREA_H
#define EDITORFOLDINGAREA_H

#include <vector>

#include <QWidget>
#include <QTextBlock>

#include "textfolder.h"

class FileView;
class EditorFoldingButton;


// Folding button hierarchy is an n-ary tree with nodes containing EditorFoldingButtons
// The root node is special as its not a folding button itself.
class EditorFoldingButtonHierarchyNode
{
protected:
    std::vector<EditorFoldingButtonHierarchyNode> m_nodes;
    EditorFoldingButton* m_foldingButton = nullptr;

    void addButton(EditorFoldingButton* button);

    bool isNodeFor(EditorFoldingButton* button);


public:
    EditorFoldingButtonHierarchyNode(EditorFoldingButton* button);

    EditorFoldingButtonHierarchyNode* findNode(EditorFoldingButton*);
    EditorFoldingButton* getButton() { return m_foldingButton; }
    std::vector<EditorFoldingButtonHierarchyNode>& getChildNodes();
    bool doAnyChildContainMouse() const;
    bool canFoldLine(int lineNumber) const;
    bool isSectionInvalid(int firstLine, int liastLine) const;
    void correctVisualOrder() const;
};

class EditorFoldingButtonHierarchy : public EditorFoldingButtonHierarchyNode
{
public:
    EditorFoldingButtonHierarchy();
    void addButton(EditorFoldingButton* button);
    bool isLowestButtonThatContainsMouse(EditorFoldingButton* button);

    void correctVisualOrder() const;
};


class EditorFoldingArea : public QWidget
{
    Q_OBJECT
    FileView *m_view;
    std::vector<EditorFoldingButton*> m_foldingButtons;
    EditorFoldingButtonHierarchy m_foldingHierarchy;

    QSize m_lastSize;
public:
    explicit EditorFoldingArea(FileView *parent);

    QSize sizeHint() const override;
    int calculateWidth() const;

    EditorFoldingButton& addFoldingButton(int firstLine, int lastLine);
    std::vector<EditorFoldingButton*> getFoldingButtons() const { return m_foldingButtons; }

public slots:
    void updateArea(const QRect &rect, int dy);
    void updateSize();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *e) override;
    void updateButtonGeometries();
    void setFoldingButtonGeometry(EditorFoldingButton& fb);
};

class EditorFoldingButton : public QWidget
{
    Q_OBJECT
    QTextBlock m_firstBlock;
    QTextBlock m_lastBlock;
    const int m_startLine;
    const int m_endLine;

    bool m_collapsed = false;
    bool m_containsMouse = false;

    EditorFoldingButtonHierarchy& m_hierarchy;
    FileView *m_view;
public:

    EditorFoldingButton(QWidget* parent, FileView *view, EditorFoldingButtonHierarchy& hierarchy, int firstLine, int lastLine);

    QTextBlock getFirstLineBlock() const;
    QTextBlock getLastVisibleBlock() const;

    int getFirstLine() const { return m_startLine; }
    int getLastLine() const { return m_endLine; }
    QString getReplacementText() const;

    bool isCollapsed() const { return m_collapsed; }
    bool isContainingMouse() const { return m_containsMouse; }

    void setContainsMouse(bool);
    QSize sizeHint() const override;

    void fold();
    void unfold();

signals:
    void changedSize();

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // EDITORFOLDINGAREA_H