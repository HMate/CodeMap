#ifndef FILEEDIT_H
#define FILEEDIT_H

#include <QTextEdit>

class FileEdit : public QTextEdit
{
    QString filePath;
public:
    FileEdit(QWidget* parent);
    void setFilePath(const QString& path);

    void contextMenuEvent(QContextMenuEvent *event);

    void foldDefines();
};

#endif // FILEEDIT_H
