#ifndef SIMWINDOW_H
#define SIMWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QTimer>
#include "vacuum.h"
#include "house.h"

namespace Ui {
class SimWindow;
}

class SimWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SimWindow(QWidget *parent = nullptr);
    ~SimWindow();

    void startSimulation(int batteryLife, int vacuumEfficiency, int whiskerEfficiency, int speed, QStringList selectedAlgorithms);
    void stopSimulation();

    House *house;
    QString house_path;

private slots:
    void updateSimulation();
    void oneSpeedPushed();
    void fiveSpeedPushed();
    void fiftySpeedPushed();
    void updateBatteryLifeLabel();
    void setSimulationSpeed(int multiplier);

private:
    Ui::SimWindow *ui;

    QGraphicsScene *scene;
    Vacuum *vacuum;
    QTimer *simulationTimer;
    int simulationSpeedMultiplier;


    QList<QRectF> rooms;
    QList<Obstruction2> obstructions;
    QList<QPointF> doors;
};

#endif // SIMWINDOW_H
