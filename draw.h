#ifndef FloorPlan_H
#define FloorPlan_H

#include <QObject>
#include "house.h"
#include "dragdrop.h"

class Draw : public QObject
{
public:
    Draw(House *house, QGraphicsScene *scene);

    void addRoom(Room room); // Generic room adding method
    void addSquareRoom();
    void addRectRoom();
    void addDoor();
    void addFurniture(Obstruction item);
    void addChest();
    void addTable();
    void addChair();
    void changeFlooring();



private:
    QGraphicsScene *m_scene;
    House *m_house;
    Room default_square = Room(QString("square"));
    Room default_rect = Room(QString("rectangle"));
    Obstruction chest = Obstruction(true, "chest");
    Obstruction table = Obstruction(QPointF(-70, 0), QPointF(70, 80), false, "table");
    Obstruction chair = Obstruction(QPointF(-20, 0), QPointF(20, 40), false, "chair");
    Door door = Door();
};

#endif // FloorPlan_H
