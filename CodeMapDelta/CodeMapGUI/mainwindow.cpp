#include "mainwindow.h"

#include <QMenuBar>
#include <QFileDialog>
#include <QDockWidget>
#include <QResizeEvent>

/* TODO
 * - Need to save/load appstate:
 *      - last opened files
 *      - last directories searched
 *      - toggled views
 *      - app user options
 * - Control flow diagram
 * - Debugger(lldb?), function interpreter
 * - Dependency graph, parameter/variable dependencies
 * -
 */

MainWindow* MainWindow::instanceP = nullptr;

void MainWindow::createInstance(QApplication& app, QWidget *parent)
{
    if(!instanceP)
        instanceP = new MainWindow(app, parent);
}

const MainWindow* MainWindow::instance()
{
    Q_ASSERT(instanceP != nullptr);
    return instanceP;
}

MainWindow::MainWindow(QApplication& app, QWidget *parent) :
    QMainWindow(parent), app(app)
{
    instanceP = this;

    resize(1240, 760);

    docManager = new DocumentManager(this);
    docManager->setObjectName(QStringLiteral("documentManager"));
    setCentralWidget(docManager);

    terminalView = new TerminalView(this);
    auto d = addDockedView(tr("Terminal"), terminalView);

    // Create actions and menu after creating widgets,
    // as actions may use a child widget.
    createActions();
    createFileMenu();
    createViewMenu(d);

    show();
    terminalView->setFocus();
}

QAction* MainWindow::addDockedView(const QString& name, QWidget* widget)
{
    QDockWidget* d = new QDockWidget(name, this);
    d->setWidget(widget);
    addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, d);
    return d->toggleViewAction();
}

void MainWindow::resizeEvent(QResizeEvent *resizeEvent)
{
    QMainWindow::resizeEvent(resizeEvent);
    //Set size of childs after resize
    // auto s = resizeEvent->size();
    // viewManager->getTerminalView()->resizeAfterParent(s);
}

void MainWindow::createActions()
{
    action.openFile = new QAction(tr("Open File"), this);
    connect(action.openFile, &QAction::triggered, this, &MainWindow::openFileWithDialog);

    action.quit = new QAction(tr("Quit"), this);
    connect(action.quit, &QAction::triggered,
            this, &MainWindow::quitApp);
}

void MainWindow::createFileMenu()
{
    QMenu* fileMenu = menuBar()->addMenu(tr("File"));
    fileMenu->addAction(action.openFile);
    fileMenu->addAction(action.quit);
}

void MainWindow::createViewMenu(QAction* terminalToggle)
{
    QMenu* viewMenu = menuBar()->addMenu(tr("View"));
    viewMenu->addAction(terminalToggle);

}

void MainWindow::toggleViewTerminalHistory(bool onoff)
{
    auto terminal = getTerminalView();
    terminal->setVisible(onoff);
}

void MainWindow::openFileWithDialog()
{
    QFileDialog dialog(this);
    if(dialog.exec())
    {
        QStringList& files = dialog.selectedFiles();
        docManager->openFileInFileView(files[0]);
    }
}

void MainWindow::quitApp()
{
    app.quit();
}

MainWindow::~MainWindow()
{
}
