#include "mainwindow.h"
#include "./ui_mainwindow.h"

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
    editWin = new edit(this); // Create edit window object
    editWin->setupScene();          // Setup scene with default floorplan
    editWin->showMaximized();
}

void MainWindow::on_loadFP_clicked()
{
    editWin = new edit(this); // Create edit window object
    editWin->setupSceneFromFile();  // Setup scene with user selected floorplan
    editWin->showMaximized();
}

void MainWindow::on_loadRep_clicked()
{
    repWin = new report(this);
    repWin->setupSceneFromFile();
    repWin->show();

}

void MainWindow::on_sumRep_clicked()
{

}
