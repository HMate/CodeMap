#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <QString>
#include <QLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSplitter>
#include <QDockWidget>

class DiagramView;
class IncludeDiagramView;
class FileView;

/* A widget to show a list of documents in string representation.
*/
class DocumentListView : public QWidget
{
    Q_OBJECT

    QLayout* layout = nullptr;
    QListWidget* listView = nullptr;
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
 * Handles document views in tabs, that are editors or usually do something for files.
 * */
class TabbedDocumentView : public QWidget
{
    Q_OBJECT

    QLayout* layout = nullptr;
    QTabWidget* tabs = nullptr;
    std::vector<FileView*> fileViews;
    DiagramView* m_diagramView;

public:
    explicit TabbedDocumentView(QWidget *parent = nullptr);
    virtual ~TabbedDocumentView(){}

    FileView* openFileView(const QString& path);
    FileView* openStringFileView(const QString& path, const QString& content);
    IncludeDiagramView* openIncludeDiagramView(const QString& id);
    void closeFileView(FileView* const view);
    bool hasFileView(FileView* const view);
    void closeDiagramView(DiagramView* const view);

    bool event(QEvent *event);
protected:
    FileView* addNewFileView(const QString& name);
    int getTabIndexByAddress(QWidget* const view);

    virtual QSize sizeHint() const;

signals:
    void gotFocus(QObject*);

public slots:
    void childGotFocus();
};

// Contains TabbedDocumentViews side by side
class SplitDocumentView : public QWidget
{
    Q_OBJECT

    QLayout* layout = nullptr;
    QSplitter* splitter = nullptr;
    long long lastFocusedTab = 0;
    std::vector<TabbedDocumentView*> tabbedViews;

public:
    explicit SplitDocumentView(QWidget *parent = nullptr);
    virtual ~SplitDocumentView(){}

    void addTabbedDocumentView();
    FileView* openFileView(const QString& path, size_t tabIndex);
    FileView* openStringFileView(const QString& path, const QString& content);
    void closeFileView(FileView* const view);
    IncludeDiagramView* openIncludeDiagramView(const QString& id);
    void closeDiagramView(DiagramView* view);

    long long getLastFocusedTabIndex();
protected:
    long long getTabbedDocumentViewIndexFromAddress(FileView* const view);

    virtual QSize sizeHint() const;

public slots:
    void childGotFocus(QObject*);
};

// Wrapper class around SplitDocumentView and SplitDocumentViewTitleBar 
// to contain them in a single widget.
class SplitDocumentViewHolder : public QDockWidget
{
    SplitDocumentView* view = nullptr;
public:
    SplitDocumentViewHolder(QWidget* parent);
    SplitDocumentView* getView() {return view;}
};

// Header of a SplitDocumentView, contains buttons and lables to expand the views 
class SplitDocumentViewTitleBar : public QWidget
{
    Q_OBJECT

    SplitDocumentViewHolder* parentDocHolder = nullptr;
public:
    SplitDocumentViewTitleBar(SplitDocumentViewHolder* parent);

public slots:
   void addDocumentViewPushed();
};

#endif // VIEWMANAGER_H
