#include "vacuum.h"
#include "house.h"
#include <QtGui/qpen.h>
#include <iostream>

Vacuum::Vacuum(QGraphicsScene* scene)
{
    batteryLife = 150;
    speed = 12;
    whiskerEfficiency = 30;
    pathingAlgorithms << "Random";
    position = {0.0, 0.0};

    vacuumGraphic = scene->addEllipse(-diameter/2, -diameter, diameter, diameter,
                                      QPen(Qt::black), QBrush(Qt::gray));
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

void Vacuum::setPathingAlgorithms(const QStringList &algorithms)
{
    pathingAlgorithms = algorithms;
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

QStringList Vacuum::getPathingAlgorithms() const
{
    return pathingAlgorithms;
}

QGraphicsEllipseItem* Vacuum::getGraphic() const
{
    return vacuumGraphic;
}

const Vector2D& Vacuum::getPosition() const
{
    return position;
}

//---------------------------------------------------------------------------------------------------------------------------------------
// COLLISON SYSTEM BELOW
//---------------------------------------------------------------------------------------------------------------------------------------

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
        Room2D room;
        room.topLeft.x = obj["x_topLeft"].toDouble();
        room.topLeft.y = obj["y_topLeft"].toDouble();
        room.bottomRight.x = obj["x_bottomRight"].toDouble();
        room.bottomRight.y = obj["y_bottomRight"].toDouble();
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
        Obstruction2D obstruction;
        obstruction.isChest = obj["is_chest"].toBool();
        obstruction.topLeft.x = obj["x_topLeft"].toDouble();
        obstruction.topLeft.y = obj["y_topLeft"].toDouble();
        obstruction.bottomRight.x = obj["x_bottomRight"].toDouble();
        obstruction.bottomRight.y = obj["y_bottomRight"].toDouble();
        obstructions.push_back(obstruction);
    }

    return true;

}

void CollisionSystem::handleCollision(Vector2D& pos, Vector2D& vel, double radius)
{
    // First: Handle room wall collisions with door gaps
    for (const auto& room : rooms) {
        // Top Wall (y = topLeft.y)
        if (pos.y - radius < room.topLeft.y &&
            pos.x >= room.topLeft.x && pos.x <= room.bottomRight.x) {
            bool doorGap = false;
            for (const auto& door : doors) {
                if (std::abs(room.topLeft.y - door.origin.y) < 1e-3 &&
                    pos.x >= door.origin.x && pos.x <= door.origin.x + 45.0f) {
                    doorGap = true;
                    break;
                }
            }
            if (!doorGap) vel.y = std::abs(vel.y); // Bounce downward
        }

        // Bottom Wall (y = bottomRight.y)
        if (pos.y + radius > room.bottomRight.y &&
            pos.x >= room.topLeft.x && pos.x <= room.bottomRight.x) {
            bool doorGap = false;
            for (const auto& door : doors) {
                if (std::abs(room.bottomRight.y - door.origin.y) < 1e-3 &&
                    pos.x >= door.origin.x && pos.x <= door.origin.x + 45.0f) {
                    doorGap = true;
                    break;
                }
            }
            if (!doorGap) vel.y = -std::abs(vel.y); // Bounce upward
        }

        // Left Wall (x = topLeft.x)
        if (pos.x - radius < room.topLeft.x &&
            pos.y >= room.bottomRight.y && pos.y <= room.topLeft.y) {
            bool doorGap = false;
            for (const auto& door : doors) {
                if (std::abs(room.topLeft.x - door.origin.x) < 1e-3 &&
                    pos.y >= door.origin.y && pos.y <= door.origin.y + 45.0f) {
                    doorGap = true;
                    break;
                }
            }
            if (!doorGap) vel.x = std::abs(vel.x); // Bounce right
        }

        // Right Wall (x = bottomRight.x)
        if (pos.x + radius > room.bottomRight.x &&
            pos.y >= room.bottomRight.y && pos.y <= room.topLeft.y) {
            bool doorGap = false;
            for (const auto& door : doors) {
                if (std::abs(room.bottomRight.x - door.origin.x) < 1e-3 &&
                    pos.y >= door.origin.y && pos.y <= door.origin.y + 45.0f) {
                    doorGap = true;
                    break;
                }
            }
            if (!doorGap) vel.x = -std::abs(vel.x); // Bounce left
        }
    }

    // Then: Handle collisions with chests (still same logic)
    for (const auto& obs : obstructions) {
        if (!obs.isChest) continue;

        double closestX = clamp(pos.x, obs.bottomRight.x, obs.topLeft.x);
        double closestY = clamp(pos.y, obs.bottomRight.y, obs.topLeft.y);

        double distX = pos.x - closestX;
        double distY = pos.y - closestY;
        double distanceSquared = distX * distX + distY * distY;

        if (distanceSquared < radius * radius) {
            if (std::abs(distX) > std::abs(distY)) {
                vel.x = -vel.x;
            } else {
                vel.y = -vel.y;
            }
            return; // Only handle one collision
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------
// VACUUM MOVEMENT BELOW
//---------------------------------------------------------------------------------------------------------------------------------------

