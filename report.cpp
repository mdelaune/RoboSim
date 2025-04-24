#include "report.h"
#include "ui_report.h"

#include <QFileDialog>
#include <cmath> // for floor function

#include <QDebug>
#include <QFileDialog>
#include <QRegularExpression>

Run::Run(){}
QString Run::getTimeString(QStringList time){
    return time[0] + ":" + time[1] + "." + time[2];
}

RunData::RunData() {}

void RunData::setEnd(){
    bool ok;
    int sHour = sTime[0].toInt(&ok, 10);
    int sMin = sTime[1].toInt(&ok, 10);
    int sSec = sTime[2].toInt(&ok, 10);

    int rHour=0;
    int rMin = 0;
    int rSec = 0;

    for (int i = 0; i <4; i++){
        if (runs[i].exists == true){
            rHour += runs[i].time[0].toInt(&ok, 10);
            rMin += runs[i].time[1].toInt(&ok, 10);
            rSec += runs[i].time[2].toInt(&ok, 10);
        }
    }

    int eDay = sDate[1].toInt(&ok, 10);
    int eMon = sDate[0].toInt(&ok, 10);
    int eYear = sDate[2].toInt(&ok, 10);


    int eHour = sHour + rHour;
    int eMin = sMin + rMin;
    int eSec = sSec + rSec;

    if (eSec > 59){
        int m = eSec % 60;
        int r = eSec / 60;
        eSec = m;
        eMin = eMin + r + 1;
    }
    if (eMin > 59){
        int m = eMin % 60;
        int r = eHour/60;
        eMin = m;
        eHour = eHour + r + 1;
    }
    if (eHour > 23){
        int m = eHour % 24;
        int r = eHour/24;
        eHour = m;
        eDay = eDay + r + 1;
    }
    if ((eMon == 1 or
         eMon == 3 or
         eMon == 5 or
         eMon == 7 or
         eMon == 8 or
         eMon == 10 or
         eMon == 12) and (eDay > 31))
    {
        eMon++;
        eDay =1;
        if (eMon == 13){
            eMon = 1;
            eYear++;
        }
    }
    else if((eMon == 4 or
              eMon == 6 or
              eMon == 9 or
              eMon == 11) and (eDay > 30))
    {
        eMon++;
        eDay =1;
    }
    else if (eMon == 2 and eDay > 28){
        eMon ++;
        eDay =1;
    }

    eTime.append(QString::number(eHour));
    eTime.append(QString::number(eMin));
    eTime.append(QString::number(eSec));

    eDate.append(QString::number(eMon));
    eDate.append(QString::number(eDay));
    eDate.append(QString::number(eYear));

    // totalRuntime[0] = QString::number(rHour);
    // totalRuntime[1] = QString::number(rMin);
    // totalRuntime[2] = QString::number(rSec);

}

void RunData::parseFile(QString file_name){
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file";
        return;
    }

    QStringList filedata[7];
    int n = 0;


    QTextStream ts(&file);
    while (!ts.atEnd()){
        QString line = ts.readLine();
        QStringList data = line.split(QRegularExpression("\\W+"), Qt::SkipEmptyParts);
        filedata[n] = data;
        n +=1;
    }

    bool ok;
    id = filedata[0][0];

    sTime.append(filedata[1][0]); //.toFloat(&ok) * 60;
    sTime.append(filedata[1][1]); //.toFloat(&ok);
    sTime.append(filedata[1][2]); //.toFloat(&ok)/100;

    sDate.append(filedata[1][3]); //.toInt(&ok));
    sDate.append(filedata[1][4]); //.toInt(&ok));
    sDate.append(filedata[1][5]); //.toInt(&ok));

    totalSF = (filedata[2][0] + "." + filedata[2][1]); //.toFloat(&ok);

    for (int i = 3; i < 7; i++){
        Run run;
        run.alg = filedata[i][0];

        if (filedata[i][1] != "0"){
            run.time.append(filedata[i][1]);
            run.time.append(filedata[i][2]);
            run.time.append(filedata[i][3]);

            run.coverSF = filedata[i][4] + "." + filedata[i][5];
            run.coverPer = QString::number(run.coverSF.toFloat(&ok)/totalSF.toFloat(&ok));
            run.exists = true;
        }
        else{
            run.exists = false;
        }

        runs.append(run);
    }
    setEnd();
    file.close();
}



report::report(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::report)
{
    ui->setupUi(this);
    data = new RunData();
}

report::~report()
{
    delete ui;
}



void report::updateText(){
    QString idText = "Floorplan ID: " + data->id;
    ui->floorplanID->setText(idText);

    QPixmap map(":/Images/Images/HeatMapEx");
    ui->heatMap->setScene(new QGraphicsScene(this));
    ui->heatMap->scene()->addPixmap(map.scaled(600, 400, Qt::KeepAspectRatio));

    QString sDateText = data->sDate[0] +"/" + data->sDate[1]+ "/" + data->sDate[2];//QString::number(data->sDate[0]) + "/" + QString::number(data->sDate[1]) + "/" + QString::number(data->sDate[2]);
    ui->startDate->setText(sDateText);

    QString sTimeText = data->sTime[0] + ":" + data->sTime[1] + "." + data->sTime[2]; //TimegetTimeString(data->sTime);
    ui->startTime->setText(sTimeText);

    QString eDateText = data->eDate[0] +"/" + data->eDate[1]+ "/" + data->eDate[2]; //QString::number(data->eDate[0]) + "/" + QString::number(data->eDate[1]) + "/" + QString::number(data->eDate[2]);
    ui->endDate->setText(eDateText);

    QString eTimeText = data->eTime[0] + ":" + data->eTime[1] + "." + data->eTime[2]; //data->getTimeString(data->eTime);
    ui->endTime->setText(eTimeText);

    QString tsfText = data->totalSF;
    ui->totalSqFt->setText(tsfText);


    if (selectedAlg == "random"){
        ui->runTime->setText(data->runs[0].getTimeString(data->runs[0].time));
        ui->coverSqFt->setText(data->runs[0].coverSF);
        ui->perCleaned->setText(data->runs[0].coverPer + " %");
    }
    else if (selectedAlg == "spiral"){
        ui->runTime->setText(data->runs[1].getTimeString(data->runs[1].time));
        ui->coverSqFt->setText(data->runs[1].coverSF);
        ui->perCleaned->setText(data->runs[1].coverPer + " %");
    }
    else if (selectedAlg == "snaking"){
        ui->runTime->setText(data->runs[2].getTimeString(data->runs[2].time));
        ui->coverSqFt->setText(data->runs[2].coverSF);
        ui->perCleaned->setText(data->runs[2].coverPer + " %");
    }
    else if (selectedAlg == "wallfollow"){
        ui->runTime->setText(data->runs[3].getTimeString(data->runs[3].time));
        ui->coverSqFt->setText(data->runs[3].coverSF);
        ui->perCleaned->setText(data->runs[3].coverPer + " %");
    }
    this->show();
    //this->showMaximized();
}


void report::setupSceneFromFile(){
    file_name = QFileDialog::getOpenFileName(this, "Select Floorplan File", "C://", "text (*.txt)");
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
}


void report::on_randomAlg_clicked()
{
    selectedAlg = "random";
    updateText();
}


void report::on_spiralAlg_clicked()
{
    selectedAlg = "spiral";
    updateText();
}


void report::on_snakingAlg_clicked()
{
    selectedAlg = "snaking";
    updateText();
}


void report::on_wallfollowAlg_clicked()
{
    selectedAlg = "wallfollow";
    updateText();
}

