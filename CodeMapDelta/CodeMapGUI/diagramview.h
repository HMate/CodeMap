#ifndef DIAGRAMVIEW_H
#define DIAGRAMVIEW_H

#include <QString>
#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QToolBar>
#include <QGraphicsItem>

class QGraphicsView;
class QGraphicsScene;


// BoxDiagramGraphicsItem
class BoxDGI : public QGraphicsItem
{
    QString m_name;
    QFont m_font;

public:
    BoxDGI(const QString& name, QGraphicsItem* parent = nullptr);
    explicit BoxDGI(const std::string& name, QGraphicsItem* parent = nullptr);
    QRectF boundingRect() const override;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
};

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

protected:
    QToolBar* createToolbar();
};

#endif //DIAGRAMVIEW_H