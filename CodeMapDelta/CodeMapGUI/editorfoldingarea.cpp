#include "editorfoldingarea.h"

#include <QTextBlock>

#include "imagehandler.h"
#include "fileedit.h"

#include "mainwindow.h"

const int FOLD_AREA_WIDTH = 13;

EditorFoldingArea::EditorFoldingArea(QWidget *parent, FileEdit *editor) : QWidget(parent)
{
    m_codeEditor = editor;
    addFoldingButton(5, 7);

    setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    setMaximumWidth(calculateWidth());

    // Need this callback to handle scrolling and size update after editor content changed
    connect(m_codeEditor, SIGNAL(updateRequest(QRect, int)), this, SLOT(updateArea(QRect, int)));
}

void EditorFoldingArea::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    QPainter painter(this);

    auto& rc = event->rect();
    painter.fillRect(event->rect(), Qt::yellow);

    QTextBlock block = m_codeEditor->firstVisibleBlock();
    int lineNumber = block.blockNumber() + 1;
    int top = (int)m_codeEditor->blockBoundingGeometry(block).translated(m_codeEditor->contentOffset()).top();
    int bottom = top + (int)m_codeEditor->blockBoundingRect(block).height();

    const int w = width();
    const int lineHeight = fontMetrics().height();

    while(block.isValid() && top <= event->rect().bottom()) {
        if(block.isVisible() && bottom >= event->rect().top()) {
            painter.setPen(Qt::black);

            // draw region folders
            {
                m_f->drawIndicator(painter, lineNumber, top, lineHeight, 0, w);
            }
        }

        block = block.next();
        top = bottom;
        bottom = top + (int)m_codeEditor->blockBoundingRect(block).height();
        ++lineNumber;
    }
}

void EditorFoldingArea::addFoldingButton(int firstLine, int lastLine)
{
    m_f = new EditorFoldingButton(this, firstLine, lastLine);

    QTextBlock block = m_codeEditor->firstVisibleBlock();
    int top = (int)m_codeEditor->blockBoundingGeometry(block).translated(m_codeEditor->contentOffset()).top();
    auto s = m_f->sizeHint();
    QRect cr = contentsRect();
    m_f->setGeometry(QRect(cr.left(), top + fontMetrics().height()*(m_f->m_startLine - 1), cr.right(), s.height()));
}

void EditorFoldingArea::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);

    // Call this manually because folding buttons are not contained in any layout.
    QTextBlock block = m_codeEditor->firstVisibleBlock();
    int top = (int)m_codeEditor->blockBoundingGeometry(block).translated(m_codeEditor->contentOffset()).top();
    auto s = m_f->sizeHint();
    QRect cr = contentsRect();
    m_f->setGeometry(QRect(cr.left(), top + fontMetrics().height()*(m_f->m_startLine - 1), cr.right(), s.height()));
}

void EditorFoldingArea::updateArea(const QRect &rect, int dy)
{
    if(dy)
        this->scroll(0, dy);
    else
        this->update(0, rect.y(), this->width(), rect.height());

    if(rect.contains(m_codeEditor->viewport()->rect()))
        updateSize();
}

void EditorFoldingArea::updateSize()
{
    auto s = sizeHint();
    resize(s);
    setMaximumWidth(s.width());
}

QSize EditorFoldingArea::sizeHint() const
{
    auto rc = QSize(calculateWidth(), m_codeEditor->size().height());
    return rc;
}

int EditorFoldingArea::calculateWidth() const
{
    return FOLD_AREA_WIDTH;
}

// --------------- Folding Button --------------------

EditorFoldingButton::EditorFoldingButton(QWidget* parent, int start, int end) : QWidget(parent), m_startLine(start), m_endLine(end)
{
    Q_ASSERT(start < end);
}

void EditorFoldingButton::enterEvent(QEvent *event)
{
    QWidget::enterEvent(event);
    setContainsMouse(true);
    parentWidget()->repaint();
}

void EditorFoldingButton::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);
    setContainsMouse(false);
    parentWidget()->repaint();
}

QSize EditorFoldingButton::sizeHint() const
{
    const int lineHeight = fontMetrics().height();
    return QSize(FOLD_AREA_WIDTH, lineHeight*(m_endLine - m_startLine + 1));
}

void EditorFoldingButton::setContainsMouse(bool c)
{
    m_containsMouse = c;
}

void EditorFoldingButton::drawIndicator(QPainter& painter, int lineNumber,
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