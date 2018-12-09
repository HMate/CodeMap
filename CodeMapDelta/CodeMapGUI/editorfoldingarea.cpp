#include "editorfoldingarea.h"


#include "imagehandler.h"
#include "fileedit.h"

#include "mainwindow.h"

const int FOLD_AREA_WIDTH = 13;

EditorFoldingArea::EditorFoldingArea(QWidget *parent, FileEdit *editor) : QWidget(parent)
{
    m_codeEditor = editor;

    setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    setMaximumWidth(calculateWidth());

    // Need this callback to handle scrolling and size update after editor content changed
    connect(m_codeEditor, &FileEdit::updateRequest, this, &EditorFoldingArea::updateArea);
}

void EditorFoldingArea::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    QPainter painter(this);

    auto& rc = event->rect();
    painter.fillRect(rc, Qt::lightGray);
}

// *layout like behaviour*
void EditorFoldingArea::addFoldingButton(int firstLine, int lastLine)
{
    // TODO: build tree of foldingButtons?
    logTerminal(tr("add button to %1 on lines %2, %3").arg(m_codeEditor->m_FilePath).arg(firstLine).arg(lastLine));
    EditorFoldingButton *fb = new EditorFoldingButton(this, m_codeEditor, firstLine, lastLine);
    connect(fb, &EditorFoldingButton::changedSize, this, &EditorFoldingArea::updateSize);
    m_foldingButtons.emplace_back(fb);

    setFoldingButtonGeometry(*fb);
}

void EditorFoldingArea::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    //logTerminal(tr("resizeEvent %1").arg(m_codeEditor->m_FilePath));

    updateSize();
}

void EditorFoldingArea::updateArea(const QRect &rect, int dy)
{
    //logTerminal(tr("updateArea %1").arg(m_codeEditor->m_FilePath));
    if(dy)
        this->scroll(0, dy);
    else
        this->update(0, rect.y(), this->width(), rect.height());

    updateSize();
}

// When a button is changed/clicked/added/removed this should be called to update the position
// and size of every button
// *layout like behaviour*
void EditorFoldingArea::updateSize()
{
    auto s = sizeHint();
    resize(s);
    setMaximumWidth(s.width());

    for(auto b : m_foldingButtons)
    {
        setFoldingButtonGeometry(*b);
    }
}

void EditorFoldingArea::setFoldingButtonGeometry(EditorFoldingButton& fb)
{
    // TODO: What if block is not visible? What does this give? Check it!
    QTextBlock block = fb.getFirstLineBlock();
    QRectF blockBB = m_codeEditor->blockBoundingGeometry(block).translated(m_codeEditor->contentOffset());
    auto s = fb.sizeHint();
    QRect cr = contentsRect();
    auto qr = QRect(cr.left(), blockBB.top(), cr.right(), s.height());
    /*logTerminal(tr("-- set geometry --"));
    logTerminal(tr("block BB: %1, %2, %3, %4").arg(blockBB.left()).arg(blockBB.top()).arg(blockBB.right()).arg(blockBB.bottom()));
    logTerminal(tr("contents rect %1, %2, %3, %4").arg(cr.left()).arg(cr.top()).arg(cr.right()).arg(cr.bottom()));
    logTerminal(tr("size: %1, %2").arg(s.width()).arg(s.height()));
    logTerminal(tr("set geometry to: %1, %2, %3, %4").arg(qr.left()).arg(qr.top()).arg(qr.right()).arg(qr.bottom()));*/
    fb.setGeometry(qr);
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

EditorFoldingButton::EditorFoldingButton(QWidget* parent, FileEdit *editor, int firstLine, int lastLine)
    : QWidget(parent), m_editor(editor), m_startLine(firstLine), m_endLine(lastLine)
{
    Q_ASSERT(firstLine < lastLine);
    
    m_firstBlock = m_editor->document()->findBlockByLineNumber(firstLine-1);
    m_lastBlock = m_editor->document()->findBlockByLineNumber(lastLine-1);
}

QTextBlock EditorFoldingButton::getFirstLineBlock()
{
    return m_firstBlock;
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

void EditorFoldingButton::mousePressEvent(QMouseEvent *event)
{
    m_collapsed = !m_collapsed;
    changedSize();
    parentWidget()->repaint();
}

QSize EditorFoldingButton::sizeHint() const
{
    // TODO: This doesnt handle multi line blocks
    const int lineHeight = fontMetrics().height();

    if(m_collapsed)
    {
        return QSize(FOLD_AREA_WIDTH, lineHeight);
    }

    return QSize(FOLD_AREA_WIDTH, lineHeight*(m_endLine - m_startLine + 1));
}

void EditorFoldingButton::setContainsMouse(bool c)
{
    m_containsMouse = c;
}

void EditorFoldingButton::fold() 
{

}

void EditorFoldingButton::unfold() 
{

}

void EditorFoldingButton::paintEvent(QPaintEvent *event) 
{
    QWidget::paintEvent(event);

    if(!m_firstBlock.isVisible())
        return;

    QPainter painter(this);

    const int lineHeight = fontMetrics().height();
    auto s = qMin(width() - 4, lineHeight - 4);
    auto topMargin = (lineHeight - s + 1) / 2;
    auto leftMargin = (width() - s + 1) / 2;
    if(true/*m_containsMouse*/)
    {
        painter.setPen(Qt::darkGray);
        painter.fillRect(event->rect(), Qt::white);
    }
    else
    {
        painter.setPen(Qt::black);
    }

    // draw start box
    auto plusImage = ImageHandler::loadIcon(icons::Plus);
    painter.drawImage(QRect(leftMargin, topMargin, s, s), plusImage);
    painter.drawRect(QRect(0, 0, width() - 1, lineHeight-1));

    if(m_collapsed)
        return;

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
