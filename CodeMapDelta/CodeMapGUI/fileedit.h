#ifndef FILEEDIT_H
#define FILEEDIT_H

#include <QPlainTextEdit>
#include <QFutureWatcher>

#include "textfolder.h"

class LineNumberArea;
class FileView;

class FileEdit : public QPlainTextEdit
{
    Q_OBJECT

    QString m_FilePath;
    FileView* m_PreprocessedFileView;
    LineNumberArea* m_lineNumberArea;
    TextFolder* m_regionFolder;
    QFutureWatcher<QString> foldWatcher;
public:
    FileEdit(QWidget* parent);
    void setFilePath(const QString& path);

    void contextMenuEvent(QContextMenuEvent *event);

    void foldDefines(); 
    QString foldDefinesForFile(const QString& filePath) const;

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    void fold();
    void unfold();
protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void foldDefinesFinished();

    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

    friend class LineNumberArea;
};

#endif // FILEEDIT_H
