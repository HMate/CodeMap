#include "viewmanager.h"

#include <QMdiSubWindow>

#include "mainwindow.h"

DocumentManager::DocumentManager(QWidget* parent) : QMdiArea(parent)
{
    setBackground(QBrush(QColor(50, 120, 50)));
    setStyleSheet("QTextEdit { background-color: rgb(120, 180, 120) }");
}

