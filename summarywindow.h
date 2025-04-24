#ifndef SUMMARYWINDOW_H
#define SUMMARYWINDOW_H

#include <QMainWindow>
#include <QList>
#include "report.h"

namespace Ui {
class SummaryWindow;
}

class SummaryWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SummaryWindow(QWidget *parent = nullptr);
    ~SummaryWindow();
    void setupSceneFromFiles();
    void updateText();


private:
    Ui::SummaryWindow *ui;
    QList<Report> reports;
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

#endif // SUMMARYWINDOW_H
