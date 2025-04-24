#include "summary.h"
#include "ui_summary.h"

#include <QDebug>
#include <QFileDialog>

summary::summary(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::summary)
{
    ui->setupUi(this);
}

summary::~summary()
{
    delete ui;
}

int summary::getNumRuns(){
    int num=0;

    for (int i = 0; i < reports.size(); i++){
        for (int j = 0; j < 4; j++){
            if (reports[i].runs[j].exists){
                num++;
            }
        }
    }
    return num;
}

QList<int> summary::getShortestRun(){
    int m = 0;
    int n = 0;
    for (int i = 0; i < reports.size(); i++){
        for (int j = 0; j<4; j++){
            if (reports[i].runs[j].exists){
                if (reports[i].runs[j].time[0] < reports[m].runs[n].time[0]){
                    m = i;
                    n = j;
                }
                else if (reports[i].runs[j].time[0] == reports[m].runs[n].time[0]){
                    if (reports[i].runs[j].time[1] < reports[m].runs[n].time[1]){
                        m = i;
                        n = j;
                    }
                    else if (reports[i].runs[j].time[1] == reports[m].runs[n].time[1]){
                        if (reports[i].runs[j].time[2] < reports[m].runs[n].time[2]){
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

QList<int> summary::getLongestRun(){
    int m = 0;
    int n = 0;
    for (int i = 0; i < reports.size(); i++){
        for (int j = 0; j<4; j++){
            if (reports[i].runs[j].exists){
                if (reports[i].runs[j].time[0] > reports[m].runs[n].time[0]){
                    m = i;
                    n = j;
                }
                else if (reports[i].runs[j].time[0] == reports[m].runs[n].time[0]){
                    if (reports[i].runs[j].time[1] > reports[m].runs[n].time[1]){
                        m = i;
                        n = j;
                    }
                    else if (reports[i].runs[j].time[1] == reports[m].runs[n].time[1]){
                        if (reports[i].runs[j].time[2] > reports[m].runs[n].time[2]){
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
QList<int> summary::getMostCovRun(){
    int m = 0;
    int n = 0;

    for (int i = 1; i < reports.size(); i++){
        for (int j = 1; j<4; j++){
            if (reports[i].runs[j].exists){
                if (reports[i].runs[j].coverSF > reports[m].runs[n].coverSF){
                    m = i;
                    n = j;
                }
            }
        }
    }

    QList<int> ans = {m, n};
    return ans;

}
QList<int> summary::getLeastCovRun(){
    int m = 0;
    int n = 0;

    for (int i = 1; i < reports.size(); i++){
        for (int j = 1; j<4; j++){
            if (reports[i].runs[j].exists){
                if (reports[i].runs[j].coverSF < reports[m].runs[n].coverSF){
                    m = i;
                    n = j;
                }
            }
        }
    }

    QList<int> ans = {m, n};
    return ans;
}

QList<int> summary::getAvgTime(){
    QList<int> timeAvg = {0,0,0};
    bool ok;

    for (int i = 0; i < reports.size(); i++){
        for (int j = 0; j < reports[i].runs.size(); j++){
            if (reports[i].runs[j].exists){
                timeAvg[0] += reports[i].runs[j].time[0].toInt(&ok);
                timeAvg[1] += reports[i].runs[j].time[1].toInt(&ok);
                timeAvg[2] += reports[i].runs[j].time[2].toInt(&ok);
            }
        }
    }

    timeAvg[0] /= reports.size();
    timeAvg[1] /= reports.size();
    timeAvg[2] /= reports.size();

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
float summary::getAvgCover(){
    float coverAvg = 0;
    int count = 0;

    bool ok;

    for (int i = 0; i < reports.size(); i++){
        for (int j = 0; j < reports[i].runs.size(); j++){
            coverAvg += reports[i].runs[j].coverSF.toFloat(&ok);
            count++;
        }
    }

    return coverAvg/count;
}
float summary::getAvgPerCl(){
    float perAvg = 0;
    int count = 0;
    bool ok;

    for (int i = 0; i < reports.size(); i++){
        for (int j = 0; j < reports[i].runs.size(); j++){
            perAvg += reports[i].runs[j].coverPer.toFloat(&ok);
            count++;
        }
    }

    return perAvg/count *100;
}

void summary::updateText(){

    ui->id->setText(reports[0].id);
    ui->totalSF->setText(reports[0].totalSF);
    ui->numSims->setText(QString::number(reports.size()));
    ui->numRuns->setText(QString::number(getNumRuns()));

    QList<int> shortRun = getShortestRun();
    ui->simSR->setText(QString::number(shortRun[0]));
    ui->algSR->setText(QString::number(shortRun[1]));
    ui->valueSR->setText(reports[shortRun[0]].runs[shortRun[1]].getTimeString(reports[shortRun[0]].runs[shortRun[1]].time));

    QList<int> longRun = getLongestRun();
    ui->simLR->setText(QString::number(longRun[0]));
    ui->algLR->setText(QString::number(longRun[1]));
    ui->valueLR->setText(reports[longRun[0]].runs[longRun[1]].getTimeString(reports[longRun[0]].runs[longRun[1]].time));

    QList<int> mostCover = getMostCovRun();
    ui->simMC->setText(QString::number(mostCover[0]));
    ui->algMC->setText(QString::number(mostCover[1]));
    ui->valueMC->setText(reports[mostCover[0]].runs[mostCover[1]].coverSF);

    QList<int> leastCover = getLeastCovRun();
    ui->simLC->setText(QString::number(leastCover[0]));
    ui->algLC->setText(QString::number(leastCover[1]));
    ui->valueLC->setText(reports[leastCover[0]].runs[leastCover[1]].coverSF);


    QList<int> at = getAvgTime();
    ui->avgTime->setText(QString::number(at[0]) + ":" + QString::number(at[1]) + "." + QString::number(at[2]));
    ui->avgCover->setText(QString::number(getAvgCover()));
    ui->avgPerCl->setText(QString::number(getAvgPerCl())+"%");

    this->show();
}


void summary::setupSceneFromFiles(){
    file_names = QFileDialog::getOpenFileNames(this, "Select Floorplans to Summarize", "C://", "text (*.txt)");

    for (int i = 0; i < file_names.size(); i++){
        RunData rep;
        rep.parseFile(file_names[i]);
        reports.append(rep);
    }

    updateText();
}
