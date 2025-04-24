#ifndef REPORT_H
#define REPORT_H

#include <QString>
#include <QList>
#include "run.h"

class Report
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

    //QStringList runTime;
    QString totalSF;

    QList<Run> runs;
    void getEnd();

private:
    void setEndValues();
}

namespace Ui {
class report;
}

class report : public QWidget
{
    Q_OBJECT

public:
    explicit report(QWidget *parent = nullptr);
    ~report();
    QString file_name;
    void setupSceneFromFile();
    void updateText();
    QString selectedAlg;



private slots:
    void on_randomAlg_clicked();
    void on_spiralAlg_clicked();
    void on_snakingAlg_clicked();
    void on_wallfollowAlg_clicked();

private:
    Ui::report *ui;
    RunData *data;
};

#endif // REPORT_H
