#include "viewmanager.h"

#include <QDebug>
#include <QStackedLayout>

#include "common_types.h"
#include "mainwindow.h"

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

void DocumentManager::openFileView(const QString& path)
{
    const MainWindow* mainW = MainWindow::instance();
    mainW->getTerminalView()->showMessage("Opening: " + path);

    addNewFileView();
    fileViews.back()->openFile(path);
    mainW->getAppState().addFileView(path);

    // TODO: check if file is already open? allow to open it twice?
    // TODO: set app focus to this file view here.
}

void DocumentManager::closeFileView(const QString& path)
{
    long long index = getFileViewIndexByName(path);
    if(index != INVALID_INDEX)
    {
        FileView* toRemove = fileViews[static_cast<unsigned long long>(index)];
        toRemove->closeView();
        fileViews.erase(fileViews.begin()+index);

        for(size_t i=0; i<fileViews.size(); i++)
        {
            splitter->addWidget(fileViews[i]);
        }

        auto& state = MainWindow::instance()->getAppState();
        state.removeFileView(path);
        state.saveStateToDisk();
    }
}

void DocumentManager::addNewFileView()
{
    fileViews.push_back(new FileView(this));
    auto fileIndex = fileViews.size()-1;
    qDebug() << "addWidget to pos " << fileIndex;
    splitter->addWidget(fileViews[fileIndex]);
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
