#include "viewmanager.h"

#include <QDebug>
#include <QStackedLayout>

#include "common_types.h"
#include "mainwindow.h"

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
            listView->takeItem(listView->row(i));
        }

        auto it = std::find(files.begin(), files.end(), path);
        files.erase(it);
    }
}

void DocumentListView::selectionChanged(QListWidgetItem *current)
{
    auto dm = MainWindow::instance()->getDocumentManager();
    dm->openFileView(current->text())->setFocus();
}

// ------------------------------------------

TabbedDocumentView::TabbedDocumentView(QWidget* parent) : QWidget(parent)
{
    //setBackground(QBrush(QColor(50, 120, 50)));
    setStyleSheet("QTextEdit { background-color: rgb(120, 180, 120) }");

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
    const MainWindow* mainW = MainWindow::instance();
    mainW->getTerminalView()->showMessage("Opening: " + path);

    FileView* v = addNewFileView(path);
    v->openFile(path);
    mainW->getAppState().addFileView(path);

    // TODO: check if file is already open? allow to open it twice?
    // TODO: set app focus to this file view here.
    return v;
}

void TabbedDocumentView::closeFileView(const QString& path)
{
    long long index = getFileViewIndexByName(path);
    if(index != INVALID_INDEX)
    {
        FileView* toRemove = fileViews[static_cast<size_t>(index)];
        toRemove->closeView();
        tabs->removeTab(static_cast<int>(index));
        fileViews.erase(fileViews.begin()+index);

        auto& state = MainWindow::instance()->getAppState();
        state.removeFileView(path);
        state.saveStateToDisk();
    }
}

FileView* TabbedDocumentView::addNewFileView(const QString& name)
{
    auto view = new FileView();
    fileViews.push_back(view);
    tabs->addTab(view, name);
    return view;
}

bool TabbedDocumentView::hasFileView(const QString& path)
{
    return getFileViewIndexByName(path) != INVALID_INDEX;
}

long long TabbedDocumentView::getFileViewIndexByName(const QString& path)
{
    long long index = 0;
    for(auto& f : fileViews)
    {
        if(f->getFilePath() == path)
        {
            return index;
        }
        index++;
    }
    return INVALID_INDEX;
}


// ----------------------------------------

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

    auto v = new TabbedDocumentView(this);
    tabbedViews.emplace_back(v);
    splitter->addWidget(v);
}

QSize SplitDocumentView::sizeHint() const
{
    QSize s = MainWindow::instance()->size();
    return QSize(s.rwidth()*8/10, s.rheight()*8/10);
}

FileView* SplitDocumentView::openFileView(const QString& path)
{
    FileView* v = tabbedViews[0]->openFileView(path);

    // TODO: check if file is already open? allow to open it twice?
    // TODO: set app focus to this file view here.
    return v;
}

void SplitDocumentView::closeFileView(const QString& path)
{
    long long index = getDocumentViewIndexFromFileName(path);
    if(index != INVALID_INDEX)
    {
        tabbedViews[static_cast<size_t>(index)]->closeFileView(path);
    }
}

long long SplitDocumentView::getDocumentViewIndexFromFileName(const QString& path)
{
    long long index = 0;
    for(auto& view : tabbedViews)
    {
        if(view->hasFileView(path))
        {
            return index;
        }
        index++;
    }
    return INVALID_INDEX;
}
