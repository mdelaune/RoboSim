#ifndef SUMMARY_H
#define SUMMARY_H

#include <QWidget>
#include "rundata.h"

namespace Ui {
class summary;
}

class summary : public QWidget
{
    Q_OBJECT

public:
    explicit summary(QWidget *parent = nullptr);
    ~summary();

    void setupSceneFromFiles();
    void updateText();

private:
    Ui::summary *ui;

    QList<RunData> reports;
    QStringList file_names;

    int getNumRuns();

    QList<int> getShortestRun();
    QList<int> getLongestRun();
    QList<int> getMostCovRun();
    QList<int> getLeastCovRun();

    QList<int> getAvgTime();
    float getAvgCover();
    float getAvgPerCl();

};

#endif // SUMMARY_H
