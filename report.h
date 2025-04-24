#ifndef REPORT_H
#define REPORT_H

#include <QWidget>
#include <QGraphicsScene>

class Run
{
public:
    Run();
    QString alg;
    bool exists;

    QStringList time;

    QString coverSF;
    QString coverPer;


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

    QStringList runTime;
    QString totalSF;

    QList<Run> runs;
    //QString getTimeString(float time);
    void getEnd();

private:
    void setEnd();
};


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
