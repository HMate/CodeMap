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
    painter.fillRect(rc, Qt::lightGray);
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

void EditorFoldingButton::paintEvent(QPaintEvent *event) 
{
    QWidget::paintEvent(event);
    QPainter painter(this);

    const int lineHeight = fontMetrics().height();
    auto s = qMin(width() - 4, lineHeight - 4);
    auto topMargin = (lineHeight - s + 1) / 2;
    auto leftMargin = (width() - s + 1) / 2;
    if(m_containsMouse)
    {
        painter.setPen(Qt::darkGray);
    }
    else
    {
        painter.setPen(Qt::black);
    }

    // draw start box
    auto plusImage = ImageHandler::loadIcon(icons::Plus);
    painter.drawImage(QRect(leftMargin, topMargin, s, s), plusImage);
    painter.drawRect(QRect(0, 0, width() - 1, lineHeight-1));

    // draw middle line
    auto half = width() / 2;
    int lineTop = lineHeight;
    int lineLength = m_endLine - m_startLine - 1;
    int lineBottom = lineTop + (lineHeight*lineLength);
    painter.drawLine(half, lineTop, half, lineBottom);

    // draw closing box
    auto minusImage = ImageHandler::loadIcon(icons::Minus);
    painter.drawImage(QRect(leftMargin, lineBottom + topMargin, s, s), minusImage);
    painter.drawRect(QRect(0, lineBottom, width() - 1, lineHeight-1));
}
