#ifndef DRAGDROP_H
#define DRAGDROP_H

#include <QGraphicsItem>
#include <QPen>
#include <QCursor>
#include <QGraphicsScene>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

class DragBase
{
protected:
    void setupDraggable(QGraphicsItem* item) {
        item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
        item->setCursor(Qt::OpenHandCursor);
    }

    void handleMousePress(QGraphicsSceneMouseEvent *event, QGraphicsItem* item) {
        item->setCursor(Qt::ClosedHandCursor);
    }

    void handleMouseRelease(QGraphicsSceneMouseEvent *event, QGraphicsItem* item) {
        item->setCursor(Qt::OpenHandCursor);
    }
};

class DragDoor : public QObject, public QGraphicsItemGroup
{
    Q_OBJECT

public:
    DragDoor(QGraphicsLineItem *doorLine,
             QGraphicsLineItem *entryLine,
             QGraphicsScene *scene,
             QObject *parent = nullptr);

private slots:
    void updateSelectionStyle();

private:
    QGraphicsLineItem *m_doorLine;
    QGraphicsLineItem *m_entryLine;

    QPen m_normalDoorPen;
    QPen m_selectedDoorPen;

    QPen m_normalEntryPen;
    QPen m_selectedEntryPen;
};

class DragObstruction : public QGraphicsItem, protected DragBase
{
public:
    DragObstruction(const QRectF &body, QRectF *legs);
    DragObstruction(const QRectF &body, const QRectF &overlay);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QRectF m_body;
    QVector<QRectF> m_legs;
    QRectF m_overlay;
    bool isChest;
    QPointF m_startPos;
};

class DragRoom : public QGraphicsRectItem, protected DragBase
{
public:
    DragRoom(const QRectF &rect, QGraphicsScene *scene);

    enum HandlePosition { TopLeft, TopRight, BottomLeft, BottomRight, None };
    HandlePosition handleAt(const QPointF &pos) const;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    HandlePosition m_currentHandle = None;
    QPointF m_lastMousePos;
    QRectF m_originalRect;
    qreal m_handleSize = 8.0;

    QRectF handleRect(HandlePosition pos) const;
};

#endif // DRAGDROP_H
