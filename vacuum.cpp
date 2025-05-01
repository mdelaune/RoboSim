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
    position = collisionSystem->getVacuumStartPosition();
    setVacuumPosition(position);
    lastTrailPoint = vacuumGraphic->pos();
    cleanedCoords.clear();
}


// Setters

void Vacuum::setHousePath(QString& path)
{
    housePath = path;
    if (!collisionSystem->loadFromJson(housePath)) {
        qWarning() << "Failed to load plan from" << housePath;
    }
}

void Vacuum::setBatteryLife(int minutes)
{
    if (minutes >= 90 && minutes <= 200)
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

double Vacuum::getCoveredArea() const
{
    return cleanedCoords.size(); //coveredArea;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// COLLISON SYSTEM BELOW
//---------------------------------------------------------------------------------------------------------------------------------------

Vector2D CollisionSystem::getVacuumStartPosition() const
{

    return vacuumStart;
}

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
    qDebug() << filePath;
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

    if (root.contains("vacuum_pos") && root["vacuum_pos"].isObject()) {
        QJsonObject v = root["vacuum_pos"].toObject();
        // assume you have a member Vector2D vacuumStart;
        vacuumStart.x = v.value("vacuumX").toDouble();
        vacuumStart.y = v.value("vacuumY").toDouble();
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
        // allow any candidate within 'radius' of the hinge‐line
        const double tol = radius + 0.1;
        for (auto &d : doors) {
            if (horiz) {
                // horizontal door at y = d.origin.y, spans x in [origin.x, origin.x+45]
                if (std::abs(wallPos - d.origin.y) <= tol &&
                    orthPos >= d.origin.x  - tol &&
                    orthPos <= d.origin.x + 45.0 + tol)
                    return true;
            } else {
                // vertical door at x = d.origin.x, spans y in [origin.y, origin.y+45]
                if (std::abs(wallPos - d.origin.x) <= tol &&
                    orthPos >= d.origin.y  - tol &&
                    orthPos <= d.origin.y + 45.0 + tol)
                    return true;
            }
        }
        return false;
    };

    bool corrected = false;

    // Top wall
    if (pos.y - radius < top &&
        pos.x >= left && pos.x <= right &&
        !doorGap(top, pos.x, /*horizontal=*/true))
    {
        pos.y = top + radius;
        corrected = true;
    }

    // Bottom wall
    if (pos.y + radius > bottom &&
        pos.x >= left && pos.x <= right &&
        !doorGap(bottom, pos.x, /*horizontal=*/true))
    {
        pos.y = bottom - radius;
        corrected = true;
    }

    // Left wall
    if (pos.x - radius < left &&
        pos.y >= top && pos.y <= bottom &&
        !doorGap(left, pos.y, /*horizontal=*/false))
    {
        pos.x = left + radius;
        corrected = true;
    }

    // Right wall
    if (pos.x + radius > right &&
        pos.y >= top && pos.y <= bottom &&
        !doorGap(right, pos.y, /*horizontal=*/false))
    {
        pos.x = right - radius;
        corrected = true;
    }

    return corrected;
}


//---------------------------------------------------------------------------------------------------------------------------------------
// VACUUM MOVEMENT BELOW
//---------------------------------------------------------------------------------------------------------------------------------------
// void Vacuum::updateMovementandTrail(QGraphicsScene* scene)
// {
//     if (batteryLife <= 0 || !vacuumGraphic) return;

//     Vector2D candidate;

//     // Select movement algorithm
//     if (currentAlgorithm.toLower() == "wall follow") {
//         candidate = moveWallFollow(position, velocity, speed);
//     } else if (currentAlgorithm.toLower() == "spiral") {
//         candidate = moveSpiral(position, velocity, speed);
//     } else if (currentAlgorithm.toLower() == "snaking") {
//         const Room2D* room = collisionSystem->getCurrentRoom(position);
//         if (room) {
//             snakeLeftBound = room->topLeft.x;
//             snakeRightBound = room->bottomRight.x;
//             snakeTopBound = room->topLeft.y;
//             snakeBottomBound = room->bottomRight.y;
//         }
//         candidate = moveSnaking(position, velocity, speed);
//     } else if (currentAlgorithm.toLower() == "random") {
//         candidate = moveRandomly(position, velocity, speed);
//     } else {
//         candidate = moveRandomly(position, velocity, speed);
//     }

//     // Collision handling
//     bool hit = collisionSystem->handleCollision(candidate, radius);
//     if (hit) {
//         qreal angle = QRandomGenerator::global()->bounded(360.0);
//         velocity = { std::cos(qDegreesToRadians(angle)), std::sin(qDegreesToRadians(angle)) };
//         candidate = moveRandomly(position, velocity, speed);
//         if (collisionSystem->handleCollision(candidate, radius)) return;
//     }

//     // Move vacuum
//     position = candidate;
//     vacuumGraphic->setPos(position.x, position.y);

//     // Trail
//     static QPointF lastPoint = vacuumGraphic->pos();
//     QPointF now(position.x, position.y);
//     QPen pen(QColor(0,0,255,vacuumEfficiency)); pen.setWidth(12);
//     scene->addLine(QLineF(lastPoint, now), pen);
//     lastPoint = now;

//     // Cover Sq Ft
//     bool add = true;

//     for (int i = 0; i < cleanedCoords.size(); i++){
//         if (static_cast<int>(cleanedCoords[i].x) >= static_cast<int>(position.x) - 6 and
//             static_cast<int>(cleanedCoords[i].x) <= static_cast<int>(position.x) + 6 and
//             static_cast<int>(cleanedCoords[i].y) >= static_cast<int>(position.y) - 6 and
//             static_cast<int>(cleanedCoords[i].y) <= static_cast<int>(position.y) + 6) {
//             add = false;
//         }
//     }

//     if (add == true){
//         cleanedCoords.append(position);
//     }

//     batteryLife--;
// }

void Vacuum::updateMovementandTrail(QGraphicsScene* scene)
{
    if (batteryLife <= 0 || !vacuumGraphic)
        return;

    // 1) Pick your full‐target based on the chosen algorithm
    Vector2D fullTarget;
    QString alg = currentAlgorithm.toLower();
    if (alg == "wall follow") {
        fullTarget = moveWallFollow(position, velocity, speed);
    }
    else if (alg == "spiral") {
        fullTarget = moveSpiral(position, velocity, speed);
    }
    else if (alg == "snaking") {
        const Room2D* room = collisionSystem->getCurrentRoom(position);
        if (room) {
            snakeLeftBound   = room->topLeft.x;
            snakeRightBound  = room->bottomRight.x;
            snakeTopBound    = room->topLeft.y;
            snakeBottomBound = room->bottomRight.y;
        }
        fullTarget = moveSnaking(position, velocity, speed);
    }
    else {
        fullTarget = moveRandomly(position, velocity, speed);
    }

    // 2) Compute micro-steps so we never move more than radius per iteration
    double radius = diameter / 2.0;
    Vector2D delta { fullTarget.x - position.x,
                   fullTarget.y - position.y };
    double  dist   = std::hypot(delta.x, delta.y);
    int     steps  = std::max(1, int(std::ceil(dist / radius)));
    Vector2D stepDelta { delta.x / steps, delta.y / steps };

    // 3) Walk those steps, handling collisions at each micro-step
    for (int i = 0; i < steps; ++i)
    {
        Vector2D candidate { position.x + stepDelta.x,
                           position.y + stepDelta.y };

        bool hit = collisionSystem->handleCollision(candidate, radius);
        if (hit)
        {
            if (alg == "random")
            {
                // -- bounce: pick a new random heading
                qreal angle = QRandomGenerator::global()->bounded(360.0);
                velocity = {
                    std::cos(qDegreesToRadians(angle)),
                    std::sin(qDegreesToRadians(angle))
                };

                // -- rebuild a fresh fullTarget & stepDelta
                fullTarget = moveRandomly(position, velocity, speed);
                delta      = { fullTarget.x - position.x,
                         fullTarget.y - position.y };
                dist       = std::hypot(delta.x, delta.y);
                steps      = std::max(1, int(std::ceil(dist / radius)));
                stepDelta  = { delta.x / steps, delta.y / steps };

                // restart the loop so we respect the new stepDelta
                i = -1;
                continue;
            }
            else
            {
                // non-random alg: stop stepping on first collision
                break;
            }
        }

        // -- commit this micro-step
        position = candidate;

        // -- draw trail
        QPointF now(position.x, position.y);
        QPen pen(QColor(0, 0, 255, vacuumEfficiency));
        pen.setWidth(12);
        scene->addLine(QLineF(lastTrailPoint, now), pen);
        lastTrailPoint = now;
    }

    // 4) Finally, update the graphic and record coverage
    vacuumGraphic->setPos(position.x, position.y);

    bool add = true;
    for (auto &pt : cleanedCoords) {
        if (int(pt.x) >= int(position.x) - 6 &&
            int(pt.x) <= int(position.x) + 6 &&
            int(pt.y) >= int(position.y) - 6 &&
            int(pt.y) <= int(position.y) + 6)
        {
            add = false;
            break;
        }
    }
    if (add) {
        cleanedCoords.append(position);
    }

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
    constexpr double rotateStep = M_PI / 12.0;   // 15 degrees
    constexpr int maxRotations = 24;
    constexpr int randomChanceOnBlock = 15;      // % chance to switch to Random if blocked

    static double wallFollowAngle = 0.0;

    auto isValid = [&](Vector2D pos) {
        return !collisionSystem->handleCollision(pos, vacuumRadius);
    };

    // Step 1: Try current direction
    Vector2D next = { currentPos.x + velocity.x * speed, currentPos.y + velocity.y * speed };
    if (isValid(next)) {
        wallFollowAngle = std::atan2(velocity.y, velocity.x);
        return next;
    }

    // Step 2: Rotate left/right to find alternative path
    for (int i = 0; i < maxRotations; ++i) {
        wallFollowAngle += rotateStep;
        if (wallFollowAngle > 2 * M_PI) wallFollowAngle -= 2 * M_PI;

        Vector2D tryVel = { std::cos(wallFollowAngle), std::sin(wallFollowAngle) };
        Vector2D tryNext = { currentPos.x + tryVel.x * speed, currentPos.y + tryVel.y * speed };

        if (isValid(tryNext)) {
            double len = std::hypot(tryVel.x, tryVel.y);
            if (len != 0) {
                velocity = { tryVel.x / len, tryVel.y / len };
            }
            return tryNext;
        }
    }

    // Step 3: Still blocked — inject random with chance
    if (QRandomGenerator::global()->bounded(100) < randomChanceOnBlock) {
        return moveRandomly(currentPos, velocity, speed);
    }

    // Step 4: Bounce back if nothing else worked
    velocity = { -velocity.x, -velocity.y };
    return currentPos;
}


Vector2D Vacuum::moveSpiral(Vector2D currentPos, Vector2D& velocity, int speed)
{
    constexpr double vacuumRadius = 6.4;
    constexpr double angleIncrement = 0.07;
    constexpr double radiusGrowthRate = 0.03;
    constexpr double minDistanceFromWall = 18.0;
    constexpr double maxSpiralRadius = 60.0;
    constexpr int randomFallbackFrames = 5;
    constexpr int randomTriggerChance = 20; // % chance spiral *actually* switches when blocked

    static bool spiralInRandomMode = false;
    static int spiralRandomCooldown = 0;

    // Step 1: Check if we should still be in fallback random mode
    if (spiralInRandomMode) {
        spiralRandomCooldown--;
        if (spiralRandomCooldown <= 0)
            spiralInRandomMode = false;
        else
            return moveRandomly(currentPos, velocity, speed); // 🔁 TEMP switch
    }

    // Step 2: Proximity probe (are we near a wall?)
    auto tooCloseToWall = [&]() {
        int blocked = 0;
        for (int i = 0; i < 8; ++i) {
            double angle = i * M_PI / 4.0;
            Vector2D probe = {
                currentPos.x + std::cos(angle) * minDistanceFromWall,
                currentPos.y + std::sin(angle) * minDistanceFromWall
            };
            if (collisionSystem->handleCollision(probe, vacuumRadius)) {
                blocked++;
            }
        }
        return blocked > 2;
    };

    // Step 3: Rarely trigger fallback random if too close
    if (tooCloseToWall() && QRandomGenerator::global()->bounded(100) < randomTriggerChance) {
        spiralInRandomMode = true;
        spiralRandomCooldown = randomFallbackFrames;
        return moveRandomly(currentPos, velocity, speed);
    }

    // Step 4: Continue normal spiral
    spiralAngle += angleIncrement;
    spiralRadius += radiusGrowthRate;

    if (maxSpiralRadius > maxSpiralRadius) {
        spiralRadius = 1.0;
        spiralAngle = 0.0;
    }

    double dx = std::cos(spiralAngle) * spiralRadius;
    double dy = std::sin(spiralAngle) * spiralRadius;
    Vector2D next = { currentPos.x + dx, currentPos.y + dy };

    auto isValid = [&](Vector2D pos) {
        return !collisionSystem->handleCollision(pos, vacuumRadius);
    };

    if (!isValid(next)) {
        // Slight bounce
        spiralAngle += (std::rand() % 60 - 30) * (M_PI / 180.0);
        dx = std::cos(spiralAngle) * spiralRadius;
        dy = std::sin(spiralAngle) * spiralRadius;
        next = { currentPos.x + dx, currentPos.y + dy };

        if (!isValid(next)) {
            spiralRadius = std::max(1.0, spiralRadius - 0.5);
            return currentPos;
        }
    }

    // Normalize velocity
    double len = std::hypot(dx, dy);
    if (len != 0)
        velocity = { dx / len, dy / len };

    return next;
}


Vector2D Vacuum::moveSnaking(Vector2D currentPos, Vector2D& velocity, int speed)
{
    const double shiftDistance = (radius * 2) -1; // * 0.5;

    bool nearWall = currentPos.x - snakeLeftBound < 10 || snakeRightBound - currentPos.x < 10 ||
                    currentPos.y - snakeTopBound < 10 || snakeBottomBound - currentPos.y < 10;

    if (nearWall && QRandomGenerator::global()->bounded(100) < 10) {
        return moveRandomly(currentPos, velocity, speed);
    }

    Vector2D next = { currentPos.x + velocity.x * speed, currentPos.y + velocity.y * speed };

    if (!movingUpward) {
        if (movingRight && (next.x + radius) >= snakeRightBound) {
            movingRight = false;
            next.x = snakeRightBound - radius;
            next.y = currentPos.y + shiftDistance;
        } else if (!movingRight && (next.x - radius) <= snakeLeftBound) {
            movingRight = true;
            next.x = snakeLeftBound + radius;
            next.y = currentPos.y + shiftDistance;
        }

        if ((next.y + radius) >= snakeBottomBound) {
            next.y = snakeBottomBound - radius;
            movingUpward = true;
            movingDown = false;
        }

        velocity = { movingRight ? 1.0 : -1.0, 0.0 };
    } else {
        if (movingRight && (next.x + radius) >= snakeRightBound) {
            movingRight = false;
            next.x = snakeRightBound - radius;
            next.y = currentPos.y - shiftDistance;
        } else if (!movingRight && (next.x - radius) <= snakeLeftBound) {
            movingRight = true;
            next.x = snakeLeftBound + radius;
            next.y = currentPos.y - shiftDistance;
        }

        if ((next.y - radius) <= snakeTopBound) {
            next.y = snakeTopBound + radius;
            movingUpward = false;
            movingDown = true;
        }

        velocity = { movingRight ? 1.0 : -1.0, 0.0 };
    }

    next.x = std::clamp(next.x, snakeLeftBound + radius, snakeRightBound - radius);
    next.y = std::clamp(next.y, snakeTopBound + radius, snakeBottomBound - radius);

    return next;
}
