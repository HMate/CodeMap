#include "viewmanager.h"

#include <QDebug>

#include <QGridLayout>

#include "common_types.h"
#include "mainwindow.h"

DocumentManager::DocumentManager(QWidget* parent) : QWidget(parent)
{
    //setBackground(QBrush(QColor(50, 120, 50)));
    setStyleSheet("QTextEdit { background-color: rgb(120, 180, 120) }");

    setSizePolicy(QSizePolicy(QSizePolicy::Policy::Expanding,
                              QSizePolicy::Policy::Expanding));

    layout = new QGridLayout(this);
    layout->setMargin(0);
}

void DocumentManager::openFileView(const QString& path)
{
    const MainWindow* mainW = MainWindow::instance();
    mainW->getTerminalView()->showMessage("Opening: " + path);

    addNewFileView();
    fileViews[fileCount-1]->openFile(path);
    mainW->getAppState().addFileView(path);

    // TODO: check if file is already open? allow to open it twice?
    // TODO: set app focus to this file view here.
}

void DocumentManager::closeFileView(const QString& path)
{
    size_t index = getFileViewIndexByName(path);
    if(index != INVALID_INDEX)
    {
        fileViews[index]->closeView();
        fileViews.erase(fileViews.begin()+index);
        auto& state = MainWindow::instance()->getAppState();
        state.removeFileView(path);
        state.saveStateToDisk();
    }
}

void DocumentManager::addNewFileView()
{
    fileViews.push_back(new FileView(this));
    fileCount++;
    layout->addWidget(fileViews[fileCount-1], 0, fileCount-1, 1, 1);
}

size_t DocumentManager::getFileViewIndexByName(const QString& path)
{
    size_t index = 0;
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
