#include "mainwindow.h"

#include <QMenuBar>
#include <QFileDialog>
#include <QDockWidget>
#include <QResizeEvent>

#include "filesystem.h"

/* TODO
 * - Need to save/load appstate:
 *      - opened project
 *      - toggled views
 *      - app user options
 * - Views:
 *      - project / opened files view
 * - Project recognizer:
 *      - First allow users to add files by hand to a project (call it workspace?)
 *      - Walk includes and try to find them?
 *      - Parse cmake/sln descriptor and build project from that
 * - Fold out defines, includes, preprocessor directives
 *      - open view for it
 * - Control flow diagram
 * - Dependency graph, parameter/variable dependencies
 *      - function interpreter: put function to separate file, collect its dependencies, and call it in separate program.
 *      - this may need to mock input/ouput operations: maybe separate window for them to define them, trap them?
 *      - Custom includes window for these?
 *      - Ability to trace a variable's path through the code.
 *        ie. x was set: x = 7; and sued in function: foo(x)
 *        then it can show in foo(int p): p will be 7 on that invocation.
 *      - Also need mechanism to switch between invocations for a function.
 *        Means right click on function call-> use this invocation.
 *        Or choose from a visual function invocation graph?
 *      - This is interpreter like: define variable values in editor for tracing with this functionality?
 *        Also show code path in the source. Greys out code that wont be run with given variable values
 *        (unused branches of ann if).
 * - Draw own module graph, compare to generated
 * - Fold out templates
 * - Debugger(lldb?)
 */

MainWindow* MainWindow::instanceP = nullptr;

void MainWindow::createInstance(QApplication& app, QWidget *parent)
{
    if(!instanceP)
        // TODO: Set instanceP only one place.
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

    appState = std::make_unique<AppStateHandler>();

    // set docknesting so I can dragndrop a widget without tabifying it
    setDockNestingEnabled(true);

    docManager = new DocumentManager(this);
    docManager->setObjectName(QStringLiteral("documentManager"));
    auto documentsToggleAction = addDockedView(tr("Documents"), docManager, Qt::DockWidgetArea::TopDockWidgetArea);

    docList = new DocumentListView(this);
    docList->setObjectName(QStringLiteral("documentList"));
    auto documentListToggleAction = addDockedView(tr("Opened Documents"), docList, Qt::DockWidgetArea::LeftDockWidgetArea);

    terminalView = new TerminalView(this);
    auto terminalToggleAction = addDockedView(tr("Terminal"), terminalView, Qt::DockWidgetArea::BottomDockWidgetArea);

    // Create actions and menu after creating widgets,
    // as actions may use a child widget.
    createActions();
    createFileMenu();
    createViewMenu(documentsToggleAction, documentListToggleAction, terminalToggleAction);

    show();
    terminalView->setFocus();

    loadAppState();
}

void MainWindow::loadAppState()
{
    auto state = getAppState();

    state.loadStateFromDisk();

    QStringList fileViews = state.getFileViews();
    for(auto& f : fileViews)
    {
        const auto path = QDir::toNativeSeparators(f);
        docList->registerFile(path);
        docManager->openFileView(path);
    }
}

QAction* MainWindow::addDockedView(const QString& name, QWidget* widget, Qt::DockWidgetArea area)
{
    auto d = new QDockWidget(name, this);
    d->setWidget(widget);
    addDockWidget(area, d);
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
    connect(action.openFile, &QAction::triggered,
            this, &MainWindow::openFileWithDialog);

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

void MainWindow::createViewMenu(QAction* documentsToggle, QAction* documentListToggleAction, QAction* terminalToggle)
{
    QMenu* viewMenu = menuBar()->addMenu(tr("View"));
    viewMenu->addAction(documentsToggle);
    viewMenu->addAction(documentListToggleAction);
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
    dialog.setFileMode(QFileDialog::ExistingFile);

    const QString& lastDir = getAppState().getLastOpenedDirectory();
    if(QDir(lastDir).exists())
        dialog.setDirectory(lastDir);
    if(dialog.exec())
    {
        const QStringList& files = dialog.selectedFiles();
        const QString& f = files[0];
        if(QFile::exists(f))
        {
            docManager->openFileView(QDir::toNativeSeparators(f));
            getAppState().setLastOpenedDirectory(FS::getDirectory(f).absolutePath());
            getAppState().saveStateToDisk();
        }
    }
}

void MainWindow::quitApp()
{
    app.quit();
}

MainWindow::~MainWindow()
{
}
