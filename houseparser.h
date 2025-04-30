#ifndef HOUSEPARSER_H
#define HOUSEPARSER_H

#include <QRectF>
#include <QPointF>
#include <QList>
#include <QString>

struct Obstruction2 {
    QRectF rect;
    bool isChest;
};

class HouseParser
{
public:
    HouseParser(const QString &filePath);

    QList<QRectF> getRooms2() const;
    QList<QPointF> getDoors2() const;
    QList<Obstruction2> getObstructions2() const;

private:
    QList<QRectF> rooms2;
    QList<QPointF> doors2;
    QList<Obstruction2> obstructions2;

    void parseJson(const QString &filePath);
};

#endif // HOUSEPARSER_H
