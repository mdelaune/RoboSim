#include "pathingalgorithms.h"
#include "houseparser.h"
#include <cmath>

namespace PathAlgorithms
{
QPointF moveRandomly(const QPointF &currentPosition, QPointF &velocity, double speed,
                     const QRectF &roomRect, const QList<Obstruction2> &obstructions, const QList<QPointF> &doors)
{
    constexpr double vacuumRadius = 6.4;  // Half of 12.8 inches
    QPointF next = currentPosition + velocity * speed;
    QRectF vacuumBounds(next.x() - vacuumRadius, next.y() - vacuumRadius, 2 * vacuumRadius, 2 * vacuumRadius);

    // Check if vacuum's body is fully inside the room
    if (roomRect.contains(vacuumBounds))
    {
        // Check if vacuum intersects with any obstruction
        for (const Obstruction2 &obs : obstructions)
        {
            if (obs.rect.intersects(vacuumBounds))
            {
                // Bounce off obstruction
                int retries = 10;
                while (retries-- > 0)
                {
                    double angle = static_cast<double>(std::rand() % 360) * (M_PI / 180.0);
                    velocity.setX(std::cos(angle));
                    velocity.setY(std::sin(angle));
                    velocity /= std::hypot(velocity.x(), velocity.y());

                    QPointF tryNext = currentPosition + velocity * speed;
                    QRectF tryBounds(tryNext.x() - vacuumRadius, tryNext.y() - vacuumRadius,
                                     2 * vacuumRadius, 2 * vacuumRadius);
                    if (roomRect.contains(tryBounds) && !obs.rect.intersects(tryBounds))
                        return tryNext;
                }
                return currentPosition;
            }
        }

        // Allow passage through nearby door (refined distance check)
        for (const QPointF &door : doors)
        {
            // Calculate distance to door's center
            double distanceToDoor = std::sqrt(std::pow(next.x() - door.x(), 2) + std::pow(next.y() - door.y(), 2));
            if (distanceToDoor < 5.0) // Threshold distance to door (in pixels)
            {
                // If within threshold, pass through door by returning next position
                return next;
            }
        }

        return next;  // Valid move within room bounds
    }

    // Bounce off walls if outside the room
    int retries = 10;
    while (retries-- > 0)
    {
        double angle = static_cast<double>(std::rand() % 360) * (M_PI / 180.0);
        velocity.setX(std::cos(angle));
        velocity.setY(std::sin(angle));
        velocity /= std::hypot(velocity.x(), velocity.y());

        QPointF tryNext = currentPosition + velocity * speed;
        QRectF tryBounds(tryNext.x() - vacuumRadius, tryNext.y() - vacuumRadius, 2 * vacuumRadius, 2 * vacuumRadius);
        if (roomRect.contains(tryBounds))
            return tryNext;
    }

    return currentPosition;
}

} // namespace PathAlgorithms




