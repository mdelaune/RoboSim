#include "pathingalgorithms.h"
#include "houseparser.h"
#include <cmath>
#include <cstdlib>



namespace PathAlgorithms
{

QPointF moveRandomly(const QPointF &currentPosition, QPointF &velocity, double speed,
                     double roomLeft, double roomRight, double roomTop, double roomBottom,
                     const QList<Obstruction2> &obstructions, const QList<QPointF> &doors)
{
    constexpr double vacuumRadius = 6.4;
    QPointF next = currentPosition + velocity * speed;

    auto isValidMove = [&](const QPointF &pos) {
        if (pos.x() - vacuumRadius < roomLeft || pos.x() + vacuumRadius > roomRight ||
            pos.y() - vacuumRadius < roomTop  || pos.y() + vacuumRadius > roomBottom)
            return false;
        for (const Obstruction2 &obs : obstructions)
        {
            QRectF obsRect = obs.rect;
            if (pos.x() + vacuumRadius > obsRect.left() &&
                pos.x() - vacuumRadius < obsRect.right() &&
                pos.y() + vacuumRadius > obsRect.top() &&
                pos.y() - vacuumRadius < obsRect.bottom())
                return false;
        }
        return true;
    };

    if (isValidMove(next))
    {
        for (const QPointF &door : doors)
        {
            double distanceToDoor = std::hypot(next.x() - door.x(), next.y() - door.y());
            if (distanceToDoor < 8.0)
                return next;
        }
        return next;
    }

    int retries = 10;
    while (retries-- > 0)
    {
        double angle = static_cast<double>(std::rand() % 360) * (M_PI / 180.0);
        velocity.setX(std::cos(angle));
        velocity.setY(std::sin(angle));
        velocity /= std::hypot(velocity.x(), velocity.y());

        QPointF tryNext = currentPosition + velocity * speed;
        if (isValidMove(tryNext))
            return tryNext;
    }

    return currentPosition;
}


QPointF moveWallFollowing(QPointF currentPosition, QPointF &velocity, double speed,
                          double roomLeft, double roomRight, double roomTop, double roomBottom,
                          const QList<Obstruction2> &obstructions)
{
    constexpr double vacuumRadius = 6.4;
    constexpr double wallHugDistance = 3.0; // how close we want to stay near walls
    constexpr double rotateStep = M_PI / 12.0; // 15 degree rotation step when bouncing
    static double wallFollowAngle = 0.0;

    auto isValid = [&](const QPointF &pos) {
        if (pos.x() - vacuumRadius < roomLeft || pos.x() + vacuumRadius > roomRight ||
            pos.y() - vacuumRadius < roomTop  || pos.y() + vacuumRadius > roomBottom)
            return false;
        for (const auto &obs : obstructions)
        {
            QRectF obsRect = obs.rect;
            if (pos.x() + vacuumRadius > obsRect.left() &&
                pos.x() - vacuumRadius < obsRect.right() &&
                pos.y() + vacuumRadius > obsRect.top() &&
                pos.y() - vacuumRadius < obsRect.bottom())
                return false;
        }
        return true;
    };

    QPointF next = currentPosition + velocity * speed;

    // If normal move is fine, keep moving forward
    if (isValid(next))
        return next;

    // If blocked, rotate slightly and try to move along wall
    for (int i = 0; i < 24; ++i) // try small angle rotations
    {
        wallFollowAngle += rotateStep;
        if (wallFollowAngle > 2 * M_PI)
            wallFollowAngle -= 2 * M_PI;

        QPointF tryVel(std::cos(wallFollowAngle), std::sin(wallFollowAngle));
        QPointF tryNext = currentPosition + tryVel * speed;

        if (isValid(tryNext))
        {
            velocity = tryVel / std::hypot(tryVel.x(), tryVel.y()); // normalize velocity
            return tryNext;
        }
    }

    // If completely trapped, reverse direction
    velocity = -velocity;
    return currentPosition;
}



QPointF moveSnaking(const QPointF &currentPosition, QPointF &velocity, double speed,
                    double roomLeft, double roomRight, double roomTop, double roomBottom,
                    const QList<Obstruction2> &obstructions,
                    bool &movingDown, bool &movingRight,
                    int &currentZoneX, int &currentZoneY,
                    bool &movingUpward)
{
    constexpr double vacuumRadius = 6.4;
    const double shiftDistance = vacuumRadius * 0.5;

    QPointF next = currentPosition + velocity * speed;

    if (!movingUpward) {
        if (movingRight && (next.x() + vacuumRadius) >= roomRight) {
            movingRight = false;
            next.setX(roomRight - vacuumRadius);
            next.setY(currentPosition.y() + shiftDistance);
        }
        else if (!movingRight && (next.x() - vacuumRadius) <= roomLeft) {
            movingRight = true;
            next.setX(roomLeft + vacuumRadius);
            next.setY(currentPosition.y() + shiftDistance);
        }

        if ((next.y() + vacuumRadius) >= roomBottom) {
            next.setY(roomBottom - vacuumRadius);
            movingUpward = true;
            movingDown = false;
        }

        velocity = movingRight ? QPointF(1, 0) : QPointF(-1, 0);
    }
    else {
        if (movingRight && (next.x() + vacuumRadius) >= roomRight) {
            movingRight = false;
            next.setX(roomRight - vacuumRadius);
            next.setY(currentPosition.y() - shiftDistance);
        }
        else if (!movingRight && (next.x() - vacuumRadius) <= roomLeft) {
            movingRight = true;
            next.setX(roomLeft + vacuumRadius);
            next.setY(currentPosition.y() - shiftDistance);
        }

        if ((next.y() - vacuumRadius) <= roomTop) {
            next.setY(roomTop + vacuumRadius);
            movingUpward = false;
            movingDown = true;
        }

        velocity = movingRight ? QPointF(1, 0) : QPointF(-1, 0);
    }

    return next;
}




QPointF moveSpiral(QPointF currentPosition, QPointF &velocity, double speed,
                   double &spiralAngle, double &spiralRadius,
                   double roomLeft, double roomRight, double roomTop, double roomBottom,
                   const QList<Obstruction2> &obstructions)
{
    constexpr double vacuumRadius = 6.4;
    constexpr double angleIncrement = 0.1;
    constexpr double radiusGrowthRate = 0.02;

    spiralAngle += angleIncrement;
    spiralRadius += radiusGrowthRate;

    double dx = std::cos(spiralAngle) * spiralRadius;
    double dy = std::sin(spiralAngle) * spiralRadius;
    QPointF next = currentPosition + QPointF(dx, dy);

    auto isValidMove = [&](const QPointF &pos) {
        if (pos.x() - vacuumRadius < roomLeft || pos.x() + vacuumRadius > roomRight ||
            pos.y() - vacuumRadius < roomTop  || pos.y() + vacuumRadius > roomBottom)
            return false;
        for (const auto& obs : obstructions)
        {
            QRectF obsRect = obs.rect;
            if (pos.x() + vacuumRadius > obsRect.left() &&
                pos.x() - vacuumRadius < obsRect.right() &&
                pos.y() + vacuumRadius > obsRect.top() &&
                pos.y() - vacuumRadius < obsRect.bottom())
                return false;
        }
        return true;
    };

    if (!isValidMove(next))
    {
        double bounceAngle = (std::rand() % 60 - 30) * (M_PI / 180.0);
        spiralAngle += bounceAngle;

        dx = std::cos(spiralAngle) * spiralRadius;
        dy = std::sin(spiralAngle) * spiralRadius;
        next = currentPosition + QPointF(dx, dy);

        if (!isValidMove(next))
        {
            spiralRadius = std::max(1.0, spiralRadius - 0.5);
            next = currentPosition;
        }
    }

    return next;
}



}
