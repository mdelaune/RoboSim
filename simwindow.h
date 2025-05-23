#ifndef SIMWINDOW_H
#define SIMWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QTimer>
#include "vacuum.h"
#include "house.h"
#include "rundata.h"
#include "reportwindow.h"


namespace Ui {
class SimWindow;
}

class SimWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SimWindow(House* housePtr, QWidget *parent = nullptr);
    ~SimWindow();

    void startSimulation(int batteryLife, int vacuumEfficiency, int whiskerEfficiency, int speed, QStringList selectedAlgorithms);
    void stopSimulation();

    House* house;
    QString house_path;
    QString save_path; // for report and heatmap

private slots:
    void updateSimulation();
    void oneSpeedPushed();
    void fiveSpeedPushed();
    void fiftySpeedPushed();
    void updateBatteryLifeLabel();
    void setSimulationSpeed(int multiplier);

    void on_stopButton_clicked();
    void startNextRun();
    void resetScene();

private:
    Ui::SimWindow *ui;

    QGraphicsScene *scene;
    Vacuum *vacuum;
    QTimer *simulationTimer;
    int simulationSpeedMultiplier;

    RunData *simData;
    QString writeReport();
    void writeRun();

    int batteryLife;
    int vacuumEfficiency;
    int whiskerEfficiency;
    int speed;
    QStringList pendingAlgorithms;
    int currentAlgorithmIndex = 0;
    bool allRunsCompleted = false;

    bool saveHeatmapImage(QString &outImageFilename);


    ReportWindow *repWin;
};

#endif // SIMWINDOW_H
