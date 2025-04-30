#ifndef VACUUM_H
#define VACUUM_H

#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QPointF>
#include <QBrush>
#include <QStringList>

struct Vector2D
{
    double x;
    double y;
};

struct Room2D
{
    Vector2D topLeft;
    Vector2D bottomRight;
};

struct Door2D
{
    Vector2D origin;
};

struct Obstruction2D
{
    Vector2D topLeft;
    Vector2D bottomRight;
    bool isChest;
};

class CollisionSystem
{
public:
    bool loadFromJson(const QString& filePath);
    void handleCollision(Vector2D& position, Vector2D& velocity, double radius);

private:
    std::vector<Room2D> rooms;
    std::vector<Door2D> doors;
    std::vector<Obstruction2D> obstructions;
};

class Vacuum
{
public:
    Vacuum(QGraphicsScene* scene);
    
    // Setters
    void setBatteryLife(int minutes);
    void setVacuumEfficiency(int vacuumEff);
    void setWhiskerEfficiency(int whiskerEff);
    void setSpeed(int inchesPerSecond);
    void setPathingAlgorithms(const QStringList &algorithms);  // Changed to accept a list of algorithms
    void setVacuumPosition(Vector2D& position);
    // Getters

    int getBatteryLife() const;
    int getVacuumEfficiency() const;
    int getWhiskerEfficiency() const;
    int getSpeed() const;
    QStringList getPathingAlgorithms() const;  // Returns a list of algorithms
    QGraphicsEllipseItem* getGraphic() const;
    const Vector2D &getPosition() const;
    Vector2D& getVelocity() const;

    // signals:
    //     void positionUpdated(QPointF newPos);
    //     void batteryDepleted();

private:
    const double diameter = 12.8;
    const double whiskerWidth = 13.5;
    const double vacuumWidth = 5.8;

    int batteryLife;
    int vacuumEfficiency;
    int whiskerEfficiency;
    int speed;

    QStringList pathingAlgorithms;
    QGraphicsEllipseItem *vacuumGraphic;

    Vector2D position;
    Vector2D velocity;
    CollisionSystem* collisionSystem;

};

#endif // VACUUM_H
