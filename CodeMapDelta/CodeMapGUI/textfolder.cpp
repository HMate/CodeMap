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

void TextFolder::fold(QTextCursor& c) {
    QVariant v;
    v.setValue(c.selection());

    QTextCharFormat f;
    f.setObjectType(type());
    f.setProperty(prop(), v);

    c.insertText(QString(QChar::ObjectReplacementCharacter), f);
}

bool TextFolder::unfold(QTextCursor& c) {
    if(!c.hasSelection()) {
        c.movePosition(c.Right, c.KeepAnchor);
        QTextCharFormat f = c.charFormat();
        if(f.objectType() == type()) {
            QVariant v = f.property(prop());
            auto q = v.value<QTextDocumentFragment>();
            c.insertFragment(q);
            return true;
        }
    }
    return false;
}