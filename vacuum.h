#ifndef VACUUM_H
#define VACUUM_H

#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QPointF>
#include <QBrush>
#include <QStringList>
#include "houseparser.h"

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
    void setVacuumPosition(double x, double y);
    void setFloorType(const QString &type);


    // Getters
    int getBatteryLife() const;
    int getVacuumEfficiency() const;
    int getWhiskerEfficiency() const;
    int getSpeed() const;
    QStringList getPathingAlgorithms() const;  // Returns a list of algorithms
    QGraphicsEllipseItem* getGraphic() const;
    QPointF getCurrentPosition() const;

    // test; will be removed.
    void move(const QList<QRectF>& rooms, const QList<Obstruction2>& obstructions, const QList<QPointF>& doors, int multiplier);

    // signals:
    //     void positionUpdated(QPointF newPos);
    //     void batteryDepleted();
    void enableAutomaticSwitching();

private:
    const double diameter = 12.8;
    //const double whiskerWidth = 13.5;
    //const double vacuumWidth = 5.8;

    int batteryLife;
    int vacuumEfficiency;
    int whiskerEfficiency;
    int speed;
    QStringList pathingAlgorithms;
    QGraphicsEllipseItem *vacuumGraphic;
    QPointF velocity;
    QPointF currentPosition;
    double spiralAngle = 0.0;
    double spiralRadius = 1.0;
    bool checkBounds(const QPointF &newPosition, const QList<QPointF> &doors);
    QGraphicsScene* scene = nullptr;
    QRectF roomRect;
    QPointF lastTrailPoint;

    int currentRoomIndex = -1;
    QString floorType = "Hard";
    bool movingDown = true;
    bool movingRight = true;
    int currentZoneX = 0; // NEW
    int currentZoneY = 0;
    bool movingUpward = false;

    int currentAlgorithmIndex = 0; // 🔥 NEW: which algorithm is currently active
    int algorithmSwitchTimer = 0;   // 🔥 NEW: how many frames passed
    const int switchInterval = 30 * 60;

    bool autoSwitchingEnabled = false;


    QMap<QPair<int, int>, int> trailHeatmap;

    QColor interpolateColor(QColor startColor, QColor endColor, double t);

};

#endif // VACUUM_H

