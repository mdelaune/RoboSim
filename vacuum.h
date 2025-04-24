#ifndef VACUUM_H
#define VACUUM_H

//#include <QObject>
#include <QTimer>
#include <QPointF>
#include <QString>
#include <QGraphicsScene>
#include <QRectF>
#include <QGraphicsEllipseItem>
#include <QStringList>
#include <QSet>
//#include <QHash>
#include <QGraphicsRectItem>

class Vacuum : public QObject {
    Q_OBJECT

public:
    explicit Vacuum(QGraphicsScene *scene, QObject *parent = nullptr);

    // Settings
    void setSpeed(double inchesPerSecond);
    void setBatteryLife(int minutes);
    void setPathingAlgorithm(const QString &algorithm);
    void setFloorType(const QString &type);
    void setEfficiency(double vacuumEff, double whiskerEff);
    void setRoomAndDoor(const QRectF &room, const QRectF &door);
    void setSpeedMultiplier(int multiplier);
    void setObstacles(const QList<QGraphicsRectItem*>& obs);
    void rotateVacuum(double angleDelta);
    void advanceToNextAlgorithm();
    void runTimedAlgorithms();

    // Simulation control
    void start();
    void stop();

    // Getters
    int getBatteryLife() const;
    double getSpeed() const;
    QString getPathingAlgorithm() const;
    QPointF getCurrentPosition() const;
    QGraphicsEllipseItem* getVacuumItem() const;
    double getCoveragePercent(int roomWidth, int roomHeight, const QList<QGraphicsRectItem*>& obstacles) const;

    //QHash<QPoint, int> cellVisitCount;
    QPointF lastTrailPoint;
    QString snakingMode;
    int snakingStepCounter;


    void setSelectedAlgs(QStringList selection);

signals:
    void batteryDepleted();

private slots:
    void tick();

private:
    QStringList selectedAlgs;
    QGraphicsScene *scene;
    QTimer movementTimer;
    QGraphicsEllipseItem *vacuumItem;

    double speed;
    int batteryLife;
    int remainingBattery;
    QString floorType;
    QString pathingAlgorithm;

    QPointF currentPosition;
    QPointF velocity;

    double cleaningEfficiency;
    double whiskerEfficiency;

    bool stoppedByUser;

    // Pathing control
    bool movingRight;
    int wallDirection;
    double spiralAngle;
    double spiralRadius;
    bool spiraling = true;
    bool wallFollowing = false;
    int wallFollowCounter = 0;
    bool snakingDown = true;
    double baseCleaningEfficiency = 0.9;
    double baseWhiskerEfficiency = 0.3;


    double currentRotation = 0.0;
    QPointF snakeTarget;
    bool snakingInitialized = false;

    QRectF roomRect;
    QRectF doorRect;

    // Obstacle info
    QList<QGraphicsRectItem*> obstacles;

    // Trail tracking
    QSet<QPoint> cleanedCells;
    int cellSize = 10;

    // Pathing algorithms
    void moveRandomly();
    void moveSnaking();
    void moveWallFollowing();
    void moveBounce();
    void moveSpiral();
    void onRunAllClicked(); // For the Run All Algorithms button


    // Collision/update
    void handleWallCollision();
    void updatePosition();
    bool checkBounds(const QPointF &newPosition);

    // Algorithm completion tracking
    QStringList allAlgorithms;
    QSet<QString> completedAlgorithms;

    int algorithmTickCount = 0;     // Tick counter for the current algorithm
    int maxAlgorithmTicks = 0;
        // Time limit in ticks for each algorithm
    bool timedExecutionEnabled = false;
    int speedMultiplier = 1;  // Default is 1x
    int globalTickCount = 0;
    int maxGlobalTicks = 36000; // 10 minutes at 60 FPS



};

#endif // VACUUM_H
