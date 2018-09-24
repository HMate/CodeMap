#include "mainwindow.h"

#include <QMenuBar>
#include <QFileDialog>
#include <QDockWidget>
#include <QResizeEvent>

#include "filesystem.h"
#include "settingsview.h"

/* DONE
	- Need to save/load appstate:
		- Save opened files
	- Views:
		- Opened fies view
	- Fold out defines
*/

/* TODO
	- Need to save/load appstate:
		- opened project
		- toggled views
		- app user options
	- Views:
		- project view
	- Project recognizer:
		- First allow users to add files by hand to a project (call it workspace?)
		- Walk includes and try to find them?
  			- For now just give includes in settings.
  			- Generate and parse compile_commands.json somehow.
		- Parse cmake/sln descriptor and build project from that
  			- (Failed) Cmake cannot be used as a library, too much hassle.
	- Fold out defines, includes, preprocessor directives
		- create collapsible regions for #includes
	- Syntax highlight
	- Control flow diagram
	- Dependency graph, parameter/variable dependencies
		- function interpreter: put function to separate file, collect its dependencies, and call it in separate program.
		- this may need to mock input/ouput operations: maybe separate window for them to define them, trap them?
		- Custom includes window for these?
		- Ability to trace a variable's path through the code.
			ie. x was set: x = 7; and used in function: foo(x)
			then it can show in foo(int p): p will be 7 on that invocation.
		- Also need mechanism to switch between invocations for a function.
			Means right click on function call-> use this invocation.
			Or choose from a visual function invocation graph?
		- This is interpreter like: define variable values in editor for tracing with this functionality?
			And show other variables values in live.
			Also show code paths in the source. Grey out code that wont be run with given variable values
			(unused branches of an if).
	- Draw own module graph, compare to generated
	- Fold out templates
	- Debugger(lldb?)
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

    docManager = new SplitDocumentViewHolder(this);
    docManager->setObjectName(QStringLiteral("documentManager"));
    auto documentsToggleAction = addDockedView(docManager, Qt::DockWidgetArea::RightDockWidgetArea);

    terminalView = new TerminalView(this);
    auto terminalToggleAction = createAddDockedView(tr("Terminal"), terminalView, Qt::DockWidgetArea::RightDockWidgetArea);

    docList = new DocumentListView(this);
    docList->setObjectName(QStringLiteral("documentList"));
    auto documentListToggleAction = createAddDockedView(tr("Opened Documents"), docList, Qt::DockWidgetArea::LeftDockWidgetArea);

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
    // TODO: look into QMainWindow::saveState for saving dockwidget state
	// TODO: look into QSettings for storing, loading window position and size.
    auto& state = getAppState();

    state.loadStateFromDisk();

	const auto& fileViews = state.getFileViews();
    for(auto& f : fileViews)
    {
        const auto path = QDir::toNativeSeparators(f.path);
        docList->registerFile(path);
        getDocumentManager()->openFileView(path, f.tabIndex);
    }
}

QAction* MainWindow::createAddDockedView(const QString& name, QWidget* widget, Qt::DockWidgetArea area)
{
    auto d = new QDockWidget(name, this);
    d->setWidget(widget);
    return addDockedView(d, area);
}

QAction* MainWindow::addDockedView(QDockWidget* widget, Qt::DockWidgetArea area)
{
    addDockWidget(area, widget);
    return widget->toggleViewAction();
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

	action.openSettingsView = new QAction(tr("Settings"), this);
	connect(action.openSettingsView, &QAction::triggered,
		this, &MainWindow::openSettingsView);

    action.quit = new QAction(tr("Quit"), this);
    connect(action.quit, &QAction::triggered,
            this, &MainWindow::quitApp);
}

void MainWindow::createFileMenu()
{
    QMenu* fileMenu = menuBar()->addMenu(tr("File"));
    fileMenu->addAction(action.openFile);
	fileMenu->addSeparator();
	fileMenu->addAction(action.openSettingsView);
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

	auto& state = getAppState();
    const QString& lastDir = state.getLastOpenedDirectory();
    if(QDir(lastDir).exists())
        dialog.setDirectory(lastDir);
    if(dialog.exec() == QDialog::Accepted)
    {
        const QStringList& files = dialog.selectedFiles();
        const QString& f = files[0];
        if(QFile::exists(f))
        {
            const auto path = QDir::toNativeSeparators(f);
			auto docManager = getDocumentManager();
			long long tabIndex = docManager->getLastFocusedTabIndex();
			docManager->openFileView(path, tabIndex);
            docList->registerFile(path);

			state.addFileView(path, tabIndex);
            state.setLastOpenedDirectory(FS::getDirectory(path).absolutePath());
            state.saveStateToDisk();
        }
    }
}

void MainWindow::openSettingsView()
{
	auto settings = new SettingsView(this);
	settings->exec();
	delete settings;
}

void MainWindow::quitApp()
{
    app.quit();
}

MainWindow::~MainWindow()
{
}
