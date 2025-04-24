#ifndef REPORT_H
#define REPORT_H

#include <QString>
#include <QList>
#include "run.h"

class Report
{
public:
    Report();
    // QString file_name;
    void parseFile(QString file_name);

    QString id;
    QString sTime[3];
    QString sDate[3];
    QString eTime[3];
    QString eDate[3];
    QString totalRuntime[3];
    // QString runtime[3];
    float totalsf;
    // QString coversf[2];
    // QString algorithms[4];
    // QString perCleaned[4];
    QList<Run> runs;

private:
    void setEndValues();

};

#endif // REPORT_H
