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
    // editWin = new edit(this);
}


void MainWindow::on_loadFP_clicked()
{

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

