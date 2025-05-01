#include "QDebug"
#include <QCursor>
#include <QVector>

#include "dragdrop.h"
#include "draw.h"

Draw::Draw(House *house, QGraphicsScene *scene)
{
    m_house = house;
    m_scene = scene;
}

void Draw::addSquareRoom()
{
    Room newSquareRoom("square");
    addRoom(newSquareRoom);
    qDebug() << "SQUARE ROOM";
}


void Draw::addRectRoom()
{
    Room newRectRoom("rectangle");
    addRoom(newRectRoom);
    qDebug() << "RECT ROOM";
}

void Draw::addRoom(Room room)
{
    // Add room to house first to get an ID

    // Add room to house first to get an ID
    m_house->addRoom(room);

    // Get the index of the last room added (which should be the one we just added)
    int lastIndex = m_house->rooms.size() - 1;

    // Get a direct pointer to the stored room in the house
    Room* actualRoom = &(m_house->rooms[lastIndex]);

    DragRoom *roomItem = new DragRoom(actualRoom->get_rectRoom(), m_scene, m_house, actualRoom, actualRoom->getId());
    roomItem->setPen(m_house->wall_pen);
    roomItem->setZValue(0);

    // Store the ID in the item for later reference
    roomItem->setData(0, actualRoom->getId());
    roomItem->setData(1, "room");

    m_scene->addItem(roomItem);
    m_house->setRoomFillColor(m_house->getFloorCovering());
    m_scene->update(m_scene->sceneRect());

    qDebug() << "ADDED " << room.get_shape() << " ROOM with ID:" << room.getId();
}

void Draw::addDoor()
{
    // Add door to house first to get an ID
    Door newDoor = Door();
    m_house->addDoor(newDoor);

    // IMPORTANT: Get a reference to the actual door in the vector
    int lastIndex = m_house->doors.size() - 1;
    Door* actualDoor = &(m_house->doors[lastIndex]);

    // Create line items but don't set their line directly
    // We'll let DragDoor handle the coordinate transformations
    QGraphicsLineItem *doorLine = new QGraphicsLineItem();
    QGraphicsLineItem *entryLine = new QGraphicsLineItem();

    // Pass the pointer to the actual door in the house
    DragDoor *doorItem = new DragDoor(doorLine, entryLine, m_scene, m_house, actualDoor);

    // Make sure the door lines are properly initialized with local coordinates
    doorItem->updateLines();

    // Store the ID for reference
    doorItem->setData(0, actualDoor->getId());
    doorItem->setData(1, "door");

    m_scene->addItem(doorItem);
    m_scene->update(m_scene->sceneRect());
    qDebug() << "DOOR added with ID:" << actualDoor->getId();
}

void Draw::addFurniture(Obstruction item)
{
    // Add obstruction to house first to get an ID
    m_house->addObstruction(item);

    // Always use the actual stored obstruction, not the temporary item
    int lastIndex = m_house->obstructions.size() - 1;
    Obstruction* actualObstruction = &(m_house->obstructions[lastIndex]);

    DragObstruction *dragItem;
    if (actualObstruction->get_isChest()) {
        dragItem = new DragObstruction(actualObstruction->get_rect(), actualObstruction->get_overlay(), m_house, actualObstruction);
    } else {
        dragItem = new DragObstruction(actualObstruction->get_rect(), actualObstruction->get_legs(), m_house, actualObstruction);
    }

    // Use the ID from the actual object!
    dragItem->setData(0, actualObstruction->getId());
    dragItem->setData(1, "obstruction");
    dragItem->setData(2, actualObstruction->get_type());

    m_scene->addItem(dragItem);
    dragItem->setZValue(1);
    m_scene->update(m_scene->sceneRect());

    qDebug() << actualObstruction->get_type() << "added with ID:" << actualObstruction->getId();
}


void Draw::addChest()
{
    Obstruction chest = Obstruction(true, "chest");
    addFurniture(chest);
}

void Draw::addTable()
{
    Obstruction table = Obstruction(QPointF(-70, 0), QPointF(70, 80), false, "table");
    addFurniture(table);
}

void Draw::addChair()
{
    Obstruction chair = Obstruction(QPointF(-20, 0), QPointF(20, 40), false, "chair");
    addFurniture(chair);
}

void Draw::changeFlooring()
{
    qDebug() << sender()->objectName();
    if(sender()->objectName() == "btn_hardFloor")
    {
        m_house->setRoomFillColor("hard_floor");
        m_house->setFloorCovering("hard_floor");
    }
    else if(sender()->objectName() == "btn_loopPile")
    {
        m_house->setRoomFillColor("loop_pile");
        m_house->setFloorCovering("loop_pile");
    }
    else if(sender()->objectName() == "btn_cutPile")
    {
        m_house->setRoomFillColor("cut_pile");
        m_house->setFloorCovering("cut_pile");
    }
    else if(sender()->objectName() == "btn_friezeCut")
    {
        m_house->setRoomFillColor("frieze_cut");
        m_house->setFloorCovering("frieze_cut");
    }
}

void Draw::addVacuum()
{
    QPointF center(0,80);
    int radius = 10;
    HouseVacuum *vacuum = new HouseVacuum(center, radius);

    // Add the vacuum to your house model
    m_house->vacuum = vacuum; // Assuming such a method exists

    // Create a rectangle with the center point and radius
    QRectF circleRect(
        center.x() - radius,
        center.y() - radius,
        radius * 2,
        radius * 2
        );

    // Create the draggable vacuum object
    DragVacuum *dragVacuum = new DragVacuum(circleRect, m_house, vacuum);

    // Add it to the scene
    m_scene->addItem(dragVacuum);
}
