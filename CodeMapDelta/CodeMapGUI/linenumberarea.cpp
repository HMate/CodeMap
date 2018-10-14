#include "linenumberarea.h"

void LineNumberArea::paintEvent(QPaintEvent *event) {
    codeEditor->lineNumberAreaPaintEvent(event);
}

QSize LineNumberArea::sizeHint() const {
    return QSize(totalWidth(), 0);
}

int LineNumberArea::totalWidth() const
{
    return numberAreaWidth() + foldAreaWidth();
}

int LineNumberArea::numberAreaWidth() const
{
    return codeEditor->lineNumberAreaWidth();
}

int LineNumberArea::foldAreaWidth() const
{
    return 13;
}
