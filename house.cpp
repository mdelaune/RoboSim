#include <QDebug>
#include <QColor>
#include <QBrush>

#include "house.h"
#include "dragdrop.h"

#include <QRandomGenerator>

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
    m_door = QLineF(m_origin, QPointF(m_origin.x(), m_origin.y() + m_size));
    m_entry = QLineF(m_origin, QPointF(m_origin.x() + m_size, m_origin.y()));
}

void Door::set_size(float size)
{
    m_size = size;
}

//---------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------

Obstruction::Obstruction(bool isChest, QString type) : m_top_left(-50, 20), m_bottom_right(50, -20)
{
    m_rect = QRectF(m_top_left, m_bottom_right);
    m_isChest = isChest;
    m_type = type;
    get_floorCoverage();
    if(!isChest)
    {
        set_legsRadius(10);
    }
}

Obstruction::Obstruction(QPointF top_left, QPointF bottom_right, bool isChest, QString type)
{
    m_top_left = top_left;
    m_bottom_right = bottom_right;
    m_rect = QRectF(m_top_left, m_bottom_right);
    m_isChest = isChest;
    m_type = type;
    get_floorCoverage();
    if(!m_isChest)
    {
        set_legsRadius(10);
    }


}

Obstruction::Obstruction(QJsonObject obstruction)
{
    m_top_left = QPointF(obstruction.value("x_topLeft").toInt(), obstruction.value("y_topLeft").toInt());
    m_bottom_right = QPointF(obstruction.value("x_bottomRight").toInt(), obstruction.value("y_bottomRight").toInt());
    m_rect = QRectF(m_top_left, m_bottom_right);
    m_isChest = obstruction.value("is_chest").toBool();
    m_type = obstruction.value("type").toString();
    get_floorCoverage();
    if(!m_isChest)
    {
        set_legsRadius(10);
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
    }

    return floorCoverage;
}

QRectF* Obstruction::get_legs()
{
    return m_legs;
}

void Obstruction::set_legs(QRectF *legs)
{
    for(int i = 0; i < 4; i++)
    {
        qDebug() << legs[i];
        m_legs[i] = legs[i];
    }
}

void Obstruction::set_legsRadius(int size)
{
    // Top-left corner leg
    m_legs[0] = QRectF(m_top_left.x() + 5, m_top_left.y() + 5, size, size);

    // Top-right corner leg
    m_legs[1] = QRectF(m_bottom_right.x() - 5 - size, m_top_left.y() + 5, size, size);

    // Bottom-right corner leg
    m_legs[2] = QRectF(m_bottom_right.x() - 5 - size, m_bottom_right.y() - 5 - size, size, size);

    // Bottom-left corner leg
    m_legs[3] = QRectF(m_top_left.x() + 5, m_bottom_right.y() - 5 - size, size, size);

}

void Obstruction::set_topLeft(QPointF top_left)
{
    m_top_left = top_left;
    m_rect = QRectF(m_top_left, m_bottom_right);
    set_legsRadius(10);
}

void Obstruction::set_bottomRight(QPointF bottom_right)
{
    m_bottom_right = bottom_right;
    m_rect = QRectF(m_top_left, m_bottom_right);
    set_legsRadius(10);
}

QString Obstruction::get_type()
{
    return m_type;
}

void Obstruction::set_type(QString& type)
{
    m_type = type;
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

int House::getTotalArea()
{
    return total_area;
}

QString House::getFloorCovering()
{
    return floor_covering;
}

void House::setFloorCovering(QString flooring)
{
    floor_covering = flooring;
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
        rooms[i].setId(scene_object_id++);
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
        door.setId(scene_object_id++);
        QGraphicsLineItem *doorLine = new QGraphicsLineItem(door.get_door());
        QGraphicsLineItem *entryLine = new QGraphicsLineItem(door.get_entry());

        DragDoor *doorItem = new DragDoor(doorLine, entryLine, m_scene, this, &door);
        doorItem->setData(0, door.getId());
        doorItem->setData(1, "door");
        m_scene->addItem(doorItem);
    }
}

void House::drawObstructions()
{
    QPen overlay_pen;
    for(int i = 0; i < obstructions.size(); i++)
    {
        obstructions[i].setId(scene_object_id++);
        if(obstructions[i].get_isChest())
        {
            DragObstruction *item = new DragObstruction(obstructions[i].get_rect(), obstructions[i].get_overlay(), this, &obstructions[i]);
            m_scene->addItem(item);
            item->setZValue(1);
            item->setData(0, obstructions[i].getId());
            item->setData(1, "obstruction");
            item->setData(2, obstructions[i].get_type());
        }
        else
        {
            obstructions[i].set_legsRadius(10);

            DragObstruction *item = new DragObstruction(obstructions[i].get_rect(), obstructions[i].get_legs(), this, &obstructions[i]);
            m_scene->addItem(item);
            item->setZValue(1);
            item->setData(0, obstructions[i].getId());
            item->setData(1, "obstruction");
            item->setData(2, obstructions[i].get_type());
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
    qDebug() << "Added obstruction with ID:" << obstruction.getId();
    obstructions.append(obstruction);
}

void House::addDoor(Door door)
{
    door.setId(scene_object_id++);
    doors.append(door);
}

void House::setNewID(){
    id = QString::number(QRandomGenerator::global()->bounded(10000, 99999));
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
    id = root.value("ID").toString();
    floor_covering = root.value("flooring").toString();


    QJsonArray roomsArray = root.value("rooms").toArray();
    loadEntities<Room>(roomsArray, rooms, [](QJsonObject& obj){ return Room(obj); });
    drawRooms();

    QJsonArray doorsArray = root.value("doors").toArray();
    loadEntities<Door>(doorsArray, doors, [](QJsonObject& obj){ return Door(obj); });
    drawDoors();

    QJsonArray obstructionsArray = root.value("obstructions").toArray();
    loadEntities<Obstruction>(obstructionsArray, obstructions, [](QJsonObject& obj){ return Obstruction(obj); });
    drawObstructions();

    setRoomFillColor(floor_covering);
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
                {"y_bottomRight", rooms[i].get_bottomRight().y()}
            };

        roomsArray.append(object);
        qDebug() << roomsArray;
    }

    for(int i = 0; i < obstructions.size(); i++)
    {
        QJsonObject object
            {
                {"is_chest", obstructions[i].get_isChest()},
                {"type", obstructions[i].get_type()}, // Add this line to store the type
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
            {"ID", id},
            {"flooring", floor_covering},
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

void House::setRoomFillColor(QString flooring)
{
    QList<QGraphicsItem *> items = m_scene->items();

    if(flooring == "hard_floor")
    {
        for (QGraphicsItem *item : items)
        {
            // Check if it's a room
            DragRoom *room = dynamic_cast<DragRoom *>(item);
            if (room)
            {
                // Create a brush with cross pattern
                QBrush brush(QColor(196, 164, 132, 127), Qt::CrossPattern);
                room->setBrush(brush);
            }
        }
    }
    else if(flooring == "cut_pile")
    {
        for (QGraphicsItem *item : items)
        {
            // Check if it's a room
            DragRoom *room = dynamic_cast<DragRoom *>(item);
            if (room)
            {
                // Create a brush with cross pattern
                QBrush brush(QColor(200, 0, 0, 127), Qt::Dense6Pattern);
                room->setBrush(brush);
            }
        }
    }
    else if(flooring == "loop_pile")
    {
        for (QGraphicsItem *item : items)
        {
            // Check if it's a room
            DragRoom *room = dynamic_cast<DragRoom *>(item);
            if (room)
            {
                // Create a brush with cross pattern
                QBrush brush(QColor(50, 50, 255, 127), Qt::Dense7Pattern);
                room->setBrush(brush);
            }
        }
    }
    else if(flooring == "frieze_cut")
    {
        for (QGraphicsItem *item : items)
        {
            // Check if it's a room
            DragRoom *room = dynamic_cast<DragRoom *>(item);
            if (room)
            {
                // Create a brush with cross pattern
                QBrush brush(QColor(255, 255, 200, 127), Qt::Dense5Pattern);
                room->setBrush(brush);
            }
        }
    }

    m_scene->update(m_scene->sceneRect());
}

void House::clear()
{
    m_scene->clearSelection();
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
    QVector<int> deletedObstructionIds; // Keep track of deleted IDs for obstructions
    QVector<int> deletedRoomIds; // Keep track of deleted IDs for rooms
    QVector<int> deletedDoorIds; // Keep track of deleted IDs for doors

    // First, collect all the IDs we need to delete
    for (QGraphicsItem* item : selectedItems) {
        if (item->data(1) == "obstruction") {
            int id = item->data(0).toInt();
            deletedObstructionIds.append(id);
        }
        else if (item->data(1) == "room") {
            int id = item->data(0).toInt();
            deletedRoomIds.append(id);
        }
        else if (item->data(1) == "door") {
            int id = item->data(0).toInt();
            deletedDoorIds.append(id);
        }
    }

    // Debug before deletion
    qDebug() << "Before deletion: Obstructions count =" << obstructions.size()
             << "Rooms count =" << rooms.size()
             << "Doors count =" << doors.size();

    // Remove obstructions
    if (!deletedObstructionIds.isEmpty()) {
        QVector<Obstruction> remainingObstructions;

        for (const Obstruction& obs : obstructions) {
            bool shouldKeep = true;
            for (int idToDelete : deletedObstructionIds) {
                if (obs.getId() == idToDelete) {
                    shouldKeep = false;
                    qDebug() << "Marking for deletion: obstruction with ID:" << idToDelete;
                    break;
                }
            }

            if (shouldKeep) {
                remainingObstructions.append(obs);
            }
        }

        obstructions = remainingObstructions;
    }

    // Remove rooms
    if (!deletedRoomIds.isEmpty()) {
        QVector<Room> remainingRooms;

        for (const Room& room : rooms) {
            bool shouldKeep = true;
            for (int idToDelete : deletedRoomIds) {
                if (room.getId() == idToDelete) {
                    shouldKeep = false;
                    qDebug() << "Marking for deletion: room with ID:" << idToDelete;
                    break;
                }
            }

            if (shouldKeep) {
                remainingRooms.append(room);
            }
        }

        rooms = remainingRooms;
    }

    // Remove doors
    if (!deletedDoorIds.isEmpty()) {
        QVector<Door> remainingDoors;

        for (const Door& door : doors) {
            bool shouldKeep = true;
            for (int idToDelete : deletedDoorIds) {
                if (door.getId() == idToDelete) {
                    shouldKeep = false;
                    qDebug() << "Marking for deletion: door with ID:" << idToDelete;
                    break;
                }
            }

            if (shouldKeep) {
                remainingDoors.append(door);
            }
        }

        doors = remainingDoors;
    }

    // Debug after filtering
    qDebug() << "After deletion: Obstructions count =" << obstructions.size()
             << "Rooms count =" << rooms.size()
             << "Doors count =" << doors.size();

    // Finally, remove items from scene
    for (QGraphicsItem* item : selectedItems) {
        m_scene->removeItem(item);
        delete item;
    }

    m_scene->update(m_scene->sceneRect());
    qDebug() << "DELETE COMPLETE";
}

int House::validateTotalAreaBeforeSave()
{
    double totalArea = 0.0;

    for (int i = 0; i < rooms.size(); ++i) {
        Room& currentRoom = rooms[i];
        QRectF currentRect = currentRoom.get_rectRoom();

        bool isContained = false;

        // Check if currentRoom is fully inside any other room
        for (int j = 0; j < rooms.size(); ++j) {
            if (i == j) continue; // don't compare with itself

            Room& otherRoom = rooms[j];
            QRectF otherRect = otherRoom.get_rectRoom();

            if (otherRect.contains(currentRect)) {
                isContained = true;
                break;
            }
        }

        // Only add area if not contained
        if (!isContained) {
            totalArea += currentRect.width() * currentRect.height();
        }
    }

    // Define your acceptable area range


    qDebug() << "Total room area (excluding contained rooms):" << totalArea;

    if (totalArea < MIN_TOTAL_AREA) {
        qDebug() << "ERROR: Total area too small!";
        return -1;
    }
    if (totalArea > MAX_TOTAL_AREA) {
        qDebug() << "ERROR: Total area too large!";
        return 1;
    }

    return 0;
}

bool House::doRoomsShareWall(Room& room1, Room& room2)
{
    QRectF rect1 = room1.get_rectRoom();
    QRectF rect2 = room2.get_rectRoom();

    // Check for horizontal walls (top/bottom edges)
    bool sharesHorizontalWall =
        // X-coordinates overlap
        (rect1.left() < rect2.right() && rect2.left() < rect1.right()) &&
        // Y-coordinates are adjacent (bottom of rect1 touches top of rect2 or vice versa)
        ((qAbs(rect1.bottom() - rect2.top()) < 5.0) || (qAbs(rect2.bottom() - rect1.top()) < 5.0));

    // Check for vertical walls (left/right edges)
    bool sharesVerticalWall =
        // Y-coordinates overlap
        (rect1.top() < rect2.bottom() && rect2.top() < rect1.bottom()) &&
        // X-coordinates are adjacent (right of rect1 touches left of rect2 or vice versa)
        ((qAbs(rect1.right() - rect2.left()) < 5.0) || (qAbs(rect2.right() - rect1.left()) < 5.0));

    return sharesHorizontalWall || sharesVerticalWall;
}

bool House::validateRoomConnectivity()
{
    if (rooms.size() <= 1) {
        // If there's only one room or no rooms, there's nothing to check
        return true;
    }

    // For each room, check if it shares a wall with at least one other room
    // or if it's completely contained within another room (like a closet)
    for (int i = 0; i < rooms.size(); i++) {
        bool hasSharedWall = false;
        bool isContainedInAnotherRoom = false;

        for (int j = 0; j < rooms.size(); j++) {
            if (i == j) continue; // Skip comparing with itself

            //Check if this room is completely inside another room (like a closet)
            if (rooms[j].get_rectRoom().contains(rooms[i].get_rectRoom())) {
                isContainedInAnotherRoom = true;
                qDebug() << "Room" << rooms[i].getId() << "is contained within room" << rooms[j].getId();
                break;
            }

            // Check for shared walls
            if (doRoomsShareWall(rooms[i], rooms[j])) {
                hasSharedWall = true;
                break;
            }
        }

        // A room is valid if it either shares a wall OR is contained in another room
        if (!hasSharedWall && !isContainedInAnotherRoom) {
            qDebug() << "ERROR: Room" << rooms[i].getId() << "doesn't share a wall with any other room and is not contained within another room!";
            return false;
        }
    }

    return true;
}

bool House::doRoomsIntersect(Room& room1, Room& room2)
{
    QRectF rect1 = room1.get_rectRoom();
    QRectF rect2 = room2.get_rectRoom();

    // Check if the rectangles intersect but neither fully contains the other
    if (rect1.intersects(rect2)) {
        // If one rectangle fully contains the other, it's not considered an intersection
        if (rect1.contains(rect2) || rect2.contains(rect1)) {
            return false; // Not an intersection case we're concerned with
        }
        return true;  // Partial intersection detected
    }

    return false;
}

bool House::validateNoRoomIntersections()
{
    if (rooms.size() <= 1) {
        // If there's only one room or no rooms, there's no intersection possible
        return true;
    }

    for (int i = 0; i < rooms.size(); i++) {
        for (int j = i + 1; j < rooms.size(); j++) {
            // Check for intersection between room i and room j
            if (doRoomsIntersect(rooms[i], rooms[j])) {
                qDebug() << "ERROR: Room" << rooms[i].getId() << "intersects with Room" << rooms[j].getId();
                return false;
            }
        }
    }

    return true;
}

bool House::validateDoorsOnWalls()
{
    for (Door& door : doors) {
        // Get the door's position
        QPointF doorOrigin = door.get_origin();
        // Increase tolerance to accommodate small movements
        double tolerance = 25.0;
        bool doorOnWall = false;

        for (Room& room : rooms) {
            QRectF rect = room.get_rectRoom();

            // Check if door is on left wall
            if (qAbs(doorOrigin.x() - rect.left()) < tolerance &&
                doorOrigin.y() >= rect.top() - tolerance && doorOrigin.y() <= rect.bottom() + tolerance) {
                doorOnWall = true;
                break;
            }

            // Check if door is on right wall
            if (qAbs(doorOrigin.x() - rect.right()) < tolerance &&
                doorOrigin.y() >= rect.top() - tolerance && doorOrigin.y() <= rect.bottom() + tolerance) {
                doorOnWall = true;
                break;
            }

            // Check if door is on top wall
            if (qAbs(doorOrigin.y() - rect.top()) < tolerance &&
                doorOrigin.x() >= rect.left() - tolerance && doorOrigin.x() <= rect.right() + tolerance) {
                doorOnWall = true;
                break;
            }

            // Check if door is on bottom wall
            if (qAbs(doorOrigin.y() - rect.bottom()) < tolerance &&
                doorOrigin.x() >= rect.left() - tolerance && doorOrigin.x() <= rect.right() + tolerance) {
                doorOnWall = true;
                break;
            }
        }

        if (!doorOnWall) {
            qDebug() << "ERROR: Door at position (" << doorOrigin.x() << ","
                     << doorOrigin.y() << ") is not placed on any wall!";
            return false;
        }
    }

    return true;
}

bool House::validateEveryRoomHasDoor()
{
    // If there's only one room, it doesn't need a door
    if (rooms.size() <= 1) {
        return true;
    }

    if(doors.size() < rooms.size() -1)
    {
        qDebug() << "not enough rooms";
        return false;
    }

    // Check each room
    for (Room& room : rooms) {
        QRectF roomRect = room.get_rectRoom();
        bool hasDoor = false;
        int tolerance = 15.0;
        // Check if any door is on this room's walls
        for (Door& door : doors) {
            QPointF doorOrigin = door.get_origin();

            // Check if door is on left wall
            if (qAbs(doorOrigin.x() - roomRect.left()) < tolerance &&
                doorOrigin.y() >= roomRect.top() && doorOrigin.y() <= roomRect.bottom()) {
                hasDoor = true;
                break;
            }

            // Check if door is on right wall
            if (qAbs(doorOrigin.x() - roomRect.right()) < tolerance &&
                doorOrigin.y() >= roomRect.top() && doorOrigin.y() <= roomRect.bottom()) {
                hasDoor = true;
                break;
            }

            // Check if door is on top wall
            if (qAbs(doorOrigin.y() - roomRect.top()) < tolerance &&
                doorOrigin.x() >= roomRect.left() && doorOrigin.x() <= roomRect.right()) {
                hasDoor = true;
                break;
            }

            // Check if door is on bottom wall
            if (qAbs(doorOrigin.y() - roomRect.bottom()) < tolerance &&
                doorOrigin.x() >= roomRect.left() && doorOrigin.x() <= roomRect.right()) {
                hasDoor = true;
                break;
            }
        }

        if (!hasDoor) {
            qDebug() << "ERROR: Room" << room.getId() << "doesn't have any doors!";
            return false;
        }
    }

    return true;
}
