#ifndef DIAGRAMVIEW_H
#define DIAGRAMVIEW_H

#include <QString>
#include <QWidget>
#include <QLabel>
#include <QToolBar>

class QGridLayout;
class QGraphicsView;
class QGraphicsScene;

class DiagramView : public QWidget
{
    Q_OBJECT

    QGridLayout* m_layout = nullptr;
    QLabel* m_idLabel = nullptr;
    QString m_id;

    QGraphicsView* m_view = nullptr;
    QGraphicsScene* m_scene = nullptr;
public:
    explicit DiagramView(QWidget *parent = nullptr);
    void setId(const QString id);
    QGraphicsScene* getScene() { return m_scene; }
    QGraphicsView* getView() { return m_view; }

protected:
    QToolBar* createToolbar();
};

#endif //DIAGRAMVIEW_H