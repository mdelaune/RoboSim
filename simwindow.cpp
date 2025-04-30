#include "simwindow.h"
#include "ui_simwindow.h"

#include <QFileDialog>

SimWindow::SimWindow(House* housePtr, QWidget *parent)
    : QMainWindow(parent), house(housePtr)
    , ui(new Ui::SimWindow)
{
    ui->setupUi(this);

    QGraphicsView *view = ui->graphicsView;

    view->setRenderHint(QPainter::Antialiasing);
    view->setRenderHint(QPainter::SmoothPixmapTransform, true);

    scene = new QGraphicsScene(this);
    view->setScene(scene);

    house->setScene(scene);
    //house->loadPlan(house_path);
    house->drawSimulationPlan();

    vacuum = new Vacuum(scene);

    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setRenderHint(QPainter::SmoothPixmapTransform, true);

    simulationTimer = new QTimer(this);
    connect(simulationTimer, &QTimer::timeout, this, &SimWindow::updateSimulation);

    simulationSpeedMultiplier = 1.0;
    simulationTimer->setInterval(1000);

    connect(ui->timesOnePushButton, &QPushButton::clicked, this, &SimWindow::oneSpeedPushed);
    connect(ui->timesFivePushButton, &QPushButton::clicked, this, &SimWindow::fiveSpeedPushed);
    connect(ui->timesFiftyPushButton, &QPushButton::clicked, this, &SimWindow::fiftySpeedPushed);

    simData = new RunData();
    simData->id = QString::number(house->getFloorplanId());
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

SimWindow::~SimWindow()
{
    delete ui;
}

void SimWindow::startSimulation(int batteryLife, int vacuumEfficiency, int whiskerEfficiency, int speed, QStringList selectedAlgorithms)
{
    this->batteryLife = batteryLife;
    this->vacuumEfficiency = vacuumEfficiency;
    this->whiskerEfficiency = whiskerEfficiency;
    this->speed = speed;
    pendingAlgorithms = selectedAlgorithms;

    currentAlgorithmIndex = 0;
    allRunsCompleted = false;

    for (int i = 0; i < 4; i++){
            Run run;
            run.exists = false;
            simData->runs.append(run);
    }

    startNextRun();
}

void SimWindow::updateSimulation()
{
    if (vacuum->getBatteryLife() <= 0)
    {
        simulationTimer->stop();
        currentAlgorithmIndex++;
        startNextRun();
        return;
    }

    vacuum->advance();
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
    simulationTimer->stop();
    QDate date = QDate::currentDate();
    QString dateString = date.QDate::toString("dd MM yy");
    simData->eDate = dateString.split(' ');

    QTime time = QTime::currentTime();
    QString timeString = time.toString();
    simData->eTime = timeString.split(':');

    writeReport();
    this->close();
}

// This method is what handles multiple runs for the simulation
void SimWindow::startNextRun()
{
    if (currentAlgorithmIndex >= pendingAlgorithms.size())
    {
        if (!allRunsCompleted) {
            simulationTimer->stop();
            qDebug() << "All runs complete";
            allRunsCompleted = true;

            // Optionally close or update UI here
            this->close();
        }
    }
    else
    {
        resetScene();
        updateBatteryLifeLabel();
        simulationTimer->start();
    }
}

// This method clears all items from the simulation window scene and resets them for the starting run and new runs
void SimWindow::resetScene()
{
    scene->clear(); // Clears all items from the scene
    house->setScene(scene); // Sets the simulation window scene as the current scene
    house->drawSimulationPlan(); // Redraws the house layout

    // Ensure vacuum exists and is properly reset
    if (vacuum != nullptr)
    {
        vacuum->reset(); // Reset the vacuum (position, battery, etc.)
        vacuum->setBatteryLife(batteryLife); // Set battery life for the new run
        vacuum->setVacuumEfficiency(vacuumEfficiency); // Set vacuum efficiency
        vacuum->setWhiskerEfficiency(whiskerEfficiency); // Set whisker efficiency
        vacuum->setSpeed(speed); // Set speed for the new run
    }
    else
    {
        qDebug() << "Error: Vacuum object is null.";
    }
}



