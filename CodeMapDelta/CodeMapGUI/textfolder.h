#ifndef TEXTFOLDER_H
#define TEXTFOLDER_H

// Source: https://github.com/CapelliC/foldingQTextEdit

#include <QObject>
#include <QPainter>
#include <QTextDocument>
#include <QTextObjectInterface>

class TextFolder : public QObject, public QTextObjectInterface
{
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

    QString m_PlaceholderText;

public:
    explicit TextFolder(QObject *parent = 0);
    explicit TextFolder(QObject *parent, const QString& placeholderText);

    static int type() { return QTextFormat::UserObject + 1; }
    static int prop() { return 1; }

    QSizeF intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format);
    void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format);

    void fold(QTextCursor c);
    bool unfold(QTextCursor c);
};

#endif // TEXTFOLDER_H