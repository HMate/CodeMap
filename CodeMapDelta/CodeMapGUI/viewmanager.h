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

public:
    explicit DocumentManager(QWidget *parent = nullptr);

    void openFileView(const QString& path);
    void closeFileView(const QString& path);
protected:
    void addNewFileView();
    size_t getFileViewIndexByName(const QString& path);
};

#endif // VIEWMANAGER_H
