#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H

#include <QWidget>

// Has to include class because it inherits QWidget, and that makes it qobject_cast-able
#include "fileedit.h"

class FoldIndicator : public QWidget
{
    bool m_containsMouse = false;
public:
    const int m_startLine;
    const int m_endLine;

    FoldIndicator(QWidget* parent, int start, int end);

    void setContainsMouse(bool);
    void drawIndicator(QPainter& painter, int lineNumber, int top, int lineHeight, int leftOffset, int width) const;
    QSize sizeHint() const override;
};

class LineNumberArea : public QWidget
{
    Q_OBJECT

    FileEdit *m_codeEditor;
    FoldIndicator *m_f;
public:
    explicit LineNumberArea(FileEdit *editor);

    QSize sizeHint() const override;
    int totalWidth() const;
    int numberAreaWidth() const;
    int foldAreaWidth() const;

protected:
    bool event(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void handleMouseOverFoldIndicator(const QPoint& pos);
};

#endif // LINENUMBERAREA_H