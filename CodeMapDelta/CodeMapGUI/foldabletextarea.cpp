#include "foldabletextarea.h"

#include <QTextDocumentFragment>

Q_DECLARE_METATYPE(QTextDocumentFragment)

FoldableTextArea::FoldableTextArea(QObject *parent) : QObject(parent) {
}

QSizeF FoldableTextArea::intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format) {
	Q_UNUSED(doc)
	Q_UNUSED(posInDocument)
	Q_ASSERT(format.type() == format.CharFormat);
	const QTextCharFormat &tf = reinterpret_cast<const QTextCharFormat&>(format);

	QFont fn = tf.font();
	QFontMetrics fm(fn);

	QString s("...");
	QSizeF sz = fm.boundingRect(s).size();
	return sz;
}

void FoldableTextArea::drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format) {

	Q_UNUSED(doc)
	Q_UNUSED(posInDocument)
	Q_ASSERT(format.type() == format.CharFormat);
	QString s("...");
	painter->drawText(rect, s);
	painter->drawRect(rect);
}

void FoldableTextArea::fold(QTextCursor c) {
	QTextCharFormat f;
	f.setObjectType(type());
	QVariant v;
	v.setValue(c.selection());
	f.setProperty(prop(), v);
	c.insertText(QString(QChar::ObjectReplacementCharacter), f);
}
bool FoldableTextArea::unfold(QTextCursor c) {
	if(!c.hasSelection()) {
		QTextCharFormat f = c.charFormat();
		if(f.objectType() == type()) {
			c.movePosition(c.Left, c.KeepAnchor);
			QVariant v = f.property(prop());
			auto q = v.value<QTextDocumentFragment>();
			c.insertFragment(q);
			return true;
		}
	}
	return false;
}