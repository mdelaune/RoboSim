#include "vacuum.h"
#include "pathingalgorithms.h"
#include <QtGui/qpen.h>

Vacuum::Vacuum(QGraphicsScene* scene)
    : scene(scene),
    batteryLife(150), vacuumEfficiency(90), whiskerEfficiency(30), speed(12),
    spiralAngle(0.0), spiralRadius(1.0), currentRoomIndex(-1)
{
    pathingAlgorithms << "Random";

    vacuumGraphic = scene->addEllipse(-diameter/2, -diameter/2, diameter, diameter,
                                      QPen(Qt::black), QBrush(Qt::gray));


    setVacuumPosition(0, 0);  // ✅ x and y should be center of starting room
    currentPosition = vacuumGraphic->pos(); // sync position
    lastTrailPoint = vacuumGraphic->pos();       // ✅ FIX: trail starts at actual location

    double angle = static_cast<double>(std::rand() % 360) * (M_PI / 180.0);
    velocity.setX(std::cos(angle));
    velocity.setY(std::sin(angle));
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



void Vacuum::setVacuumPosition(double x, double y)
{
    vacuumGraphic->setPos(x, y);
    currentPosition = vacuumGraphic->pos();
    lastTrailPoint = currentPosition;  // ✅ align trail

    // ✅ Draw a 1-pixel dot to mark the starting position
    QPen trailPen(Qt::blue);
    trailPen.setWidth(2);
    scene->addLine(QLineF(currentPosition, currentPosition), trailPen);
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

QPointF Vacuum::getCurrentPosition() const
{
    return currentPosition;
}

void Vacuum::setFloorType(const QString &type)
{
    floorType = type;
}


void Vacuum::move(const QList<QRectF>& rooms, const QList<Obstruction2>& obstructions, const QList<QPointF>& doors, int multiplier)
{
    if (batteryLife <= 0 || !vacuumGraphic) return;

    QRectF currentRoom;
    currentPosition = vacuumGraphic->pos();

    // --- Find current room ---
    for (const QRectF& room : rooms)
    {
        if (room.contains(currentPosition))
        {
            currentRoom = room;
            break;
        }
    }

    if (!currentRoom.isNull())
    {
        QPointF nextPos;
        QString currentAlgorithm;

        // --- Handle automatic algorithm switching ---
        if (pathingAlgorithms.size() == 4)
        {
            algorithmSwitchTimer++;

            if (algorithmSwitchTimer >= switchInterval)
            {
                algorithmSwitchTimer = 0;
                currentAlgorithmIndex++;

                if (currentAlgorithmIndex >= pathingAlgorithms.size())
                {
                    currentAlgorithmIndex = 0;
                    std::random_shuffle(pathingAlgorithms.begin(), pathingAlgorithms.end());
                    qDebug() << "Shuffled algorithms:" << pathingAlgorithms;
                }

                qDebug() << "Switched to algorithm:" << pathingAlgorithms[currentAlgorithmIndex];
            }

            currentAlgorithm = pathingAlgorithms[currentAlgorithmIndex];
        }
        else
        {
            currentAlgorithm = pathingAlgorithms.first();
        }

        // --- Choose next move based on algorithm ---
        if (currentAlgorithm == "Wall Follow")
            nextPos = PathAlgorithms::moveWallFollowing(currentPosition, velocity, speed * multiplier, currentRoom, obstructions);
        else if (currentAlgorithm == "Snaking")
            nextPos = PathAlgorithms::moveSnaking(currentPosition, velocity, speed * multiplier, currentRoom, obstructions, movingDown, movingRight, currentZoneX, currentZoneY, movingUpward);
        else if (currentAlgorithm == "Spiral")
            nextPos = PathAlgorithms::moveSpiral(currentPosition, velocity, speed * multiplier, spiralAngle, spiralRadius, currentRoom, obstructions);
        else
            nextPos = PathAlgorithms::moveRandomly(currentPosition, velocity, speed * multiplier, currentRoom, obstructions, doors);

        // --- Update vacuum position ---
        vacuumGraphic->setPos(nextPos);
        lastTrailPoint = nextPos;
        currentPosition = nextPos;

        qDebug() << "Moved to:" << nextPos;
    }
    else
    {
        qDebug() << "Vacuum is not inside any room!";
    }

    batteryLife--;
}


void Vacuum::setPathingAlgorithms(const QStringList &algorithms)
{
    if (algorithms.size() == 4)
    {
        pathingAlgorithms.clear();
        pathingAlgorithms << "Random" << "Wall Follow" << "Snaking" << "Spiral";
    }
    else
    {
        pathingAlgorithms = algorithms;
    }

    qDebug() << "Pathings algorithms set to:" << pathingAlgorithms; // 🔥 Print
}


