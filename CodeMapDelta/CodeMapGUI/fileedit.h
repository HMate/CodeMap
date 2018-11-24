#ifndef FILEEDIT_H
#define FILEEDIT_H

#include <QPlainTextEdit>
#include <QFutureWatcher>


class FileView;

class FileEdit : public QPlainTextEdit
{
    Q_OBJECT

    QString m_FilePath;
    
    FileView* m_PreprocessedFileView;
    QFutureWatcher<QString> m_foldWatcher;
public:
    FileEdit(QWidget* parent);
    void setFilePath(const QString& path);
    const QString& getFilePath() { return m_FilePath; }

    void contextMenuEvent(QContextMenuEvent *event);

    void foldDefines(); 
    QString foldDefinesForFile(const QString& filePath) const;
    
private slots:
    void foldDefinesFinished();
    void highlightCurrentLine();

    friend class LineNumberArea;
    friend class EditorFoldingArea;
};

#endif // FILEEDIT_H
