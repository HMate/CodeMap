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
        QAction* openSettingsView;
        QAction* quit;

        QAction* toggleTerminalHistory;
    };
    Actions action;

    std::unique_ptr<AppStateHandler> appState;
    SplitDocumentViewHolder* docManager = nullptr;
    DocumentListView* docList =  nullptr;
    TerminalView* terminalView = nullptr;

    QApplication& app;

    static MainWindow* instanceP;
    explicit MainWindow(QApplication& app, QWidget *parent = nullptr);

    void loadAppState();
    void createActions();
    void createFileMenu();
    void createViewMenu(QAction* documentsToggle, QAction* documentListToggleAction, QAction* terminalToggle);
    virtual void resizeEvent(QResizeEvent *resizeEvent) override;

public:
    static void createInstance(QApplication& app, QWidget *parent = nullptr);
    static const MainWindow* instance();
    ~MainWindow();

    QAction* createAddDockedView(const QString& name, QWidget* widget, Qt::DockWidgetArea area);
    QAction* addDockedView(QDockWidget* widget, Qt::DockWidgetArea area);

    AppStateHandler& getAppState() const {return *appState;}
    TerminalView* getTerminalView() const {return terminalView;}
    SplitDocumentView* getDocumentManager() const {return docManager->getView();}
    const Actions& getActions() const {return action;}

public slots:
    void toggleViewTerminalHistory(bool onoff);
    void openFileWithDialog();
    void openSettingsView();
    void quitApp();

};

#endif // MAINWINDOW_H
