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
    addRoom(default_square);
    qDebug() << "SQUARE ROOM";
}


void Draw::addRectRoom()
{
    addRoom(default_rect);
    qDebug() << "RECT ROOM";
}

void Draw::addRoom(Room& room)
{
    // Add room to house first to get an ID
    m_house->addRoom(room);
    int roomId = room.getId();

    DragRoom *roomItem = new DragRoom(room.get_rectRoom(), m_scene, m_house, &room, room.getId());
    roomItem->setPen(m_house->wall_pen);
    roomItem->setZValue(0);

    // Store the ID in the item for later reference
    roomItem->setData(0, roomId);
    roomItem->setData(1, "room");

    m_scene->addItem(roomItem);
    m_house->setRoomFillColor(QColor(196, 164, 132, 127), Qt::CrossPattern);
    m_scene->update(m_scene->sceneRect());

    qDebug() << "ADDED " << room.get_shape() << " ROOM with ID:" << roomId;
}

void Draw::addDoor()
{
    // Add door to house first to get an ID
    m_house->addDoor(door);
    int doorId = door.getId();

    QGraphicsLineItem *doorLine = new QGraphicsLineItem(door.get_door());
    QGraphicsLineItem *entryLine = new QGraphicsLineItem(door.get_entry());

    DragDoor *doorItem = new DragDoor(doorLine, entryLine, m_scene);

    // Store the ID in the item
    doorItem->setData(0, doorId);
    doorItem->setData(1, "door");

    m_scene->addItem(doorItem);
    m_scene->update(m_scene->sceneRect());

    qDebug() << "DOOR added with ID:" << doorId;
}

void Draw::addFurniture(Obstruction *item, QString name)
{
    // Add obstruction to house first to get an ID
    m_house->addObstruction(item);
    int obstructionId = item->getId();

DragObstruction *dragItem;
    if (item->get_isChest()) {

        dragItem = new DragObstruction(item->get_rect(), item->get_overlay(), m_house, item);
    } else {
        dragItem = new DragObstruction(item->get_rect(), item->get_legs(), m_house, item);
    }

    // Store the ID in the item
    dragItem->setData(0, obstructionId);
    dragItem->setData(1, "obstruction");

    m_scene->addItem(dragItem);
    dragItem->setZValue(1);
    m_scene->update(m_scene->sceneRect());

    qDebug() << name << "added with ID:" << obstructionId;
}

void Draw::addChest()
{
    addFurniture(&chest, "CHEST");
}

void Draw::addTable()
{
    addFurniture(&table, "TABLE");
}

void Draw::addChair()
{
    addFurniture(&chair, "CHAIR");
}

void Draw::changeFlooring()
{
    qDebug() << sender()->objectName();
    if(sender()->objectName() == "btn_hardFloor")
    {
        m_house->setRoomFillColor(QColor(196, 164, 132, 127), Qt::CrossPattern);
        m_house->flooring = "Hard";
    }
    else if(sender()->objectName() == "btn_loopPile")
    {
        m_house->setRoomFillColor(QColor(50, 50, 255, 127), Qt::Dense7Pattern);
        m_house->flooring = "Loop Pile";
    }
    else if(sender()->objectName() == "btn_cutPile")
    {
        m_house->setRoomFillColor(QColor(200, 0, 0, 127), Qt::Dense6Pattern);
        m_house->flooring = "Cut Pile";
    }
    else if(sender()->objectName() == "btn_friezeCut")
    {
        m_house->setRoomFillColor(QColor(255, 255, 200, 127), Qt::Dense5Pattern);
        m_house->flooring = "Frieze Cut";
    }
}
