#ifndef DIAGRAMVIEW_H
#define DIAGRAMVIEW_H

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QToolBar>

#include <QGraphicsView>

class QGridLayout;
class QGraphicsScene;

class DiagramGraphicsView : public QGraphicsView
{
    Q_OBJECT

    QWidget* m_legendUi;
public:
    DiagramGraphicsView(QWidget* parent);

    void registerLegendUiWidget(QWidget* widget);

    void enterEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    // draw gui
    void drawForeground(QPainter *painter, const QRectF &rect);
    void scrollContentsBy(int dx, int dy);
};

class DiagramView : public QWidget
{
    Q_OBJECT

    QGridLayout* m_layout = nullptr;
    QLabel* m_idLabel = nullptr;
    QString m_id;

    DiagramGraphicsView* m_view = nullptr;
    QGraphicsScene* m_scene = nullptr;


    QPointF m_targetScenePos, m_targetViewportPos;
public:
    explicit DiagramView(QWidget *parent = nullptr);
    void setId(const QString id);
    QGraphicsScene* getScene() { return m_scene; }
    DiagramGraphicsView* getView() { return m_view; }

    bool eventFilter(QObject *object, QEvent *ev);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
protected:
    QToolBar* createToolbar();
};

#endif //DIAGRAMVIEW_H