#include "fileedit.h"

#include <QMenu>
#include <QContextMenuEvent>

#include <QtConcurrent>

#include "mainwindow.h"
#include "fileview.h"
#include "editorfoldingarea.h"

FileEdit::FileEdit(QWidget* parent) : QPlainTextEdit(parent), m_view((FileView*)parent)
{
    this->setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);
    this->setFont(QFont("Consolas"));
    
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    connect(&m_foldWatcher, SIGNAL(finished()), this, SLOT(foldDefinesFinished()));

    highlightCurrentLine();
}

void FileEdit::contextMenuEvent(QContextMenuEvent* event)
{
    // TODO: put check to centext menu disable action if not available
    // TODO: write help in context menu, show what is missing
    // in case action is not available
    std::unique_ptr<QMenu> menu(this->createStandardContextMenu());
    menu->addAction(tr("Expand macros"), this, &FileEdit::foldDefines);
    menu->exec(event->globalPos());
}

void FileEdit::resizeEvent(QResizeEvent* e)
{
    QPlainTextEdit::resizeEvent(e);
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

    // TODO: Put folding defines command to a separate command handler class
    // There could be a static class that acts like a "separate thread" for running long commands liek this
    // This class should handle set/clean up after commands etc.
    auto foldFuture = QtConcurrent::run(this, &FileEdit::foldDefinesForFile, m_FilePath);
    m_foldWatcher.setFuture(foldFuture);
}

cm::ParserResult FileEdit::foldDefinesForFile(const QString& filePath) const
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
    return processed;
}

void FileEdit::foldDefinesFinished()
{
    cm::ParserResult result = m_foldWatcher.future().result();
    if(result.hasErrors())
    {
        auto mw = MainWindow::instance();
        const auto& terminal = mw->getTerminalView();
        terminal->showMessage(tr("Had %1 errors:").arg(result.errors.size()));
        for(auto& e : result.errors)
        {
            terminal->showMessage(e);
        }
    }
    m_PreprocessedFileView->setText(result.code.content);
    qDebug() << "start adding includes";
    auto& foldingArea = m_PreprocessedFileView->getFoldingArea();

    // TODO: This is really slow now, maybe try batching?
    for(auto& include : result.code.includes)
    {
        foldingArea.addFoldingButton(include.firstLine, include.lastLine);
    }
    for(auto& fb : foldingArea.getFoldingButtons())
    {
        fb->fold();
    }
    qDebug() << "end adding includes";
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

void FileEdit::paintEvent(QPaintEvent *event)
{
    QPlainTextEdit::paintEvent(event);

    QPainter painter(viewport());
    QPointF offset = contentOffset();

    EditorFoldingArea& fArea = m_view->getFoldingArea();

    // qDebug() << "fileedit overlay paint";
    // draw text overlay for folded blocks
    for(auto btn : fArea.getFoldingButtons())
    {
        auto block = btn->getFirstBlock();
        if(!block.isVisible())
            continue;

        // TODO: Why do some nodes become invalid?
        if(!block.isValid())
            continue;

        if(btn->isCollapsed())
        {
            auto boundingRect = blockBoundingGeometry(block).translated(offset);

            auto layout = block.layout();
            QTextLine line = layout->lineAt(layout->lineCount() - 1);
            QRectF lineRect = line.naturalTextRect().translated(offset.x(), boundingRect.top());
            QString replacement = btn->getReplacementText();

            const qreal overlayLeftMargin = 12;
            QRectF collapseRect(lineRect.right() + overlayLeftMargin, lineRect.top(),
                fontMetrics().width(replacement), lineRect.height());
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.translate(.5, .5);
            painter.drawRoundedRect(collapseRect.adjusted(0, 0, 0, -1), 1, 1);
            painter.setRenderHint(QPainter::Antialiasing, false);
            painter.translate(-.5, -.5);

            painter.drawText(collapseRect, Qt::AlignCenter, replacement);
        }
    }

}
