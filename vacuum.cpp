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
#include <QRandomGenerator>

Vacuum::Vacuum(QGraphicsScene* scene)
{
    batteryLife = 150;
    speed = 12;
    whiskerEfficiency = 30;
    currentAlgorithm = "Random";
    velocity = {0.0, 0.0};
    this->scene = scene;

    collisionSystem = new CollisionSystem();
    collisionSystem->loadFromJson(":/Default/default_plan.json");
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
    position = {0, 200.0};
    setVacuumPosition(position);
    lastTrailPoint = QPointF(position.x, position.y);

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

// Corrects pos if it overlaps a wall or chest. Returns true if any correction was done.
bool CollisionSystem::handleCollision(Vector2D& pos, double radius)
{
    // 1) find the room this candidate is in (or just outside)
    const Room2D* room = getCurrentRoom(pos);
    if (!room) {
        for (const auto &r : rooms) {
            double left   = std::min(r.topLeft.x,    r.bottomRight.x);
            double right  = std::max(r.topLeft.x,    r.bottomRight.x);
            double top    = std::min(r.topLeft.y,    r.bottomRight.y);
            double bottom = std::max(r.topLeft.y,    r.bottomRight.y);

            if (pos.x + radius > left && pos.x - radius < right &&
                pos.y + radius > top  && pos.y - radius < bottom)
            {
                room = &r;
                break;
            }
        }
    }
    // if still no room, give up—vacuum must be completely outside any known area
    if (!room) return false;

    // 2) now apply your chest + wall corrections exactly as before
    double left   = std::min(room->topLeft.x,    room->bottomRight.x);
    double right  = std::max(room->topLeft.x,    room->bottomRight.x);
    double top    = std::min(room->topLeft.y,    room->bottomRight.y);
    double bottom = std::max(room->topLeft.y,    room->bottomRight.y);

    // --- Chest collisions ---
    for (auto &obs : obstructions) {
        if (!obs.isChest) continue;
        if (obs.bottomRight.x < left || obs.topLeft.x > right ||
            obs.bottomRight.y < top  || obs.topLeft.y > bottom)
            continue;

        double oL = std::min(obs.topLeft.x, obs.bottomRight.x);
        double oR = std::max(obs.topLeft.x, obs.bottomRight.x);
        double oT = std::min(obs.topLeft.y, obs.bottomRight.y);
        double oB = std::max(obs.topLeft.y, obs.bottomRight.y);

        if (pos.x + radius > oL && pos.x - radius < oR &&
            pos.y + radius > oT && pos.y - radius < oB)
        {
            double overlapX = std::min((pos.x+radius)-oL, oR-(pos.x-radius));
            double overlapY = std::min((pos.y+radius)-oT, oB-(pos.y-radius));

            if (overlapX < overlapY) {
                if ((pos.x - oL) < (oR - pos.x)) pos.x -= overlapX;
                else                               pos.x += overlapX;
            } else {
                if ((pos.y - oT) < (oB - pos.y)) pos.y -= overlapY;
                else                               pos.y += overlapY;
            }
            return true;
        }
    }

    // helper to skip door gaps
    auto doorGap = [&](double wallPos, double orthPos, bool horiz){
        for (auto &d : doors) {
            if (horiz) {
                if (std::abs(wallPos - d.origin.y) < 1e-3 &&
                    orthPos >= d.origin.x && orthPos <= d.origin.x + 45.0)
                    return true;
            } else {
                if (std::abs(wallPos - d.origin.x) < 1e-3 &&
                    orthPos >= d.origin.y && orthPos <= d.origin.y + 45.0)
                    return true;
            }
        }
        return false;
    };

    // --- Wall collisions (early return each time) ---
    if (pos.y - radius < top    && pos.x >= left && pos.x <= right && !doorGap(top,    pos.x, true)) {
        pos.y = top + radius;    return true;
    }
    if (pos.y + radius > bottom && pos.x >= left && pos.x <= right && !doorGap(bottom, pos.x, true)) {
        pos.y = bottom - radius; return true;
    }
    if (pos.x - radius < left   && pos.y >= top  && pos.y <= bottom&& !doorGap(left,   pos.y, false)) {
        pos.x = left + radius;   return true;
    }
    if (pos.x + radius > right  && pos.y >= top  && pos.y <= bottom&& !doorGap(right,  pos.y, false)) {
        pos.x = right - radius;  return true;
    }

    return false;
}


//---------------------------------------------------------------------------------------------------------------------------------------
// VACUUM MOVEMENT BELOW
//---------------------------------------------------------------------------------------------------------------------------------------
void Vacuum::updateMovementandTrail(QGraphicsScene* scene)
{
    if (batteryLife <= 0 || !vacuumGraphic) return;

    Vector2D candidate;

    if (currentAlgorithm.toLower() == "wallfollow") {
        candidate = moveWallFollow(position, velocity, speed);
    } else if (currentAlgorithm.toLower() == "spiral") {
        candidate = moveSpiral(position, velocity, speed);
    } else if (currentAlgorithm.toLower() == "snaking") {
        const Room2D* room = collisionSystem->getCurrentRoom(position);
        if (room) {
            snakeLeftBound = room->topLeft.x;
            snakeRightBound = room->bottomRight.x;
            snakeTopBound = room->topLeft.y;
            snakeBottomBound = room->bottomRight.y;
        }
        candidate = moveSnaking(position, velocity, speed);
    } else if (currentAlgorithm.toLower() == "random") {
        candidate = moveRandomly(position, velocity, speed);
    } else {
        candidate = moveRandomly(position, velocity, speed);
    }

    bool hit = collisionSystem->handleCollision(candidate, radius);
    if (hit) {
        qreal angle = QRandomGenerator::global()->bounded(360.0);
        velocity = { std::cos(qDegreesToRadians(angle)), std::sin(qDegreesToRadians(angle)) };
        candidate = moveRandomly(position, velocity, speed);
        if (collisionSystem->handleCollision(candidate, radius)) return;
    }

    position = candidate;
    vacuumGraphic->setPos(position.x, position.y);

    QPointF now(position.x, position.y);
    QPointF roundedPos(std::round(now.x() / 10.0) * 10.0, std::round(now.y() / 10.0) * 10.0);
    QString posKey = QString::number(roundedPos.x()) + "," + QString::number(roundedPos.y());

    visitCount[posKey]++;
    int visits = visitCount[posKey];

    int baseGreen = std::max(0, 255 - visits * 25);
    double normalizedSpeed = static_cast<double>(speed) / 18.0;
    normalizedSpeed = std::clamp(normalizedSpeed, 0.0, 1.0);
    int speedAdjustedGreen = static_cast<int>(baseGreen * (0.5 + 0.5 * normalizedSpeed));
    speedAdjustedGreen = std::clamp(speedAdjustedGreen, 0, 255);

    QColor color;
    if (floorType == "Hard") {
        color = QColor(0, speedAdjustedGreen, 0);
    } else if (floorType == "Loop Pile") {
        color = QColor(0, 0, speedAdjustedGreen);
    } else if (floorType == "Cut Pile") {
        color = QColor(speedAdjustedGreen, 0, 0);
    } else if (floorType == "Frieze-cut Pile") {
        color = QColor(speedAdjustedGreen, speedAdjustedGreen, 0);
    } else {
        color = QColor(0, speedAdjustedGreen, 0);
    }

    static QPointF lastPoint = vacuumGraphic->pos();
    QPen pen(color); pen.setWidth(6);
    scene->addLine(QLineF(lastPoint, now), pen);
    lastPoint = now;

    batteryLife--;
}


Vector2D Vacuum::moveRandomly(Vector2D currentPos, Vector2D& velocity, int speed)
{
    if (velocity.x == 0 && velocity.y == 0) {
        qreal angle = QRandomGenerator::global()->bounded(360.0);
        velocity = { std::cos(qDegreesToRadians(angle)), std::sin(qDegreesToRadians(angle)) };
    }
    return currentPos + velocity * speed;
}

Vector2D Vacuum::moveWallFollow(Vector2D currentPos, Vector2D& velocity, int speed)
{
    constexpr double vacuumRadius = 6.4;
    constexpr double rotateStep = M_PI / 12.0;
    constexpr int maxRotations = 24;
    static double wallFollowAngle = 0.0;

    auto isValid = [&](const Vector2D& pos) -> bool {
        Vector2D test = pos;
        return !collisionSystem->handleCollision(test, vacuumRadius);
    };

    // Try normal movement first
    Vector2D next = { currentPos.x + velocity.x * speed, currentPos.y + velocity.y * speed };
    if (isValid(next)) {
        wallFollowAngle = std::atan2(velocity.y, velocity.x);
        return next;
    }

    // Try rotating until a clear direction is found
    for (int i = 0; i < maxRotations; ++i) {
        wallFollowAngle += rotateStep;
        if (wallFollowAngle > 2 * M_PI) wallFollowAngle -= 2 * M_PI;

        Vector2D tryVel;
        tryVel.x = std::cos(wallFollowAngle);
        tryVel.y = std::sin(wallFollowAngle);

        Vector2D tryNext;
        tryNext.x = currentPos.x + tryVel.x * speed;
        tryNext.y = currentPos.y + tryVel.y * speed;

        if (isValid(tryNext)) {
            double len = std::hypot(tryVel.x, tryVel.y);
            if (len != 0) {
                velocity.x = tryVel.x / len;
                velocity.y = tryVel.y / len;
            }
            return tryNext;
        }
    }

    // Fully blocked — bounce back
    velocity.x = -velocity.x;
    velocity.y = -velocity.y;
    return currentPos;
}



Vector2D Vacuum::moveSpiral(Vector2D currentPos, Vector2D& velocity, int speed)
{
    constexpr double vacuumRadius = 6.4;
    constexpr double angleIncrement = 0.1;
    constexpr double radiusGrowthRate = 0.02;

    spiralAngle += angleIncrement;
    spiralRadius += radiusGrowthRate;

    double dx = std::cos(spiralAngle) * spiralRadius;
    double dy = std::sin(spiralAngle) * spiralRadius;

    Vector2D next = { currentPos.x + dx, currentPos.y + dy };

    auto isValidMove = [&](const Vector2D& pos) {
        Vector2D test = pos;
        return !collisionSystem->handleCollision(test, vacuumRadius);
    };

    if (!isValidMove(next)) {
        double bounceAngle = (std::rand() % 60 - 30) * (M_PI / 180.0);
        spiralAngle += bounceAngle;
        dx = std::cos(spiralAngle) * spiralRadius;
        dy = std::sin(spiralAngle) * spiralRadius;
        next = { currentPos.x + dx, currentPos.y + dy };
        if (!isValidMove(next)) {
            spiralRadius = std::max(1.0, spiralRadius - 0.5);
            next = currentPos;
        }
    }

    double len = std::hypot(dx, dy);
    if (len != 0) {
        velocity.x = dx / len;
        velocity.y = dy / len;
    }

    return next;
}

Vector2D Vacuum::moveSnaking(Vector2D currentPos, Vector2D& velocity, int speed)
{
    constexpr double vacuumRadius = 6.4;
    const double shiftDistance = vacuumRadius * 0.5;

    bool nearWall = currentPos.x - snakeLeftBound < 10 || snakeRightBound - currentPos.x < 10 ||
                    currentPos.y - snakeTopBound < 10 || snakeBottomBound - currentPos.y < 10;

    if (nearWall && QRandomGenerator::global()->bounded(100) < 10) {
        return moveRandomly(currentPos, velocity, speed);
    }

    Vector2D next = { currentPos.x + velocity.x * speed, currentPos.y + velocity.y * speed };

    if (!movingUpward) {
        if (movingRight && (next.x + vacuumRadius) >= snakeRightBound) {
            movingRight = false;
            next.x = snakeRightBound - vacuumRadius;
            next.y = currentPos.y + shiftDistance;
        } else if (!movingRight && (next.x - vacuumRadius) <= snakeLeftBound) {
            movingRight = true;
            next.x = snakeLeftBound + vacuumRadius;
            next.y = currentPos.y + shiftDistance;
        }

        if ((next.y + vacuumRadius) >= snakeBottomBound) {
            next.y = snakeBottomBound - vacuumRadius;
            movingUpward = true;
            movingDown = false;
        }

        velocity = { movingRight ? 1.0 : -1.0, 0.0 };
    } else {
        if (movingRight && (next.x + vacuumRadius) >= snakeRightBound) {
            movingRight = false;
            next.x = snakeRightBound - vacuumRadius;
            next.y = currentPos.y - shiftDistance;
        } else if (!movingRight && (next.x - vacuumRadius) <= snakeLeftBound) {
            movingRight = true;
            next.x = snakeLeftBound + vacuumRadius;
            next.y = currentPos.y - shiftDistance;
        }

        if ((next.y - vacuumRadius) <= snakeTopBound) {
            next.y = snakeTopBound + vacuumRadius;
            movingUpward = false;
            movingDown = true;
        }

        velocity = { movingRight ? 1.0 : -1.0, 0.0 };
    }

    next.x = std::clamp(next.x, snakeLeftBound + vacuumRadius, snakeRightBound - vacuumRadius);
    next.y = std::clamp(next.y, snakeTopBound + vacuumRadius, snakeBottomBound - vacuumRadius);

    return next;
}
