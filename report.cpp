#include "report.h"

#include <QDebug>
#include <QFileDialog>
#include <QRegularExpression>

Report::Report() {

}

void Report::setEndValues(){
    bool ok;
    int sHour = sTime[0].toInt(&ok, 10);
    int sMin = sTime[1].toInt(&ok, 10);
    int sSec = sTime[2].toInt(&ok, 10);

    int rHour=0;
    int rMin = 0;
    int rSec = 0;

    for (int i = 0; i <4; i++){
        if (runs[i].exists == true){
            rHour += runs[i].runtime[0].toInt(&ok, 10);
            rMin += runs[i].runtime[1].toInt(&ok, 10);
            rSec += runs[i].runtime[2].toInt(&ok, 10);
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

    eTime[0] = QString::number(eHour);
    eTime[1] = QString::number(eMin);
    eTime[2] = QString::number(eSec);

    eDate[0] = QString::number(eMon);
    eDate[1] = QString::number(eDay);
    eDate[2] = QString::number(eYear);

    totalRuntime[0] = QString::number(rHour);
    totalRuntime[1] = QString::number(rMin);
    totalRuntime[2] = QString::number(rSec);
}


void Report::parseFile(QString file_name){
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

    sTime[0] = filedata[1][0];
    sTime[1] = filedata[1][1];
    sTime[2] = filedata[1][2];

    sDate[0] = filedata[1][3];
    sDate[1] = filedata[1][4];
    sDate[2] = filedata[1][5];

    QString totalString = filedata[2][0] + "." + filedata[2][1];
    totalsf = totalString.toFloat(&ok);

    for (int i = 3; i < 7; i++){
        Run run;
        run.alg = filedata[i][0];

        if (filedata[i][1] != "0"){
            run.runtime[0] = filedata[i][1];
            run.runtime[1] = filedata[i][2];
            run.runtime[2] = filedata[i][3];
            QString coverString = filedata[i][4] + "." + filedata[i][5];
            qDebug() << coverString;
            run.coversf = coverString.toFloat(&ok);
            run.coverPer = run.coversf/totalsf;
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
