#include "fileedit.h"

#include <QMenu>
#include <QContextMenuEvent>

#include "mainwindow.h"


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
    MainWindow::instance()->getTerminalView()->showMessage("foldDefines");
    //CodeParser().getPreprocessedCodeFromPath(codePath);
}
