#include "vacuum.h"
#include <QtGui/qpen.h>
#include <iostream>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QFile>
#include <QString>
#include <cmath>
#include <iostream>

Vacuum::Vacuum(QGraphicsScene* scene)
{
    batteryLife = 150;
    speed = 12;
    whiskerEfficiency = 30;
    currentAlgorithm = "Random";
    velocity = {0.0, 10.0};
    this->scene = scene;

    collisionSystem = new CollisionSystem();
    collisionSystem->loadFromJson(":/Default/default_plan.json");
}

void Vacuum::advance()
{
    if (batteryLife <= 0 || !vacuumGraphic) return;

    batteryLife--;

    position.x += velocity.x;
    position.y += velocity.y;

    collisionSystem->handleCollision(position, velocity, diameter/2.0);

    vacuumGraphic->setPos(position.x, position.y);
}

// A method to reset the vacuum and add back into the simulation for multiple runs
void Vacuum::reset()
{
    vacuumGraphic = scene->addEllipse(-diameter/2, -diameter/2, diameter, diameter,
                                      QPen(Qt::black), QBrush(Qt::red));
    if (scene && vacuumGraphic)
    {
        scene->removeItem(vacuumGraphic);
        delete vacuumGraphic;
        vacuumGraphic = nullptr;
    }
    else
    {
        qWarning() << "Scene or vacuumGraphic is null.";
    }
    vacuumGraphic = scene->addEllipse(-diameter/2, -diameter/2, diameter, diameter,
                                            QPen(Qt::black), QBrush(Qt::red));
    position = {300.0, -210.0};
    setVacuumPosition(position);
}


// Setters
void Vacuum::setBatteryLife(int minutes)
{
    if (minutes >= 1 && minutes <= 200)
    {
        batteryLife = minutes * 60;
    }
}

void Vacuum::setVacuumEfficiency(int vacuumEff)
{
    if (vacuumEff >= 10 && vacuumEff <= 90)
    {
        vacuumEfficiency = vacuumEff;
    }
}

void Vacuum::setWhiskerEfficiency(int whiskerEff)
{
    if (whiskerEff >= 10 && whiskerEff <= 50)
    {
        whiskerEfficiency = whiskerEff;
    }
}

void Vacuum::setSpeed(int inchesPerSecond)
{
    if (inchesPerSecond >= 6 && inchesPerSecond <= 18)
    {
        speed = inchesPerSecond;
    }
}

void Vacuum::setPathingAlgorithm(const QString& algorithm)
{
    currentAlgorithm = algorithm;
}

void Vacuum::setVacuumPosition(Vector2D& startPosition)
{
    vacuumGraphic->setPos(startPosition.x, startPosition.y);
    startPosition.x = vacuumGraphic->pos().x();
    startPosition.y = vacuumGraphic->pos().y();

}

// Getters
int Vacuum::getBatteryLife() const
{
    return batteryLife;
}

int Vacuum::getVacuumEfficiency() const
{
    return vacuumEfficiency;
}

int Vacuum::getWhiskerEfficiency() const
{
    return whiskerEfficiency;
}

int Vacuum::getSpeed() const
{
    return speed;
}

QString Vacuum::getPathingAlgorithm() const
{
    return currentAlgorithm;
}

QGraphicsEllipseItem* Vacuum::getGraphic() const
{
    return vacuumGraphic;
}

const Vector2D& Vacuum::getPosition() const
{
    return position;
}

int Vacuum::getElapsedTime() const
{
    return elapsedTime;
}

double Vacuum::getCoveredArea() const
{
    return coveredArea;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// COLLISON SYSTEM BELOW
//---------------------------------------------------------------------------------------------------------------------------------------

const Room2D* CollisionSystem::getCurrentRoom(const Vector2D& pos) const
{
    for (const auto& room : rooms) {
        if (pos.x >= room.topLeft.x && pos.x <= room.bottomRight.x &&
            pos.y >= room.topLeft.y && pos.y <= room.bottomRight.y) {
            return &room;
        }
    }
    return nullptr;
}

static double clamp(double value, double minVal, double maxVal)
{
    return std::max(minVal, std::min(maxVal, value));
}

bool CollisionSystem::loadFromJson(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cerr << "Failed to open JSON file: " << filePath.toStdString() << std::endl;
        return false;
    }

    QByteArray jsonData = file.readAll();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        std::cerr << "JSON parse error: " << parseError.errorString().toStdString() << std::endl;
        return false;
    }

    QJsonObject root = doc.object();

    // Parse rooms
    QJsonArray roomsArray = root["rooms"].toArray();
    for (const QJsonValue& val : roomsArray) {
        QJsonObject obj = val.toObject();
        QPointF p1(obj["x_topLeft"].toDouble(), obj["y_topLeft"].toDouble());
        QPointF p2(obj["x_bottomRight"].toDouble(), obj["y_bottomRight"].toDouble());

        Room2D room;
        room.topLeft.x = std::min(p1.x(), p2.x());
        room.topLeft.y = std::min(p1.y(), p2.y());
        room.bottomRight.x = std::max(p1.x(), p2.x());
        room.bottomRight.y = std::max(p1.y(), p2.y());
        rooms.push_back(room);
    }

    // Parse doors
    QJsonArray doorsArray = root["doors"].toArray();
    for (const QJsonValue& val : doorsArray) {
        QJsonObject obj = val.toObject();
        Door2D door;
        door.origin.x = obj["x"].toDouble();
        door.origin.y = obj["y"].toDouble();
        doors.push_back(door);
    }

    // Parse obstructions
    QJsonArray obsArray = root["obstructions"].toArray();
    for (const QJsonValue& val : obsArray) {
        QJsonObject obj = val.toObject();
        QPointF p1(obj["x_topLeft"].toDouble(), obj["y_topLeft"].toDouble());
        QPointF p2(obj["x_bottomRight"].toDouble(), obj["y_bottomRight"].toDouble());

        Obstruction2D obstruction;
        obstruction.isChest = obj["is_chest"].toBool();
        obstruction.topLeft.x = std::min(p1.x(), p2.x());
        obstruction.topLeft.y = std::min(p1.y(), p2.y());
        obstruction.bottomRight.x = std::max(p1.x(), p2.x());
        obstruction.bottomRight.y = std::max(p1.y(), p2.y());
        obstructions.push_back(obstruction);
    }

    return true;

}

void CollisionSystem::handleCollision(Vector2D& pos, Vector2D& vel, double radius)
{
    const Room2D* currentRoom = getCurrentRoom(pos);
    if (!currentRoom) return; // Vacuum is not inside any known room

    double left = std::min(currentRoom->topLeft.x, currentRoom->bottomRight.x);
    double right = std::max(currentRoom->topLeft.x, currentRoom->bottomRight.x);
    double top = std::min(currentRoom->topLeft.y, currentRoom->bottomRight.y);
    double bottom = std::max(currentRoom->topLeft.y, currentRoom->bottomRight.y);

    // Handle chest (obstruction) collisions inside current room
    for (const auto& obs : obstructions) {
        if (!obs.isChest) continue;

        // Skip obstruction if it's outside the current room
        if (obs.bottomRight.x < left || obs.topLeft.x > right ||
            obs.bottomRight.y < top || obs.topLeft.y > bottom) {
            continue;
        }

        double obsLeft = std::min(obs.topLeft.x, obs.bottomRight.x);
        double obsRight = std::max(obs.topLeft.x, obs.bottomRight.x);
        double obsTop = std::min(obs.topLeft.y, obs.bottomRight.y);
        double obsBottom = std::max(obs.topLeft.y, obs.bottomRight.y);

        if (pos.x + radius > obsLeft && pos.x - radius < obsRight &&
            pos.y + radius > obsTop && pos.y - radius < obsBottom) {

            // Calculate overlaps on each axis
            double overlapLeft = (pos.x + radius) - obsLeft;
            double overlapRight = obsRight - (pos.x - radius);
            double overlapTop = (pos.y + radius) - obsTop;
            double overlapBottom = obsBottom - (pos.y - radius);

            // Pick smallest overlap direction
            double minOverlapX = std::min(overlapLeft, overlapRight);
            double minOverlapY = std::min(overlapTop, overlapBottom);

            if (minOverlapX < minOverlapY) {
                // Horizontal collision
                if (overlapLeft < overlapRight) {
                    pos.x -= overlapLeft;
                } else {
                    pos.x += overlapRight;
                }
                vel.x = -vel.x;
            } else {
                // Vertical collision
                if (overlapTop < overlapBottom) {
                    pos.y -= overlapTop;
                } else {
                    pos.y += overlapBottom;
                }
                vel.y = -vel.y;
            }

            return;
        }

    }

    // Handle wall collisions for current room (with door gaps)
    // Top wall
    if (pos.y - radius < top &&
        pos.x >= left && pos.x <= right) {
        bool doorGap = false;
        for (const auto& door : doors) {
            if (std::abs(top - door.origin.y) < 1e-3 &&
                door.origin.x <= pos.x && pos.x <= door.origin.x + 45.0f) {
                doorGap = true;
                break;
            }
        }
        if (!doorGap) {
            vel.y = std::abs(vel.y);
            pos.y = top + radius;
        }
    }

    // Bottom wall
    if (pos.y + radius > bottom &&
        pos.x >= left && pos.x <= right) {
        bool doorGap = false;
        for (const auto& door : doors) {
            if (std::abs(bottom - door.origin.y) < 1e-3 &&
                door.origin.x <= pos.x && pos.x <= door.origin.x + 45.0f) {
                doorGap = true;
                break;
            }
        }
        if (!doorGap) {
            vel.y = -std::abs(vel.y);
            pos.y = bottom - radius;
        }
    }

    // Left wall
    if (pos.x - radius < left &&
        pos.y >= top && pos.y <= bottom) {
        bool doorGap = false;
        for (const auto& door : doors) {
            if (std::abs(left - door.origin.x) < 1e-3 &&
                door.origin.y <= pos.y && pos.y <= door.origin.y + 45.0f) {
                doorGap = true;
                break;
            }
        }
        if (!doorGap) {
            vel.x = std::abs(vel.x);
            pos.x = left + radius;
        }
    }

    // Right wall
    if (pos.x + radius > right &&
        pos.y >= top && pos.y <= bottom) {
        bool doorGap = false;
        for (const auto& door : doors) {
            if (std::abs(right - door.origin.x) < 1e-3 &&
                door.origin.y <= pos.y && pos.y <= door.origin.y + 45.0f) {
                doorGap = true;
                break;
            }
        }
        if (!doorGap) {
            vel.x = -std::abs(vel.x);
            pos.x = right - radius;
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------
// VACUUM MOVEMENT BELOW
//---------------------------------------------------------------------------------------------------------------------------------------
// void Vacuum::updateMovementandTrail(QGraphicsScene* scene)
// {
//     if (batteryLife <= 0) return;

//     constexpr double radius = diameter / 2.0;

//     // Determine current algorithm
//     QString currentAlgorithm = pathingAlgorithms.isEmpty() ? "Random" : pathingAlgorithms.first();
//     QPointF currentPos(position.x, position.y);
//     QPointF nextPos;

//     // --- ALGORITHM-BASED MOVEMENT ---
//     if (currentAlgorithm == "Random")
//     {
//         nextPos = PathAlgorithms::moveRandomly(
//             currentPos, velocity, speed,
//             collisionSystem->getRoomBounds(),
//             collisionSystem->getObstructions(),
//             collisionSystem->getDoors()
//             );
//     }
//     else if (currentAlgorithm == "Wall Follow")
//     {
//         nextPos = PathAlgorithms::moveWallFollowing(
//             currentPos, velocity, speed,
//             collisionSystem->getRoomBounds(),
//             collisionSystem->getObstructions()
//             );
//     }
//     else if (currentAlgorithm == "Snaking")
//     {
//         nextPos = PathAlgorithms::moveSnaking(
//             currentPos, velocity, speed,
//             collisionSystem->getRoomBounds(),
//             collisionSystem->getObstructions(),
//             movingDown, movingRight, currentZoneX, currentZoneY, movingUpward
//             );
//     }
//     else if (currentAlgorithm == "Spiral")
//     {
//         nextPos = PathAlgorithms::moveSpiral(
//             currentPos, velocity, speed,
//             spiralAngle, spiralRadius,
//             collisionSystem->getRoomBounds(),
//             collisionSystem->getObstructions()
//             );
//     }
//     else
//     {
//         // Fallback: simple forward motion
//         nextPos.setX(position.x + velocity.x * speed);
//         nextPos.setY(position.y + velocity.y * speed);
//     }

//     // Update Vector2D position
//     position.x = nextPos.x();
//     position.y = nextPos.y();

//     // Handle collisions
//     if (collisionSystem)
//         collisionSystem->handleCollision(position, velocity, radius);

//     // Move the vacuum graphic
//     vacuumGraphic->setPos(position.x, position.y);

//     // Draw trail
//     static QPointF lastTrailPoint = vacuumGraphic->pos();
//     QPointF currentTrailPoint(position.x, position.y);

//     QPen trailPen(Qt::blue);
//     trailPen.setWidth(2);
//     scene->addLine(QLineF(lastTrailPoint, currentTrailPoint), trailPen);

//     lastTrailPoint = currentTrailPoint;

//     batteryLife--;
// }

// QPointF moveRandomly(const QPointF &currentPosition, QPointF &velocity, double speed,
//                      double roomLeft, double roomRight, double roomTop, double roomBottom,
//                      const QList<Obstruction2> &obstructions, const QList<QPointF> &doors)
// {
//     constexpr double vacuumRadius = 6.4;
//     QPointF next = currentPosition + velocity * speed;

//     auto isValidMove = [&](const QPointF &pos) {
//         if (pos.x() - vacuumRadius < roomLeft || pos.x() + vacuumRadius > roomRight ||
//             pos.y() - vacuumRadius < roomTop  || pos.y() + vacuumRadius > roomBottom)
//             return false;
//         for (const Obstruction2 &obs : obstructions)
//         {
//             QRectF obsRect = obs.rect;
//             if (pos.x() + vacuumRadius > obsRect.left() &&
//                 pos.x() - vacuumRadius < obsRect.right() &&
//                 pos.y() + vacuumRadius > obsRect.top() &&
//                 pos.y() - vacuumRadius < obsRect.bottom())
//                 return false;
//         }
//         return true;
//     };

//     if (isValidMove(next))
//     {
//         for (const QPointF &door : doors)
//         {
//             double distanceToDoor = std::hypot(next.x() - door.x(), next.y() - door.y());
//             if (distanceToDoor < 8.0)
//                 return next;
//         }
//         return next;
//     }

//     int retries = 10;
//     while (retries-- > 0)
//     {
//         double angle = static_cast<double>(std::rand() % 360) * (M_PI / 180.0);
//         velocity.setX(std::cos(angle));
//         velocity.setY(std::sin(angle));
//         velocity /= std::hypot(velocity.x(), velocity.y());

//         QPointF tryNext = currentPosition + velocity * speed;
//         if (isValidMove(tryNext))
//             return tryNext;
//     }

//     return currentPosition;
// }


// QPointF moveWallFollowing(QPointF currentPosition, QPointF &velocity, double speed,
//                           double roomLeft, double roomRight, double roomTop, double roomBottom,
//                           const QList<Obstruction2> &obstructions)
// {
//     constexpr double vacuumRadius = 6.4;
//     constexpr double wallHugDistance = 3.0; // how close we want to stay near walls
//     constexpr double rotateStep = M_PI / 12.0; // 15 degree rotation step when bouncing
//     static double wallFollowAngle = 0.0;

//     auto isValid = [&](const QPointF &pos) {
//         if (pos.x() - vacuumRadius < roomLeft || pos.x() + vacuumRadius > roomRight ||
//             pos.y() - vacuumRadius < roomTop  || pos.y() + vacuumRadius > roomBottom)
//             return false;
//         for (const auto &obs : obstructions)
//         {
//             QRectF obsRect = obs.rect;
//             if (pos.x() + vacuumRadius > obsRect.left() &&
//                 pos.x() - vacuumRadius < obsRect.right() &&
//                 pos.y() + vacuumRadius > obsRect.top() &&
//                 pos.y() - vacuumRadius < obsRect.bottom())
//                 return false;
//         }
//         return true;
//     };

//     QPointF next = currentPosition + velocity * speed;

//     // If normal move is fine, keep moving forward
//     if (isValid(next))
//         return next;

//     // If blocked, rotate slightly and try to move along wall
//     for (int i = 0; i < 24; ++i) // try small angle rotations
//     {
//         wallFollowAngle += rotateStep;
//         if (wallFollowAngle > 2 * M_PI)
//             wallFollowAngle -= 2 * M_PI;

//         QPointF tryVel(std::cos(wallFollowAngle), std::sin(wallFollowAngle));
//         QPointF tryNext = currentPosition + tryVel * speed;

//         if (isValid(tryNext))
//         {
//             velocity = tryVel / std::hypot(tryVel.x(), tryVel.y()); // normalize velocity
//             return tryNext;
//         }
//     }

//     // If completely trapped, reverse direction
//     velocity = -velocity;
//     return currentPosition;
// }



// QPointF moveSnaking(const QPointF &currentPosition, QPointF &velocity, double speed,
//                     double roomLeft, double roomRight, double roomTop, double roomBottom,
//                     const QList<Obstruction2> &obstructions,
//                     bool &movingDown, bool &movingRight,
//                     int &currentZoneX, int &currentZoneY,
//                     bool &movingUpward)
// {
//     constexpr double vacuumRadius = 6.4;
//     const double shiftDistance = vacuumRadius * 0.5;

//     QPointF next = currentPosition + velocity * speed;

//     if (!movingUpward) {
//         if (movingRight && (next.x() + vacuumRadius) >= roomRight) {
//             movingRight = false;
//             next.setX(roomRight - vacuumRadius);
//             next.setY(currentPosition.y() + shiftDistance);
//         }
//         else if (!movingRight && (next.x() - vacuumRadius) <= roomLeft) {
//             movingRight = true;
//             next.setX(roomLeft + vacuumRadius);
//             next.setY(currentPosition.y() + shiftDistance);
//         }

//         if ((next.y() + vacuumRadius) >= roomBottom) {
//             next.setY(roomBottom - vacuumRadius);
//             movingUpward = true;
//             movingDown = false;
//         }

//         velocity = movingRight ? QPointF(1, 0) : QPointF(-1, 0);
//     }
//     else {
//         if (movingRight && (next.x() + vacuumRadius) >= roomRight) {
//             movingRight = false;
//             next.setX(roomRight - vacuumRadius);
//             next.setY(currentPosition.y() - shiftDistance);
//         }
//         else if (!movingRight && (next.x() - vacuumRadius) <= roomLeft) {
//             movingRight = true;
//             next.setX(roomLeft + vacuumRadius);
//             next.setY(currentPosition.y() - shiftDistance);
//         }

//         if ((next.y() - vacuumRadius) <= roomTop) {
//             next.setY(roomTop + vacuumRadius);
//             movingUpward = false;
//             movingDown = true;
//         }

//         velocity = movingRight ? QPointF(1, 0) : QPointF(-1, 0);
//     }

//     return next;
// }




// QPointF moveSpiral(QPointF currentPosition, QPointF &velocity, double speed,
//                    double &spiralAngle, double &spiralRadius,
//                    double roomLeft, double roomRight, double roomTop, double roomBottom,
//                    const QList<Obstruction2> &obstructions)
// {
//     constexpr double vacuumRadius = 6.4;
//     constexpr double angleIncrement = 0.1;
//     constexpr double radiusGrowthRate = 0.02;

//     spiralAngle += angleIncrement;
//     spiralRadius += radiusGrowthRate;

//     double dx = std::cos(spiralAngle) * spiralRadius;
//     double dy = std::sin(spiralAngle) * spiralRadius;
//     QPointF next = currentPosition + QPointF(dx, dy);

//     auto isValidMove = [&](const QPointF &pos) {
//         if (pos.x() - vacuumRadius < roomLeft || pos.x() + vacuumRadius > roomRight ||
//             pos.y() - vacuumRadius < roomTop  || pos.y() + vacuumRadius > roomBottom)
//             return false;
//         for (const auto& obs : obstructions)
//         {
//             QRectF obsRect = obs.rect;
//             if (pos.x() + vacuumRadius > obsRect.left() &&
//                 pos.x() - vacuumRadius < obsRect.right() &&
//                 pos.y() + vacuumRadius > obsRect.top() &&
//                 pos.y() - vacuumRadius < obsRect.bottom())
//                 return false;
//         }
//         return true;
//     };

//     if (!isValidMove(next))
//     {
//         double bounceAngle = (std::rand() % 60 - 30) * (M_PI / 180.0);
//         spiralAngle += bounceAngle;

//         dx = std::cos(spiralAngle) * spiralRadius;
//         dy = std::sin(spiralAngle) * spiralRadius;
//         next = currentPosition + QPointF(dx, dy);

//         if (!isValidMove(next))
//         {
//             spiralRadius = std::max(1.0, spiralRadius - 0.5);
//             next = currentPosition;
//         }
//     }

//     return next;
// }
