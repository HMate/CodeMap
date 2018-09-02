#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>

#include "viewmanager.h"
#include "terminalgui.h"
#include "appstate.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    struct Actions {
        QAction* openFile;
        QAction* quit;

        QAction* toggleTerminalHistory;
    };
    Actions action;

    std::unique_ptr<AppStateHandler> appState;
    DocumentManager* docManager = nullptr;
    TerminalView* terminalView = nullptr;

    QApplication& app;

    static MainWindow* instanceP;
    explicit MainWindow(QApplication& app, QWidget *parent = nullptr);

    void loadAppState();
    void createActions();
    void createFileMenu();
    void createViewMenu(QAction* documentsToggle, QAction* terminalToggle);
    virtual void resizeEvent(QResizeEvent *resizeEvent) override;

public:
    static void createInstance(QApplication& app, QWidget *parent = nullptr);
    static const MainWindow* instance();
    ~MainWindow();

    QAction* addDockedView(const QString& name, QWidget* widget, Qt::DockWidgetArea area);

    AppStateHandler& getAppState() const {return *appState;}
    TerminalView* getTerminalView() const {return terminalView;}
    DocumentManager* getDocumentManager() const {return docManager;}
    const Actions& getActions() const {return action;}
signals:

public slots:
    void toggleViewTerminalHistory(bool onoff);
    void openFileWithDialog();
    void quitApp();

};

#endif // MAINWINDOW_H
