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


    setVacuumPosition(0, 0);  // x and y should be center of starting room
    currentPosition = vacuumGraphic->pos(); // sync position
    lastTrailPoint = vacuumGraphic->pos();       // FIX: trail starts at actual location

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
            nextPos = PathAlgorithms::moveWallFollowing(currentPosition, velocity, speed * multiplier,
                                                        currentRoom.left(), currentRoom.right(), currentRoom.top(), currentRoom.bottom(),
                                                        obstructions);
        else if (currentAlgorithm == "Snaking")
            nextPos = PathAlgorithms::moveSnaking(currentPosition, velocity, speed * multiplier,
                                                  currentRoom.left(), currentRoom.right(), currentRoom.top(), currentRoom.bottom(),
                                                  obstructions, movingDown, movingRight, currentZoneX, currentZoneY, movingUpward);
        else if (currentAlgorithm == "Spiral")
            nextPos = PathAlgorithms::moveSpiral(currentPosition, velocity, speed * multiplier, spiralAngle, spiralRadius,
                                                 currentRoom.left(), currentRoom.right(), currentRoom.top(), currentRoom.bottom(),
                                                 obstructions);
        else
            nextPos = PathAlgorithms::moveRandomly(currentPosition, velocity, speed * multiplier,
                                                   currentRoom.left(), currentRoom.right(), currentRoom.top(), currentRoom.bottom(),
                                                   obstructions, doors);

        // --- Update vacuum position and draw heatmap trail ---
        vacuumGraphic->setPos(nextPos);

        // Track visit counts
        // Track visit counts
        QPointF roundedPos(std::round(nextPos.x() / 10.0) * 10.0, std::round(nextPos.y() / 10.0) * 10.0);
        QString posKey = QString::number(roundedPos.x()) + "," + QString::number(roundedPos.y());

        visitCount[posKey]++;
        int visits = visitCount[posKey];

        // Calculate base green based on visits
        int baseGreen = std::max(0, 255 - visits * 25);

        // Adjust based on speed: slower = darker, faster = lighter
        double normalizedSpeed = static_cast<double>(speed) / 18.0;
        normalizedSpeed = std::clamp(normalizedSpeed, 0.0, 1.0);

        // Corrected: slow = dark, fast = light
        int speedAdjustedGreen = static_cast<int>(baseGreen * (0.5 + 0.5 * normalizedSpeed));
        speedAdjustedGreen = std::clamp(speedAdjustedGreen, 0, 255);

        // Set the trail color
        QColor color(0, speedAdjustedGreen, 0);

        QPen trailPen(color);
        trailPen.setWidth(8);
        scene->addLine(QLineF(lastTrailPoint, nextPos), trailPen);

        lastTrailPoint = nextPos;
        currentPosition = nextPos;


        // Track whisker cleaning activity
        if (whiskerEfficiency > 0)
        {
            double whiskerRange = 13.5 / 2.0; // Whisker radius in inches
            QRectF whiskerArea(currentPosition.x() - whiskerRange, currentPosition.y() - whiskerRange,
                               whiskerRange * 2, whiskerRange * 2);

            // Check if this area overlaps with a cleaning spot
            if (whiskerArea.contains(currentPosition))
            {
                whiskerCleaningCount++;
            }
        }

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

    qDebug() << "Pathings algorithms set to:" << pathingAlgorithms;
}

double Vacuum::calculateWhiskerEffectiveness() const
{
    if (whiskerCleaningCount == 0) return 0.0;

    return static_cast<double>(whiskerCleaningCount) * (static_cast<double>(whiskerEfficiency) / 100.0);
}


