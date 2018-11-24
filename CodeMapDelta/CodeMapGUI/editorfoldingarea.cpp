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
    connect(m_codeEditor, SIGNAL(updateRequest(QRect, int)), this, SLOT(updateArea(QRect, int)));
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
    EditorFoldingButton *fb = new EditorFoldingButton(this, m_codeEditor, firstLine, lastLine);
    connect(fb, SIGNAL(changedSize()), this, SLOT(updateSize()));
    m_foldingButtons.emplace_back(fb);

    setFoldingButtonGeometry(*fb);
}

void EditorFoldingArea::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);

    // Call this manually because folding buttons are not contained in any layout.
    for(auto b : m_foldingButtons)
    {
        setFoldingButtonGeometry(*b);
    }
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
    QRectF blockBB = m_codeEditor->blockBoundingGeometry(block).translated(m_codeEditor->contentOffset());
    auto s = fb.sizeHint();
    QRect cr = contentsRect();
    auto qr = QRect(cr.left(), blockBB.top(), cr.right(), s.height());
    logTerminal(tr("set geometry to: %1, %2, %3, %4").arg(qr.left()).arg(qr.top()).arg(qr.right()).arg(qr.bottom()));
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
    
    m_foldingCursor = m_editor->textCursor();
    m_firstBlock = m_editor->document()->findBlockByLineNumber(firstLine-1);
    m_lastBlock = m_editor->document()->findBlockByLineNumber(lastLine-1);
    int p = m_firstBlock.position();
    m_foldingCursor.setPosition(p);
    int lastP = m_lastBlock.position();
    int lastBlockLength = m_lastBlock.length() - 1;
    m_foldingCursor.setPosition(lastP+lastBlockLength, QTextCursor::KeepAnchor);

    m_unfoldingCursor  = m_editor->textCursor();
    m_unfoldingCursor.setPosition(m_firstBlock.position());

    // IMMEDIATETODO: type() should change here I guess ....
    m_regionFolder = new TextFolder(this, "ASD");
    m_foldingCursor.document()->documentLayout()->registerHandler(m_regionFolder->type(), m_regionFolder);
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

void EditorFoldingButton::fold() {
    m_regionFolder->fold(m_foldingCursor);
}

void EditorFoldingButton::unfold() {
    m_regionFolder->unfold(m_unfoldingCursor);
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
