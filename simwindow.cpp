#include "simwindow.h"
#include "ui_simwindow.h"

SimWindow::SimWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SimWindow)
{
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    house = new House(scene);

    house->drawSimulationPlan();

    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setRenderHint(QPainter::SmoothPixmapTransform, true);

    vacuum = new Vacuum(scene);

    houseParser = new HouseParser("/Users/Batman/SimTest/default_plan.json");

    simulationTimer = new QTimer(this);
    connect(simulationTimer, &QTimer::timeout, this, &SimWindow::updateSimulation);

    simulationSpeedMultiplier = 1.0;
    simulationTimer->setInterval(1000);

    connect(ui->timesOnePushButton, &QPushButton::clicked, this, &SimWindow::oneSpeedPushed);
    connect(ui->timesFivePushButton, &QPushButton::clicked, this, &SimWindow::fiveSpeedPushed);
    connect(ui->timesFiftyPushButton, &QPushButton::clicked, this, &SimWindow::fiftySpeedPushed);
}

SimWindow::~SimWindow()
{
    delete ui;
    delete houseParser;
}

void SimWindow::startSimulation(int batteryLife, int vacuumEfficiency, int whiskerEfficiency, int speed, QStringList selectedAlgorithms)
{
    vacuum->setBatteryLife(batteryLife);
    vacuum->setVacuumEfficiency(vacuumEfficiency);
    vacuum->setWhiskerEfficiency(whiskerEfficiency);
    vacuum->setSpeed(speed);
    vacuum->setPathingAlgorithms(selectedAlgorithms);

    updateBatteryLifeLabel();

    simulationTimer->start(1000);
}

void SimWindow::updateSimulation()
{
    if (vacuum->getBatteryLife() <= 0)
    {
        simulationTimer->stop();
        return;
    }

    QList<QRectF> rooms = houseParser->getRooms2();
    QList<Obstruction2> obstructions = houseParser->getObstructions2();
    QList<QPointF> doors = houseParser->getDoors2();

    vacuum->move(rooms, obstructions, doors, simulationSpeedMultiplier);
    scene->update();
    updateBatteryLifeLabel();
}

void SimWindow::updateBatteryLifeLabel()
{
    int batteryLife = vacuum->getBatteryLife();

    int minutes = batteryLife / 60;
    int seconds = batteryLife % 60;

    QString timeString = QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
    ui->secondsLeftLabel->setText(timeString);
}

void SimWindow::setSimulationSpeed(int multiplier)
{
    simulationSpeedMultiplier = multiplier;
    int newInterval = 1000 / multiplier;
    simulationTimer->setInterval(newInterval);
}

void SimWindow::oneSpeedPushed()
{
    setSimulationSpeed(1);
}

void SimWindow::fiveSpeedPushed()
{
    setSimulationSpeed(5);
}

void SimWindow::fiftySpeedPushed()
{
    setSimulationSpeed(50);
}
