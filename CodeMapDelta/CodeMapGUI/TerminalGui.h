#ifndef TERMINALINPUT_H
#define TERMINALINPUT_H

#include <QWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QTextEdit>
#include <QMdiSubWindow>

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
    void showMessage(const QString& command);
protected:
    void focusInEvent(QFocusEvent *event);
    virtual QSize sizeHint() const;

public slots:
    void handleTerminalCommand();
};

#endif // TERMINALINPUT_H
