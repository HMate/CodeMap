#pragma once

#include <QHash>
#include <QGraphicsItem>
#include <QGroupBox>
#include <QCheckBox>

#include "DiagramView.h"

class IncludeDiagramView;

class ArrowDGI : public QGraphicsItem
{
    QGraphicsItem* m_startItem;
    QGraphicsItem* m_endItem;

public:
    ArrowDGI(QGraphicsItem* startItem, QGraphicsItem* endItem, QGraphicsItem* parent = nullptr);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QRectF boundingRect() const override;

    QPointF startPoint() const;
    QPointF endPoint() const;
};

// BoxDiagramGraphicsItem
class BoxDGI : public QGraphicsItem
{
    IncludeDiagramView& m_parentView;
    QString m_displayName;
    QString m_fullName;
    QFont m_font;
    bool m_fullInclude = false;

public:
    explicit BoxDGI(IncludeDiagramView& parentView, const std::string& displayName, const std::string& fullName, QGraphicsItem* parent = nullptr);
    BoxDGI(IncludeDiagramView& parentView, const QString& displayName, const QString& fullName, QGraphicsItem* parent = nullptr);
    void setFullInclude(bool fullInclude) { m_fullInclude = fullInclude; }
    QRectF boundingRect() const override;
    QString getFullName() { return m_fullName; }

    void hoverMoveEvent(QGraphicsSceneHoverEvent* event);
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};


// TODO:
// -Multiselect, Ctrl+select, move multiple
// -Open file on double click / right click menu
// -Gui: 
//  - show/hide include guarded files
//  - Filter boxes by path
//  - color legend
//  - statistics: how many includes total / unique includes / child of selected / tree of selected
class IncludeDiagramView : public DiagramView
{
    Q_OBJECT

    QHash<QString, int> m_selectedIDs;

    QGroupBox* m_box;
    QLabel* m_label;
    QCheckBox* m_check;
public:
    explicit IncludeDiagramView(QWidget *parent = nullptr);

    void clearSelectedID(const QString& id);
    void setSelectedID(const QString& id);
    bool isBoxSelectedWithID(const QString& id);
};
