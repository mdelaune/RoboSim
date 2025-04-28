#include "pathingalgorithms.h"
#include "houseparser.h"
#include <cmath>
#include <cstdlib>



namespace PathAlgorithms
{

QPointF moveRandomly(const QPointF &currentPosition, QPointF &velocity, double speed,
                     const QRectF &roomRect, const QList<Obstruction2> &obstructions, const QList<QPointF> &doors)
{
    constexpr double vacuumRadius = 6.4;
    QPointF next = currentPosition + velocity * speed;
    QRectF vacuumBounds(next.x() - vacuumRadius, next.y() - vacuumRadius, 2 * vacuumRadius, 2 * vacuumRadius);

    auto isValidMove = [&](const QPointF &pos) {
        QRectF bounds(pos.x() - vacuumRadius, pos.y() - vacuumRadius, 2 * vacuumRadius, 2 * vacuumRadius);
        if (!roomRect.contains(bounds)) return false;
        for (const Obstruction2 &obs : obstructions)
            if (obs.rect.intersects(bounds))
                return false;
        return true;
    };

    // --- Normal move if no collision ---
    if (isValidMove(next))
    {
        for (const QPointF &door : doors)
        {
            double distanceToDoor = std::hypot(next.x() - door.x(), next.y() - door.y());
            if (distanceToDoor < 8.0) // 🔥 slight relax: 8 pixels
                return next; // Allow through door
        }
        return next;
    }

    // --- Bounce if invalid move ---
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

    // --- All retries failed ---
    return currentPosition;
}


QPointF moveWallFollowing(QPointF currentPosition, QPointF &velocity, double speed,
                          const QRectF &roomRect, const QList<Obstruction2> &obstructions)
{
    constexpr double vacuumRadius = 6.4;
    static double wallFollowAngle = 0.0;

    auto isValid = [&](const QPointF &pos) {
        QRectF bounds(pos.x() - vacuumRadius, pos.y() - vacuumRadius, 2 * vacuumRadius, 2 * vacuumRadius);
        if (!roomRect.contains(bounds)) return false;
        for (const auto &obs : obstructions)
            if (obs.rect.intersects(bounds)) return false;
        return true;
    };

    QPointF next = currentPosition + velocity * speed;
    if (isValid(next)) return next;

    for (int i = 0; i < 36; ++i) {
        wallFollowAngle += M_PI / 18.0;
        if (wallFollowAngle > 2 * M_PI) wallFollowAngle -= 2 * M_PI;

        QPointF tryVel(std::cos(wallFollowAngle) * speed,
                       std::sin(wallFollowAngle) * speed);
        QPointF tryPos = currentPosition + tryVel;

        QRectF tryBounds(tryPos.x() - vacuumRadius, tryPos.y() - vacuumRadius,
                         2 * vacuumRadius, 2 * vacuumRadius);

        if (isValid(tryPos)) {
            velocity = tryVel / std::hypot(tryVel.x(), tryVel.y());
            return tryPos;
        }
    }

    velocity = -velocity;
    return currentPosition;
}

QPointF moveSnaking(const QPointF &currentPosition, QPointF &velocity, double speed,
                    const QRectF &roomRect, const QList<Obstruction2> &obstructions,
                    bool &movingDown, bool &movingRight,
                    int &currentZoneX, int &currentZoneY,
                    bool &movingUpward)
{
    constexpr double vacuumRadius = 6.4;
    const double shiftDistance = vacuumRadius * 0.5;

    QPointF next = currentPosition + velocity * speed;
    QRectF bounds(next.x() - vacuumRadius, next.y() - vacuumRadius, 2 * vacuumRadius, 2 * vacuumRadius);

    if (!movingUpward) {
        // --- Snaking Down ---
        if (movingRight && bounds.right() >= roomRect.right()) {
            movingRight = false;
            next.setX(roomRect.right() - vacuumRadius);
            next.setY(currentPosition.y() + shiftDistance);
        }
        else if (!movingRight && bounds.left() <= roomRect.left()) {
            movingRight = true;
            next.setX(roomRect.left() + vacuumRadius);
            next.setY(currentPosition.y() + shiftDistance);
        }

        // If reached the bottom, switch to snaking upward
        if (next.y() + vacuumRadius >= roomRect.bottom()) {
            next.setY(roomRect.bottom() - vacuumRadius);
            movingUpward = true;     // 🔥 Start snaking upward
            movingDown = false;
        }

        velocity = movingRight ? QPointF(1, 0) : QPointF(-1, 0);
    }
    else {
        // --- Snaking Up ---
        if (movingRight && bounds.right() >= roomRect.right()) {
            movingRight = false;
            next.setX(roomRect.right() - vacuumRadius);
            next.setY(currentPosition.y() - shiftDistance);
        }
        else if (!movingRight && bounds.left() <= roomRect.left()) {
            movingRight = true;
            next.setX(roomRect.left() + vacuumRadius);
            next.setY(currentPosition.y() - shiftDistance);
        }

        // If reached the top, switch back to snaking downward
        if (next.y() - vacuumRadius <= roomRect.top()) {
            next.setY(roomRect.top() + vacuumRadius);
            movingUpward = false;    // 🔥 Start snaking downward again
            movingDown = true;
        }

        velocity = movingRight ? QPointF(1, 0) : QPointF(-1, 0);
    }

    return next;
}




QPointF moveSpiral(QPointF currentPosition, QPointF &velocity, double speed,
                   double &spiralAngle, double &spiralRadius,
                   const QRectF &roomRect, const QList<Obstruction2> &obstructions)
{
    constexpr double vacuumRadius = 6.4;
    constexpr double angleIncrement = 0.1;   // How much to turn each frame
    constexpr double radiusGrowthRate = 0.02; // How fast the spiral grows outward

    // Update spiral
    spiralAngle += angleIncrement;
    spiralRadius += radiusGrowthRate;

    // Calculate next spiral position
    double dx = std::cos(spiralAngle) * spiralRadius;
    double dy = std::sin(spiralAngle) * spiralRadius;
    QPointF next = currentPosition + QPointF(dx, dy);

    // Prepare boundary box
    QRectF bounds(next.x() - vacuumRadius, next.y() - vacuumRadius, 2 * vacuumRadius, 2 * vacuumRadius);

    auto isValidMove = [&](const QPointF &pos) {
        QRectF checkBounds(pos.x() - vacuumRadius, pos.y() - vacuumRadius, 2 * vacuumRadius, 2 * vacuumRadius);
        if (!roomRect.contains(checkBounds)) return false;
        for (const auto& obs : obstructions)
            if (obs.rect.intersects(checkBounds))
                return false;
        return true;
    };

    // --- If next position invalid (wall or obstruction) ---
    if (!isValidMove(next))
    {
        // Bounce by changing angle slightly
        double bounceAngle = (std::rand() % 60 - 30) * (M_PI / 180.0); // random -30 to +30 degrees
        spiralAngle += bounceAngle; // nudge spiral angle

        // Recalculate next position
        dx = std::cos(spiralAngle) * spiralRadius;
        dy = std::sin(spiralAngle) * spiralRadius;
        next = currentPosition + QPointF(dx, dy);

        // If still invalid after bounce attempt, shrink spiral radius slightly
        if (!isValidMove(next))
        {
            spiralRadius = std::max(1.0, spiralRadius - 0.5); // shrink radius a little
            next = currentPosition; // stay in place this tick
        }
    }

    return next;
}



QPointF bounceOffWalls(QPointF currentPosition, QPointF &velocity, const QRectF &room)

{
    constexpr double margin = 10.0;
    constexpr double slightAngleRange = 10.0; // degrees

    double left = room.left() + margin;
    double right = room.right() - margin;
    double top = room.top() + margin;
    double bottom = room.bottom() - margin;

    QPointF next = currentPosition + velocity;

    bool bounced = false;

    // Reflect horizontally
    if (next.x() < left || next.x() > right) {
        velocity.setX(-velocity.x());
        bounced = true;
    }

    // Reflect vertically
    if (next.y() < top || next.y() > bottom) {
        velocity.setY(-velocity.y());
        bounced = true;
    }

    if (bounced)
    {
        // Apply slight random angle to bounce direction
        double randomAngle = ((std::rand() % int(2 * slightAngleRange + 1)) - slightAngleRange) * (M_PI / 180.0);
        double cosA = std::cos(randomAngle);
        double sinA = std::sin(randomAngle);

        QPointF rotated(
            velocity.x() * cosA - velocity.y() * sinA,
            velocity.x() * sinA + velocity.y() * cosA
            );
        velocity = rotated;
        velocity /= std::hypot(velocity.x(), velocity.y()); // normalize
    }

    return currentPosition + velocity;
}


}
