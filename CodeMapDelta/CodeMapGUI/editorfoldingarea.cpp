#include "editorfoldingarea.h"


#include "imagehandler.h"
#include "fileedit.h"
#include "fileview.h"

#include "mainwindow.h"

const int FOLD_AREA_WIDTH = 13;

EditorFoldingArea::EditorFoldingArea(FileView *parent) : QWidget((QWidget*)parent)
{
    m_view = parent;

    setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    setMaximumWidth(calculateWidth());

    // Need this callback to handle scrolling and size update after editor content changed
    connect(&parent->getEditor(), &FileEdit::updateRequest, this, &EditorFoldingArea::updateArea);
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
    EditorFoldingButton *fb = new EditorFoldingButton(this, m_view, firstLine, lastLine);
    connect(fb, &EditorFoldingButton::changedSize, this, &EditorFoldingArea::updateSize);
    fb->setVisible(true);
    m_foldingButtons.emplace_back(fb);

    setFoldingButtonGeometry(*fb);
}

// *layout like behaviour -> should call setGeometry on every child to update them*
void EditorFoldingArea::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    updateSize();
}

void EditorFoldingArea::updateArea(const QRect &rect, int dy)
{
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
    auto& editor = m_view->getEditor();
    QRectF blockBB = editor.blockBoundingGeometry(block).translated(editor.contentOffset());
    auto s = fb.sizeHint();
    QRect cr = contentsRect();
    auto qr = QRect(cr.left(), blockBB.top(), cr.right(), s.height());
    fb.setGeometry(qr);
}

QSize EditorFoldingArea::sizeHint() const
{
    auto rc = QSize(calculateWidth(), m_view->size().height());
    return rc;
}

int EditorFoldingArea::calculateWidth() const
{
    return FOLD_AREA_WIDTH;
}

// --------------- Folding Button --------------------

EditorFoldingButton::EditorFoldingButton(QWidget* parent, FileView *view, int firstLine, int lastLine)
    : QWidget(parent), m_view(view), m_startLine(firstLine), m_endLine(lastLine)
{
    Q_ASSERT(firstLine < lastLine);
    
    auto& editor = m_view->getEditor();
    m_firstBlock = editor.document()->findBlockByLineNumber(firstLine-1);
    m_lastBlock = editor.document()->findBlockByLineNumber(lastLine-1);
}

QTextBlock EditorFoldingButton::getFirstLineBlock()
{
    return m_firstBlock;
}


QTextBlock EditorFoldingButton::getLastVisibleBlock()
{
    for(auto b = m_lastBlock; b != m_firstBlock; b = b.previous())
    {
        if(b.isValid() && b.isVisible())
            return b;
    }
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

    if(m_collapsed)
        fold();
    else
        unfold();

    changedSize();
    m_view->update();
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
    /*TODO: Use block isVisible to fold lines 
      -> Needs to handle hierarchy of folding buttons.
    */

    auto& oneAfterLast = m_lastBlock.next();
    for(auto block = m_firstBlock.next(); block != oneAfterLast; block = block.next())
    {
        block.setVisible(false);
    }
}

void EditorFoldingButton::unfold() 
{
    auto& oneAfterLast = m_lastBlock.next();
    for(auto block = m_firstBlock.next(); block != oneAfterLast; block = block.next())
    {
        block.setVisible(true);
    }
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
    if(m_containsMouse)
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

    QTextBlock lastBlock = getLastVisibleBlock();

    if(m_collapsed || m_firstBlock.firstLineNumber() == lastBlock.firstLineNumber())
    {
        return;
    }

    auto& editor = m_view->getEditor();
    auto offset = editor.contentOffset();
    // draw middle line
    auto half = width() / 2;
    int lineTop = lineHeight;
    int lastBlockTop = editor.blockBoundingGeometry(lastBlock).translated(offset).top();
    int firstBlockBottom = editor.blockBoundingGeometry(m_firstBlock).translated(offset).bottom();
    int lineLength  = lastBlockTop - firstBlockBottom;
    int lineBottom = lineTop + lineLength;
    painter.drawLine(half, lineTop, half, lineBottom);

    // draw closing box
    auto minusImage = ImageHandler::loadIcon(icons::Minus);
    painter.drawImage(QRect(leftMargin, lineBottom + topMargin, s, s), minusImage);
    painter.drawRect(QRect(0, lineBottom, width() - 1, lineHeight-1));
}
