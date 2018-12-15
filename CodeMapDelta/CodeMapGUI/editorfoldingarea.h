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
    std::vector<EditorFoldingButtonHierarchyNode> m_childNodes;
    EditorFoldingButton* m_foldingButton;

    EditorFoldingButtonHierarchyNode(EditorFoldingButton* button);
    EditorFoldingButtonHierarchyNode() = delete;

    bool canContainLine(int lineNumber);
    void addChildNode(EditorFoldingButton* button);

    friend class EditorFoldingButtonHierarchy;
};

class EditorFoldingButtonHierarchy
{
    std::vector<EditorFoldingButtonHierarchyNode> m_topNodes;
public:
    void addButton(EditorFoldingButton*);
    //std::vector<EditorFoldingButton*> enumerate() const;
};


class EditorFoldingArea : public QWidget
{
    Q_OBJECT
    FileView *m_view;
    std::vector<EditorFoldingButton*> m_foldingButtons;
    EditorFoldingButtonHierarchy m_foldingHierarchy;
public:
    explicit EditorFoldingArea(FileView *parent);

    QSize sizeHint() const override;
    int calculateWidth() const;

    void addFoldingButton(int firstLine, int lastLine);
    std::vector<EditorFoldingButton*> getFoldingButtons() const { return m_foldingButtons; }

public slots:
    void updateArea(const QRect &rect, int dy);
    void updateSize();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *e) override;
    void setFoldingButtonGeometry(EditorFoldingButton& fb);
};

class EditorFoldingButton : public QWidget
{
    Q_OBJECT
    FileView *m_view;
    QTextBlock m_firstBlock;
    QTextBlock m_lastBlock;

    const int m_startLine;
    const int m_endLine;

    bool m_collapsed = false;
    bool m_containsMouse = false;
public:

    EditorFoldingButton(QWidget* parent, FileView *view, int firstLine, int lastLine);

    QTextBlock getFirstLineBlock() const;
    QTextBlock getLastVisibleBlock() const;

    int getFirstLine() const { return m_startLine; }
    int getLastLine() const { return m_endLine; }

    bool isCollapsed() const { return m_collapsed; }

    void setContainsMouse(bool);
    QSize sizeHint() const override;

signals:
    void changedSize();

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

    void fold();
    void unfold();
};

#endif // EDITORFOLDINGAREA_H