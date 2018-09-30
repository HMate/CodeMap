#ifndef FILEEDIT_H
#define FILEEDIT_H

//#include <QTextEdit>
#include <QPlainTextEdit>

class FileEdit : public QPlainTextEdit
{
    QString filePath;
public:
    FileEdit(QWidget* parent);
    void setFilePath(const QString& path);

    void contextMenuEvent(QContextMenuEvent *event);

    void foldDefines();
};

#endif // FILEEDIT_H
