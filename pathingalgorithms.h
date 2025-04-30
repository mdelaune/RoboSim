#ifndef PATHINGALGORITHMS_H
#define PATHINGALGORITHMS_H

#include <QPointF>
#include <QList>
//#include "houseparser.h"
class Obstruction2;

namespace PathAlgorithms
{

QPointF moveRandomly(const QPointF &currentPosition, QPointF &velocity, double speed,
                     double roomLeft, double roomRight, double roomTop, double roomBottom,
                     const QList<Obstruction2> &obstructions, const QList<QPointF> &doors);

QPointF moveWallFollowing(QPointF currentPosition, QPointF &velocity, double speed,
                          double roomLeft, double roomRight, double roomTop, double roomBottom,
                          const QList<Obstruction2> &obstructions);

QPointF moveSnaking(const QPointF &currentPosition, QPointF &velocity, double speed,
                    double roomLeft, double roomRight, double roomTop, double roomBottom,
                    const QList<Obstruction2> &obstructions,
                    bool &movingDown, bool &movingRight,
                    int &currentZoneX, int &currentZoneY,
                    bool &movingUpward);

QPointF moveSpiral(QPointF currentPosition, QPointF &velocity, double speed,
                   double &spiralAngle, double &spiralRadius,
                   double roomLeft, double roomRight, double roomTop, double roomBottom,
                   const QList<Obstruction2> &obstructions);

QPointF bounceOffWalls(QPointF currentPosition, QPointF &velocity,
                       double roomLeft, double roomRight, double roomTop, double roomBottom);

}

#endif // PATHINGALGORITHMS_H
