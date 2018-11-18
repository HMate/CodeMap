#ifndef EDITORFOLDINGAREA_H
#define EDITORFOLDINGAREA_H

#include <QWidget>

class FileEdit;
class EditorFoldingButton;

class EditorFoldingArea : public QWidget
{
    Q_OBJECT
    FileEdit *m_codeEditor;
    EditorFoldingButton *m_f;
public:
    explicit EditorFoldingArea(QWidget *parent, FileEdit *editor);

    QSize sizeHint() const override;
    int calculateWidth() const;

    void addFoldingButton(int firstLine, int lastLine);

public slots:
    void updateArea(const QRect &rect, int dy);
    void updateSize();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *e);
};

class EditorFoldingButton : public QWidget
{
    bool m_containsMouse = false;
public:
    const int m_startLine;
    const int m_endLine;

    EditorFoldingButton(QWidget* parent, int start, int end);

    void setContainsMouse(bool);
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
};

#endif // EDITORFOLDINGAREA_H