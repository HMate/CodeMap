#include "linenumberarea.h"

#include <QTextBlock>

#include "imagehandler.h"
#include "mainwindow.h"

const int FOLD_AREA_WIDTH = 13;

LineNumberArea::LineNumberArea(QWidget *parent, FileEdit *editor) : QWidget(parent)
{
    m_codeEditor = editor;
    m_f = new FoldIndicator(this, 3, 5);

    setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    setMaximumWidth(totalWidth());

    // Need this callback to handle scrolling and size update after editor content changed
    connect(m_codeEditor, SIGNAL(updateRequest(QRect, int)), this, SLOT(updateLineNumberArea(QRect, int)));
}

void LineNumberArea::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = m_codeEditor->firstVisibleBlock();
    int lineNumber = block.blockNumber() + 1;
    int top = (int)m_codeEditor->blockBoundingGeometry(block).translated(m_codeEditor->contentOffset()).top();
    int bottom = top + (int)m_codeEditor->blockBoundingRect(block).height();

    const int rightMargin = 2;
    const int numAreaWidth = numberAreaWidth();
    const int lineHeight = fontMetrics().height();

    while(block.isValid() && top <= event->rect().bottom()) {
        if(block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(lineNumber);
            painter.setPen(Qt::black);
            painter.drawText(0, top, numAreaWidth - rightMargin, lineHeight,
                Qt::AlignRight, number);

            // draw region folders
            {
                m_f->drawIndicator(painter, lineNumber, top, lineHeight, numAreaWidth, foldAreaWidth());
            }
        }

        block = block.next();
        top = bottom;
        bottom = top + (int)m_codeEditor->blockBoundingRect(block).height();
        ++lineNumber;
    }
}

bool LineNumberArea::event(QEvent *event)
{
    switch(event->type())
    {
    case QEvent::HoverEnter:
        qDebug() << "HoverEnter";
        handleMouseOverFoldIndicator(static_cast<QHoverEvent*>(event)->pos());
        return true;
        break;
    case QEvent::HoverLeave:
        qDebug() << "HoverLeave";
        handleMouseOverFoldIndicator(static_cast<QHoverEvent*>(event)->pos());
        return true;
        break;
    case QEvent::HoverMove:
        qDebug() << "Move";
        handleMouseOverFoldIndicator(static_cast<QHoverEvent*>(event)->pos());
        return true;
        break;
    default:
        break;
    }
    return QWidget::event(event);
}

void LineNumberArea::mouseMoveEvent(QMouseEvent *event)
{
    auto p = event->pos();
    handleMouseOverFoldIndicator(p);
}

void LineNumberArea::handleMouseOverFoldIndicator(const QPoint& pos)
{
    // TODO: we dont get the vent when leaving on right side to editor
    // TODO: this either needs to be handled by the layout system, or placed to the editor
    // because if i want to highlight a block here, the code will become messy.
    QTextBlock block = m_codeEditor->firstVisibleBlock();
    int top = (int)m_codeEditor->blockBoundingGeometry(block).translated(m_codeEditor->contentOffset()).top();
    auto s = m_f->sizeHint();
    QPoint fpos(numberAreaWidth(), top + fontMetrics().height()*(m_f->m_startLine-1));
    QRect frect = QRect(fpos, s);
    m_f->setContainsMouse(frect.contains(pos));
    repaint();
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
    return QSize(totalWidth(), m_codeEditor->size().height());
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
    return FOLD_AREA_WIDTH;
}


// ---------------Fold Indicator -------------------------

FoldIndicator::FoldIndicator(QWidget* parent, int start, int end) : QWidget(parent), m_startLine(start), m_endLine(end)
{
    Q_ASSERT(start < end);
}

QSize FoldIndicator::sizeHint() const
{
    const int lineHeight = fontMetrics().height();
    return QSize(FOLD_AREA_WIDTH, lineHeight*(m_endLine - m_startLine + 1));
}

void FoldIndicator::setContainsMouse(bool c)
{
    m_containsMouse = c;
}

void FoldIndicator::drawIndicator(QPainter& painter, int lineNumber,
    int top, int lineHeight, int leftOffset, int width) const
{
    auto s = qMin(width - 4, lineHeight - 4);
    auto topMargin = (lineHeight - s + 1) / 2;
    auto leftMargin = (width - s + 1) / 2;
    if(m_containsMouse)
    {
        //painter.fillRect(QRect(leftOffset, top, width - 1, lineHeight), Qt::white);
        painter.setPen(Qt::darkGray);
    }

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