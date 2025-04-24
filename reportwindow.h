#ifndef REPORTWINDOW_H
#define REPORTWINDOW_H

#include "report.h"
#include <QMainWindow>
#include <QGraphicsScene>
#include <QString>

namespace Ui {
class ReportWindow;
}

class ReportWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ReportWindow(QWidget *parent = nullptr);
    ~ReportWindow();
    QString file_name;
    void setupSceneFromFile();
    void updateText();
    QString selectedAlg;
    // QMainWindow *mw;

private slots:
    void on_randomAlg_clicked();

    void on_spiralAlg_clicked();

    void on_snakingAlg_clicked();

    void on_wallFollowAlg_clicked();

    // void on_homeButton_clicked();

private:
    Ui::ReportWindow *ui;
    Report *report;
};

#endif // REPORTWINDOW_H
