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
    DragRoom *roomItem = new DragRoom(room.get_rectRoom(), m_scene);
    roomItem->setPen(m_house->wall_pen);
    roomItem->setZValue(0);
    m_scene->addItem(roomItem);
    m_house->addRoom(room);
    m_house->setRoomFillColor(QColor(196, 164, 132, 127), Qt::CrossPattern);
    m_scene->update(m_scene->sceneRect());

    qDebug() << "ADDED " << room.get_shape() << " ROOM";
}

void Draw::addDoor()
{
    QGraphicsLineItem *doorLine = new QGraphicsLineItem(door.get_door());
    QGraphicsLineItem *entryLine = new QGraphicsLineItem(door.get_entry());

    DragDoor *doorItem = new DragDoor(doorLine, entryLine, m_scene);
    m_scene->addItem(doorItem);
    m_scene->update(m_scene->sceneRect());

    qDebug() << "DOOR";
}

void Draw::addFurniture(Obstruction& item, QString name)
{
    DragObstruction *dragItem;

    if (item.get_isChest()) {
        dragItem = new DragObstruction(item.get_rect(), item.get_overlay());
    } else {
        Obstruction mutableItem = item;
        mutableItem.set_legs(10);
        dragItem = new DragObstruction(mutableItem.get_rect(), mutableItem.get_legs());
    }

    m_scene->addItem(dragItem);
    dragItem->setZValue(1);
    m_house->addObstruction(item);
    m_scene->update(m_scene->sceneRect());

    qDebug() << name;
}

void Draw::addChest()
{
    addFurniture(chest, "CHEST");
}

void Draw::addTable()
{
    addFurniture(table, "TABLE");
}

void Draw::addChair()
{
    addFurniture(chair, "CHAIR");
}

void Draw::changeFlooring()
{
    qDebug() << sender()->objectName();
    if(sender()->objectName() == "btn_hardFloor")
    {
        m_house->setRoomFillColor(QColor(196, 164, 132, 127), Qt::CrossPattern);
    }
    else if(sender()->objectName() == "btn_loopPile")
    {
        m_house->setRoomFillColor(QColor(50, 50, 255, 127), Qt::Dense7Pattern);
    }
    else if(sender()->objectName() == "btn_cutPile")
    {
        m_house->setRoomFillColor(QColor(200, 0, 0, 127), Qt::Dense6Pattern);
    }
    else if(sender()->objectName() == "btn_friezeCut")
    {
        m_house->setRoomFillColor(QColor(255, 255, 200, 127), Qt::Dense5Pattern);
    }
}
