#ifndef HOUSE_H
#define HOUSE_H

#include <QVector>
#include <QPen>
#include <QGraphicsScene>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QObject>
#include <QGraphicsRectItem>
#include <QPointF>
#include <QRectF>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include <QCursor>

class DoorItem : public QObject, public QGraphicsItemGroup
{
    Q_OBJECT

public:
    DoorItem(QGraphicsLineItem *doorLine,
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

class Door
{
public:
    Door();
    Door(QPointF origin);
    Door(QJsonObject door);

    QPointF get_origin();
    QLineF  get_door();
    QLineF  get_entry();
    float get_size();

    void set_origin(QPointF origin);
    void set_size(float size);

    void set_topLeft(QPointF top_left);
    void set_bottomRight(QPointF bottom_right);

    void setId(int id) { m_id = id; }
    int getId() const { return m_id; }

private:
    QPointF m_origin;
    QLineF  m_door;
    QLineF  m_entry;
    float m_size;
    int m_id;

};

class Room
{
public:
    Room(QString shape);
    Room(QPointF top_left, QPointF bottom_right, QString shape);
    Room(QJsonObject room);

    QPointF get_topLeft();
    QPointF get_bottomRight();
    QRectF get_rectRoom();
    QString get_shape();

    void set_topLeft(QPointF top_left);
    void set_bottomRight(QPointF bottom_right);

    void setId(int id) { m_id = id; }
    int getId() const { return m_id; }

private:
    QPointF m_top_left;
    QPointF m_bottom_right;
    QRectF  m_rectRoom;
    QString m_shape;
    int m_id;

};

class Obstruction
{
public:
    Obstruction(bool isChest);
    Obstruction(QPointF top_left, QPointF bottom_right, bool isChest);
    Obstruction(QJsonObject obstruction);

    QPointF get_topLeft();
    QPointF get_bottomRight();
    QRectF get_rect();
    QRectF get_overlay();
    bool get_isChest();
    float get_floorCoverage();
    QRectF* get_legs();
    void set_legs(int radius);

    void set_topLeft(QPointF top_left);
    void set_bottomRight(QPointF bottom_right);

    void setId(int id) { m_id = id; }
    int getId() const { return m_id; }

private:
    QPointF m_top_left;
    QPointF m_bottom_right;
    QRectF  m_rect;
    QRectF m_floorOverlay;

    bool m_isChest;

    float floorCoverage;

    QRectF legs[4];
    int m_id;

};

class DragItem : public QGraphicsRectItem
{
public:
    DragItem(const QRectF &rect, QGraphicsScene *scene);

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

class DragObstruction : public QGraphicsItem
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


class House
{
public:
    House(QGraphicsScene *scene); // creates house frame

    void loadPlan();
    QString floorplan_name;

    void addRoom(Room room);
    void addObstruction(Obstruction obstruction);
    void addDoor(Door door);

    void drawRooms(QVector<Room> rooms);
    void drawDoors(QVector<Door> doors);
    void drawObstructions(QVector<Obstruction> obstructions);


    void clear();
    void deleteItem();

    QJsonDocument toJson();
    QPen wall_pen;
    QPen obstruct_pen;

    void setRoomFillColor(QColor color, Qt::BrushStyle style);

    bool obstructionIntersection();
    bool roomIntersection();

    void verticalFlip();
    void horizontalFlip();
    bool roomsShareWall(QRectF &a, QRectF &b);

    bool roomsShareWall(Room &room1, Room &room2);
    bool allRoomsConnected();

    bool roomHasDoor(Room &room); // New function
    bool roomsHaveDoors();              // Modified function
    QLineF getSharedWall(Room &room1, Room &room2);
    bool wallHasDoor(QLineF &wall);
    bool allSharedWallsHaveDoors();

    qreal getTotalRoomArea();

private:
    void loadRooms(QJsonArray roomsArray);
    void loadDoors(QJsonArray doorsArray);
    void loadObstructions(QJsonArray obstructionsArray);

    long id;

    QGraphicsScene *m_scene;

    QVector<Room> rooms;
    QVector<Door> doors;
    QVector<Obstruction> obstructions;

    int next_id = 1;
};

class Floorplan // previously floorplan edit
{
public:
    Floorplan(House *house, QGraphicsScene *scene);

    void addSquareRoom();
    void addRectRoom();
    void addLRoom();
    void addChest();
    void addTable();
    void addChair();
    void changeFlooring();
    void addDoor();

    void snapRoomToOtherRooms(DragItem *movingRoom);
    int m_snappingDistance;

private:
    QGraphicsScene *m_scene;
    House *m_house;
    Room default_square = Room(QString("square"));
    Room default_rect = Room(QString("rectangle"));
    Obstruction chest = Obstruction(true);
    Obstruction table = Obstruction(QPointF(-70, 0), QPointF(70, 80), false);
    Obstruction chair = Obstruction(QPointF(-20, 0), QPointF(20, 40), false);
    Door door = Door();

};

#endif // HOUSE_H
