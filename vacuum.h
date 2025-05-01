#ifndef VACUUM_H
#define VACUUM_H

#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QPointF>
#include <QBrush>
#include <QString>
#include <cstdio>
#include <QtMath>

struct Vector2D {
    double x;
    double y;

    // Addition
    Vector2D operator+(const Vector2D& other) const {
        return {x + other.x, y + other.y};
    }

    // Scalar multiplication
    Vector2D operator*(double scalar) const {
        return {x * scalar, y * scalar};
    }
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
    bool handleCollision(Vector2D& position, double radius);
    const Room2D* getCurrentRoom(const Vector2D& pos) const;

    Vector2D getVacuumStartPosition() const;

private:
    std::vector<Room2D> rooms;
    std::vector<Door2D> doors;
    std::vector<Obstruction2D> obstructions;
    Vector2D vacuumStart;
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
    void setPathingAlgorithm(const QString &algorithm);  // Changed to accept a list of algorithms
    void setVacuumPosition(Vector2D& position);
    void setHousePath(QString& path);
    // Getters

    int getBatteryLife() const;
    int getVacuumEfficiency() const;
    int getWhiskerEfficiency() const;
    int getSpeed() const;
    QString getPathingAlgorithm() const;
    QGraphicsEllipseItem* getGraphic() const;
    const Vector2D &getPosition() const;
    Vector2D& getVelocity() const;
    int getElapsedTime() const;
    double getCoveredArea() const;

    void updateMovementandTrail(QGraphicsScene* scene);
    void reset();
    Vector2D moveRandomly(Vector2D position, Vector2D& velocity, int speed);
    Vector2D moveWallFollow(Vector2D currentPos, Vector2D& velocity, int speed);
    Vector2D moveSpiral(Vector2D currentPos, Vector2D& velocity, int speed);
    Vector2D moveSnaking(Vector2D currentPos, Vector2D& velocity, int speed);
    QString floorType = "Hard";
    QMap<QString, int> visitCount;
QPointF lastTrailPoint;
    Vector2D moveWallFollowWithRandom(Vector2D currentPos, Vector2D& velocity, int speed);

    // signals:
    //     void positionUpdated(QPointF newPos);
    //     void batteryDepleted();

private:
    const double diameter = 12.8;
    double radius = diameter/2.0;
    const double whiskerWidth = 13.5;
    const double vacuumWidth = 5.8;

    int batteryLife;
    int vacuumEfficiency;
    int whiskerEfficiency;
    int speed;
    QString currentAlgorithm;

    QString housePath;

    QGraphicsEllipseItem *vacuumGraphic;

    Vector2D position;
    Vector2D nextPosition;
    Vector2D velocity;
    CollisionSystem* collisionSystem;

    int elapsedTime = 0;
    double coveredArea = 0.0;
    double spiralAngle = 0.0;
    double spiralRadius = 1.0;
    bool movingRight = true;
    bool movingDown = true;
    bool movingUpward = false;

    double snakeLeftBound = 0.0;
    double snakeRightBound = 0.0;
    double snakeTopBound = 0.0;
    double snakeBottomBound = 0.0;

    QGraphicsScene* scene;

};

#endif // VACUUM_H
