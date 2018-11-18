#include "fileedit.h"

#include <QMenu>
#include <QContextMenuEvent>

#include <QPainter>
#include <QtConcurrent>

#include "mainwindow.h"
#include "codeparser.h"
#include "fileview.h"

FileEdit::FileEdit(QWidget* parent) : QPlainTextEdit(parent)
{
    m_regionFolder = new TextFolder(this, "ASD");
    
    this->document()->documentLayout()->registerHandler(m_regionFolder->type(), m_regionFolder);

    this->setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);
    this->setFont(QFont("Consolas"));
    
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    connect(&m_foldWatcher, SIGNAL(finished()), this, SLOT(foldDefinesFinished()));

    highlightCurrentLine();
}

void FileEdit::fold() {
    auto c = this->textCursor();
    c.setPosition(10);
    c.setPosition(20, QTextCursor::KeepAnchor);
    m_regionFolder->fold(c);
}

void FileEdit::unfold() {
    auto c = this->textCursor();
    c.setPosition(10);
    m_regionFolder->unfold(c);
}

void FileEdit::contextMenuEvent(QContextMenuEvent* event)
{
    // TODO: put check to centext menu disable action if not available
    // TODO: write help in context menu, show what is missing
    // in case action is not available
    std::unique_ptr<QMenu> menu(this->createStandardContextMenu());
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
    m_foldWatcher.setFuture(foldFuture);
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
    QString result = m_foldWatcher.future().result();
    m_PreprocessedFileView->setText(result);
}

void FileEdit::setFilePath(const QString& path)
{
    m_FilePath = path;
}

void FileEdit::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if(!this->isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::green).darker(150);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = this->textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    this->setExtraSelections(extraSelections);
}
