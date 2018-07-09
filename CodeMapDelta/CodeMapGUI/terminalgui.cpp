#include "terminalgui.h"

#include <QBoxLayout>
#include <QAction>
#include <QListWidgetItem>

#include "mainwindow.h"
#include "filesystem.h"

TerminalView::TerminalView(QWidget* parent)
    : QWidget(parent)
{
    setWindowTitle("Terminal");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setContentsMargins(0,0,0,0);

    tHistory = new TerminalHistory(this);
    layout->addWidget(tHistory);

    tInput = new TerminalInput(this);
    layout->addWidget(tInput);

    connect(tInput, &QLineEdit::returnPressed,
            this, &TerminalView::handleTerminalCommand);
}

void TerminalView::handleTerminalCommand()
{
    QString command = tInput->text();
    showMessage(command);

    /* Terminal TODO:
     * - todo commands:
     *      - open files, directories
     * - show guesses for :
     *      - possible commands
     *      - directory/files names
     * - handle tab and enter presses
    */
    const QString& lcmd = command.toLower();
    if(lcmd == "q" || lcmd == "quit")
    {
        MainWindow::instance()->getActions().quit->trigger();
    }

    if(lcmd == "cwd")
    {
        QString cwd = FS::getCWD();
        showMessage(cwd);
    }

    if(lcmd == "asd")
    {
        auto& s = MainWindow::instance()->getDocumentManager()->size();
        showMessage(QStringLiteral("Size: %1x%2 r: %3x%4").arg(s.width()).arg(s.height()).arg(s.rwidth()).arg(s.rheight()));
    }

    tInput->clear();
}

void TerminalView::showMessage(const QString& command)
{
    tHistory->insertItem(tHistory->count(), new QListWidgetItem(command, tHistory));
}

void TerminalView::focusInEvent(QFocusEvent *event)
{
    tInput->setFocus();
}

TerminalHistory::TerminalHistory(QWidget* parent) : QListWidget(parent)
{
    setLayoutMode(LayoutMode::Batched);
    setBatchSize(20);
}

TerminalInput::TerminalInput(QWidget* parent) : QLineEdit(parent)
{

}
