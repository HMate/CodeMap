#ifndef FILEEDIT_H
#define FILEEDIT_H

#include <QPlainTextEdit>
#include <QFutureWatcher>
#include <QTextBlock>

class FileView;

class FileEdit : public QPlainTextEdit
{
    Q_OBJECT

    QString m_FilePath;

    FileView* m_view;
    FileView* m_PreprocessedFileView;
    QFutureWatcher<QString> m_foldWatcher;
public:
    FileEdit(QWidget* parent);
    void setFilePath(const QString& path);
    const QString& getFilePath() { return m_FilePath; }

    void contextMenuEvent(QContextMenuEvent *event);

    void foldDefines(); 
    QString foldDefinesForFile(const QString& filePath) const;
    
    QPointF contentOffset() const { return QPlainTextEdit::contentOffset(); }
    QRectF blockBoundingGeometry(const QTextBlock& b) const { return QPlainTextEdit::blockBoundingGeometry(b); }
    QRectF blockBoundingRect(const QTextBlock& b) const { return QPlainTextEdit::blockBoundingRect(b); }
    QTextBlock firstVisibleBlock() const { return QPlainTextEdit::firstVisibleBlock(); }

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void foldDefinesFinished();
    void highlightCurrentLine();
};

#endif // FILEEDIT_H
