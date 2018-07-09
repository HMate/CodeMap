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

    fileView = new FileView(this);
    layout->addWidget(fileView, 0, 0, 1, 1);

}

void DocumentManager::openFileInFileView(QString& path)
{
    const MainWindow* mainW = MainWindow::instance();
    mainW->getTerminalView()->showMessage("Opening: " + path);

    fileView->openFile(path);

    mainW->getAppState().addFileView(path);
}
