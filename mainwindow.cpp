#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPalette>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_createFP_clicked()
{
    editWin = new EditWindow(this); // Create edit window object
    editWin->setupScene();          // Setup scene with default floorplan
    editWin->showMaximized();
}

void MainWindow::on_loadFP_clicked()
{
    editWin = new EditWindow(this); // Create edit window object
    editWin->setupSceneFromFile();  // Setup scene with user selected floorplan
    editWin->showMaximized();
}

void MainWindow::on_loadRep_clicked()
{


    repWin = new ReportWindow(this);
    repWin->setupSceneFromFile();
    repWin->showMaximized();
}

void MainWindow::on_sumRep_clicked()
{
    sumWin = new SummaryWindow(this);
    bool run = sumWin->setupSceneFromFiles();
    if (run == true){
        sumWin->showMaximized();
    }
}


void MainWindow::on_runSim_clicked()
{
    simWin = new SimWindow(this);
    simWin->show();
}
