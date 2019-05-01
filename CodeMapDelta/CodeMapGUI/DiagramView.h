#ifndef DIAGRAMVIEW_H
#define DIAGRAMVIEW_H

#include <QString>
#include <QWidget>
#include <QLabel>
#include <QToolBar>

#include <QGraphicsView>

class QGridLayout;
class QGraphicsScene;

class DiagramGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    DiagramGraphicsView(QWidget* parent) : QGraphicsView(parent) {}

    void enterEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
};

class DiagramView : public QWidget
{
    Q_OBJECT

    QGridLayout* m_layout = nullptr;
    QLabel* m_idLabel = nullptr;
    QString m_id;

    QGraphicsView* m_view = nullptr;
    QGraphicsScene* m_scene = nullptr;


    QPointF m_targetScenePos, m_targetViewportPos;
public:
    explicit DiagramView(QWidget *parent = nullptr);
    void setId(const QString id);
    QGraphicsScene* getScene() { return m_scene; }
    QGraphicsView* getView() { return m_view; }

    bool eventFilter(QObject *object, QEvent *ev);
protected:
    QToolBar* createToolbar();
};

#endif //DIAGRAMVIEW_H