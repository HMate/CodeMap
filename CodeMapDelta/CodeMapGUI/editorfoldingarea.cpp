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

    auto& editor = parent->getEditor();
    // Need this callback to handle scrolling and size update after editor content changed
    connect(&editor, &FileEdit::updateRequest, this, &EditorFoldingArea::updateArea);
    // update area after editor content changed -> folding bars can disappear if new lines are added
    connect(editor.document(), &QTextDocument::modificationChanged, this, &EditorFoldingArea::updateSize);
}

void EditorFoldingArea::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    QPainter painter(this);

    auto& rc = event->rect();
    painter.fillRect(rc, Qt::lightGray);
}

// *layout like behaviour*
EditorFoldingButton& EditorFoldingArea::addFoldingButton(int firstLine, int lastLine)
{
    if(firstLine > lastLine)
    {
        qSwap(firstLine, lastLine);
    }
    EditorFoldingButton *fb = new EditorFoldingButton(this, m_view, m_foldingHierarchy, firstLine, lastLine);
    m_foldingHierarchy.addButton(fb);
    connect(fb, &EditorFoldingButton::changedSize, this, &EditorFoldingArea::updateSize);
    fb->setVisible(true);
    m_foldingButtons.emplace_back(fb);

    setFoldingButtonGeometry(*fb);
    return *fb;
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

    if(m_lastSize != s)
    {
        m_lastSize = s;
        for(auto b : m_foldingButtons)
        {
            setFoldingButtonGeometry(*b);
        }
        m_foldingHierarchy.correctVisualOrder();
    }
}

void EditorFoldingArea::setFoldingButtonGeometry(EditorFoldingButton& fb)
{
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

EditorFoldingButton::EditorFoldingButton(QWidget* parent, FileView *view, EditorFoldingButtonHierarchy& hierarchy, int firstLine, int lastLine)
    : QWidget(parent), m_view(view), m_hierarchy(hierarchy), m_startLine(firstLine), m_endLine(lastLine)
{
    Q_ASSERT(firstLine < lastLine);
    
    auto& editor = m_view->getEditor();
    m_firstBlock = editor.document()->findBlockByLineNumber(firstLine-1);
    m_lastBlock = editor.document()->findBlockByLineNumber(lastLine-1);
}

QTextBlock EditorFoldingButton::getFirstLineBlock() const
{
    return m_firstBlock;
}

QTextBlock EditorFoldingButton::getLastVisibleBlock() const
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
    if(m_hierarchy.isLowestButtonThatContainsMouse(this))
    {
        if(!m_collapsed)
            fold();
        else
            unfold();

        changedSize();
        m_view->update();

        event->accept();
    }
    else
    {
        event->setAccepted(false);
    }
}

QSize EditorFoldingButton::sizeHint() const
{
    auto& editor = m_view->getEditor();
    auto offset = editor.contentOffset();

    if(m_collapsed)
    {
        auto block = editor.blockBoundingGeometry(m_firstBlock).translated(offset);
        int top = block.top();
        int bottom = block.bottom();
        return QSize(FOLD_AREA_WIDTH, bottom-top);
    }

    int top = editor.blockBoundingGeometry(m_firstBlock).translated(offset).top();
    QTextBlock lastBlock = getLastVisibleBlock();
    int bottom = editor.blockBoundingGeometry(lastBlock).translated(offset).bottom();
    return QSize(FOLD_AREA_WIDTH, bottom - top);
}

void EditorFoldingButton::setContainsMouse(bool c)
{
    m_containsMouse = c;
}


void EditorFoldingButton::fold() 
{
    m_collapsed = true;
    auto& oneAfterLast = m_lastBlock.next();
    for(auto block = m_firstBlock.next(); block.isValid() && block != oneAfterLast; block = block.next())
    {
        block.setVisible(false);
    }
}

void EditorFoldingButton::unfold() 
{
    m_collapsed = false;
    auto myNode = m_hierarchy.findNode(this);
    Q_ASSERT(myNode != nullptr);
    if(myNode == nullptr)
        return;

    auto childs = myNode->getChildNodes();

    auto last = m_lastBlock.firstLineNumber();
    for(auto block = m_firstBlock.next(); block.isValid() && block.firstLineNumber() <= last; block = block.next())
    {
       block.setVisible(true);
    }

    for(auto child : childs)
    {
        auto button = child.getButton();
        if(button->isCollapsed())
            button->fold();
    }
}

QString EditorFoldingButton::getReplacementText() const
{
    return " ... ";
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
    if(m_containsMouse && m_hierarchy.isLowestButtonThatContainsMouse(this))
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

// ------------ Folding Button hierarchy ----------


EditorFoldingButtonHierarchy::EditorFoldingButtonHierarchy() : EditorFoldingButtonHierarchyNode(nullptr)
{

}

void EditorFoldingButtonHierarchy::addButton(EditorFoldingButton* button)
{
    EditorFoldingButtonHierarchyNode::addButton(button);

    correctVisualOrder();
}

void EditorFoldingButtonHierarchy::correctVisualOrder() const
{
    for(auto& node : m_nodes)
    {
        node.correctVisualOrder();
    }
}

bool EditorFoldingButtonHierarchy::isLowestButtonThatContainsMouse(EditorFoldingButton* button)
{
    auto node = findNode(button);
    if(node != nullptr)
    {
        return !node->doAnyChildContainMouse();
    }

    return false;
}

EditorFoldingButtonHierarchyNode::EditorFoldingButtonHierarchyNode(EditorFoldingButton* button) : m_foldingButton(button)
{

}

bool EditorFoldingButtonHierarchyNode::isNodeFor(EditorFoldingButton* button)
{
    return button == m_foldingButton;
}

// Tells if this node is responsible for folding this line (expects line number, not line index!)
bool EditorFoldingButtonHierarchyNode::canFoldLine(int lineNumber) const
{
    bool res = m_foldingButton->getFirstLine() < lineNumber && lineNumber <= m_foldingButton->getLastLine();
    return res;
}

/* A section is invalid, if there is any existing section, which overlaps with it
 * but one doesn't encapsulate the other properly */
bool EditorFoldingButtonHierarchyNode::isSectionInvalid(int firstLine, int lastLine) const
{
    bool res = (m_foldingButton->getFirstLine() < firstLine && firstLine < m_foldingButton->getLastLine() && 
        m_foldingButton->getLastLine() < lastLine) ||
        (m_foldingButton->getFirstLine() < lastLine && lastLine < m_foldingButton->getLastLine() && 
            firstLine <= m_foldingButton->getFirstLine());
    return res;
}

void EditorFoldingButtonHierarchyNode::addButton(EditorFoldingButton* button)
{
    auto insertFirst = button->getFirstLine();
    auto insertLast = button->getLastLine();

    /* Possible scenarios:
    - new section is child node of my child node
    - new section is a new child and doesnt contain anything
    - new section is a new child and contains older child nodes
    */

    // TODO: Theres a bug here, hierarchy doesnt contain everything thats added
    for(auto& node : m_nodes)
    {
        if(node.isSectionInvalid(insertFirst, insertLast))
        {
            // sections are crossing, should never happen
            Q_UNREACHABLE();
            return;
        }

        if(node.canFoldLine(insertFirst) && node.canFoldLine(insertLast))
        {
            node.addButton(button);
            return;
        }
    }

    EditorFoldingButtonHierarchyNode newNode(button);
    for(auto it = begin(m_nodes); it != end(m_nodes);)
    {
        auto& node = *it;
        if(&newNode == &node)
        {
            it++;
            continue;
        }

        int first = node.m_foldingButton->getFirstLine();
        int last = node.m_foldingButton->getLastLine();

        if(newNode.canFoldLine(first) && newNode.canFoldLine(last))
        {
            newNode.m_nodes.emplace_back(node);
            it = m_nodes.erase(it);
            continue;
        }
        it++;
    }
    m_nodes.emplace_back(newNode);
}


std::vector<EditorFoldingButtonHierarchyNode>& EditorFoldingButtonHierarchyNode::getChildNodes() 
{ 
    return m_nodes; 
}

EditorFoldingButtonHierarchyNode* EditorFoldingButtonHierarchyNode::findNode(EditorFoldingButton* button)
{
    if(isNodeFor(button))
    {
        return this;
    }

    auto first = button->getFirstLine();
    auto last = button->getLastLine();
    for(auto& node : m_nodes)
    {
        if(node.isNodeFor(button))
        {
            return &node;
        }
        if(node.canFoldLine(first) && node.canFoldLine(last))
        {
            return node.findNode(button);
        }
    }
    return nullptr;
}

bool EditorFoldingButtonHierarchyNode::doAnyChildContainMouse() const
{
    for(auto& node : m_nodes)
    {
        if(node.m_foldingButton->isContainingMouse())
            return true;
    }
    return false;
}

// Makes child nodes to be painted later, so they are painted on top of this node
void EditorFoldingButtonHierarchyNode::correctVisualOrder() const
{
    m_foldingButton->raise();
    for(auto& node : m_nodes)
    {
        node.correctVisualOrder();
    }
}