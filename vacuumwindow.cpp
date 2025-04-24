#include "vacuumwindow.h"
// #include "ui_vacuumwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QPen>
#include <QBrush>
#include <QDebug>
#include <QMessageBox>

VacuumWindow::VacuumWindow(QWidget *parent)
    : QWidget(parent), vacuum(nullptr),
    innerX(200), innerY(150), innerWidth(400), innerHeight(300),
    doorWidth(60), doorX(0), doorY(0)
{
    const int roomWidth = 800;
    const int roomHeight = 600;

    scene = new QGraphicsScene(0, 0, roomWidth, roomHeight);
    view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setFixedSize(roomWidth + 2, roomHeight + 2);

    QPen wallPen(Qt::black);
    wallPen.setWidth(5);
    scene->addRect(0, 0, roomWidth, roomHeight, wallPen, QBrush(Qt::lightGray));

    doorX = innerX + (innerWidth / 2) - (doorWidth / 2);
    doorY = innerY + innerHeight;

    scene->addRect(innerX, innerY, innerWidth, innerHeight, wallPen, QBrush(Qt::white));
    scene->addLine(innerX, doorY, doorX, doorY, wallPen);
    scene->addLine(doorX + doorWidth, doorY, innerX + innerWidth, doorY, wallPen);
    scene->addLine(innerX, innerY, innerX, innerY + innerHeight, wallPen);
    scene->addLine(innerX, innerY, innerX + innerWidth, innerY, wallPen);
    scene->addLine(innerX + innerWidth, innerY, innerX + innerWidth, innerY + innerHeight, wallPen);

    QPen doorPen(Qt::darkGreen);
    doorPen.setWidth(5);
    scene->addLine(doorX, doorY, doorX + doorWidth, doorY, doorPen);

    scene->addRect(300, 200, 50, 100, QPen(Qt::darkGray), QBrush(Qt::gray));
    scene->addRect(500, 400, 80, 40, QPen(Qt::darkGray), QBrush(Qt::gray));

    // GUI Controls
    pathingBox = new QComboBox();
    pathingBox->addItems({"All", "Random", "WallFollow", "Snaking", "Spiral"});
    pathingBox->setCurrentText("All");

    connect(pathingBox, &QComboBox::currentTextChanged, this, &VacuumWindow::onPathingChanged);

    floorTypeBox = new QComboBox();
    floorTypeBox->addItems({"Hard", "Loop Pile", "Cut Pile", "Frieze-cut pile"});

    speedSpin = new QSpinBox();
    speedSpin->setRange(6, 18);
    speedSpin->setValue(12);

    batterySpin = new QSpinBox();
    batterySpin->setRange(90, 200);
    batterySpin->setValue(150);

    // Speed multiplier (1x, 5x, 50x)
    speedMultiplierBox = new QComboBox();
    speedMultiplierBox->addItems({"1x", "5x", "50x"});
    speedMultiplierBox->setCurrentIndex(0); // Default to 1x

    connect(speedMultiplierBox, &QComboBox::currentTextChanged, this, [=](const QString &text) {
        if (vacuum) {
            int multiplier = text.left(text.length() - 1).toInt();  // Remove the 'x' and convert to int
            vacuum->setSpeedMultiplier(multiplier);
        }
    });



    QPushButton *startButton = new QPushButton("Start Simulation");
    QPushButton *stopButton = new QPushButton("Stop Simulation");


    connect(startButton, &QPushButton::clicked, this, &VacuumWindow::startSimulation);
    connect(stopButton, &QPushButton::clicked, this, &VacuumWindow::stopSimulation);

    connect(speedSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int newSpeed) {
        if (vacuum) vacuum->setSpeed(newSpeed);
    });

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *configLayout = new QHBoxLayout();

    configLayout->addWidget(new QLabel("Pathing Algorithm:"));
    configLayout->addWidget(pathingBox);
    configLayout->addWidget(new QLabel("Floor Type:"));
    configLayout->addWidget(floorTypeBox);
    configLayout->addWidget(new QLabel("Speed (in/sec):"));
    configLayout->addWidget(speedSpin);
    configLayout->addWidget(new QLabel("Battery (min):"));
    configLayout->addWidget(batterySpin);
    configLayout->addWidget(new QLabel("Sim Speed:"));
    configLayout->addWidget(speedMultiplierBox);
    configLayout->addWidget(startButton);
    configLayout->addWidget(stopButton);



    mainLayout->addLayout(configLayout);
    mainLayout->addWidget(view);
}

void VacuumWindow::startSimulation() {
    if (vacuum) {
        vacuum->stop();
        delete vacuum;
        vacuum = nullptr;
    }

    vacuum = new Vacuum(scene);
    QRectF innerRoom(innerX, innerY, innerWidth, innerHeight);
    QRectF doorArea(doorX, doorY - 5, doorWidth, 10);

    // ✅ Add obstacle rectangles (same as Run All)
    QList<QGraphicsRectItem*> obs;
    obs << scene->addRect(300, 200, 50, 100) // Chest
        << scene->addRect(500, 400, 10, 10)  // Table legs
        << scene->addRect(570, 400, 10, 10)
        << scene->addRect(500, 430, 10, 10)
        << scene->addRect(570, 430, 10, 10);

    obstacles = obs;               // Save for later reuse
    vacuum->setObstacles(obs);     // Pass to vacuum


    // Continue setup...
    vacuum->setRoomAndDoor(innerRoom, doorArea);
    vacuum->setSpeed(speedSpin->value());
    vacuum->setBatteryLife(batterySpin->value());
    vacuum->setFloorType(floorTypeBox->currentText());
    vacuum->setSpeedMultiplier(speedMultiplierBox->currentText().remove("x").toInt());

    vacuum->start();

    QString selectedAlg = pathingBox->currentText();
    if (selectedAlg == "All") {
        vacuum->runTimedAlgorithms();  // ✅ Run all algorithms in sequence
    } else {
        vacuum->setPathingAlgorithm(selectedAlg);  // ✅ Run only one
        vacuum->start();
    }


}

void VacuumWindow::stopSimulation() {
    if (vacuum) {
        vacuum->stop();
    }
}

void VacuumWindow::onPathingChanged(const QString &newAlgorithm) {
    if (vacuum) {
        vacuum->setPathingAlgorithm(newAlgorithm);
        qDebug() << "Live algorithm switched to:" << newAlgorithm;
    }
}

void VacuumWindow::clearTrailDots() {
    for (QGraphicsItem *item : scene->items()) {
        QGraphicsEllipseItem *dot = qgraphicsitem_cast<QGraphicsEllipseItem *>(item);
        if (dot && dot->rect().width() <= 2.0) {
            scene->removeItem(dot);
            delete dot;
        }
    }
}


void VacuumWindow::runAllAlgorithms() {
    if (!vacuum) return;

    vacuum->stop();
    clearTrailDots();
    vacuum->runTimedAlgorithms();
    qDebug() << "Started all algorithms in sequence.";

}
