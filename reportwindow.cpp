#include "reportwindow.h"
#include "ui_reportwindow.h"

#include <QDebug>
#include <QStyleFactory>
#include <QFileDialog>
#include <QStringList>
#include <QRegularExpression>

ReportWindow::ReportWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ReportWindow)
{
    ui->setupUi(this);
    data = new RunData();
}

ReportWindow::~ReportWindow()
{
    delete ui;
}

void ReportWindow::updateText(){
    QString idText = "Floorplan ID: " + data->id;
    ui->floorplanID->setText(idText);


    QString sDateText = data->sDate[0] +"/" + data->sDate[1]+ "/" + data->sDate[2];//QString::number(data->sDate[0]) + "/" + QString::number(data->sDate[1]) + "/" + QString::number(data->sDate[2]);
    ui->startDate->setText(sDateText);

    QString sTimeText = data->sTime[0] + ":" + data->sTime[1] + "." + data->sTime[2]; //TimegetTimeString(data->sTime);
    ui->startTime->setText(sTimeText);

    // QString eDateText = data->eDate[0] +"/" + data->eDate[1]+ "/" + data->eDate[2]; //QString::number(data->eDate[0]) + "/" + QString::number(data->eDate[1]) + "/" + QString::number(data->eDate[2]);
    // ui->endDate->setText(eDateText);

    // QString eTimeText = data->eTime[0] + ":" + data->eTime[1] + "." + data->eTime[2]; //data->getTimeString(data->eTime);
    // ui->endTime->setText(eTimeText);

    //QString tsfText = data->totalSF;
    ui->totalSqFt->setText(data->totalSF);
    ui->openSqFt->setText(data->openSF);

    if (selectedAlg == "random"){
        ui->runTime->setText(data->runs[0].getTimeString(data->runs[0].time));
        ui->cleanSqFt->setText(data->runs[0].coverSF);
        ui->perCleaned->setText(data->runs[0].coverPer + " %");
        QPixmap map(data->runs[0].heatmapPath);
        ui->heatMap->setScene(new QGraphicsScene(this));
        ui->heatMap->scene()->addPixmap(map.scaled(600, 400, Qt::KeepAspectRatio));
    }
    else if (selectedAlg == "spiral"){
        ui->runTime->setText(data->runs[1].getTimeString(data->runs[1].time));
        ui->cleanSqFt->setText(data->runs[1].coverSF);
        ui->perCleaned->setText(data->runs[1].coverPer + " %");
        QPixmap map(data->runs[1].heatmapPath);
        ui->heatMap->setScene(new QGraphicsScene(this));
        ui->heatMap->scene()->addPixmap(map.scaled(600, 400, Qt::KeepAspectRatio));
    }
    else if (selectedAlg == "snaking"){
        ui->runTime->setText(data->runs[2].getTimeString(data->runs[2].time));
        ui->cleanSqFt->setText(data->runs[2].coverSF);
        ui->perCleaned->setText(data->runs[2].coverPer + " %");
        QPixmap map(data->runs[2].heatmapPath);
        ui->heatMap->setScene(new QGraphicsScene(this));
        ui->heatMap->scene()->addPixmap(map.scaled(600, 400, Qt::KeepAspectRatio));
    }
    else if (selectedAlg == "wallfollow"){
        ui->runTime->setText(data->runs[3].getTimeString(data->runs[3].time));
        ui->cleanSqFt->setText(data->runs[3].coverSF);
        ui->perCleaned->setText(data->runs[3].coverPer + " %");
        QPixmap map(data->runs[3].heatmapPath);
        ui->heatMap->setScene(new QGraphicsScene(this));
        ui->heatMap->scene()->addPixmap(map.scaled(600, 400, Qt::KeepAspectRatio));
    }


    this->show();
    //this->showMaximized();
}


bool ReportWindow::setupSceneFromFile(){
    file_name = QFileDialog::getOpenFileName(this, "Select Floorplan File", "C://", "text (*.txt)");
    QFile fileTest(file_name);
    if (fileTest.open(QIODevice::ReadOnly)){
        data->parseFile(file_name);

        if (!data->runs[0].exists){
            ui->randomAlg->setEnabled(0);
        }
        else{
            ui->randomAlg->setEnabled(1);
        }

        if (!data->runs[1].exists){
            ui->spiralAlg->setEnabled(0);
        }
        else{
            ui->spiralAlg->setEnabled(1);
        }

        if (!data->runs[2].exists){
            ui->snakingAlg->setEnabled(0);
        }
        else{
            ui->snakingAlg->setEnabled(1);
        }

        if (!data->runs[3].exists){
            ui->wallfollowAlg->setEnabled(0);
        }
        else{
            ui->wallfollowAlg->setEnabled(1);
        }
        updateText();
        return true;
    }
    else{
        return false;
    }
}


void ReportWindow::on_randomAlg_clicked()
{
    selectedAlg = "random";
    updateText();
}


void ReportWindow::on_spiralAlg_clicked()
{
    selectedAlg = "spiral";
    updateText();
}


void ReportWindow::on_snakingAlg_clicked()
{
    selectedAlg = "snaking";
    updateText();
}


void ReportWindow::on_wallfollowAlg_clicked()
{
    selectedAlg = "wallfollow";
    updateText();
}


