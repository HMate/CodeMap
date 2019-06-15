#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H

#include <QWidget>

class FileView;

class LineNumberArea : public QWidget
{
    Q_OBJECT

    FileView *m_view;
public:
    explicit LineNumberArea(FileView *parent);

    QSize sizeHint() const override;
    int calculateWidth() const;

public slots:
    void updateLineNumberArea(const QRect &rect, int dy);
    void updateSize();

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // LINENUMBERAREA_H