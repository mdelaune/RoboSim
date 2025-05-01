#ifndef REPORTWINDOW_H
#define REPORTWINDOW_H

#include "rundata.h"
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
    bool setupSceneFromFile();
    void updateText();
    QString selectedAlg;
    // QMainWindow *mw;

private slots:
    void on_randomAlg_clicked();

    void on_spiralAlg_clicked();

    void on_snakingAlg_clicked();

    void on_wallfollowAlg_clicked();

private:
    Ui::ReportWindow *ui;
    RunData *data;
};

#endif // REPORTWINDOW_H
