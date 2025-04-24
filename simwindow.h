#ifndef SIMWINDOW_H
#define SIMWINDOW_H

#include <QMainWindow>
#include "settingswindow.h"
#include "vacuumwindow.h"

namespace Ui {
class SimWindow;
}

class SimWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit SimWindow(QWidget *parent = nullptr);
    ~SimWindow();

    void startSimulation(int batteryLife, int vacuumEfficiency, int whiskerEfficiency, int speed, QStringList selectedAlgorithms);

private:
    Ui::SimWindow *ui;
    VacuumWindow *vacWin;
};

#endif // SIMWINDOW_H
