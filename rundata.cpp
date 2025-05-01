#include "rundata.h"

#include <QDebug>
#include <QFileDialog>
#include <QRegularExpression>
#include <QRandomGenerator>

Run::Run(){

}

QString Run::getTimeString(QStringList time){
    return time[0].rightJustified(2, '0') + ":" + time[1].rightJustified(2, '0') + ":" + time[2].rightJustified(2, '0');
}

RunData::RunData(){}

void RunData::parseFile(QString file_name){
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file";
        return;
    }

    QStringList filedata[4];
    QString algdata[4];

    int n = 0;
    int m =0;


    QTextStream ts(&file);
    while (!ts.atEnd()){
        if (n<4){
            QString line = ts.readLine();
            QStringList data = line.split(QRegularExpression("\\W+"), Qt::SkipEmptyParts);
            filedata[n] = data;
            n++;
        }
        else{
            QString line = ts.readLine();
            algdata[m] = line;
            m++;

        }
    }

    bool ok;
    //qDebug() << filedata[0];
    id = filedata[0][0];
    report_id = filedata[0][1].toInt();

    sTime.append(filedata[1][0]);
    sTime.append(filedata[1][1]);
    sTime.append(filedata[1][2]);

    sDate.append(filedata[1][3]);
    sDate.append(filedata[1][4]);
    sDate.append(filedata[1][5]);

    //QString totalString = filedata[2][0] + "." + filedata[2][1];
    totalSF = filedata[2][0]; //totalString;
    openSF = filedata[3][0];


    for (int i = 0; i < 4; i++){
        Run run;
        QStringList runString = algdata[i].split(' ');
        if (runString.size() >2){
            run.alg = runString[0];
            run.time = runString[1].split(':');
            run.coverSF = runString[2];
            run.heatmapPath = runString[3];
            qDebug() << run.heatmapPath;
            run.exists = true;
            run.coverPer = QString::number(run.coverSF.toDouble()/openSF.toDouble() * 100, 'g', 4);
        }
        else run.exists = false;
        runs.append(run);
    }

    file.close();
}

void RunData::setNewID(){
    report_id = QRandomGenerator::global()->bounded(10000, 99999);
}
