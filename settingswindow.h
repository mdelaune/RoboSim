#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui {
class SettingsWindow;
}
QT_END_NAMESPACE

class SettingsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

signals:
    void settingsUpdated(int batteryLife,
                         int vacuumEfficiency,
                         int whiskerEfficiency,
                         int speed,
                         QStringList pathingAlgorithms);

private:
    Ui::SettingsWindow *ui;
    void setupAlgorithmList();
    void setupLineEdits();
    void handleSaveClicked();
    bool validateInputs();
};

#endif // SETTINGSWINDOW_H
