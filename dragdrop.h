#ifndef DRAGDROP_H
#define DRAGDROP_H

#include <QGraphicsItem>
#include <QPen>
#include <QCursor>
#include <QGraphicsScene>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include "house.h"

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
};

// Add to DragDoor class
class DragDoor : public QObject, public QGraphicsItemGroup
{
    Q_OBJECT

public:
    DragDoor(QGraphicsLineItem *doorLine,
             QGraphicsLineItem *entryLine,
             QGraphicsScene *scene,
             House *house,
             Door *door,
             QObject *parent = nullptr);

    ~DragDoor();

    // Add these getters
    QPointF getOrigin() const;
    int getId() const { return data(0).toInt(); }

signals:
    void doorPositionChanged(int id, QPointF newOrigin);

private slots:
    void updateSelectionStyle();

protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QGraphicsLineItem *m_doorLine;
    QGraphicsLineItem *m_entryLine;
    QPointF m_originalPos;

    QPen m_normalDoorPen;
    QPen m_selectedDoorPen;

    QPen m_normalEntryPen;
    QPen m_selectedEntryPen;

    House *m_house;
    Door *m_door;
    QGraphicsScene *m_scene;
};

// Add to DragObstruction class
class DragObstruction : public QGraphicsItem, protected DragBase
{
public:

    enum HandlePosition {
        None = -1,
        TopLeft = 0,
        TopRight = 1,
        BottomLeft = 2,
        BottomRight = 3
    };

    DragObstruction(const QRectF &body, QRectF *legs, House *house, Obstruction *obstruction);
    DragObstruction(const QRectF &body, const QRectF &overlay, House *house, Obstruction *obstruction);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QRectF m_body;
    QVector<QRectF> m_legs;
    QRectF *legs_array;
    QRectF m_overlay;
    bool isChest;
    QPointF m_startPos;
    House *m_house;
    Obstruction *m_obstruction;
    QString m_type;
    // Handle variables
    HandlePosition m_currentHandle;
    QPointF m_lastMousePos;
    qreal m_handleSize;
    QRectF m_originalBody;
    QRectF m_originalOverlay;
    QList<QRectF> m_originalLegs;

    // Size constraints
    qreal m_minWidth;
    qreal m_minHeight;
    qreal m_maxWidth;
    qreal m_maxHeight;

    // Methods for handling resizing
    HandlePosition handleAt(const QPointF &pos) const;
    QRectF handleRect(HandlePosition pos) const;
    void updateLegsPositions(const QRectF &newBody);
    void setSizeConstraints();
};

class DragRoom : public QObject, public QGraphicsRectItem, protected DragBase
{
    Q_OBJECT
public:
    const qreal MIN_ROOM_WIDTH = 100.0;
    const qreal MIN_ROOM_HEIGHT = 100.0;

    DragRoom(const QRectF &rect, QGraphicsScene *scene, House *house, Room *room, long id);
    bool checkRoomIntersection(const QRectF &newRect);

    enum HandlePosition { TopLeft, TopRight, BottomLeft, BottomRight, None };
    HandlePosition handleAt(const QPointF &pos) const;

    long getId() const { return data(0).toInt(); }
    void setId(long id) {m_id = id;}

signals:
    void roomGeometryChanged(int id, QPointF topLeft, QPointF bottomRight);

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
    QGraphicsScene *m_scene;
    QPointF m_originalPos;

    QRectF handleRect(HandlePosition pos) const;

    long m_id;

    House *m_house;
    Room *m_room;
};

#endif // DRAGDROP_H
