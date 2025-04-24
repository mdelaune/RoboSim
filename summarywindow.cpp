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

    for (int i = 0; i < reports.size(); i++){
        for (int j = 0; j < 4; j++){
            if (reports[i].runs[j].exists){
                num++;
            }
        }
    }
    return num;
}

QList<int> SummaryWindow::getShortestRun(){
    int m = 0;
    int n = 0;
    for (int i = 0; i < reports.size(); i++){
        for (int j = 0; j<4; j++){
            if (reports[i].runs[j].exists){
                if (reports[i].runs[j].runtime[0] < reports[m].runs[n].runtime[0]){
                    m = i;
                    n = j;
                }
                else if (reports[i].runs[j].runtime[0] == reports[m].runs[n].runtime[0]){
                    if (reports[i].runs[j].runtime[1] < reports[m].runs[n].runtime[1]){
                        m = i;
                        n = j;
                    }
                    else if (reports[i].runs[j].runtime[1] == reports[m].runs[n].runtime[1]){
                        if (reports[i].runs[j].runtime[2] < reports[m].runs[n].runtime[2]){
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
    for (int i = 0; i < reports.size(); i++){
        for (int j = 0; j<4; j++){
            if (reports[i].runs[j].exists){
                if (reports[i].runs[j].runtime[0] > reports[m].runs[n].runtime[0]){
                    m = i;
                    n = j;
                }
                else if (reports[i].runs[j].runtime[0] == reports[m].runs[n].runtime[0]){
                    if (reports[i].runs[j].runtime[1] > reports[m].runs[n].runtime[1]){
                        m = i;
                        n = j;
                    }
                    else if (reports[i].runs[j].runtime[1] == reports[m].runs[n].runtime[1]){
                        if (reports[i].runs[j].runtime[2] > reports[m].runs[n].runtime[2]){
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

    for (int i = 1; i < reports.size(); i++){
        for (int j = 1; j<4; j++){
            if (reports[i].runs[j].exists){
                if (reports[i].runs[j].coversf > reports[m].runs[n].coversf){
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

    for (int i = 1; i < reports.size(); i++){
        for (int j = 1; j<4; j++){
            if (reports[i].runs[j].exists){
                if (reports[i].runs[j].coversf < reports[m].runs[n].coversf){
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

    for (int i = 0; i < reports.size(); i++){
        timeAvg[0] += reports[i].totalRuntime[0].toInt(&ok);
        timeAvg[1] += reports[i].totalRuntime[1].toInt(&ok);
        timeAvg[2] += reports[i].totalRuntime[2].toInt(&ok);
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
float SummaryWindow::getAvgCover(){
    float coverAvg = 0;
    int count = 0;

    for (int i = 0; i < reports.size(); i++){
        for (int j = 0; j < reports[i].runs.size(); j++){
            coverAvg += reports[i].runs[j].coversf;
            count++;
        }
    }

    return coverAvg/count;
}
float SummaryWindow::getAvgPerCl(){
    float perAvg = 0;
    int count = 0;

    for (int i = 0; i < reports.size(); i++){
        for (int j = 0; j < reports[i].runs.size(); j++){
            perAvg += reports[i].runs[j].coverPer;
            count++;
        }
    }

    return perAvg/count *100;
}

void SummaryWindow::updateText(){

    ui->id->setText(reports[0].id);
    ui->totalSF->setText(QString::number(reports[0].totalsf));
    ui->numSims->setText(QString::number(reports.size()));
    ui->numRuns->setText(QString::number(getNumRuns()));

    QList<int> shortRun = getShortestRun();
    ui->simSR->setText(QString::number(shortRun[0]));
    ui->algSR->setText(QString::number(shortRun[1]));
    ui->valueSR->setText(reports[shortRun[0]].runs[shortRun[1]].getRuntimeString());

    QList<int> longRun = getLongestRun();
    ui->simLR->setText(QString::number(longRun[0]));
    ui->algLR->setText(QString::number(longRun[1]));
    ui->valueLR->setText(reports[longRun[0]].runs[longRun[1]].getRuntimeString());

    QList<int> mostCover = getMostCovRun();
    ui->simMC->setText(QString::number(mostCover[0]));
    ui->algMC->setText(QString::number(mostCover[1]));
    ui->valueMC->setText(QString::number(reports[mostCover[0]].runs[mostCover[1]].coversf));

    QList<int> leastCover = getLeastCovRun();
    ui->simLC->setText(QString::number(leastCover[0]));
    ui->algLC->setText(QString::number(leastCover[1]));
    ui->valueLC->setText(QString::number(reports[leastCover[0]].runs[leastCover[1]].coversf));


    QList<int> at = getAvgTime();
    ui->avgTime->setText(QString::number(at[0]) + ":" + QString::number(at[1]) + "." + QString::number(at[2]));
    ui->avgCover->setText(QString::number(getAvgCover()));
    ui->avgPerCl->setText(QString::number(getAvgPerCl())+"%");

    this->show();
}


void SummaryWindow::setupSceneFromFiles(){
    file_names = QFileDialog::getOpenFileNames(this, "Select Floorplans to Summarize", "C://", "text (*.txt)");

    for (int i = 0; i < file_names.size(); i++){
        Report rep;
        rep.parseFile(file_names[i]);
        reports.append(rep);
    }

    updateText();
}
