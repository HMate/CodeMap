#include "viewmanager.h"

#include <QDebug>

#include <QFile>
#include <QTextStream>
#include <QGridLayout>

#include "mainwindow.h"

DocumentManager::DocumentManager(QWidget* parent) : QWidget(parent)
{
    //setBackground(QBrush(QColor(50, 120, 50)));
    setStyleSheet("QTextEdit { background-color: rgb(120, 180, 120) }");
//    auto& s = size();
//    qDebug() << QStringLiteral("Size: %1x%2 r: %3x%4").
//           arg(s.width()).arg(s.height()).arg(s.rwidth()).arg(s.rheight());

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
    mainW->getTerminalView()->registerCommand("Opening: " + path);

    // TODO: Show error, or log it somewhere if not exists?
    if(!QFile::exists(path))
        return;

    fileView->setFilePath(path);
    fileView->showMaximized();

    QFile file(path);

    // TODO: Show error, or log it somewhere if not exists?
    if(!file.open(QFile::ReadOnly | QFile::Text))
        return;

    QTextStream ReadFile(&file);
    fileView->setText(ReadFile.readAll());
}
