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

Door::Door(QPointF origin, QPointF doorEnd, QPointF entryEnd)
    : m_origin(origin), m_doorEnd(doorEnd), m_entryEnd(entryEnd), m_id(0)
{
    // Calculate size from the door line for consistency
    m_size = QLineF(m_origin, m_doorEnd).length();
    updateLines();
}

Door::Door(QJsonObject door) : m_size(45)
{
    m_origin = QPointF(door.value("x").toInt(), door.value("y").toInt());
    m_doorEnd = QPointF(door.value("doorX").toInt(), door.value("doorY").toInt());
    m_entryEnd = QPointF(door.value("entryX").toInt(), door.value("entryY").toInt());

    // Update size based on actual door length
    m_size = QLineF(m_origin, m_doorEnd).length();

    updateLines();
}

void Door::updateLines()
{
    // Simply update line objects using the current endpoints
    m_door = QLineF(m_origin, m_doorEnd);
    m_entry = QLineF(m_origin, m_entryEnd);
}

QPointF Door::get_origin()
{
    return m_origin;
}

QPointF Door::get_doorEnd()
{
    return m_doorEnd;
}

QPointF Door::get_entryEnd()
{
    return m_entryEnd;
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
    // Calculate vectors (offsets) before changing origin
    QPointF doorOffset = m_doorEnd - m_origin;
    QPointF entryOffset = m_entryEnd - m_origin;

    // Update origin
    m_origin = origin;

    // Apply same offsets to maintain door shape and orientation
    m_doorEnd = m_origin + doorOffset;
    m_entryEnd = m_origin + entryOffset;

    // Update the line objects directly
    m_door = QLineF(m_origin, m_doorEnd);
    m_entry = QLineF(m_origin, m_entryEnd);
}

void Door::set_doorEnd(QPointF doorEnd)
{
    m_doorEnd = doorEnd;
    m_door = QLineF(m_origin, m_doorEnd);
}

// Door::set_entryEnd - Modified version
void Door::set_entryEnd(QPointF entryEnd)
{
    m_entryEnd = entryEnd;
    m_entry = QLineF(m_origin, m_entryEnd);
}


void Door::set_size(float size)
{
    m_size = size;

    // Adjust the endpoints based on the direction vectors
    QPointF doorDir = m_doorEnd - m_origin;
    QPointF entryDir = m_entryEnd - m_origin;

    // Normalize and scale by new size
    if (!qFuzzyIsNull(doorDir.manhattanLength())) {
        qreal len = QLineF(QPointF(0, 0), doorDir).length();
        doorDir = doorDir * (m_size / len);
        m_doorEnd = m_origin + doorDir;
    }

    if (!qFuzzyIsNull(entryDir.manhattanLength())) {
        qreal len = QLineF(QPointF(0, 0), entryDir).length();
        entryDir = entryDir * (m_size / len);
        m_entryEnd = m_origin + entryDir;
    }

    updateLines();
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

    return totalArea;
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
    floorplan_id = 0;
    loadPlan(defaultPlanLocation);
}

void House::setScene(QGraphicsScene* scene)
{
    m_scene = scene;
}

QGraphicsScene* House::getScene() const
{
    return m_scene;
}

int House::getFloorplanId() const
{
    return floorplan_id;
}

void House::setFloorplanId(int id)
{
    floorplan_id = id;
}

void House::generateNewFloorplanId()
{
    // Assign the next available ID and increment it
    floorplan_id = next_floorplan_id++;
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
    floorplan_id = QRandomGenerator::global()->bounded(10000, 99999);
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
    floor_covering = root.value("flooring").toString();
    floorplan_id = root.value("floorplan_id").toInt();

    // Make sure next_floorplan_id stays ahead of any loaded IDs
    if (floorplan_id >= next_floorplan_id) {
        next_floorplan_id = floorplan_id + 1;
    }


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

void House::loadNonInteractivePlan(QString plan)
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
    floor_covering = root.value("flooring").toString();
    floorplan_id = root.value("floorplan_id").toInt();

    // Make sure next_floorplan_id stays ahead of any loaded IDs
    if (floorplan_id >= next_floorplan_id) {
        next_floorplan_id = floorplan_id + 1;
    }

    QJsonArray roomsArray = root.value("rooms").toArray();
    loadEntities<Room>(roomsArray, rooms, [](QJsonObject& obj){ return Room(obj); });

    QJsonArray doorsArray = root.value("doors").toArray();
    loadEntities<Door>(doorsArray, doors, [](QJsonObject& obj){ return Door(obj); });

    QJsonArray obstructionsArray = root.value("obstructions").toArray();
    loadEntities<Obstruction>(obstructionsArray, obstructions, [](QJsonObject& obj){ return Obstruction(obj); });

    drawSimulationPlan();
    setRoomFillColor(floor_covering);
    m_scene->update(m_scene->sceneRect());

}

void House::createNewFloorplan()
{
    clear();
    floorplan_name = "Untitled";
    floor_covering = "hard_floor";  // Default flooring

    // Generate a new unique ID for this floorplan
    generateNewFloorplanId();

    // Add a default room if needed
    Room defaultRoom("square");
    addRoom(defaultRoom);
    drawRooms();

    setRoomFillColor(floor_covering);
    m_scene->update(m_scene->sceneRect());
}

QJsonDocument House::toJson()
{
    QJsonArray roomsArray = QJsonArray();
    QJsonArray obstructionsArray = QJsonArray();
    QJsonArray doorsArray = QJsonArray();


    QJsonObject vacuumPos
    {
        {"vacuumX", vacuum->get_center().x()},
        {"vacuumY", vacuum->get_center().y()},
    };

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
            {"y", doors[i].get_origin().y()},
            {"doorX", doors[i].get_doorEnd().x()},
            {"doorY", doors[i].get_doorEnd().y()},
            {"entryX", doors[i].get_entryEnd().x()},
            {"entryY", doors[i].get_entryEnd().y()}
        };

        doorsArray.append(object);
        qDebug() << doorsArray;
    }

    QJsonObject root
    {
        {"vacuum_pos", vacuumPos},
        {"floorplan_id", floorplan_id},
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

void House::set_floorplanName(QString name)
{
    floorplan_name = name;
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

            Room *r = dynamic_cast<Room *>(item);
            if (r)
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

            Room *r = dynamic_cast<Room *>(item);
            if (r)
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

            Room *r = dynamic_cast<Room *>(item);
            if (r)
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

            Room *r = dynamic_cast<Room *>(item);
            if (r)
            {
                // Create a brush with cross pattern
                QBrush brush(QColor(255, 255, 200, 127), Qt::Dense5Pattern);
                room->setBrush(brush);
            }
        }
    }

    m_scene->update(m_scene->sceneRect());
}

int House::getOpenArea()
{
    int coveredArea = 0;
    totalArea = 0;
    for (Obstruction& obstruction : obstructions) {
        coveredArea += static_cast<int>(obstruction.get_floorCoverage());
    }

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

    total_area = totalArea;

    return totalArea - coveredArea;
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

void House::rotate()
{
    for (QGraphicsItem* item : m_scene->selectedItems()) {
        if (auto dragDoor = dynamic_cast<DragDoor*>(item)) {
            // Get the associated Door object
            Door* doorData = dragDoor->getDoor();
            if (doorData) {
                // Get current positions
                QPointF origin = doorData->get_origin();
                QPointF doorEnd = doorData->get_doorEnd();
                QPointF entryEnd = doorData->get_entryEnd();

                // Calculate vectors relative to origin
                QPointF doorVector = doorEnd - origin;
                QPointF entryVector = entryEnd - origin;

                // Perform 90-degree clockwise rotation of the vectors
                // For clockwise rotation: (x, y) -> (y, -x)
                QPointF rotatedDoorVector(doorVector.y(), -doorVector.x());
                QPointF rotatedEntryVector(entryVector.y(), -entryVector.x());

                // Calculate new endpoint positions
                QPointF newDoorEnd = origin + rotatedDoorVector;
                QPointF newEntryEnd = origin + rotatedEntryVector;

                // Update the Door model with new endpoints
                doorData->set_doorEnd(newDoorEnd);
                doorData->set_entryEnd(newEntryEnd);

                // Update the visual representation
                dragDoor->updateLines();

                // Make sure the dragDoor position is synchronized with the Door model
                // This depends on the DragDoor implementation, but generally:
                dragDoor->setPos(origin);

                qDebug() << "After rotation - Origin:" << origin
                         << "DoorEnd:" << newDoorEnd
                         << "EntryEnd:" << newEntryEnd;
            }
        }
    }

    // Update the scene
    m_scene->update(m_scene->sceneRect());
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
        ((qAbs(rect1.bottom() - rect2.top()) < 15.0) || (qAbs(rect2.bottom() - rect1.top()) < 15.0));

    // Check for vertical walls (left/right edges)
    bool sharesVerticalWall =
        // Y-coordinates overlap
        (rect1.top() < rect2.bottom() && rect2.top() < rect1.bottom()) &&
        // X-coordinates are adjacent (right of rect1 touches left of rect2 or vice versa)
        ((qAbs(rect1.right() - rect2.left()) < 15.0) || (qAbs(rect2.right() - rect1.left()) < 15.0));

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

    QRectF overlap = rect1.intersected(rect2);
    if (overlap.width() > 50 && overlap.height() > 50) {
        // Considered significant intersection
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
        // Get the door line
        QLineF doorLine = door.get_door();
        QPointF doorStart = doorLine.p1();
        QPointF doorEnd = doorLine.p2();

        // Calculate the midpoint of the door line for better tolerance
        QPointF doorMidpoint((doorStart.x() + doorEnd.x()) / 2,
                             (doorStart.y() + doorEnd.y()) / 2);

        // Increase tolerance to accommodate small movements
        double tolerance = 25.0;
        bool doorOnWall = false;

        for (Room& room : rooms) {
            QRectF rect = room.get_rectRoom();

            // Check if any part of the door line is on a wall
            // Left wall
            if ((qAbs(doorStart.x() - rect.left()) < tolerance &&
                 doorStart.y() >= rect.top() - tolerance && doorStart.y() <= rect.bottom() + tolerance) ||
                (qAbs(doorEnd.x() - rect.left()) < tolerance &&
                 doorEnd.y() >= rect.top() - tolerance && doorEnd.y() <= rect.bottom() + tolerance) ||
                (qAbs(doorMidpoint.x() - rect.left()) < tolerance &&
                 doorMidpoint.y() >= rect.top() - tolerance && doorMidpoint.y() <= rect.bottom() + tolerance)) {
                doorOnWall = true;
                break;
            }

            // Right wall
            if ((qAbs(doorStart.x() - rect.right()) < tolerance &&
                 doorStart.y() >= rect.top() - tolerance && doorStart.y() <= rect.bottom() + tolerance) ||
                (qAbs(doorEnd.x() - rect.right()) < tolerance &&
                 doorEnd.y() >= rect.top() - tolerance && doorEnd.y() <= rect.bottom() + tolerance) ||
                (qAbs(doorMidpoint.x() - rect.right()) < tolerance &&
                 doorMidpoint.y() >= rect.top() - tolerance && doorMidpoint.y() <= rect.bottom() + tolerance)) {
                doorOnWall = true;
                break;
            }

            // Top wall
            if ((qAbs(doorStart.y() - rect.top()) < tolerance &&
                 doorStart.x() >= rect.left() - tolerance && doorStart.x() <= rect.right() + tolerance) ||
                (qAbs(doorEnd.y() - rect.top()) < tolerance &&
                 doorEnd.x() >= rect.left() - tolerance && doorEnd.x() <= rect.right() + tolerance) ||
                (qAbs(doorMidpoint.y() - rect.top()) < tolerance &&
                 doorMidpoint.x() >= rect.left() - tolerance && doorMidpoint.x() <= rect.right() + tolerance)) {
                doorOnWall = true;
                break;
            }

            // Bottom wall
            if ((qAbs(doorStart.y() - rect.bottom()) < tolerance &&
                 doorStart.x() >= rect.left() - tolerance && doorStart.x() <= rect.right() + tolerance) ||
                (qAbs(doorEnd.y() - rect.bottom()) < tolerance &&
                 doorEnd.x() >= rect.left() - tolerance && doorEnd.x() <= rect.right() + tolerance) ||
                (qAbs(doorMidpoint.y() - rect.bottom()) < tolerance &&
                 doorMidpoint.x() >= rect.left() - tolerance && doorMidpoint.x() <= rect.right() + tolerance)) {
                doorOnWall = true;
                break;
            }
        }

        if (!doorOnWall) {
            qDebug() << "ERROR: Door with line from" << doorStart << "to" << doorEnd << "is not placed on any wall!";
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

    if(doors.size() < rooms.size() - 1)
    {
        qDebug() << "Not enough doors for the number of rooms";
        return false;
    }

    // Check each room
    for (Room& room : rooms) {
        QRectF roomRect = room.get_rectRoom();
        bool hasDoor = false;
        double tolerance = 15.0;

        // Check if any door is on this room's walls
        for (Door& door : doors) {
            QLineF doorLine = door.get_door();
            QPointF doorStart = doorLine.p1();
            QPointF doorEnd = doorLine.p2();

            // Calculate the midpoint of the door line
            QPointF doorMidpoint((doorStart.x() + doorEnd.x()) / 2,
                                 (doorStart.y() + doorEnd.y()) / 2);

            // Check all four walls with all parts of the door

            // Left wall check
            if ((qAbs(doorStart.x() - roomRect.left()) < tolerance &&
                 doorStart.y() >= roomRect.top() && doorStart.y() <= roomRect.bottom()) ||
                (qAbs(doorEnd.x() - roomRect.left()) < tolerance &&
                 doorEnd.y() >= roomRect.top() && doorEnd.y() <= roomRect.bottom()) ||
                (qAbs(doorMidpoint.x() - roomRect.left()) < tolerance &&
                 doorMidpoint.y() >= roomRect.top() && doorMidpoint.y() <= roomRect.bottom())) {
                hasDoor = true;
                break;
            }

            // Right wall check
            if ((qAbs(doorStart.x() - roomRect.right()) < tolerance &&
                 doorStart.y() >= roomRect.top() && doorStart.y() <= roomRect.bottom()) ||
                (qAbs(doorEnd.x() - roomRect.right()) < tolerance &&
                 doorEnd.y() >= roomRect.top() && doorEnd.y() <= roomRect.bottom()) ||
                (qAbs(doorMidpoint.x() - roomRect.right()) < tolerance &&
                 doorMidpoint.y() >= roomRect.top() && doorMidpoint.y() <= roomRect.bottom())) {
                hasDoor = true;
                break;
            }

            // Top wall check
            if ((qAbs(doorStart.y() - roomRect.top()) < tolerance &&
                 doorStart.x() >= roomRect.left() && doorStart.x() <= roomRect.right()) ||
                (qAbs(doorEnd.y() - roomRect.top()) < tolerance &&
                 doorEnd.x() >= roomRect.left() && doorEnd.x() <= roomRect.right()) ||
                (qAbs(doorMidpoint.y() - roomRect.top()) < tolerance &&
                 doorMidpoint.x() >= roomRect.left() && doorMidpoint.x() <= roomRect.right())) {
                hasDoor = true;
                break;
            }

            // Bottom wall check
            if ((qAbs(doorStart.y() - roomRect.bottom()) < tolerance &&
                 doorStart.x() >= roomRect.left() && doorStart.x() <= roomRect.right()) ||
                (qAbs(doorEnd.y() - roomRect.bottom()) < tolerance &&
                 doorEnd.x() >= roomRect.left() && doorEnd.x() <= roomRect.right()) ||
                (qAbs(doorMidpoint.y() - roomRect.bottom()) < tolerance &&
                 doorMidpoint.x() >= roomRect.left() && doorMidpoint.x() <= roomRect.right())) {
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

bool House::doesObstructionIntersectRoom(Obstruction& obstruction, Room& room)
{
    QRectF obsRect;

    // For chest type obstructions, use the full rectangle
    if (obstruction.get_isChest()) {
        obsRect = obstruction.get_rect();
    } else {
        // For non-chest obstructions, we only check if any leg intersects with the room
        QRectF* legs = obstruction.get_legs();
        for (int i = 0; i < 4; i++) {
            if (room.get_rectRoom().intersects(legs[i])) {
                return true;
            }
        }
        return false;
    }

    // Check if the obstruction rect intersects with the room's rect
    return room.get_rectRoom().intersects(obsRect);
}

// Add this method to the House class to validate all obstructions
bool House::validateObstructionPlacements()
{
    for (Obstruction& obstruction : obstructions) {
        if (!isObstructionInsideAnyRoom(obstruction)) {
            qDebug() << "ERROR: Obstruction" << obstruction.getId()
            << "(" << obstruction.get_type() << ")"
            << "is not fully contained within any room!";
            return false;
        }
    }

    // First check if any obstruction is outside of all rooms
    for (Obstruction& obstruction : obstructions) {
        bool isInsideAnyRoom = false;

        for (Room& room : rooms) {
            QRectF roomRect = room.get_rectRoom();

            // For chest type, check if the entire obstruction is inside a room
            if (obstruction.get_isChest()) {
                QRectF obsRect = obstruction.get_rect();
                if (roomRect.contains(obsRect)) {
                    isInsideAnyRoom = true;
                    break;
                }
            } else {
                // For furniture with legs, check if all legs are inside a room
                QRectF* legs = obstruction.get_legs();
                bool allLegsInside = true;

                for (int i = 0; i < 4; i++) {
                    if (!roomRect.contains(legs[i])) {
                        allLegsInside = false;
                        break;
                    }
                }

                if (allLegsInside) {
                    isInsideAnyRoom = true;
                    break;
                }
            }
        }

        if (!isInsideAnyRoom) {
            qDebug() << "ERROR: Obstruction" << obstruction.getId() << "is not fully inside any room!";
            return false;
        }
    }

    // Next check if any obstruction intersects with room walls
    for (Obstruction& obstruction : obstructions) {
        for (Room& room : rooms) {
            // We don't want to check if the obstruction is inside the room,
            // only if it's on the border (intersection with walls)

            QRectF roomRect = room.get_rectRoom();
            QRectF obsRect = obstruction.get_rect();

            // Skip if obstruction is fully inside or fully outside this room
            if (roomRect.contains(obsRect) || !roomRect.intersects(obsRect)) {
                continue;
            }

            // If we get here, the obstruction intersects with a room wall
            qDebug() << "ERROR: Obstruction" << obstruction.getId()
                     << "intersects with room" << room.getId() << "wall!";
            return false;
        }
    }

    // Also check if obstructions are blocking doors
    for (Obstruction& obstruction : obstructions) {
        QRectF obsRect = obstruction.get_rect();

        for (Door& door : doors) {
            QLineF doorLine = door.get_door();
            QLineF entryLine = door.get_entry();

            // Create a small rect around the door for intersection checking
            QRectF doorRect(
                QPointF(qMin(doorLine.p1().x(), doorLine.p2().x()) - 5,
                        qMin(doorLine.p1().y(), doorLine.p2().y()) - 5),
                QPointF(qMax(doorLine.p1().x(), doorLine.p2().x()) + 5,
                        qMax(doorLine.p1().y(), doorLine.p2().y()) + 5)
                );

            // Create a small rect around the entry line
            QRectF entryRect(
                QPointF(qMin(entryLine.p1().x(), entryLine.p2().x()) - 5,
                        qMin(entryLine.p1().y(), entryLine.p2().y()) - 5),
                QPointF(qMax(entryLine.p1().x(), entryLine.p2().x()) + 5,
                        qMax(entryLine.p1().y(), entryLine.p2().y()) + 5)
                );

            // Check if obstruction intersects with door or entry line
            if (obsRect.intersects(doorRect) || obsRect.intersects(entryRect)) {
                qDebug() << "ERROR: Obstruction" << obstruction.getId()
                << "is blocking a door!";
                return false;
            }
        }
    }

    return true;
}

bool House::doObstructionsIntersect(Obstruction& obs1, Obstruction& obs2)
{
    // Check if the full rectangles intersect, regardless of legs or chest type
    return obs1.get_rect().intersects(obs2.get_rect());
}

// Validate that no obstructions intersect with each other
bool House::validateNoObstructionIntersections()
{
    if (obstructions.size() <= 1) {
        // If there's only one obstruction or none, there's no intersection possible
        return true;
    }

    for (int i = 0; i < obstructions.size(); i++) {
        for (int j = i + 1; j < obstructions.size(); j++) {
            // Check for intersection between obstruction i and obstruction j
            if (doObstructionsIntersect(obstructions[i], obstructions[j])) {
                qDebug() << "ERROR: Obstruction" << obstructions[i].getId()
                << "(" << obstructions[i].get_type() << ")"
                << "intersects with Obstruction" << obstructions[j].getId()
                << "(" << obstructions[j].get_type() << ")";
                return false;
            }
        }
    }

    return true;
}

bool House::isObstructionInsideAnyRoom(Obstruction& obstruction)
{
    // Get the full rectangle of the obstruction
    QRectF obsRect = obstruction.get_rect();

    // Check if this obstruction is fully contained in any room
    for (Room& room : rooms) {
        QRectF roomRect = room.get_rectRoom();
        if (roomRect.contains(obsRect)) {
            return true; // Fully contained in this room
        }
    }

    return false; // Not fully contained in any room
}

