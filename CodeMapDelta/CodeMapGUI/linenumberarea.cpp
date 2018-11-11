#include "linenumberarea.h"

#include <QTextBlock>

#include "imagehandler.h"

explicit LineNumberArea::LineNumberArea(FileEdit *editor) : QWidget(qobject_cast<QWidget*>(editor))
{
    m_codeEditor = editor;
}

void LineNumberArea::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = m_codeEditor->firstVisibleBlock();
    int lineNumber = block.blockNumber() + 1;
    int top = (int)m_codeEditor->blockBoundingGeometry(block).translated(m_codeEditor->contentOffset()).top();
    int bottom = top + (int)m_codeEditor->blockBoundingRect(block).height();

    const int rightMargin = 2;
    int numAreaWidth = numberAreaWidth();

    while(block.isValid() && top <= event->rect().bottom()) {
        if(block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(lineNumber);
            int lineHeight = fontMetrics().height();
            painter.setPen(Qt::black);
            painter.drawText(0, top, numAreaWidth - rightMargin, lineHeight,
                Qt::AlignRight, number);

            // draw region folders
            {
                FoldIndicator fi(3, 5);
                fi.drawIndicator(painter, lineNumber, top, lineHeight, numAreaWidth, foldAreaWidth());
            }
        }

        block = block.next();
        top = bottom;
        bottom = top + (int)m_codeEditor->blockBoundingRect(block).height();
        ++lineNumber;
    }
}

void FoldIndicator::drawIndicator(QPainter& painter, int lineNumber, 
    int top, int lineHeight, int leftOffset, int width) const
{
    auto s = qMin(width-4, lineHeight-4);
    auto topMargin = (lineHeight - s + 1) / 2;
    auto leftMargin = (width - s + 1) / 2;
    if(lineNumber == m_startLine)
    {
        auto image = ImageHandler::loadIcon(icons::Plus);
        painter.drawImage(QRect(leftOffset + leftMargin, top + topMargin, s, s), image);
        painter.drawRect(QRect(leftOffset, top, width - 1, lineHeight));
    }

    if(lineNumber > m_startLine && lineNumber < m_endLine)
    {
        auto half = width / 2;
        painter.drawLine(leftOffset + half, top, leftOffset + half, top + lineHeight);
    }

    if(lineNumber == m_endLine)
    {
        auto image = ImageHandler::loadIcon(icons::Minus);
        painter.drawImage(QRect(leftOffset + leftMargin, top + topMargin, s, s), image);
        painter.drawRect(QRect(leftOffset, top, width - 1, lineHeight));
    }
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
    int digits = 1;
    int max = qMax(1, m_codeEditor->blockCount());
    while(max >= 10) {
        max /= 10;
        ++digits;
    }

    int width = 3 + fontMetrics().width(QLatin1Char('9'))*digits;
    return width;
}

int LineNumberArea::foldAreaWidth() const
{
    return 13;
}


