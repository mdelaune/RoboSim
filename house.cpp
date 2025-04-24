#include <QDebug>
#include <QColor>
#include <QBrush>

#include "house.h"
#include "dragdrop.h"

Door::Door() : m_origin(QPointF(0,0)), m_size(45)
{
    m_door = QLineF(m_origin, QPointF(m_origin.x(), m_origin.y() + m_size));
    m_entry = QLineF(m_origin, QPointF(m_origin.x() + m_size, m_origin.y()));
}

Door::Door(QPointF origin) : m_origin(origin), m_size(45)
{
    m_door = QLineF(m_origin, QPointF(m_origin.x(), m_origin.y() + m_size));
    m_entry = QLineF(m_origin, QPointF(m_origin.x() + m_size, m_origin.y()));
}

Door::Door(QJsonObject door) : m_size(45)
{
    m_origin = QPointF(door.value("x").toInt(), door.value("y").toInt());
    m_door = QLineF(m_origin, QPointF(m_origin.x(), m_origin.y() + m_size));
    m_entry = QLineF(m_origin, QPointF(m_origin.x() + m_size, m_origin.y()));
}

QPointF Door::get_origin()
{
    return m_origin;
}

QLineF Door::get_door()
{
    return m_door;
}

QLineF Door::get_entry()
{
    return m_entry;
}

float Door::get_size()
{
    return m_size;
}

void Door::set_origin(QPointF origin)
{
    m_origin = origin;
}

void Door::set_size(float size)
{
    m_size = size;
}

//---------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------

Obstruction::Obstruction(bool isChest) : m_top_left(-50, 20), m_bottom_right(50, -20)
{
    m_rect = QRectF(m_top_left, m_bottom_right);
    m_isChest = isChest;
    get_floorCoverage();

    if(isChest)
    {
        set_legs(0);
    } else {
        set_legs(10);
    }

}

Obstruction::Obstruction(QPointF top_left, QPointF bottom_right, bool isChest)
{
    m_top_left = top_left;
    m_bottom_right = bottom_right;
    m_rect = QRectF(m_top_left, m_bottom_right);
    m_isChest = isChest;
    get_floorCoverage();
    if(m_isChest)
    {
        set_legs(0);
    } else {
        set_legs(10);
    }
}

Obstruction::Obstruction(QJsonObject room)
{
    m_top_left = QPointF(room.value("x_topLeft").toInt(), room.value("y_topLeft").toInt());
    m_bottom_right = QPointF(room.value("x_bottomRight").toInt(), room.value("y_bottomRight").toInt());
    m_rect = QRectF(m_top_left, m_bottom_right);
    m_isChest = room.value("is_chest").toBool();
    get_floorCoverage();
    if(m_isChest)
    {
        set_legs(0);
    } else {
        set_legs(10);
    }
}

QPointF Obstruction::get_topLeft()
{
    return m_top_left;
}

QPointF Obstruction::get_bottomRight()
{
    return m_bottom_right;
}

QRectF Obstruction::get_rect()
{
    return m_rect;
}

QRectF Obstruction::get_overlay()
{
    get_floorCoverage();
    return m_floorOverlay;
}

bool Obstruction::get_isChest()
{
    return m_isChest;
}

float Obstruction::get_floorCoverage()
{
    if(m_isChest)
    {
        floorCoverage = m_rect.width() * m_rect.height();
        m_floorOverlay = QRectF(m_top_left, m_bottom_right);
    }
    else
    {
        floorCoverage = (m_rect.width() * m_rect.height()) * 0.2;
        m_floorOverlay = QRectF(m_top_left, m_bottom_right);

        legs[0] = QRectF(m_top_left + QPointF(5, 5), QSize(15,15));
        legs[1] = QRectF(m_top_left + QPointF(m_rect.width() - 20, 5), QSize(15,15));
        legs[2] = QRectF(m_bottom_right - QPointF(20, 20), QSize(15,15));
        legs[3] = QRectF(m_bottom_right - QPointF(m_rect.width() - 5, 20), QSize(15,15));
    }

    return floorCoverage;
}

QRectF* Obstruction::get_legs()
{
    return legs;
}

void Obstruction::set_legs(int size)
{
    legs[0] = QRectF(m_top_left + QPointF(5, 5), QSize(size,size));
    legs[1] = QRectF(m_top_left + QPointF(m_rect.width() - 15, 5), QSize(size,size));
    legs[2] = QRectF(m_bottom_right - QPointF(20 - 5, 15), QSize(size,size));
    legs[3] = QRectF(m_bottom_right - QPointF(m_rect.width() - 5, 15), QSize(size,size));
}

void Obstruction::set_topLeft(QPointF top_left)
{
    m_top_left = top_left;
    m_rect = QRectF(m_top_left, m_bottom_right);
    set_legs(10);
}

void Obstruction::set_bottomRight(QPointF bottom_right)
{
    m_bottom_right = bottom_right;
    m_rect = QRectF(m_top_left, m_bottom_right);
    set_legs(10);
}

//---------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------

Room::Room(QString shape) //, QGraphicsItem(nullptr)
{
    m_shape = shape;
    if(m_shape == "square")
    {
        m_top_left = QPointF(-100, 100);
        m_bottom_right = QPointF(100, -100);
    }
    else if(m_shape == "rectangle")
    {
        m_top_left = QPointF(-150, 80);
        m_bottom_right = QPointF(150, -80);
    }
    m_rectRoom = QRectF(m_top_left, m_bottom_right).normalized();
}

Room::Room(QPointF top_left, QPointF bottom_right, QString shape)// : QGraphicsItem(nullptr)
{
    m_shape = shape;
    m_top_left = top_left;
    m_bottom_right = bottom_right;
    m_rectRoom = QRectF(m_top_left, m_bottom_right).normalized();
}

Room::Room(QJsonObject room)// : QGraphicsItem(nullptr)
{
    m_shape = room.value("shape").toString();
    m_top_left = QPointF(room.value("x_topLeft").toInt(), room.value("y_topLeft").toInt());
    m_bottom_right = QPointF(room.value("x_bottomRight").toInt(), room.value("y_bottomRight").toInt());
    m_rectRoom = QRectF(m_top_left, m_bottom_right).normalized();
}

QPointF Room::get_topLeft()
{
    return m_top_left;
}

QPointF Room::get_bottomRight()
{
    return m_bottom_right;
}

QRectF Room::get_rectRoom()
{
    return m_rectRoom;
}

QString Room::get_shape()
{
    return m_shape;
}

void Room::set_topLeft(QPointF top_left)
{
    qDebug() << "top left " << m_top_left;
    m_top_left = top_left;
    m_rectRoom = QRectF(m_top_left, m_bottom_right).normalized();
}

void Room::set_bottomRight(QPointF bottom_right)
{
    qDebug() << "bottom right " << m_bottom_right;
    m_bottom_right = bottom_right;
    m_rectRoom = QRectF(m_top_left, m_bottom_right).normalized();
}

//---------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------
House* House::instance = nullptr;
House::House(QGraphicsScene *scene)
{
    m_scene = scene;
    wall_pen.setWidth(4);
    obstruct_pen.setWidth(3);
    instance = this;
    loadPlan(defaultPlanLocation);
}

void House::drawRooms()
{
    for(int i = 0; i < rooms.size(); i++)
    {
        DragRoom *rect_item = new DragRoom(rooms[i].get_rectRoom(), m_scene, this, &rooms[i], rooms[i].getId());
        rect_item->setPen(wall_pen);

        rect_item->setData(0, rooms[i].getId());
        rect_item->setData(1, "room");

        m_scene->addItem(rect_item);
    }
}

void House::drawDoors()
{
    for (Door &door : doors)
    {
        QGraphicsLineItem *doorLine = new QGraphicsLineItem(door.get_door());
        QGraphicsLineItem *entryLine = new QGraphicsLineItem(door.get_entry());

        DragDoor *doorItem = new DragDoor(doorLine, entryLine, m_scene);
        m_scene->addItem(doorItem);
    }
}

void House::drawObstructions()
{
    QPen overlay_pen;
    for(int i = 0; i < obstructions.size(); i++)
    {
        if(obstructions[i].get_isChest())
        {
            DragObstruction *item = new DragObstruction(obstructions[i].get_rect(), obstructions[i].get_overlay(), this, &obstructions[i]);
            m_scene->addItem(item);
            item->setZValue(1);        }
        else
        {
            obstructions[i].set_legs(10);

            DragObstruction *item = new DragObstruction(obstructions[i].get_rect(), obstructions[i].get_legs(), this, &obstructions[i]);
            m_scene->addItem(item);
            item->setZValue(1);
        }
    }
}

void House::drawSimulationPlan()
{
    for(int i = 0; i < rooms.size(); i++)
    {
        m_scene->addRect(rooms[i].get_rectRoom(), wall_pen);
    }

    QPen penWhite = QPen(Qt::white);
    penWhite.setWidth(2);
    for(int i = 0; i < doors.size(); i++)
    {
        m_scene->addLine(doors[i].get_door(), wall_pen);
        m_scene->addLine(doors[i].get_entry(), penWhite);
    }

    QPen overlay_pen;
    for(int i = 0; i < obstructions.size(); i++)
    {
        if(obstructions[i].get_isChest())
        {
            m_scene->addRect(obstructions[i].get_rect(), obstruct_pen);
            m_scene->addRect(obstructions[i].get_overlay(), QPen(Qt::NoPen), QBrush(QColor(255,0,0,127)));
        }
        else
        {
            m_scene->addRect(obstructions[i].get_rect(), obstruct_pen);
            m_scene->addEllipse(obstructions[i].get_legs()[0], QPen(Qt::NoPen), QBrush(QColor(255,0,0,127)));
            m_scene->addEllipse(obstructions[i].get_legs()[1], QPen(Qt::NoPen), QBrush(QColor(255,0,0,127)));
            m_scene->addEllipse(obstructions[i].get_legs()[2], QPen(Qt::NoPen), QBrush(QColor(255,0,0,127)));
            m_scene->addEllipse(obstructions[i].get_legs()[3], QPen(Qt::NoPen), QBrush(QColor(255,0,0,127)));
        }

    }
}

// Generic method to load entities from JSON
template<typename T>
void House::loadEntities(QJsonArray entitiesArray, QVector<T>& entities, std::function<T(QJsonObject&)> createEntity)
{
    for(int i = 0; i < entitiesArray.size(); i++)
    {
        QJsonObject obj = entitiesArray[i].toObject();
        entities.append(createEntity(obj));
    }
}

template<typename T>
void House::addItem(T item, QVector<T> &collection)
{
    item.setId(scene_object_id++);
    collection.append(item);
}

void House::addRoom(Room room)
{
    room.setId(scene_object_id++);
    rooms.append(room);
}

void House::addObstruction(Obstruction obstruction)
{
    obstruction.setId(scene_object_id++);
    obstructions.append(obstruction);
}

void House::addDoor(Door door)
{
    door.setId(scene_object_id++);
    doors.append(door);
}

void House::loadPlan(QString plan)
{
    clear();

    qDebug() << plan;
    QFile file(plan);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file";
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    //TODO: check for errors when reading
    file.close();

    floorplan_name = plan;

    QJsonObject root = doc.object();
    QJsonArray roomsArray = root.value("rooms").toArray();
    loadEntities<Room>(roomsArray, rooms, [](QJsonObject& obj){ return Room(obj); });
    drawRooms();

    QJsonArray doorsArray = root.value("doors").toArray();
    loadEntities<Door>(doorsArray, doors, [](QJsonObject& obj){ return Door(obj); });
    drawDoors();

    QJsonArray obstructionsArray = root.value("obstructions").toArray();
    loadEntities<Obstruction>(obstructionsArray, obstructions, [](QJsonObject& obj){ return Obstruction(obj); });

    drawObstructions();

    setRoomFillColor(QColor(196, 164, 132, 127), Qt::CrossPattern);
    m_scene->update(m_scene->sceneRect());
}

QJsonDocument House::toJson()
{
    QJsonArray roomsArray = QJsonArray();
    QJsonArray obstructionsArray = QJsonArray();
    QJsonArray doorsArray = QJsonArray();


    for(int i = 0; i < rooms.size(); i++)
    {
        QJsonObject object
        {
            {"shape", rooms[i].get_shape()},
            {"x_topLeft", rooms[i].get_topLeft().x()},
            {"y_topLeft", rooms[i].get_topLeft().y()},
            {"x_bottomRight", rooms[i].get_bottomRight().x()},
            {"y_bottomRight", rooms[i].get_bottomRight().y()},
        };

        roomsArray.append(object);
        qDebug() << roomsArray;
    }

    for(int i = 0; i < obstructions.size(); i++)
    {
        QJsonObject object
        {
            {"is_chest", obstructions[i].get_isChest()},
            {"x_topLeft", obstructions[i].get_topLeft().x()},
            {"y_topLeft", obstructions[i].get_topLeft().y()},
            {"x_bottomRight", obstructions[i].get_bottomRight().x()},
            {"y_bottomRight", obstructions[i].get_bottomRight().y()},
        };

        obstructionsArray.append(object);
        qDebug() << obstructionsArray;
    }

    for(int i = 0; i < doors.size(); i++)
    {
        QJsonObject object
            {
                {"x", doors[i].get_origin().x()},
                {"y", doors[i].get_origin().y()}
            };

        doorsArray.append(object);
        qDebug() << doorsArray;
    }

    QJsonObject root
        {
            {"doors", doorsArray},
            {"obstructions", obstructionsArray},
            {"rooms", roomsArray}
        };

    QJsonDocument doc = QJsonDocument(root);
    return doc;
}

QString House::get_floorplanName()
{
    return floorplan_name;
}

void House::setRoomFillColor(QColor color, Qt::BrushStyle style)
{
    QList<QGraphicsItem *> items = m_scene->items();

    for (QGraphicsItem *item : items)
    {
        // Check if it's a room
        DragRoom *room = dynamic_cast<DragRoom *>(item);
        if (room)
        {
            // Create a brush with cross pattern
            QBrush brush(color, style);
            room->setBrush(brush);
        }
    }

    m_scene->update(m_scene->sceneRect());
}

void House::clear()
{
    m_scene->clear();
    rooms.clear();
    doors.clear();
    obstructions.clear();
    scene_object_id = 1;
    m_scene->update(m_scene->sceneRect());
    //floorplan_name = "";
}

void House::deleteItem()
{
    QList<QGraphicsItem*> selectedItems = m_scene->selectedItems();

    for (auto it = selectedItems.begin(); it != selectedItems.end(); ) {
        QGraphicsItem* item = *it;

        if (item->data(1) == "room") {
            int id = item->data(0).toInt();

            for (auto roomIt = rooms.begin(); roomIt != rooms.end(); ++roomIt) {
                if (roomIt->getId() == id) {
                    qDebug() << "Found and deleting from vector, ID:" << roomIt->getId();
                    rooms.erase(roomIt);
                    break;
                }
            }
        } else if (item->data(1) == "door") {
            int id = item->data(0).toInt();
            for (auto doorIt = doors.begin(); doorIt != doors.end(); ++doorIt) {
                if (doorIt->getId() == id) {
                    qDebug() << "Found and deleting from vector, ID:" << doorIt->getId();
                    doors.erase(doorIt);
                    break;
                }
            }
        } else if (item->data(1) == "obstruction") {
            int id = item->data(0).toInt();
            for (auto obstructionIt = obstructions.begin(); obstructionIt != obstructions.end(); ++obstructionIt) {
                if (obstructionIt->getId() == id) {
                    qDebug() << "Found and deleting from vector, ID:" << obstructionIt->getId();
                    obstructions.erase(obstructionIt);
                    break;
                }
            }
        }

        m_scene->removeItem(item);
        delete item;
        it = selectedItems.erase(it); //erase returns the next valid iterator
    }

    m_scene->update(m_scene->sceneRect());
    qDebug() << "DELETE";
}
