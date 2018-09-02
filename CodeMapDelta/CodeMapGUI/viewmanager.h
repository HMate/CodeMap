#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <QString>
#include <QLayout>
#include <QSplitter>

#include "fileview.h"

/*
 * Handles document views, that are editors or usually do something for files.
 * */
class DocumentManager : public QWidget
{
    QLayout* layout;
    QSplitter* splitter;
    std::vector<FileView*> fileViews;

public:
    explicit DocumentManager(QWidget *parent = nullptr);

    void openFileView(const QString& path);
    void closeFileView(const QString& path);
protected:
    void addNewFileView();
    long long getFileViewIndexByName(const QString& path);
};

#endif // VIEWMANAGER_H
