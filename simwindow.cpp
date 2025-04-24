#include "simwindow.h"
#include "ui_simwindow.h"
#include <QDebug>


SimWindow::SimWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SimWindow)
{
    ui->setupUi(this);
}

SimWindow::~SimWindow()
{
    delete ui;
}

void SimWindow::startSimulation(int batteryLife, int vacuumEfficiency, int whiskerEfficiency, int speed, QStringList selectedAlgorithms)
{
    qDebug() << "Starting Simulation with Algorithms:" << selectedAlgorithms;
    qDebug() << "Battery Life:" << batteryLife;
    qDebug() << "Vacuum Efficiency:" << vacuumEfficiency;
    qDebug() << "Whisker Efficiency:" << whiskerEfficiency;
    qDebug() << "Speed:" << speed;

    // vacWin = new VacuumWindow(this);
    // vacWin->vacuum->setSelectedAlgs(selectedAlgorithms);
    // vacWin->vacuum->setBatteryLife(batteryLife);
    // vacWin->vacuum->setEfficiency(vacuumEfficiency, whiskerEfficiency);
    // vacWin->vacuum->setSpeed(speed);
    // vacWin->show();
}
