#ifndef VACUUMWINDOW_H
#define VACUUMWINDOW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QComboBox>
#include <QSpinBox>
#include <QRectF>
#include "vacuum.h"

// namespace Ui {
// class VacuumWindow;
// }

class VacuumWindow : public QWidget
{
    Q_OBJECT

public:
    explicit VacuumWindow(QWidget *parent = nullptr);
    // ~VacuumWindow();

    Vacuum *vacuum;          // Pointer to current vacuum object


private slots:
    // Initializes vacuum and starts simulation
    void startSimulation();                         // Start simulation on button press
    void stopSimulation();                          // Stop simulation on button press
    void onPathingChanged(const QString &newAlgorithm); // Update algorithm live if changed
    void clearTrailDots();
    void runAllAlgorithms();


private:
    // Graphics
    QGraphicsScene *scene;   // Main graphics scene
    QGraphicsView *view;     // View for the scene

    // UI controls
    QComboBox *pathingBox;   // Dropdown for selecting pathing algorithm
    QComboBox *floorTypeBox; // Dropdown for floor type
    QSpinBox *speedSpin;     // Speed selector
    QSpinBox *batterySpin;   // Battery selector

    QComboBox *speedMultiplierBox;  // NEW: Simulation speed multiplier box


    // Room dimensions
    int innerX;              // Inner room X position
    int innerY;              // Inner room Y position
    int innerWidth;          // Inner room width
    int innerHeight;         // Inner room height
    int doorWidth;           // Width of the doorway
    int doorX;               // X position of the door
    int doorY;               // Y position of the door
    int globalTickCount = 0;
    int maxGlobalTicks = 36000; // 10 minutes at 60 FPS

    QList<QGraphicsRectItem*> obstacles;

};

#endif // VACUUMWINDOW_H
