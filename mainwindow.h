#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "editwindow.h"
#include "settingswindow.h"
#include "simwindow.h"
#include "reportwindow.h"
#include "summarywindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_createFP_clicked();
    void on_loadRep_clicked();
    void on_loadFP_clicked();
    void on_sumRep_clicked();
    void on_robSet_clicked();
    void on_runSim_clicked();
    void updateRunSimButtonState();
    void onSettingsUpdated(int batteryLife, int vacuumEfficiency, int whiskerEfficiency, int speed, QStringList selectedAlgorithms);

private:
    Ui::MainWindow *ui;

    EditWindow *editWin;
    SettingsWindow *setWin;
    SimWindow *simWin;
    ReportWindow *repWin;
    SummaryWindow *sumWin;

    bool floorplanCreated = false;
    bool robotSetup = false;

    int batteryLife;
    int vacuumEfficiency;
    int whiskerEfficiency;
    int speed;
    QStringList selectedAlgorithms;

};
#endif // MAINWINDOW_H
