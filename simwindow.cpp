#include "simwindow.h"
#include "ui_simwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QPainter>
#include <QImage>
#include <QDir>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>

SimWindow::SimWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SimWindow)
    , simulationTimer(new QTimer(this))
    , simulationSpeedMultiplier(1.0)
    , vacuum(nullptr)
    , house(nullptr)
    , houseParser(nullptr)
{
    ui->setupUi(this);

    setupScene();
    setupConnections();
}

SimWindow::~SimWindow()
{
    delete ui;
    delete houseParser;
    delete vacuum;
    delete house;
}

void SimWindow::setupScene()
{
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    house = new House(scene);
    house->drawSimulationPlan();

    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setRenderHint(QPainter::SmoothPixmapTransform, true);


    houseParser = new HouseParser(":/Default/default_plan.json");
}

void SimWindow::setupConnections()
{
    connect(simulationTimer, &QTimer::timeout, this, &SimWindow::updateSimulation);

    connect(ui->timesOnePushButton, &QPushButton::clicked, this, [=](){ setSimulationSpeed(1); });
    connect(ui->timesFivePushButton, &QPushButton::clicked, this, [=](){ setSimulationSpeed(5); });
    connect(ui->timesFiftyPushButton, &QPushButton::clicked, this, [=](){ setSimulationSpeed(50); });
    connect(ui->stopPushButton, &QPushButton::clicked, this, &SimWindow::stopSimulation);



}

void SimWindow::startSimulation(int batteryLife, int vacuumEfficiency, int whiskerEfficiency, int speed, const QStringList &selectedAlgorithms)
{
    qDebug() << "[SimWindow] Starting Simulation with Algorithms:" << selectedAlgorithms;

    if (vacuum) {
        delete vacuum;
        vacuum = nullptr;
    }
    vacuum = new Vacuum(scene);

    vacuum->setBatteryLife(batteryLife);
    vacuum->setVacuumEfficiency(vacuumEfficiency);
    vacuum->setWhiskerEfficiency(whiskerEfficiency);
    vacuum->setSpeed(speed);
    vacuum->setPathingAlgorithms(selectedAlgorithms);

    // ❌ Removed useAllAlgorithms block

    QVector<Room> rooms = house->getRooms();
    if (!rooms.isEmpty()) {
        QRectF firstRoom = rooms.first().get_rectRoom();
        QPointF center = firstRoom.center();
        vacuum->setVacuumPosition(center.x(), center.y());
        qDebug() << "[SimWindow] Vacuum starting at:" << center;
    } else {
        qDebug() << "[SimWindow] No rooms found!";
    }

    updateBatteryLifeLabel();
    simulationTimer->start(1000);
}


void SimWindow::stopSimulation()
{
    simulationTimer->stop();

    QString heatmapFilename;
    if (saveHeatmapImage(heatmapFilename))
    {
        QString resultFilePath = QCoreApplication::applicationDirPath() + "/Resources/simulation_results.txt";
        QFile resultFile(resultFilePath);
        if (resultFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&resultFile);
            out << "Heatmap Filename: " << heatmapFilename << "\n";

            if (vacuum) {
                double whiskerEffectiveness = vacuum->calculateWhiskerEffectiveness();
                out << "Whisker Cleaning Effectiveness: " << whiskerEffectiveness << "%\n";

                double sqFtCleaned = vacuum->getSquareFeetCovered();
                out << "Square Feet Cleaned: " << sqFtCleaned << " sq ft\n";
            }

            out << "Simulation End Time: " << QDateTime::currentDateTime().toString() << "\n";
            resultFile.close();
        }
        qDebug() << "[SimWindow] Results saved to:" << resultFilePath;

        // Open results file automatically
        QDesktopServices::openUrl(QUrl::fromLocalFile(resultFilePath));
    }

    close();
}


void SimWindow::updateSimulation()
{
    if (!vacuum || vacuum->getBatteryLife() <= 0) {
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
    if (!vacuum) return;

    int batteryLife = vacuum->getBatteryLife();
    int minutes = batteryLife / 60;
    int seconds = batteryLife % 60;

    QString timeString = QString("%1:%2")
                             .arg(minutes, 2, 10, QChar('0'))
                             .arg(seconds, 2, 10, QChar('0'));

    ui->secondsLeftLabel->setText(timeString);
}

void SimWindow::setSimulationSpeed(int multiplier)
{
    simulationSpeedMultiplier = multiplier;
    simulationTimer->setInterval(1000 / multiplier);
    qDebug() << "[SimWindow] Simulation speed set to:" << multiplier << "x";
}

bool SimWindow::saveHeatmapImage(QString &outImageFilename)
{
    if (!scene) return false;

    QString appDir = QCoreApplication::applicationDirPath();
    QString saveFolderPath = appDir + "/Resources/heatmaps/";

    QDir dir(saveFolderPath);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qDebug() << "[SimWindow] Failed to create Resources/heatmaps/ folder.";
            return false;
        }
    }

    QImage image(scene->sceneRect().size().toSize(), QImage::Format_ARGB32);
    image.fill(Qt::white);

    QPainter painter(&image);
    scene->render(&painter);
    painter.end();

    QString filename = QString("heatmap_%1.png").arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
    QString fullPath = dir.filePath(filename);

    bool success = image.save(fullPath);

    if (success) {
        outImageFilename = "Resources/heatmaps/" + filename;  // ✅ Return relative path
        qDebug() << "[SimWindow] Heatmap saved at:" << fullPath;
    }
    else {
        qDebug() << "[SimWindow] Failed to save heatmap.";
    }

    return success;
}
