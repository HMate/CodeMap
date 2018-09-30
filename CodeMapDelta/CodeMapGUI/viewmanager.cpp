#include "viewmanager.h"

#include <QDebug>
#include <QPushButton>
#include <QStackedLayout>

#include "common_types.h"
#include "mainwindow.h"
#include "fileview.h"

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

// ------------------------------------------

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
    fileViews.push_back(view);
    tabs->addTab(view, name);
	connect(view, &FileView::gotFocus,
		this, &TabbedDocumentView::childGotFocus);
    return view;
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

void TabbedDocumentView::closeFileView(FileView* const view)
{
    long long index = getFileViewIndexByAddress(view);
    if(index != INVALID_INDEX)
    {
        FileView* toRemove = fileViews[static_cast<size_t>(index)];
        toRemove->closeView();
        tabs->removeTab(static_cast<int>(index));
        fileViews.erase(fileViews.begin()+index);
    }
}

bool TabbedDocumentView::hasFileView(FileView* const view)
{
    return getFileViewIndexByAddress(view) != INVALID_INDEX;
}

long long TabbedDocumentView::getFileViewIndexByAddress(FileView* const view)
{
    long long index = 0;
    for(auto f : fileViews)
    {
        if(f == view)
        {
            return index;
        }
        index++;
    }
    return INVALID_INDEX;
}


// ----------------------------------------

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
    const MainWindow* mainW = MainWindow::instance();
    mainW->getTerminalView()->showMessage("Opening: " + path);

    if(tabbedViews.size() < 2)
        addTabbedDocumentView();

    FileView* v = tabbedViews[1]->openStringFileView(path, content);
    return v;
}

void SplitDocumentView::closeFileView(FileView* const view)
{
    long long index = getDocumentViewIndexFromAddress(view);
    if(index != INVALID_INDEX)
    {
        tabbedViews[static_cast<size_t>(index)]->closeFileView(view);

        auto& state = MainWindow::instance()->getAppState();
        state.removeFileView(view->getFilePath(), index);
        state.saveStateToDisk();
    }
}

long long SplitDocumentView::getDocumentViewIndexFromAddress(FileView* const view)
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
