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
    report = new Report();
}

ReportWindow::~ReportWindow()
{
    delete ui;
}

void ReportWindow::updateText(){
    QString idText = "Floorplan ID: " + report->id;
    ui->floorplanID->setText(idText);

    QString sDateText = report->sDate[0] + "/" + report->sDate[1] + "/" + report->sDate[2];
    ui->startDate->setText(sDateText);

    QString sTimeText = report->sTime[0] + ":" + report->sTime[1] + "." + report->sTime[2];
    ui->startTime->setText(sTimeText);

    QString eDateText = report->eDate[0] + "/" + report->eDate[1] + "/" + report->eDate[2];
    ui->endDate->setText(eDateText);

    QString eTimeText = report->eTime[0] + ":" + report->eTime[1] + "." + report->eTime[2];
    ui->endTime->setText(eTimeText);



    QString tsfText = QString::number(report->totalsf);
    ui->totalSqFt->setText(tsfText);


    if (selectedAlg == "random"){
        QString rTimeText = report->runs[0].runtime[0] + ":" + report->runs[0].runtime[1] + "." + report->runs[0].runtime[2];
        ui->runTime->setText(rTimeText);

        QString csfText = QString::number(report->runs[0].coversf);
        ui->coverSqFt->setText(csfText);

        QString perCleanText = QString::number(report->runs[0].coverPer) + " %";
        ui->perCleaned->setText(perCleanText);
    }
    else if (selectedAlg == "spiral"){
        QString rTimeText = report->runs[1].runtime[0] + ":" + report->runs[1].runtime[1] + "." + report->runs[1].runtime[2];
        ui->runTime->setText(rTimeText);

        QString csfText = QString::number(report->runs[1].coversf);
        ui->coverSqFt->setText(csfText);

        QString perCleanText = QString::number(report->runs[1].coverPer) + " %";
        ui->perCleaned->setText(perCleanText);
    }
    else if (selectedAlg == "snaking"){
        QString rTimeText = report->runs[2].runtime[0] + ":" + report->runs[2].runtime[1] + "." + report->runs[2].runtime[2];
        ui->runTime->setText(rTimeText);

        QString csfText = QString::number(report->runs[2].coversf);
        ui->coverSqFt->setText(csfText);

        QString perCleanText = QString::number(report->runs[2].coverPer) + " %";
        ui->perCleaned->setText(perCleanText);
    }
    else if (selectedAlg == "wallfollow"){
        QString rTimeText = report->runs[3].runtime[0] + ":" + report->runs[3].runtime[1] + "." + report->runs[3].runtime[2];
        ui->runTime->setText(rTimeText);

        QString csfText = QString::number(report->runs[3].coversf);
        ui->coverSqFt->setText(csfText);

        QString perCleanText = QString::number(report->runs[3].coverPer) + " %";
        ui->perCleaned->setText(perCleanText);
    }
    this->show();
    //this->showMaximized();
}


void ReportWindow::setupSceneFromFile(){
    file_name = QFileDialog::getOpenFileName(this, "Select Floorplan File", "C://", "text (*.txt)");
    report->parseFile(file_name);

    if (!report->runs[0].exists){
        ui->randomAlg->setEnabled(0);
    }
    else{
        ui->randomAlg->setEnabled(1);
    }

    if (!report->runs[1].exists){
        ui->spiralAlg->setEnabled(0);
    }
    else{
        ui->spiralAlg->setEnabled(1);
    }

    if (!report->runs[2].exists){
        ui->snakingAlg->setEnabled(0);
    }
    else{
        ui->snakingAlg->setEnabled(1);
    }

    if (!report->runs[3].exists){
        ui->wallfollowAlg->setEnabled(0);
    }
    else{
        ui->wallfollowAlg->setEnabled(1);
    }

    void updateText();
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


void ReportWindow::on_wallFollowAlg_clicked()
{
    selectedAlg = "wallfollow";
    updateText();
}


// void ReportWindow::on_homeButton_clicked()
// {
//     mw->showMaximized;
// }

