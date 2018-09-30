#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H

#include <QWidget>

#include "fileedit.h"

class LineNumberArea : public QWidget
{
	Q_OBJECT

public:
	LineNumberArea(FileEdit *editor) : QWidget(qobject_cast<QWidget*>(editor)) {
		codeEditor = editor;
	}

	QSize sizeHint() const override;

protected:
	void paintEvent(QPaintEvent *event) override;

private:
	FileEdit *codeEditor;

};

#endif // LINENUMBERAREA_H