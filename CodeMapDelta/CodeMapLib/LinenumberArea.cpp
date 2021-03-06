#include "LinenumberArea.h"

#include <QTextBlock>
#include <QPainter>

#include "FileEdit.h"
#include "FileView.h"

LineNumberArea::LineNumberArea(FileView *parent) : QWidget((QWidget*)parent)
{
    m_view = parent;

    setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    setMaximumWidth(calculateWidth());

    // Need this callback to handle scrolling and size update after editor content changed
    connect(&m_view->getEditor(), SIGNAL(updateRequest(QRect, int)), this, SLOT(updateLineNumberArea(QRect, int)));
}

void LineNumberArea::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.fillRect(event->rect(), Qt::lightGray);

    auto& editor = m_view->getEditor();
    QTextBlock block = editor.firstVisibleBlock();
    int lineNumber = block.blockNumber() + 1;
    int top = (int)editor.blockBoundingGeometry(block).translated(editor.contentOffset()).top();
    int bottom = top + (int)editor.blockBoundingRect(block).height();
    
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
        bottom = top + (int)editor.blockBoundingRect(block).height();
        ++lineNumber;
    }
}

void LineNumberArea::updateLineNumberArea(const QRect &rect, int dy)
{
    if(dy)
        this->scroll(0, dy);
    else
        this->update(0, rect.y(), this->width(), rect.height());

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
    return QSize(calculateWidth(), m_view->size().height());
}

int LineNumberArea::calculateWidth() const
{
    int digits = 1;
    int max = qMax(1, m_view->getEditor().blockCount());
    while(max >= 10) {
        max /= 10;
        ++digits;
    }

    int width = 3 + fontMetrics().width(QLatin1Char('9'))*digits;
    return width;
}
