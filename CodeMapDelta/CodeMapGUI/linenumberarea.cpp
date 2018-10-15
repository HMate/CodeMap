#include "linenumberarea.h"

#include <QTextBlock>

#include "imagehandler.h"

void LineNumberArea::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = codeEditor->firstVisibleBlock();
    int lineNumber = block.blockNumber() + 1;
    int top = (int)codeEditor->blockBoundingGeometry(block).translated(codeEditor->contentOffset()).top();
    int bottom = top + (int)codeEditor->blockBoundingRect(block).height();

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
                auto fw = foldAreaWidth();
                auto s = qMin(fw, lineHeight);
                auto topMargin = (lineHeight - s + 1) / 2;
                if(lineNumber == 2)
                {
                    auto image = ImageHandler::loadIcon(icons::Plus);
                    painter.drawImage(QRect(numAreaWidth, top + topMargin, s, s), image);

                    painter.drawRect(QRect(numAreaWidth, top, fw - 1, lineHeight));
                }

                if(lineNumber > 2 && lineNumber < 5)
                {
                    auto half = fw / 2;
                    painter.drawLine(numAreaWidth + half, top, numAreaWidth + half, top + lineHeight);
                }

                if(lineNumber == 5)
                {
                    auto image = ImageHandler::loadIcon(icons::Minus);
                    painter.drawImage(QRect(numAreaWidth, top + topMargin, s, s), image);

                    painter.drawRect(QRect(numAreaWidth, top, fw - 1, lineHeight));
                }
            }
        }

        block = block.next();
        top = bottom;
        bottom = top + (int)codeEditor->blockBoundingRect(block).height();
        ++lineNumber;
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
    int max = qMax(1, codeEditor->blockCount());
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


