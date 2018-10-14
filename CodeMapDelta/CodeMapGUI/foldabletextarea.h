#ifndef FOLDABLETEXTAREA_H
#define FOLDABLETEXTAREA_H

// Source: https://github.com/CapelliC/foldingQTextEdit

#include <QObject>
#include <QPainter>
#include <QTextDocument>
#include <QTextObjectInterface>

class FoldableTextArea : public QObject, public QTextObjectInterface
{
	Q_OBJECT
	Q_INTERFACES(QTextObjectInterface)

	QString m_PlaceholderText;

public:
	explicit FoldableTextArea(QObject *parent = 0);
	explicit FoldableTextArea(QObject *parent, const QString& placeholderText);

	static int type() { return QTextFormat::UserObject + 1; }
	static int prop() { return 1; }

	QSizeF intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format);
	void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format);

	void fold(QTextCursor c);
	bool unfold(QTextCursor c);
};

#endif // FOLDABLETEXTAREA_H