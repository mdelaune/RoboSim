#include "simwindow.h"
#include "ui_simwindow.h"

#include <QFileDialog>

SimWindow::SimWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SimWindow)
{
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    house = new House(scene);
    house->loadPlan(house_path);
    house->drawSimulationPlan();

    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setRenderHint(QPainter::SmoothPixmapTransform, true);


    vacuum = new Vacuum(scene);

    // if (!rooms.isEmpty())
    // {
    //     QRectF startRoom = rooms.first();   // take first room
    //     double centerX = (startRoom.left() + startRoom.right()) / 2.0;
    //     double centerY = (startRoom.top() + startRoom.bottom()) / 2.0;

    //     vacuum->setVacuumPosition(centerX, centerY);
    // }
    // else
    // {
    //     qDebug() << "No rooms available to place vacuum!";
    // }

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

    simData = new RunData();
    simData->id = house->id;
    QDate date = QDate::currentDate();
    QString dateString = date.QDate::toString("dd MM yy");
    simData->sDate = dateString.split(' ');
    qDebug() << dateString;

    QTime time = QTime::currentTime();
    QString timeString = time.toString();
    simData->sTime = timeString.split(':');
    qDebug() << timeString;

    simData->totalSF = QString::number(house->getTotalArea()/16000);

    for (int i = 0; i < 4; i++){
            Run run;
            run.exists = false;
            simData->runs.append(run);
    }
}

void SimWindow::updateSimulation()
{
    if (vacuum->getBatteryLife() <= 0)
    {
        simulationTimer->stop();
        return;
    }

    // vacuum->move(rooms, obstructions, doors, simulationSpeedMultiplier); // <<<<<< MOVE the vacuum!

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

void SimWindow::writeReport(){
    QString filename = QFileDialog::getSaveFileName(this, "Select Report Save Location", "C://", "text(*.txt)");
    QFile file(filename);
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        stream << simData->id << Qt::endl;
        stream << simData->sTime[0]<< ":" << simData->sTime[1]<< "." << simData->sTime[2] << " " << simData->sDate[1] << ":" << simData->sDate[0] << "." << simData->sDate[2] << Qt::endl;
        stream << simData->totalSF << Qt::endl;
        if (simData->runs[0].exists){
            stream << "random" << simData->runs[0].getTimeString(simData->runs[0].time) << simData->runs[0].coverSF << Qt::endl;
        }
        else{
            stream << "random 0" << Qt::endl;
        }
        if (simData->runs[1].exists){
            stream << "spiral" << simData->runs[1].getTimeString(simData->runs[1].time) << simData->runs[0].coverSF << Qt::endl;
        }
        else{
            stream << "spiral 0" << Qt::endl;
        }
        if (simData->runs[2].exists){
            stream << "snaking" << simData->runs[2].getTimeString(simData->runs[2].time) << simData->runs[0].coverSF << Qt::endl;
        }
        else{
            stream << "snaking 0" << Qt::endl;
        }
        if (simData->runs[3].exists){
            stream << "wallfollow" << simData->runs[3].getTimeString(simData->runs[3].time) << simData->runs[0].coverSF << Qt::endl;
        }
        else{
            stream << "wallfollow 0" << Qt::endl;
        }

    }
}

void SimWindow::on_stopButton_clicked()
{
    QDate date = QDate::currentDate();
    QString dateString = date.QDate::toString("dd MM yy");
    simData->eDate = dateString.split(' ');

    QTime time = QTime::currentTime();
    QString timeString = time.toString();
    simData->eTime = timeString.split(':');

    writeReport();
    this->close();
}

