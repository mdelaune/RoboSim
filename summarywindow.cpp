#include "summarywindow.h"
#include "ui_summarywindow.h"

#include <QDebug>
#include <QFileDialog>

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
        for (int j = 0; j<4; j++){
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
        for (int j = 0; j<4; j++){
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

    for (int i = 1; i < data.size(); i++){
        for (int j = 1; j<4; j++){
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

    for (int i = 1; i < data.size(); i++){
        for (int j = 1; j<4; j++){
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
    QList<int> timeAvg = {0,0,0};
    bool ok;

    for (int i = 0; i < data.size(); i++){
        timeAvg[0] += data[i].totalRuntime[0].toInt(&ok);
        timeAvg[1] += data[i].totalRuntime[1].toInt(&ok);
        timeAvg[2] += data[i].totalRuntime[2].toInt(&ok);
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
            coverAvg += data[i].runs[j].coverSF.toFloat(&ok);
            count++;
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
            perAvg += data[i].runs[j].coverPer.toFloat(&ok);
            count++;
        }
    }

    return perAvg/count *100;
}

void SummaryWindow::updateText(){

    ui->id->setText(data[0].id);
    ui->totalSF->setText(data[0].totalSF);
    ui->numSims->setText(QString::number(data.size()));
    ui->numRuns->setText(QString::number(getNumRuns()));

    QList<int> shortRun = getShortestRun();
    ui->simSR->setText(QString::number(shortRun[0]));
    ui->algSR->setText(QString::number(shortRun[1]));
    ui->valueSR->setText(data[shortRun[0]].runs[shortRun[1]].getTimeString(data[shortRun[0]].runs[shortRun[1]].time));

    QList<int> longRun = getLongestRun();
    ui->simLR->setText(QString::number(longRun[0]));
    ui->algLR->setText(QString::number(longRun[1]));
    ui->valueLR->setText(data[longRun[0]].runs[longRun[1]].getTimeString(data[longRun[0]].runs[longRun[1]].time));

    QList<int> mostCover = getMostCovRun();
    ui->simMC->setText(QString::number(mostCover[0]));
    ui->algMC->setText(QString::number(mostCover[1]));
    ui->valueMC->setText(data[mostCover[0]].runs[mostCover[1]].coverSF);

    QList<int> leastCover = getLeastCovRun();
    ui->simLC->setText(QString::number(leastCover[0]));
    ui->algLC->setText(QString::number(leastCover[1]));
    ui->valueLC->setText(data[leastCover[0]].runs[leastCover[1]].coverSF);


    QList<int> at = getAvgTime();
    ui->avgTime->setText(QString::number(at[0]) + ":" + QString::number(at[1]) + "." + QString::number(at[2]));
    ui->avgCover->setText(QString::number(getAvgCover()));
    ui->avgPerCl->setText(QString::number(getAvgPerCl())+"%");

    this->show();
}


void SummaryWindow::setupSceneFromFiles(){
    file_names = QFileDialog::getOpenFileNames(this, "Select Floorplans to Summarize", "C://", "text (*.txt)");

    for (int i = 0; i < file_names.size(); i++){
        RunData rep;
        rep.parseFile(file_names[i]);
        data.append(rep);
    }

    updateText();
}
