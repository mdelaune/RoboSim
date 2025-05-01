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

    vacuum->setHousePath(house_path);
    qDebug() << "HOUSE PATH TEST" << house_path;

    currentAlgorithmIndex = 0;
    allRunsCompleted = false;

    simData = new RunData();
    simData->setNewID();
    simData->id = QString::number(house->getFloorplanId());
    simData->openSF = QString::number(house->getOpenArea()/120);
    simData->totalSF = QString::number(house->getTotalArea()/120);


    QDate date = QDate::currentDate();
    QString dateString = date.QDate::toString("dd MM yy");
    simData->sDate = dateString.split(' ');

    QTime time = QTime::currentTime();
    QString timeString = time.toString();
    simData->sTime = timeString.split(':');

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
        writeRun();
        simulationTimer->stop();
        currentAlgorithmIndex++;
        startNextRun();
        return;
    }

    vacuum->updateMovementandTrail(scene);
    updateBatteryLifeLabel();
}

void SimWindow::updateBatteryLifeLabel()
{
    int batteryLife = vacuum->getBatteryLife();

    int minutes = batteryLife / 60;
    int seconds = batteryLife % 60;

    QString timeString = QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
    ui->secondsLeftLabel->setText(timeString);
    ui->coverSF->setText(QString::number(vacuum->getCoveredArea()));
    double perD = vacuum->getCoveredArea()/house->getTotalArea() * 10000;
    ui->perCleaned->setText(QString::number(perD, 'g' ,4));


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

QString SimWindow::writeReport(){
    save_path = QFileDialog::getExistingDirectory(this, "Select Report Save Location", "C://", QFileDialog::ShowDirsOnly);
    QString pathToSavedReport = save_path+"/"+simData->id+ "-" + QString::number(simData->report_id) + ".txt";
    QFile file(pathToSavedReport);
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        stream << simData->id << " " << simData->report_id << Qt::endl;
        stream << simData->sTime[0]<< ":" << simData->sTime[1]<< ":" << simData->sTime[2] << " " << simData->sDate[1] << ":" << simData->sDate[0] << "." << simData->sDate[2] << Qt::endl;
        stream << simData->totalSF << Qt::endl;
        stream << simData->openSF << Qt::endl;
        qDebug() << simData->runs[0].exists;
        if (simData->runs[0].exists){
            simData->runs[0].heatmapPath = save_path + "/" + simData->id + "_" + QString::number(simData->report_id) + "-" + simData->runs[0].alg + ".png";
            stream << "random " << simData->runs[0].getTimeString(simData->runs[0].time) << " " << simData->runs[0].coverSF  << " " << simData->runs[0].heatmapPath << Qt::endl;
            //QFile saveMap(simData->runs[0].heatmapPath);
            simData->runs[0].heatmap.save(simData->runs[0].heatmapPath,"PNG");
        }
        else{
            stream << "random 0" << Qt::endl;

        }
        if (simData->runs[1].exists){
            simData->runs[1].heatmapPath = save_path + "/" + simData->id + "_" + QString::number(simData->report_id) + "-" + simData->runs[1].alg+ ".png";
            stream << "spiral " << simData->runs[1].getTimeString(simData->runs[1].time) << " " << simData->runs[1].coverSF  << " " << simData->runs[1].heatmapPath << Qt::endl;
            simData->runs[1].heatmap.save(simData->runs[1].heatmapPath,"PNG");
        }
        else{
            stream << "spiral 0" << Qt::endl;
        }
        if (simData->runs[2].exists){
            simData->runs[2].heatmapPath = save_path + "/" + simData->id + "_" + QString::number(simData->report_id) + "-" + simData->runs[2].alg+ ".png";
            stream << "snaking " << simData->runs[2].getTimeString(simData->runs[2].time) << " " << simData->runs[2].coverSF  << " " << simData->runs[2].heatmapPath << Qt::endl;
            simData->runs[2].heatmap.save(simData->runs[2].heatmapPath,"PNG");
        }
        else{
            stream << "snaking 0" << Qt::endl;
        }
        if (simData->runs[3].exists){
            simData->runs[3].heatmapPath = save_path + "/" + simData->id + "_" + QString::number(simData->report_id) + "-" + simData->runs[3].alg+ ".png";
            stream << "wallfollow " << simData->runs[3].getTimeString(simData->runs[3].time) << " " << simData->runs[3].coverSF  << " " << simData->runs[3].heatmapPath << Qt::endl;
            simData->runs[3].heatmap.save(simData->runs[3].heatmapPath,"PNG");
        }
        else{
            stream << "wallfollow 0";
        }

    }

    return pathToSavedReport;
}

void SimWindow::writeRun(){
    Run run;
    if (pendingAlgorithms[currentAlgorithmIndex] == "Random"){
        run.alg = "random";
    }
    if (pendingAlgorithms[currentAlgorithmIndex] == "Spiral"){
        run.alg = "spiral";
    }
    if (pendingAlgorithms[currentAlgorithmIndex] == "Snaking"){
        run.alg = "snaking";
    }
    if (pendingAlgorithms[currentAlgorithmIndex] == "Wall Follow"){
        run.alg = "wallfollow";
    }
    run.exists = true;
    int batRuntime = batteryLife*60 - vacuum->getBatteryLife();
    int m = batRuntime/60;
    run.time.append(QString::number(m/60));
    run.time.append(QString::number(m % 60));
    run.time.append(QString::number(batRuntime % 60));
    run.coverSF = QString::number(vacuum->getCoveredArea());

    QPixmap heatmap = ui->graphicsView->grab();
    if (pendingAlgorithms[currentAlgorithmIndex] == "Random"){
        simData->runs[0] = run;
        simData->runs[0].heatmap = heatmap;
    }
    if (pendingAlgorithms[currentAlgorithmIndex] == "Spiral"){
        simData->runs[1] = run;
        simData->runs[1].heatmap = heatmap;
    }
    if (pendingAlgorithms[currentAlgorithmIndex] == "Snaking"){
        simData->runs[2] = run;
        simData->runs[2].heatmap = heatmap;
    }
    if (pendingAlgorithms[currentAlgorithmIndex] == "Wall Follow"){
        simData->runs[3] = run;
        simData->runs[3].heatmap = heatmap;
    }
}

void SimWindow::stopSimulation(){
    simulationTimer->stop();
    QDate date = QDate::currentDate();
    QString dateString = date.QDate::toString("dd MM yy");
    simData->eDate = dateString.split(' ');

    QTime time = QTime::currentTime();
    QString timeString = time.toString();
    simData->eTime = timeString.split(':');


    repWin = new ReportWindow(this);
    QString newPath = writeReport();
    bool fileSelected = repWin->setupSceneFromSim(newPath);
    if (fileSelected){
        repWin->showMaximized();
    }
    this->close();

}

void SimWindow::on_stopButton_clicked()
{
    writeRun();
    stopSimulation();
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

            stopSimulation();

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
    house->loadPlan(house_path); // Redraws the house layout

    // Ensure vacuum exists and is properly reset
    if (vacuum != nullptr)
    {
        vacuum->reset(); // Reset the vacuum (position, battery, etc.)
        vacuum->setBatteryLife(batteryLife);
        vacuum->setVacuumEfficiency(vacuumEfficiency);
        vacuum->setWhiskerEfficiency(whiskerEfficiency);
        vacuum->setSpeed(speed);

        if (currentAlgorithmIndex < pendingAlgorithms.size()) {
            vacuum->setPathingAlgorithm(pendingAlgorithms[currentAlgorithmIndex]);
        }

        // Update label
        if (vacuum->getPathingAlgorithm() == "Random"){
            ui->algLabel->setText("Random");
        }
        if (vacuum->getPathingAlgorithm() == "Spiral"){
            ui->algLabel->setText("Spiral");
        }
        if (vacuum->getPathingAlgorithm() == "Snaking"){
            ui->algLabel->setText("Snaking");
        }
        if (vacuum->getPathingAlgorithm() == "Wall Follow"){
            ui->algLabel->setText("Wall Follow");
        }
    }
    else
    {
        qDebug() << "Error: Vacuum object is null.";
    }
}
