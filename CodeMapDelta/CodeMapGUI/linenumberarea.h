#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H

#include <QWidget>

// Has to include class because it inherits QWidget, and that makes it qobject_cast-able
#include "fileedit.h"

class FoldIndicator
{
    int m_startLine;
    int m_endLine;
public:

    FoldIndicator(int start, int end) : m_startLine(start), m_endLine(end) {}

    void drawIndicator(QPainter& painter, int lineNumber, int top, int lineHeight, int leftOffset, int width) const;
};

class LineNumberArea : public QWidget
{
    Q_OBJECT

    FileEdit *m_codeEditor;

public:
    explicit LineNumberArea(FileEdit *editor);

    QSize sizeHint() const override;
    int totalWidth() const;
    int numberAreaWidth() const;
    int foldAreaWidth() const;

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // LINENUMBERAREA_H