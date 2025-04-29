#include "vacuum.h"
#include <QtGui/qpen.h>

Vacuum::Vacuum(QGraphicsScene* scene)
    : batteryLife(150), vacuumEfficiency(90), whiskerEfficiency(30), speed(12)
{
    pathingAlgorithms << "Random";

    vacuumGraphic = scene->addEllipse(-diameter/2, -diameter, diameter, diameter,
                                      QPen(Qt::black), QBrush(Qt::gray));
    setVacuumPosition(0, 0);

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

void Vacuum::setPathingAlgorithms(const QStringList &algorithms)
{
    pathingAlgorithms = algorithms;
}

void Vacuum::setVacuumPosition(double x, double y)
{
    vacuumGraphic->setPos(x, y);
    currentPosition = vacuumGraphic->pos();
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
