#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H

#include <QWidget>

// Has to include class because it inherits QWidget, and that makes it qobject_cast-able
#include "fileedit.h"

class LineNumberArea : public QWidget
{
    Q_OBJECT

public:
    LineNumberArea(FileEdit *editor) : QWidget(qobject_cast<QWidget*>(editor)) {
        codeEditor = editor;
    }

    QSize sizeHint() const override;
    int totalWidth() const;
    int numberAreaWidth() const;
    int foldAreaWidth() const;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    FileEdit *codeEditor;

};

#endif // LINENUMBERAREA_H