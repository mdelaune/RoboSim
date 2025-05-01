#include "summarywindow.h"
#include "ui_summarywindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

SummaryWindow::SummaryWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SummaryWindow)
{
    ui->setupUi(this);

}

SummaryWindow::~SummaryWindow()
{
    delete ui;
}



int SummaryWindow::getNumRuns(){
    int num=0;

    for (int i = 0; i < data.size(); i++){
        for (int j = 0; j < 4; j++){
            if (data[i].runs[j].exists){
                num++;
            }
        }
    }
    return num;
}

QList<int> SummaryWindow::getShortestRun(){
    int m = 0;
    int n = 0;
    for (int i = 0; i < data.size(); i++){
        for (int j = 1; j<4; j++){
            if (data[i].runs[j].exists){
                if (data[i].runs[j].time[0] < data[m].runs[n].time[0]){
                    m = i;
                    n = j;
                }
                else if (data[i].runs[j].time[0] == data[m].runs[n].time[0]){
                    if (data[i].runs[j].time[1] < data[m].runs[n].time[1]){
                        m = i;
                        n = j;
                    }
                    else if (data[i].runs[j].time[1] == data[m].runs[n].time[1]){
                        if (data[i].runs[j].time[2] < data[m].runs[n].time[2]){
                           m = i;
                           n = j;
                        }
                    }
                }
            }
        }
    }

    QList<int> ans = {m, n};
    return ans;
}

QList<int> SummaryWindow::getLongestRun(){
    int m = 0;
    int n = 0;
    for (int i = 0; i < data.size(); i++){
        for (int j = 1; j<4; j++){
            if (data[i].runs[j].exists){
                if (data[i].runs[j].time[0] > data[m].runs[n].time[0]){
                    m = i;
                    n = j;
                }
                else if (data[i].runs[j].time[0] == data[m].runs[n].time[0]){
                    if (data[i].runs[j].time[1] > data[m].runs[n].time[1]){
                        m = i;
                        n = j;
                    }
                    else if (data[i].runs[j].time[1] == data[m].runs[n].time[1]){
                        if (data[i].runs[j].time[2] > data[m].runs[n].time[2]){
                            m = i;
                            n = j;
                        }
                    }
                }
            }
        }
    }

    QList<int> ans = {m, n};
    return ans;
}
QList<int> SummaryWindow::getMostCovRun(){
    int m = 0;
    int n = 0;
    bool ok;

    for (int i = 0; i < data.size(); i++){
        for (int j = 1; j<data[i].runs.size(); j++){
            if (data[i].runs[j].exists){
                if (data[i].runs[j].coverSF.toFloat(&ok) > data[m].runs[n].coverSF.toFloat(&ok)){
                    m = i;
                    n = j;
                }
            }
        }
    }

    QList<int> ans = {m, n};
    return ans;

}
QList<int> SummaryWindow::getLeastCovRun(){
    int m = 0;
    int n = 0;
    bool ok;

    for (int i = 0; i < data.size(); i++){
        for (int j = 1; j<data[i].runs.size(); j++){
            if (data[i].runs[j].exists){
                if (data[i].runs[j].coverSF.toFloat(&ok) < data[m].runs[n].coverSF.toFloat(&ok)){
                    m = i;
                    n = j;
                }
            }
        }
    }

    QList<int> ans = {m, n};
    return ans;
}

QList<int> SummaryWindow::getAvgTime(){
    QList<int> timeAvg= {0,0,0};
    bool ok;

    for (int i = 1; i < data.size(); i++){
        for (int j = 1; j < data[i].runs.size(); j++){
            if (data[i].runs[j].exists){
                timeAvg[0] += (data[i].runs[j].time[0].toInt(&ok));
                timeAvg[1] += (data[i].runs[j].time[1].toInt(&ok));
                timeAvg[2] += (data[i].runs[j].time[2].toInt(&ok));
            }
        }
    }

    timeAvg[0] /= data.size();
    timeAvg[1] /= data.size();
    timeAvg[2] /= data.size();

    if (timeAvg[2] > 59){
        timeAvg[1] +=1;
        timeAvg[2] = timeAvg[2] % 60;
    }
    if (timeAvg[1] > 59){
        timeAvg[0] +=1;
        timeAvg[1] = timeAvg[1] % 60;
    }

    return timeAvg;
}
float SummaryWindow::getAvgCover(){
    float coverAvg = 0;
    int count = 0;
    bool ok;

    for (int i = 0; i < data.size(); i++){
        for (int j = 0; j < data[i].runs.size(); j++){
            if (data[i].runs[j].exists){
                coverAvg += data[i].runs[j].coverSF.toFloat(&ok);
                count++;
            }
        }
    }

    return coverAvg/count;
}
float SummaryWindow::getAvgPerCl(){
    float perAvg = 0;
    int count = 0;
    bool ok;

    for (int i = 0; i < data.size(); i++){
        for (int j = 0; j < data[i].runs.size(); j++){
            if (data[i].runs[j].exists){
                perAvg += data[i].runs[j].coverPer.toFloat(&ok);
                count++;
            }
        }
    }

    return perAvg/count *100;
}

void SummaryWindow::updateText(){

    ui->Title->setText("Floorplan " + data[0].id + " Summary");
    ui->id->setText(data[0].id);
    ui->totalSF->setText(data[0].totalSF);
    ui->numSims->setText(QString::number(data.size()));
    ui->numRuns->setText(QString::number(getNumRuns()));

    QList<int> shortRun = getShortestRun();
    ui->simSR->setText(QString::number(shortRun[0]));
    if (data[shortRun[0]].runs[shortRun[1]].alg == "random"){
        ui->algSR->setText("Random");
    }
    else if (data[shortRun[0]].runs[shortRun[1]].alg == "spiral"){
        ui->algSR->setText("Spiral");
    }
    else if (data[shortRun[0]].runs[shortRun[1]].alg == "snaking"){
        ui->algSR->setText("Snaking");
    }
    else if (data[shortRun[0]].runs[shortRun[1]].alg == "wallfollow"){
        ui->algSR->setText("Wall Follow");
    }
    ui->valueSR->setText(data[shortRun[0]].runs[shortRun[1]].getTimeString(data[shortRun[0]].runs[shortRun[1]].time));

    QList<int> longRun = getLongestRun();
    ui->simLR->setText(QString::number(longRun[0]));
    if (data[longRun[0]].runs[longRun[1]].alg == "random"){
        ui->algLR->setText("Random");
    }
    else if (data[longRun[0]].runs[longRun[1]].alg == "spiral"){
        ui->algLR->setText("Spiral");
    }
    else if (data[longRun[0]].runs[longRun[1]].alg == "snaking"){
        ui->algLR->setText("Snaking");
    }
    else if (data[longRun[0]].runs[longRun[1]].alg == "wallfollow"){
        ui->algLR->setText("Wall Follow");
    }
    ui->valueLR->setText(data[longRun[0]].runs[longRun[1]].getTimeString(data[longRun[0]].runs[longRun[1]].time));

    QList<int> mostCover = getMostCovRun();
    ui->simMC->setText(QString::number(mostCover[0]));
    if (data[mostCover[0]].runs[mostCover[1]].alg == "random"){
        ui->algMC->setText("Random");
    }
    else if (data[mostCover[0]].runs[mostCover[1]].alg == "spiral"){
        ui->algMC->setText("Spiral");
    }
    else if (data[mostCover[0]].runs[mostCover[1]].alg == "snaking"){
        ui->algMC->setText("Snaking");
    }
    else if (data[mostCover[0]].runs[mostCover[1]].alg == "wallfollow"){
        ui->algMC->setText("Wall Follow");
    }
    ui->valueMC->setText(data[mostCover[0]].runs[mostCover[1]].coverSF);

    QList<int> leastCover = getLeastCovRun();
    ui->simLC->setText(QString::number(leastCover[0]));
    if (data[leastCover[0]].runs[leastCover[1]].alg == "random"){
        ui->algLC->setText("Random");
    }
    else if (data[leastCover[0]].runs[leastCover[1]].alg == "spiral"){
        ui->algLC->setText("Spiral");
    }
    else if (data[leastCover[0]].runs[leastCover[1]].alg == "snaking"){
        ui->algLC->setText("Snaking");
    }
    else if (data[leastCover[0]].runs[leastCover[1]].alg == "wallfollow"){
        ui->algLC->setText("Wall Follow");
    }
    ui->valueLC->setText(data[leastCover[0]].runs[leastCover[1]].coverSF);


    QList<int> at = getAvgTime();
    ui->avgTime->setText(QString::number(at[0]) + ":" + QString::number(at[1]) + "." + QString::number(at[2]));
    ui->avgCover->setText(QString::number(getAvgCover()));
    ui->avgPerCl->setText(QString::number(getAvgPerCl())+"%");

    QStringList keyString;
    for (int i = 0; i < data.size(); i++){
        keyString.append(QString::number(i) + "-> " + data[i].sDate[0] + "/" + data[i].sDate[1] + "/" + data[i].sDate[2]);
    }

    ui->key->setText(keyString.join("\n"));

    this->show();
}


bool SummaryWindow::setupSceneFromFiles(){
    file_names = QFileDialog::getOpenFileNames(this, "Select Floorplans to Summarize", "C://", "text (*.txt)");
    if (file_names.size() != 0){
        QString matchID = "";
        for (int i = 0; i < file_names.size(); i++){
            RunData rep;
            rep.parseFile(file_names[i]);
            if (matchID == ""){
                matchID = rep.id;
            }
            if (matchID == rep.id){
                data.append(rep);
            }
            else{
                QMessageBox errorMsg;
                errorMsg.setText("Selected files do not belong to same floorplan. Please try again.");
                errorMsg.exec();
                return false;
            }
        }
        updateText();
        return true;
    }
    else{
        return false;
    }
}
