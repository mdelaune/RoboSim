#ifndef HOUSE_H
#define HOUSE_H

#include <QVector>
#include <QPen>
#include <QGraphicsScene>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QString>
#include <QObject>

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

class Obstruction
{
public:
    Obstruction(bool isChest, QString type);
    Obstruction(QPointF top_left, QPointF bottom_right, bool isChest, QString type);
    Obstruction(QJsonObject obstruction);

    QPointF get_topLeft();
    QPointF get_bottomRight();
    QRectF get_rect();
    QRectF get_overlay();
    bool get_isChest();
    float get_floorCoverage();
    QRectF* get_legs();
    void set_legsRadius(int radius);
    void set_legs(QRectF *legs);

    QString get_type(); // New getter
    void set_type(QString& type);

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
    QString m_type;

    float floorCoverage;

    QRectF m_legs[4];
    int m_id;
};

//---------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------

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

//---------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------

class House : public QObject
{
    Q_OBJECT
public:
    House(QGraphicsScene *scene);

    static House* instance;
    const double MIN_TOTAL_AREA = 40000.0;
    const double MAX_TOTAL_AREA = 600000.0;

    void loadPlan(QString plan);

    void addRoom(Room room);
    void addObstruction(Obstruction obstruction);
    void addDoor(Door door);

    void drawRooms();
    void drawDoors();
    void drawObstructions();

    void drawSimulationPlan();

    void clear();
    void deleteItem();
    void rotate();

    QString get_floorplanName();
    QJsonDocument toJson();

    template<typename T>
    void loadEntities(QJsonArray entitiesArray, QVector<T>& entities, std::function<T(QJsonObject&)> createEntity);

    template<typename T>
    void addItem(T item, QVector<T>& collection);

    QPen wall_pen;
    QPen obstruct_pen;

    void setRoomFillColor(QString flooring);

    void updateRoomGeometry(int id, QPointF topLeft, QPointF bottomRight);
    void updateDoorPosition(int id, QPointF newOrigin);
    void updateObstructionPosition(int id, QPointF topLeft, QPointF bottomRight);

    int getTotalArea();

    QString getFloorCovering();
    void setFloorCovering(QString flooring);

    QVector<Room> rooms;
    QVector<Door> doors;
    QVector<Obstruction> obstructions;
    QString flooring;

    Room* getRoomById(long id);

    int validateTotalAreaBeforeSave();
    bool doRoomsShareWall(Room& room1, Room& room2);
    bool validateRoomConnectivity();

    bool doRoomsIntersect(Room& room1, Room& room2);
    bool validateNoRoomIntersections();

    bool validateDoorsOnWalls();
    bool validateEveryRoomHasDoor();

    bool doesObstructionIntersectRoom(Obstruction& obstruction, Room& room);
    bool validateObstructionPlacements();

    bool validateNoObstructionIntersections();
    bool doObstructionsIntersect(Obstruction& obs1, Obstruction& obs2);
    bool isObstructionInsideAnyRoom(Obstruction& obstruction);

    int getFloorplanId() const;
    void setFloorplanId(int id);
    void generateNewFloorplanId();

    void createNewFloorplan();

    int getCoveredArea();

private:
    void loadRooms(QJsonArray roomsArray);
    void loadDoors(QJsonArray doorsArray);
    void loadObstructions(QJsonArray obstructionsArray);

    QString floorplan_name;
    int floorplan_id;
    int next_floorplan_id;

    QGraphicsScene *m_scene;
    QString defaultPlanLocation = ":/Default/default_plan.json";

    int scene_object_id = 1;
    int total_area;

    QString floor_covering = "hard_floor";
};

#endif // HOUSE_H
