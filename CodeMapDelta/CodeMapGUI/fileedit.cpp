#include "fileedit.h"

#include <QMenu>
#include <QContextMenuEvent>

#include <QPainter>
#include <QtConcurrent>

#include "mainwindow.h"
#include "codeparser.h"
#include "linenumberarea.h"
#include "fileview.h"

FileEdit::FileEdit(QWidget* parent) : QPlainTextEdit(parent)
{
    m_lineNumberArea = new LineNumberArea(this);
    m_regionFolder = new TextFolder(this, "ASD");

    document()->documentLayout()->registerHandler(m_regionFolder->type(), m_regionFolder);

    this->setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);
    this->setFont(QFont("Consolas"));
    
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect, int)), this, SLOT(updateLineNumberArea(QRect, int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    connect(&foldWatcher, SIGNAL(finished()), this, SLOT(foldDefinesFinished()));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

void FileEdit::fold() {
    auto c = textCursor();
    c.setPosition(10);
    c.setPosition(20, QTextCursor::KeepAnchor);
    m_regionFolder->fold(c);
}

void FileEdit::unfold() {
    auto c = textCursor();
    c.setPosition(10);
    m_regionFolder->unfold(c);
}

void FileEdit::contextMenuEvent(QContextMenuEvent* event)
{
    // TODO: put check to centext menu disable action if not available
    // TODO: write help in context menu, show what is missing
    // in case action is not available
    std::unique_ptr<QMenu> menu(createStandardContextMenu());
    menu->addAction(tr("Fold defines"), this, &FileEdit::foldDefines);
    menu->exec(event->globalPos());
}

void FileEdit::foldDefines()
{
    if(m_FilePath == "")
        return;
    auto mw = MainWindow::instance();
    const auto& terminal = mw->getTerminalView();
    terminal->showMessage(tr("Folding defines for %1").arg(m_FilePath));

    QString name = tr("Folded defines for: %1").arg(m_FilePath);
    m_PreprocessedFileView = MainWindow::instance()->getDocumentManager()->openStringFileView(name, "Loading...");

    auto foldFuture = QtConcurrent::run(this, &FileEdit::foldDefinesForFile, m_FilePath);
    foldWatcher.setFuture(foldFuture);
}

QString FileEdit::foldDefinesForFile(const QString& filePath) const
{
    auto mw = MainWindow::instance();
    const auto& terminal = mw->getTerminalView();
    auto& appState = mw->getAppState();

    std::vector<QString> includes;
    for(auto& i : appState.settings().globalIncludes)
    {
        includes.emplace_back(i);
    }
    auto processed = cm::CodeParser().getPreprocessedCodeFromPath(filePath, includes);

    if(processed.hasErrors())
    {
        terminal->showMessage(tr("Had %1 errors:").arg(processed.errors.size()));
        for(auto& e : processed.errors)
        {
            terminal->showMessage(e);
        }
    }
    return processed.code.content;
}

void FileEdit::foldDefinesFinished()
{
    QString result = foldWatcher.future().result();
    m_PreprocessedFileView->setText(result);
}

void FileEdit::setFilePath(const QString& path)
{
    m_FilePath = path;
}

// regions



// Line numbering things

void FileEdit::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);
    
    // Call this manually because linenumberarea is not contained in any layout.
    QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), m_lineNumberArea->totalWidth(), cr.height()));
}

void FileEdit::updateLineNumberAreaWidth(int newBlockCount)
{
    setViewportMargins(m_lineNumberArea->width(), 0, 0, 0);
}

void FileEdit::updateLineNumberArea(const QRect &rect, int dy)
{
    if(dy)
        m_lineNumberArea->scroll(0, dy);
    else
        m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());

    if(rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void FileEdit::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if(!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::green).darker(150);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}
