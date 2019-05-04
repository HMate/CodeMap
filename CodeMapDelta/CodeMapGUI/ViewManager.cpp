#include "ViewManager.h"

#include <QDebug>
#include <QPushButton>
#include <QStackedLayout>

#include "CommonTypes.h"
#include "MainWindow.h"
#include "FileView.h"
#include "IncludeDiagramView.h"
#include "FileSystem.h"

DocumentListView::DocumentListView(QWidget* parent) : QWidget(parent)
{
    layout = new QVBoxLayout(this);
    layout->setMargin(0);

    listView = new QListWidget(this);
    layout->addWidget(listView);

    connect(listView, &QListWidget::itemDoubleClicked,
            this, &DocumentListView::selectionChanged);
}

QSize DocumentListView::sizeHint() const
{
    QSize s = MainWindow::instance()->size();
    return QSize(s.rwidth()*2/10, s.rheight());
}

void DocumentListView::registerFile(const QString& path)
{
    if(!isFilepathRegistered(path))
    {
        files.emplace_back(path);
        listView->addItem(path);
    }
}

bool DocumentListView::isFilepathRegistered(const QString& path)
{
    return std::find(files.begin(), files.end(), path) != files.end();
}

void DocumentListView::removeFile(const QString& path)
{
    if(isFilepathRegistered(path))
    {
        auto items = listView->findItems(path, Qt::MatchFixedString);
        for(const auto& i : items)
        {
            auto it = listView->takeItem(listView->row(i));
            delete it;
        }

        auto it = std::find(files.begin(), files.end(), path);
        files.erase(it);
    }
}

void DocumentListView::selectionChanged(QListWidgetItem *current)
{
    auto dm = MainWindow::instance()->getDocumentManager();
    const auto& path = current->text();
    long long tabIndex = dm->getLastFocusedTabIndex();
    dm->openFileView(path, tabIndex)->setFocus();

    auto& state = MainWindow::instance()->getAppState();
    state.addFileView(path, tabIndex);
    state.saveStateToDisk();
}

// #############################################################################################################

TabbedDocumentView::TabbedDocumentView(QWidget* parent) : QWidget(parent)
{
    //setBackground(QBrush(QColor(50, 120, 50)));
    setStyleSheet("QPlainTextEdit { background-color: rgb(120, 180, 120) }");

    setSizePolicy(QSizePolicy(QSizePolicy::Policy::Expanding,
                              QSizePolicy::Policy::Expanding));

    layout = new QHBoxLayout(this);
    layout->setMargin(0);

    tabs = new QTabWidget(this);
    tabs->show();
    layout->addWidget(tabs);
}

QSize TabbedDocumentView::sizeHint() const
{
    QSize s = MainWindow::instance()->size();
    return QSize(s.rwidth()*8/10, s.rheight()*8/10);
}

FileView* TabbedDocumentView::openFileView(const QString& path)
{
    FileView* v = addNewFileView(path);
    v->openFile(path);
    return v;
}

FileView* TabbedDocumentView::openStringFileView(const QString& path,
                                                const QString& content)
{
    FileView* v = addNewFileView(path);
    v->setText(content);
    return v;
}

FileView* TabbedDocumentView::addNewFileView(const QString& name)
{
    auto view = new FileView(this);
    //fileViews.push_back(view);
    connect(view, &FileView::gotFocus,
        this, &TabbedDocumentView::childGotFocus);

    QString tabName = name;
    if(FS::doesFileExist(tabName))
    {
        tabName = QFileInfo(tabName).fileName();
    }
    logTerminal(tr("adding tab with name %1").arg(tabName));
    tabs->addTab(view, tabName);

    return view;
}

IncludeDiagramView* TabbedDocumentView::openIncludeDiagramView(const QString& id)
{
    auto view = new IncludeDiagramView(this);
    view->setId(id);
    logTerminal(tr("adding tab with id %1").arg(id));
    tabs->addTab(view, id);
    return view;
}

void TabbedDocumentView::closeFileView(FileView* const view)
{
    long long index = getTabIndexByAddress(view);
    if(index != INVALID_INDEX)
    {
        //FileView* toRemove = fileViews[static_cast<size_t>(index)];
        //toRemove->closeView();
        tabs->widget(index)->close();
        tabs->removeTab(static_cast<int>(index));
        //fileViews.erase(fileViews.begin()+index);
    }
}

void TabbedDocumentView::closeDiagramView(DiagramView* const view)
{
    long long index = getTabIndexByAddress(view);
    if(index != INVALID_INDEX)
    {
        tabs->widget(index)->close();
        tabs->removeTab(index);
    }
}

bool TabbedDocumentView::hasFileView(FileView* const view)
{
    return getTabIndexByAddress(view) != INVALID_INDEX;
}

int TabbedDocumentView::getTabIndexByAddress(QWidget* const view)
{
    return tabs->indexOf(view);
}

bool TabbedDocumentView::event(QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)
    {
        setFocus(Qt::FocusReason::MouseFocusReason);
        // TODO log MainWindow::instance()->getTerminalView()->showMessage(tr("last tab is %1").arg((long long)this));
        gotFocus(this);
    }
    return false;
}

void TabbedDocumentView::childGotFocus()
{
    gotFocus(this);
}


// #############################################################################################################

SplitDocumentViewHolder::SplitDocumentViewHolder(QWidget* parent) : QDockWidget (tr("Documents"), parent)
{
    view = new SplitDocumentView(this);
    this->setWidget(view);

    auto titleBar = new SplitDocumentViewTitleBar(this);
    titleBar->show();
    setTitleBarWidget(titleBar);
}

SplitDocumentViewTitleBar::SplitDocumentViewTitleBar(SplitDocumentViewHolder* parent) : QWidget (parent)
{
    parentDocHolder = parent;
    auto layout = new QHBoxLayout();
    setLayout(layout);
    layout->addWidget(new QLabel("Documents", this));

    auto addDocumentViewButton = new QPushButton("++", this);
    layout->addWidget(addDocumentViewButton);
    
    connect(addDocumentViewButton, &QPushButton::clicked,
            this, &SplitDocumentViewTitleBar::addDocumentViewPushed);
}

void SplitDocumentViewTitleBar::addDocumentViewPushed()
{
    parentDocHolder->getView()->addTabbedDocumentView();
};

SplitDocumentView::SplitDocumentView(QWidget* parent) : QWidget(parent)
{
    //setBackground(QBrush(QColor(50, 120, 50)));
    setStyleSheet("QTextEdit { background-color: rgb(120, 180, 120) }");

    setSizePolicy(QSizePolicy(QSizePolicy::Policy::Expanding,
                              QSizePolicy::Policy::Expanding));

    layout = new QHBoxLayout(this);
    layout->setMargin(0);

    splitter = new QSplitter(this);
    splitter->show();
    layout->addWidget(splitter);

    addTabbedDocumentView();
}

QSize SplitDocumentView::sizeHint() const
{
    QSize s = MainWindow::instance()->size();
    return QSize(s.rwidth()*8/10, s.rheight()*8/10);
}

void SplitDocumentView::addTabbedDocumentView()
{
    auto v = new TabbedDocumentView(this);
    tabbedViews.emplace_back(v);
    splitter->addWidget(v);

    v->installEventFilter(this);
    connect(v, &TabbedDocumentView::gotFocus,
        this, &SplitDocumentView::childGotFocus);
}

void SplitDocumentView::childGotFocus(QObject* focused)
{
    for(auto i = 0; i < tabbedViews.size(); i++)
    {
        auto tab = tabbedViews[i];
        if(tab == focused)
        {
            // TODO log MainWindow::instance()->getTerminalView()->showMessage(tr("last tab is %1").arg(i));
            lastFocusedTab = i;
        }
    }
}

long long SplitDocumentView::getLastFocusedTabIndex()
{
    return lastFocusedTab;
}

FileView* SplitDocumentView::openFileView(const QString& path, size_t tabIndex)
{
    const MainWindow* mainW = MainWindow::instance();
    mainW->getTerminalView()->showMessage( tr("Opening in tab %1: %2").arg(tabIndex).arg(path));

    while(tabIndex >= tabbedViews.size())
    {
        addTabbedDocumentView();
    }

    FileView* v = tabbedViews[tabIndex]->openFileView(path);
    
    // TODO: check if file is already open? allow to open it twice?
    // TODO: set app focus to this file view here.
    return v;
}

FileView* SplitDocumentView::openStringFileView(const QString& path,
                                                const QString& content)
{
    logTerminal("Opening: " + path);

    if(tabbedViews.size() < 2)
        addTabbedDocumentView();

    // TODO: Dont use burnt in indicies for tabbedviews
    // I guess the index should be the next after the currently selected view
    FileView* v = tabbedViews[1]->openStringFileView(path, content);
    return v;
}

void SplitDocumentView::closeFileView(FileView* const view)
{
    long long index = getTabbedDocumentViewIndexFromAddress(view);
    if(index != INVALID_INDEX)
    {
        tabbedViews[static_cast<size_t>(index)]->closeFileView(view);

        auto& state = MainWindow::instance()->getAppState();
        state.removeFileView(view->getFilePath(), index);
        state.saveStateToDisk();
    }
}

IncludeDiagramView* SplitDocumentView::openIncludeDiagramView(const QString& id)
{
    logTerminal("Opening diagram");

    // TODO: Dont use burnt in indicies for tabbedviews -> closeDiagramView has to use the same index.
    size_t usedTab = 0;
    if(tabbedViews.size() < usedTab+1)
        addTabbedDocumentView();

    return tabbedViews[usedTab]->openIncludeDiagramView(id);
}

void SplitDocumentView::closeDiagramView(DiagramView* view)
{
    size_t usedTab = 0;
    tabbedViews[usedTab]->closeDiagramView(view);
}

long long SplitDocumentView::getTabbedDocumentViewIndexFromAddress(FileView* const view)
{
    long long index = 0;
    for(auto& t : tabbedViews)
    {
        if(t->hasFileView(view))
        {
            return index;
        }
        index++;
    }
    return INVALID_INDEX;
}
