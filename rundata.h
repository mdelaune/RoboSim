#ifndef RUNDATA_H
#define RUNDATA_H

#include <QWidget>

#include <QString>
#include <QList>


class Run
{
public:
    Run();
    QString alg;
    bool exists;

    QStringList time;

    QString coverSF;
    QString coverPer;

    QString getTimeString(QStringList time);


};



class RunData
{
public:
    RunData();

    void parseFile(QString file_name);

    QString id;

    QStringList sTime; // in minutes
    QStringList sDate;

    QStringList eTime;
    QStringList eDate;

    QStringList totalRuntime;
    QString totalSF;

    QList<Run> runs;
    void getEnd();

private:
    void setEndValues();
};

#endif //RUNDATA
