#ifndef EDITORFOLDINGAREA_H
#define EDITORFOLDINGAREA_H

#include <QWidget>
#include <QTextBlock>

#include <vector>

#include "textfolder.h"

class FileEdit;
class EditorFoldingButton;

class EditorFoldingArea : public QWidget
{
    Q_OBJECT
    FileEdit *m_codeEditor;
    std::vector<EditorFoldingButton*> m_foldingButtons;
public:
    explicit EditorFoldingArea(QWidget *parent, FileEdit *editor);

    QSize sizeHint() const override;
    int calculateWidth() const;

    void addFoldingButton(int firstLine, int lastLine);

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
    FileEdit *m_editor;
    QTextBlock m_firstBlock;
    QTextBlock m_lastBlock;

    const int m_startLine;
    const int m_endLine;

    bool m_collapsed = false;
    bool m_containsMouse = false;
public:

    EditorFoldingButton(QWidget* parent, FileEdit *editor, int firstLine, int lastLine);

    int getFirstLine() { return m_startLine; }
    QTextBlock getFirstLineBlock();

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