#include "rundata.h"

#include <QDebug>
#include <QFileDialog>
#include <QRegularExpression>
#include <QRandomGenerator>

Run::Run(){

}

QString Run::getTimeString(QStringList time){
    return time[0] + ":" + time[1] + "." + time[2];
}

RunData::RunData(){}

void RunData::setEndValues(){
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

    // totalRuntime.append(QString::number(rHour));
    // totalRuntime.append(QString::number(rMin));
    // totalRuntime.append(QString::number(rSec));
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
    //qDebug() << filedata[0];
    id = filedata[0][0];

    sTime.append(filedata[1][0]);
    sTime.append(filedata[1][1]);
    sTime.append(filedata[1][2]);

    sDate.append(filedata[1][3]);
    sDate.append(filedata[1][4]);
    sDate.append(filedata[1][5]);

    //QString totalString = filedata[2][0] + "." + filedata[2][1];
    totalSF = filedata[2][0]; //totalString;

    for (int i = 3; i < 7; i++){
        Run run;
        run.alg = filedata[i][0];

        if (filedata[i].size() > 3){
            run.time.append(filedata[i][1]);
            run.time.append(filedata[i][2]);
            run.time.append(filedata[i][3]);
            run.coverSF = filedata[i][4] + "." + filedata[i][5];
            run.coverPer = QString::number(run.coverSF.toFloat(&ok)/totalSF.toFloat(&ok));
            run.heatmapPath = filedata[i][5];
            for (int j = 6; j < filedata[i].size(); j++){
                run.heatmapPath.append(filedata[i][j]);
            }
            run.exists = true;
        }
        else{
            run.exists = false;
        }

        runs.append(run);
    }

    file.close();
    setEndValues();
}

void RunData::setNewID(){
    report_id = QRandomGenerator::global()->bounded(10000, 99999);
}
