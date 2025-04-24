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
    // simWin = new SimWindow(this);
    // simWin->show();

    repWin = new report(this);
    repWin->setupSceneFromFile();
    //reportWin->mw = this;
    repWin->showMaximized();
    //QString report_fname = QFileDialog::getOpenFileName(this, "Select Report File", "C://", "Text (*.txt)");
}

void MainWindow::on_sumRep_clicked()
{

}


void MainWindow::on_runSim_clicked()
{

}
