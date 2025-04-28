#ifndef PATHALGORITHMS_H
#define PATHALGORITHMS_H

#include <QPointF>
#include <QRectF>
#include <QList>
#include <QSet>
#include <QMap>
#include <QPen>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QColor>
#include <map>              // for std::map
#include <QPoint>           // for QPoint
#include "houseparser.h"    // for Obstruction2

namespace PathAlgorithms
{

QPointF moveRandomly(const QPointF &currentPosition, QPointF &velocity, double speed,
                     const QRectF &roomRect, const QList<Obstruction2> &obstructions, const QList<QPointF> &doors);

QPointF updatePosition(QPointF &currentPosition, const QPointF &velocity,
                       QGraphicsScene *scene, const QRectF &roomRect, const QRectF &doorRect,
                       QGraphicsEllipseItem *vacuumItem, QPointF &lastTrailPoint,
                       std::map<QPoint, int> &cellVisitCount, QSet<QPoint> &cleanedCells,
                       const QString &floorType, int cellSize = 10);


QPointF moveWallFollowing(QPointF currentPosition, QPointF &velocity, double speed,
                          const QRectF &roomRect, const QList<Obstruction2> &obstructions);

QPointF moveSnaking(const QPointF &currentPosition, QPointF &velocity, double speed,
                    const QRectF &roomRect, const QList<Obstruction2> &obstructions,
                    bool &movingDown, bool &movingRight,
                    int &currentZoneX, int &currentZoneY,
                    bool &movingUpward);


QPointF moveSpiral(QPointF currentPosition, QPointF &velocity, double speed,
                   double &spiralAngle, double &spiralRadius,
                   const QRectF &roomRect, const QList<Obstruction2> &obstructions);


QPointF bounceOffWalls(QPointF currentPosition, QPointF &velocity, const QRectF &room, double speed);
}

#endif // PATHALGORITHMS_H
