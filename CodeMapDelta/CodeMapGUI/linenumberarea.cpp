#include "linenumberarea.h"

#include <QTextBlock>

#include "fileedit.h"

LineNumberArea::LineNumberArea(QWidget *parent, FileEdit *editor) : QWidget(parent)
{
    m_codeEditor = editor;

    setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    setMaximumWidth(calculateWidth());

    // Need this callback to handle scrolling and size update after editor content changed
    connect(m_codeEditor, SIGNAL(updateRequest(QRect, int)), this, SLOT(updateLineNumberArea(QRect, int)));
}

void LineNumberArea::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = m_codeEditor->firstVisibleBlock();
    int lineNumber = block.blockNumber() + 1;
    int top = (int)m_codeEditor->blockBoundingGeometry(block).translated(m_codeEditor->contentOffset()).top();
    int bottom = top + (int)m_codeEditor->blockBoundingRect(block).height();

    const int rightMargin = 2;
    const int numAreaWidth = width();
    const int lineHeight = fontMetrics().height();

    while(block.isValid() && top <= event->rect().bottom()) {
        if(block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(lineNumber);
            painter.setPen(Qt::black);
            painter.drawText(0, top, numAreaWidth - rightMargin, lineHeight,
                Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int)m_codeEditor->blockBoundingRect(block).height();
        ++lineNumber;
    }
}

void LineNumberArea::updateLineNumberArea(const QRect &rect, int dy)
{
    if(dy)
        this->scroll(0, dy);
    else
        this->update(0, rect.y(), this->width(), rect.height());

    if(rect.contains(m_codeEditor->viewport()->rect()))
        updateSize();
}

void LineNumberArea::updateSize()
{
    auto s = sizeHint();
    resize(s);
    setMaximumWidth(s.width());
}

QSize LineNumberArea::sizeHint() const 
{
    return QSize(calculateWidth(), m_codeEditor->size().height());
}

int LineNumberArea::calculateWidth() const
{
    int digits = 1;
    int max = qMax(1, m_codeEditor->blockCount());
    while(max >= 10) {
        max /= 10;
        ++digits;
    }

    int width = 3 + fontMetrics().width(QLatin1Char('9'))*digits;
    return width;
}
