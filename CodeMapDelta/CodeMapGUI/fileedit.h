#ifndef FILEEDIT_H
#define FILEEDIT_H

#include <QTextEdit>

class FileEdit : public QTextEdit
{
public:
    FileEdit(QWidget* parent);

    void contextMenuEvent(QContextMenuEvent *event);

    void foldDefines();
};

#endif // FILEEDIT_H
