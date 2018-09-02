#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <QString>
#include <QLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSplitter>

#include "fileview.h"

class DocumentListView : public QWidget
{
    Q_OBJECT

    QLayout* layout;
    QListWidget* listView;
    std::vector<QString> files;
public:
    explicit DocumentListView(QWidget *parent = nullptr);

    void registerFile(const QString& path);
    void removeFile(const QString& path);
    bool isFilepathRegistered(const QString& path);
protected:
    virtual QSize sizeHint() const;
public slots:
   void selectionChanged(QListWidgetItem *current);
};

/*
 * Handles document views, that are editors or usually do something for files.
 * */
class DocumentManager : public QWidget
{
    Q_OBJECT

    QLayout* layout;
    QSplitter* splitter;
    std::vector<FileView*> fileViews;

public:
    explicit DocumentManager(QWidget *parent = nullptr);

    FileView* openFileView(const QString& path);
    void closeFileView(const QString& path);
protected:
    FileView* addNewFileView();
    long long getFileViewIndexByName(const QString& path);

    virtual QSize sizeHint() const;
};

#endif // VIEWMANAGER_H
