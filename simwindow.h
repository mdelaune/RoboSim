#ifndef SIMWINDOW_H
#define SIMWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QTimer>

#include "house.h"
#include "houseparser.h"
#include "vacuum.h"

namespace Ui {
class SimWindow;
}

class SimWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SimWindow(QWidget *parent = nullptr);
    ~SimWindow();
    // simwindow.h
    void startSimulation(int batteryLife, int vacuumEfficiency, int whiskerEfficiency, int speed, const QStringList &selectedAlgorithms);

    void stopSimulation();

private slots:
    void updateSimulation();
    void updateBatteryLifeLabel();
    void setSimulationSpeed(int multiplier);

private:
    void setupScene();
    void setupConnections();

    Ui::SimWindow *ui;
    QGraphicsScene *scene;
    House *house;
    Vacuum *vacuum;
    HouseParser *houseParser;
    QTimer *simulationTimer;
    int simulationSpeedMultiplier;
    bool saveHeatmapImage(QString &outImageFilename);



};

#endif // SIMWINDOW_H
