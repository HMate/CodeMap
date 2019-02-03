#ifndef DIAGRAMVIEW_H
#define DIAGRAMVIEW_H

#include <QString>
#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QToolBar>

class DiagramView : public QWidget
{
    Q_OBJECT

    QGridLayout* m_layout = nullptr;
    QLabel* m_idLabel = nullptr;
    QString m_id;
public:
    explicit DiagramView(QWidget *parent = nullptr);
    void setId(const QString id);
protected:
    QToolBar* createToolbar();
};

#endif //DIAGRAMVIEW_H