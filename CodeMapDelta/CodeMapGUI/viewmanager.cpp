#include "viewmanager.h"

#include <QDebug>

#include <QGridLayout>

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

void DocumentManager::openFileInFileView(QString& path)
{
    const MainWindow* mainW = MainWindow::instance();
    mainW->getTerminalView()->showMessage("Opening: " + path);

    addNewFileView();
    fileViews[fileCount-1]->openFile(path);

    mainW->getAppState().addFileView(path);
}

void DocumentManager::addNewFileView()
{
    fileViews.push_back(new FileView(this));
    fileCount++;
    layout->addWidget(fileViews[fileCount-1], 0, fileCount-1, 1, 1);
}
