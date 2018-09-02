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


// ----------------------------------------

DocumentManager::DocumentManager(QWidget* parent) : QWidget(parent)
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
}

QSize DocumentManager::sizeHint() const
{
    QSize s = MainWindow::instance()->size();
    return QSize(s.rwidth()*8/10, s.rheight()*8/10);
}

FileView* DocumentManager::openFileView(const QString& path)
{
    const MainWindow* mainW = MainWindow::instance();
    mainW->getTerminalView()->showMessage("Opening: " + path);

    FileView* v = addNewFileView();
    v->openFile(path);
    mainW->getAppState().addFileView(path);

    // TODO: check if file is already open? allow to open it twice?
    // TODO: set app focus to this file view here.
    return v;
}

void DocumentManager::closeFileView(const QString& path)
{
    long long index = getFileViewIndexByName(path);
    if(index != INVALID_INDEX)
    {
        FileView* toRemove = fileViews[static_cast<unsigned long long>(index)];
        toRemove->closeView();
        fileViews.erase(fileViews.begin()+index);

        auto& state = MainWindow::instance()->getAppState();
        state.removeFileView(path);
        state.saveStateToDisk();
    }
}

FileView* DocumentManager::addNewFileView()
{
    auto view = new FileView(this);
    fileViews.push_back(view);
    splitter->addWidget(view);
    return view;
}

long long DocumentManager::getFileViewIndexByName(const QString& path)
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
