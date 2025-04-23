#include "house.h"

#include <QResource>

static bool pointsFuzzyEqual(const QPointF &a, const QPointF &b, qreal epsilon = 0.1)
{
    return qAbs(a.x() - b.x()) < epsilon && qAbs(a.y() - b.y()) < epsilon;
}

static bool linesFuzzyMatch(const QLineF &a, const QLineF &b, qreal epsilon = 0.1)
{
    return (pointsFuzzyEqual(a.p1(), b.p1(), epsilon) && pointsFuzzyEqual(a.p2(), b.p2(), epsilon)) ||
           (pointsFuzzyEqual(a.p1(), b.p2(), epsilon) && pointsFuzzyEqual(a.p2(), b.p1(), epsilon));
}

House::House(QGraphicsScene *scene)
{
    scene = scene;
    wall_pen.setWidth(4);
    obstruct_pen.setWidth(3);
    loadPlan();
}

void House::drawRooms(QVector<Room> rooms)
{
    for(int i = 0; i < rooms.size(); i++)
    {
        DragItem *rect_item = new DragItem(rooms[i].get_rectRoom(), m_scene);
        rect_item->setPen(wall_pen);

        rect_item->setData(0, rooms[i].getId());
        rect_item->setData(1, "room");

        m_scene->addItem(rect_item);

    }
}

void House::drawDoors(QVector<Door> doors)
{
    for (Door &door : doors)
    {
        QGraphicsLineItem *doorLine = new QGraphicsLineItem(door.get_door());
        QGraphicsLineItem *entryLine = new QGraphicsLineItem(door.get_entry());

        DoorItem *doorItem = new DoorItem(doorLine, entryLine, m_scene);
        m_scene->addItem(doorItem);
    }
}

void House::drawObstructions(QVector<Obstruction> obstructions)
{
    QPen overlay_pen;
    for(int i = 0; i < obstructions.size(); i++)
    {
        if(obstructions[i].get_isChest())
        {
            DragObstruction *item = new DragObstruction(obstructions[i].get_rect(), obstructions[i].get_overlay());
            m_scene->addItem(item);
            item->setZValue(1);        }
        else
        {
            obstructions[i].set_legs(10);

            DragObstruction *item = new DragObstruction(obstructions[i].get_rect(), obstructions[i].get_legs());
            m_scene->addItem(item);
            item->setZValue(1);
        }
    }
}

void House::loadRooms(QJsonArray roomsArray)
{
    clear();
    for(int i = 0; i < roomsArray.size(); i++)
    {
        QJsonObject obj = roomsArray[i].toObject();
        Room rectRoom = Room(obj);
        rooms.append(rectRoom);
    }
}

void House::loadDoors(QJsonArray doorsArray)
{
    for(int i = 0; i < doorsArray.size(); i++)
    {
        QJsonObject obj = doorsArray[i].toObject();
        Door door = Door(obj);
        doors.append(door);
    }
}

void House::loadObstructions(QJsonArray obstructionsArray)
{
    for(int i = 0; i < obstructionsArray.size(); i++)
    {
        QJsonObject obj = obstructionsArray[i].toObject();
        Obstruction obstruct = Obstruction(obj);
        obstructions.append(obstruct);
    }
}

void House::addRoom(Room room)
{
    room.setId(next_id++);
    rooms.append(room);
}

void House::addObstruction(Obstruction obstruction)
{
    obstruction.setId(next_id++);
    obstructions.append(obstruction);
}

void House::addDoor(Door door)
{
    door.setId(next_id++);
    doors.append(door);
}

void House::loadPlan()
{
    clear();

    QFile file(floorplan_name);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file";
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    //TODO: check for errors when reading
    file.close();

    // floorplan_name = plan;

    QJsonObject root = doc.object();
    QJsonArray roomsArray = root.value("rooms").toArray();
    loadRooms(roomsArray);
    drawRooms(rooms);

    QJsonArray doorsArray = root.value("doors").toArray();
    loadDoors(doorsArray);
    drawDoors(doors);

    QJsonArray obstructionsArray = root.value("obstructions").toArray();
    loadObstructions(obstructionsArray);
    drawObstructions(obstructions);
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
        //qDebug() << roomsArray;
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
        //qDebug() << obstructionsArray;
    }

    for(int i = 0; i < doors.size(); i++)
    {
        QJsonObject object
            {
                {"x", doors[i].get_origin().x()},
                {"y", doors[i].get_origin().y()}
            };

        doorsArray.append(object);
        qDebug() << doors[i].get_origin();
        qDebug() << doorsArray;
    }

    QJsonObject root
        {
            {"rooms", roomsArray},
            {"obstructions", obstructionsArray},
            {"doors", doorsArray}
        };

    QJsonDocument doc = QJsonDocument(root);
    return doc;
}

void House::setRoomFillColor(QColor color, Qt::BrushStyle style)
{
    QList<QGraphicsItem *> items = m_scene->items();

    for (QGraphicsItem *item : items)
    {
        // Check if it's a room
        DragItem *room = dynamic_cast<DragItem *>(item);
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
    next_id = 1;
    m_scene->update(m_scene->sceneRect());
    //floorplan_name = "";
}

void House::deleteItem()
{
    QList<QGraphicsItem*> selectedItems = m_scene->selectedItems();

    for (QGraphicsItem *item : selectedItems)
    {
        QString type = item->data(1).toString();
        int id = item->data(0).toInt();

        if (type == "room") {
            for (int i = 0; i < rooms.size(); ++i) {
                if (rooms[i].getId() == id) {
                    rooms.remove(i);
                    break;
                }
            }
        } else if (type == "obstruction") {
            for (int i = 0; i < obstructions.size(); ++i) {
                if (obstructions[i].getId() == id) {
                    obstructions.remove(i);
                    break;
                }
            }
        } else if (type == "door") {
            for (int i = 0; i < doors.size(); ++i) {
                if (doors[i].getId() == id) {
                    doors.remove(i);
                    break;
                }
            }
        }

        m_scene->removeItem(item);
        delete item;
    }

    m_scene->update(m_scene->sceneRect());

    qDebug() << "DELETE";
}

bool House::obstructionIntersection()
{
    for (QGraphicsItem *item : m_scene->items())
    {
        auto *obstruction = dynamic_cast<DragObstruction *>(item);
        if (!obstruction) continue;

        for (QGraphicsItem *other : m_scene->items())
        {
            if (other == obstruction) continue;

            auto *room = dynamic_cast<DragItem *>(other);
            if (!room) continue;

            if (obstruction->collidesWithItem(room))
            {
                qDebug() << "Error: Obstruction intersects with room.";
                // Optionally display a dialog or flag the error
                return false;
            }
        }
    }

    return true;
}

bool House::roomIntersection()
{
    QList<QGraphicsItem*> items = m_scene->items();

    for (int i = 0; i < items.size(); ++i)
    {
        DragItem *roomA = dynamic_cast<DragItem *>(items[i]);
        if (!roomA) continue;

        for (int j = i + 1; j < items.size(); ++j)
        {
            DragItem *roomB = dynamic_cast<DragItem *>(items[j]);
            if (!roomB) continue;

            if (roomA->collidesWithItem(roomB))
            {
                qDebug() << "Error: Room intersects with another room.";
                return false; // return empty to cancel save
            }
        }
    }

    return true;
}

void House::verticalFlip()
{
    QList<QGraphicsItem *> selectedItems = m_scene->selectedItems();

    for (QGraphicsItem *item : selectedItems)
    {
        if (!item) return;

        QRectF bounds = item->boundingRect();
        QPointF center = bounds.center();

        QTransform transform;
        transform.translate(center.x(), center.y());
        transform.scale(1, -1);  // Flip vertically
        transform.translate(-center.x(), -center.y());

        item->setTransform(transform * item->transform());
    }
}

void House::horizontalFlip()
{
    QList<QGraphicsItem *> selectedItems = m_scene->selectedItems();

    for (QGraphicsItem *item : selectedItems)
    {
        if (!item) return;

        QRectF bounds = item->boundingRect();
        QPointF center = bounds.center();

        QTransform transform;
        transform.translate(center.x(), center.y());
        transform.scale(-1, 1);  // Flip horizontal
        transform.translate(-center.x(), -center.y());

        item->setTransform(transform * item->transform());
    }
}

bool House::roomsShareWall(QRectF &a, QRectF &b)
{
    constexpr qreal EPSILON = 0.01;

    // Shared vertical wall
    bool verticalTouch =
        qAbs(a.right() - b.left()) < EPSILON || qAbs(b.right() - a.left()) < EPSILON;
    bool verticalAligned =
        !(a.bottom() < b.top() || a.top() > b.bottom());

    if (verticalTouch && verticalAligned)
        return true;

    // Shared horizontal wall
    bool horizontalTouch =
        qAbs(a.bottom() - b.top()) < EPSILON || qAbs(b.bottom() - a.top()) < EPSILON;
    bool horizontalAligned =
        !(a.right() < b.left() || a.left() > b.right());

    if (horizontalTouch && horizontalAligned)
        return true;

    return false;
}

// Helper function to get the walls of a room as QLineF
QVector<QLineF> getRoomWalls(Room &room)
{
    QRectF rect = room.get_rectRoom();
    return {
        QLineF(rect.topLeft(), rect.topRight()),
        QLineF(rect.topRight(), rect.bottomRight()),
        QLineF(rect.bottomRight(), rect.bottomLeft()),
        QLineF(rect.bottomLeft(), rect.topLeft())
    };
}

// Function to check if two rooms share a wall
QLineF House::getSharedWall(Room &room1, Room &room2)
{
    QVector<QLineF> walls1 = getRoomWalls(room1);
    QVector<QLineF> walls2 = getRoomWalls(room2);

    for (QLineF &wall1 : walls1)
    {
        for (QLineF &wall2 : walls2)
        {
            if (linesFuzzyMatch(wall1, wall2))
            {
                return wall1; // Return the shared wall
            }
        }
    }
    return QLineF(); // Return an invalid QLineF if no shared wall
}

// Function to check if a wall has a door
bool House::wallHasDoor(QLineF &wall)
{
    for (Door &door : doors)
    {
        QLineF entryLine = door.get_entry();
        if (linesFuzzyMatch(entryLine, wall))
        {
            return true; // Wall has a door
        }
    }
    return false; // Wall has no door
}

// Function to check if all shared walls have doors
bool House::allSharedWallsHaveDoors()
{
    if (rooms.size() <= 1)
    {
        return true; // No shared walls if there's only one or zero rooms
    }

    // Iterate through all pairs of rooms
    for (int i = 0; i < rooms.size(); ++i)
    {
        for (int j = i + 1; j < rooms.size(); ++j) // Avoid checking the same pair twice
        {
            QLineF sharedWall = getSharedWall(rooms[i], rooms[j]);
            if (sharedWall.length() > 0 && !wallHasDoor(sharedWall)) //check that the wall is actually a wall
            {
                return false; // Found a shared wall without a door
            }
        }
    }
    return true; // All shared walls have doors
}

// Function to check if a room has at least one door on its walls
bool House::roomHasDoor(Room &room)
{
    QVector<QLineF> walls = getRoomWalls(room);

    for (Door &door : doors)
    {
        QLineF entryLine = door.get_entry();
        for (QLineF &wall : walls)
        {
            if (linesFuzzyMatch(entryLine, wall))
            {
                return true; // Room has a door on this wall
            }
        }
    }
    return false; // Room has no doors
}

// Function to check if all rooms have at least one door
bool House::roomsHaveDoors()
{
    for (Room &room : rooms)
    {
        if (!roomHasDoor(room))
        {
            return false; // Found a room without a door
        }
    }
    return true; // All rooms have at least one door
}

// Function to check if two rooms share a wall
bool House::roomsShareWall(Room &room1, Room &room2)
{
    QVector<QLineF> walls1 = getRoomWalls(room1);
    QVector<QLineF> walls2 = getRoomWalls(room2);

    for (QLineF &wall1 : walls1)
    {
        for (QLineF &wall2 : walls2)
        {
            if (linesFuzzyMatch(wall1, wall2))
            {
                return true; // Found a shared wall
            }
        }
    }
    return false; // No shared wall
}

// Function to check if all rooms are connected
bool House::allRoomsConnected()
{
    qDebug() << "House::allRoomsConnected() called";
    if (rooms.size() <= 1)
    {
        return true; // Zero or one room is considered connected
    }

    QVector<Room> connectedRooms;
    connectedRooms.append(rooms.first());

    QVector<Room> unconnectedRooms;
    for (int i = 1; i < rooms.size(); ++i)
    {
        unconnectedRooms.append(rooms[i]);
    }

    bool changed = true;
    while (changed)
    {
        changed = false;
        for (int i = 0; i < unconnectedRooms.size();)
        {
            bool foundConnection = false;
            for (Room &connectedRoom : connectedRooms)
            {
                if (roomsShareWall(unconnectedRooms[i], connectedRoom))
                {
                    connectedRooms.append(unconnectedRooms[i]);
                    unconnectedRooms.removeAt(i);
                    changed = true;
                    foundConnection = true;
                    break;
                }
            }
            if (!foundConnection)
            {
                ++i;
            }
        }
    }

    return unconnectedRooms.isEmpty();
}

qreal House::getTotalRoomArea()
{
    qreal totalArea = 0.0;
    for (Room &room : rooms) {
        QRectF rect = room.get_rectRoom();
        totalArea += rect.width() * rect.height();
        qDebug() << totalArea;
    }

    return totalArea;
}
