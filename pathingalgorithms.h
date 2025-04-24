#ifndef PATHALGORITHMS_H
#define PATHALGORITHMS_H

#include <QPointF>
#include <QRectF>
#include <QList>
#include <QGraphicsRectItem> // if you use obstacles
#include "houseparser.h"

namespace PathAlgorithms
{
QPointF moveRandomly(const QPointF &currentPosition, QPointF &velocity, double speed,
                     const QRectF &roomRect, const QList<Obstruction2> &obstructions, const QList<QPointF> &doors);
}

#endif // PATHALGORITHMS_H
