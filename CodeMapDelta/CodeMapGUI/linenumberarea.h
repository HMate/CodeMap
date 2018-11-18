#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H

#include <QWidget>

class FileEdit;

class LineNumberArea : public QWidget
{
    Q_OBJECT

    FileEdit *m_codeEditor;
public:
    explicit LineNumberArea(QWidget *parent, FileEdit *editor);

    QSize sizeHint() const override;
    int calculateWidth() const;

public slots:
    void updateLineNumberArea(const QRect &rect, int dy);
    void updateSize();

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // LINENUMBERAREA_H