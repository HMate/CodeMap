#include "fileedit.h"

#include <QMenu>
#include <QContextMenuEvent>

#include "mainwindow.h"
#include "codeparser.h"

FileEdit::FileEdit(QWidget* parent) : QTextEdit(parent)
{
    this->setLineWrapMode(QTextEdit::LineWrapMode::NoWrap);
    this->setFont(QFont("Consolas"));
}

void FileEdit::contextMenuEvent(QContextMenuEvent* event)
{
    std::unique_ptr<QMenu> menu(createStandardContextMenu());
    menu->addAction(tr("Fold defines"), this, &FileEdit::foldDefines);
    menu->exec(event->globalPos());
}

void FileEdit::foldDefines()
{
    // TODO: put check to centext menu disable action if not availabe
    // TODO: write help in context menu, show what is missing
    // in case action is not available
    if(filePath == "")
        return;
    const auto& terminal = MainWindow::instance()->getTerminalView();
    terminal->showMessage(tr("Folding defines for %1").arg(filePath));
    auto processed = cm::CodeParser().getPreprocessedCodeFromPath(filePath);
    // TODO Handle errors? How exactly?

    QString name = tr("Folded defines for: %1").arg(filePath);
    if(processed.hasErrors())
    {
        terminal->showMessage(tr("Had %1 errors:").arg(processed.errors.size()));
        for(auto& e : processed.errors)
        {
            terminal->showMessage(e);
        }
    }
    MainWindow::instance()->getDocumentManager()->openStringFileView(name, processed.content);

}

void FileEdit::setFilePath(const QString& path)
{
    filePath = path;
}
