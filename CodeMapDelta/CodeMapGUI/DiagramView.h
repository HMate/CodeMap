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
    QLabel* m_displayNameLabel = nullptr;
    QString m_displayName;

    QPointF m_targetScenePos, m_targetViewportPos;

    DiagramGraphicsView* m_view = nullptr;
    QGraphicsScene* m_scene = nullptr;


public:
    explicit DiagramView(QWidget *parent = nullptr);
    void setDisplayName(const QString id);
    QGraphicsScene* getScene() { return m_scene; }
    DiagramGraphicsView* getView() { return m_view; }

    bool eventFilter(QObject *object, QEvent *ev);
protected:
    QToolBar* createToolbar();
};

#endif //DIAGRAMVIEW_H