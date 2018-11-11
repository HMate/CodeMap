#include "textfolder.h"

#include <QTextDocumentFragment>

Q_DECLARE_METATYPE(QTextDocumentFragment)

TextFolder::TextFolder(QObject *parent) : TextFolder(parent, "")
{}

TextFolder::TextFolder(QObject *parent, const QString& placeholderText) : QObject(parent)
{
    m_PlaceholderText = placeholderText;
}

QSizeF TextFolder::intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format) {
    Q_UNUSED(doc)
    Q_UNUSED(posInDocument)
    Q_ASSERT(format.type() == format.CharFormat);
    const QTextCharFormat &tf = reinterpret_cast<const QTextCharFormat&>(format);

    QFont fn = tf.font();
    QFontMetrics fm(fn);

    QSizeF sz = fm.boundingRect(m_PlaceholderText).size();
    return sz;
}

void TextFolder::drawObject(QPainter *painter, const QRectF &rect, 
    QTextDocument *doc, int posInDocument, const QTextFormat &format) {

    Q_UNUSED(doc)
    Q_UNUSED(posInDocument)
    Q_ASSERT(format.type() == format.CharFormat);
    painter->drawText(rect, m_PlaceholderText);
    painter->drawRect(rect);
}

void TextFolder::fold(QTextCursor c) {
    QTextCharFormat f;
    f.setObjectType(type());
    QVariant v;
    c.selection().toPlainText().size();
    v.setValue(c.selection());
    f.setProperty(prop(), v);
    c.insertText(QString(QChar::ObjectReplacementCharacter), f);
}

bool TextFolder::unfold(QTextCursor c) {
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