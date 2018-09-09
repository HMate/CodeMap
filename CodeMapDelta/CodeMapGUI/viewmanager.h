#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <QString>
#include <QLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSplitter>
#include <QDockWidget>

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
class TabbedDocumentView : public QWidget
{
    Q_OBJECT

    QLayout* layout;
    QTabWidget* tabs;
    std::vector<FileView*> fileViews;

public:
    explicit TabbedDocumentView(QWidget *parent = nullptr);
    virtual ~TabbedDocumentView(){}

    FileView* openFileView(const QString& path);
    FileView* openStringFileView(const QString& path, const QString& content);
    void closeFileView(FileView* const view);
    bool hasFileView(FileView* const view);

protected:
    FileView* addNewFileView(const QString& name);
    long long getFileViewIndexByAddress(FileView* const view);

    virtual QSize sizeHint() const;
};

/*
 * Contains TabbedDocumentViews
 * */
class SplitDocumentView : public QWidget
{
    Q_OBJECT

    QLayout* layout;
    QSplitter* splitter;
    std::vector<TabbedDocumentView*> tabbedViews;

public:
    explicit SplitDocumentView(QWidget *parent = nullptr);
    virtual ~SplitDocumentView(){}

    void addTabbedDocumentView();
    FileView* openFileView(const QString& path);
    void closeFileView(FileView* const view);
    FileView* openStringFileView(const QString& path, const QString& content);
protected:
    long long getDocumentViewIndexFromAddress(FileView* const view);

    virtual QSize sizeHint() const;
};

class SplitDocumentViewHolder : public QDockWidget
{
    SplitDocumentView* view;
public:
    SplitDocumentViewHolder(QWidget* parent);
    SplitDocumentView* getView() {return view;}
};


class SplitDocumentViewTitleBar : public QWidget
{
    Q_OBJECT

    SplitDocumentViewHolder* parentDocHolder;
public:
    SplitDocumentViewTitleBar(SplitDocumentViewHolder* parent);

public slots:
   void addDocumentViewPushed();
};

#endif // VIEWMANAGER_H
