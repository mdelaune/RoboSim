#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "editwindow.h"
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
    void on_runSim_clicked();

private:
    Ui::MainWindow *ui;

    EditWindow *editWin;
    SimWindow *simWin;
    ReportWindow *repWin;
    SummaryWindow *sumWin;

};
#endif // MAINWINDOW_H
