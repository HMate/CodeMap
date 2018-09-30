#ifndef FILEEDIT_H
#define FILEEDIT_H

#include <QPlainTextEdit>

class LineNumberArea;

class FileEdit : public QPlainTextEdit
{
	Q_OBJECT

    QString filePath;
	LineNumberArea* lineNumberArea;
public:
    FileEdit(QWidget* parent);
    void setFilePath(const QString& path);

    void contextMenuEvent(QContextMenuEvent *event);

    void foldDefines();

	void lineNumberAreaPaintEvent(QPaintEvent *event);
	int lineNumberAreaWidth();

protected:
	void resizeEvent(QResizeEvent *event) override;

private slots:
	void updateLineNumberAreaWidth(int newBlockCount);
	void highlightCurrentLine();
	void updateLineNumberArea(const QRect &, int);
};

#endif // FILEEDIT_H
