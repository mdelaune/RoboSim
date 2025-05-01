#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPalette>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFocus();
    ui->runSim->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_createFP_clicked()
{
    editWin = new EditWindow(this); // Create edit window object
    editWin->setupScene();          // Setup scene with default floorplan
    ui->activePlan->setText("Current Floorplan ID: " + QString::number(editWin->house->getFloorplanId()));
    editWin->showMaximized();
    //ui->runSim->setEnabled(true);
    floorplanCreated = true;
    updateRunSimButtonState();
}

void MainWindow::on_loadFP_clicked()
{
    editWin = new EditWindow(this); // Create edit window object

    bool fileSelected = editWin->setupSceneFromFile();  // Setup scene with user selected floorplan
    if (fileSelected){
        ui->activePlan->setText("Current Floorplan ID: " + QString::number(editWin->house->getFloorplanId()));
        editWin->showMaximized();
        //ui->runSim->setEnabled(true);
        floorplanCreated = true;
        updateRunSimButtonState();
    }
}

void MainWindow::on_loadRep_clicked()
{
    repWin = new ReportWindow(this);
    bool fileSelected = repWin->setupSceneFromFile();
        if (fileSelected){
        repWin->showMaximized();
    }
}

void MainWindow::on_sumRep_clicked()
{
    sumWin = new SummaryWindow(this);
    bool validFilesSelected = sumWin->setupSceneFromFiles();
    if (validFilesSelected == true){
        sumWin->showMaximized();
    }
}

void MainWindow::on_robSet_clicked()
{
    if (editWin)
    {
        setWin = new SettingsWindow(this);
        setWin->floorCovering = editWin->house->getFloorCovering();

        qDebug() << "FloorCovering: " << setWin->floorCovering;
        setWin->showMaximized();
        robotSetup = true;
        updateRunSimButtonState();
        connect(setWin, &SettingsWindow::settingsUpdated, this, &MainWindow::onSettingsUpdated);
    }
}

void MainWindow::on_runSim_clicked()
{
    if (editWin && setWin){
        simWin = new SimWindow(editWin->house, this);
        simWin->house_path = editWin->house->get_floorplanName();
        qDebug() << "house path: " << simWin->house_path;
        simWin->startSimulation(batteryLife, vacuumEfficiency, whiskerEfficiency, speed, selectedAlgorithms);
        simWin->showMaximized();
    }
}

void MainWindow::updateRunSimButtonState()
{
    if (floorplanCreated && robotSetup)
    {
        ui->runSim->setEnabled(true);
    }
}

void MainWindow::onSettingsUpdated(int batteryLife, int vacuumEfficiency, int whiskerEfficiency, int speed, QStringList selectedAlgorithms)
{
    this->batteryLife = batteryLife;
    this->vacuumEfficiency = vacuumEfficiency;
    this->whiskerEfficiency = whiskerEfficiency;
    this->speed = speed;
    this->selectedAlgorithms = selectedAlgorithms;
}

