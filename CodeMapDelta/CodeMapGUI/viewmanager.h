#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <QString>
#include <QGridLayout>
//#include <QMdiArea>

#include "fileview.h"

/*
 * Handles document views, that are editors or usually do something for files.
 * */
class DocumentManager : public QWidget
{
    QGridLayout* layout;
    FileView* fileView;

public:
    explicit DocumentManager(QWidget *parent = nullptr);

    void openFileInFileView(QString& path);
};

#endif // VIEWMANAGER_H
