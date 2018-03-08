#ifndef TERMINALINPUT_H
#define TERMINALINPUT_H

#include <QWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QTextEdit>
#include <QMdiSubWindow>

class TerminalInput;
class TerminalHistory;

class TerminalView : public QWidget
{
    Q_OBJECT
protected:
    TerminalInput* tInput = nullptr;
    TerminalHistory* tHistory = nullptr;
public:
    TerminalView(QWidget* parent);

    /* Registers the command that was typed in to the terminal,
     * but doesn't actually execute it. */
    void registerCommand(const QString& command);
    //void resizeAfterParent(const QSize& parentSize); 
protected:
//    void closeEvent(QCloseEvent *closeEvent);

public slots:
    void handleTerminalCommand();
};

class TerminalHistory : public QListWidget
{
public:
    TerminalHistory(QWidget* parent);
};

class TerminalInput : public QLineEdit
{
public:
    TerminalInput(QWidget* parent);
};

#endif // TERMINALINPUT_H
