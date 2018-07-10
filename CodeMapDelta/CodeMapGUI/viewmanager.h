#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <QString>
#include <QGridLayout>

#include "fileview.h"

/*
 * Handles document views, that are editors or usually do something for files.
 * */
class DocumentManager : public QWidget
{
    QGridLayout* layout;
    std::vector<FileView*> fileViews;
    int fileCount = 0;

public:
    explicit DocumentManager(QWidget *parent = nullptr);

    void openFileInFileView(QString& path);
protected:
    void addNewFileView();
};

#endif // VIEWMANAGER_H
