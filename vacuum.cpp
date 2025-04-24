#include "vacuum.h"
#include <QTimer>
#include <QPointF>
#include <QRectF>
#include <QDebug>
#include <cmath>
#include <cstdlib>

Vacuum::Vacuum(QGraphicsScene *scene, QObject *parent)
    : QObject(parent),
    scene(scene),
    speed(12.0),
    batteryLife(150 * 60),
    remainingBattery(batteryLife),
    floorType("Hard"),
    pathingAlgorithm("Random"),
    currentPosition(scene->width() / 2, scene->height() / 2),
    velocity(speed, 0),
    cleaningEfficiency(0.9),
    whiskerEfficiency(0.3),
    stoppedByUser(false),
    movingRight(true),
    wallDirection(0),
    spiralAngle(0),
    spiralRadius(1),
    spiraling(true),
    wallFollowing(false),
    wallFollowCounter(0),
    speedMultiplier(1)
{
    lastTrailPoint = currentPosition;
    snakeTarget = currentPosition;
    snakingInitialized = false;

    connect(&movementTimer, &QTimer::timeout, this, &Vacuum::tick);
    movementTimer.start(16);

    setFloorType(floorType);

    vacuumItem = scene->addEllipse(currentPosition.x(), currentPosition.y(), 10, 10,
                                   Qt::NoPen, QBrush(Qt::red));

    double angle = (rand() % 360) * M_PI / 180.0;
    velocity.setX(std::cos(angle) * speed);
    velocity.setY(std::sin(angle) * speed);

    //allAlgorithms = {"Random", "WallFollow", "Snaking", "Spiral"};
}

void Vacuum::setRoomAndDoor(const QRectF &room, const QRectF &door) {
    roomRect = room;
    doorRect = door;
}

void Vacuum::setSpeed(double inchesPerSecond) {
    if (inchesPerSecond >= 6.0 && inchesPerSecond <= 18.0)
        speed = inchesPerSecond;
}

void Vacuum::setBatteryLife(int minutes) {
    if (minutes >= 90 && minutes <= 200) {
        batteryLife = minutes * 60;
        remainingBattery = batteryLife;
    }
}

void Vacuum::setPathingAlgorithm(const QString &algorithm) {
    if (completedAlgorithms.contains(algorithm)) {
        qDebug() << "Algorithm '" << algorithm << "' already completed. Please press Start to run it again.";
        return;
    }

    pathingAlgorithm = algorithm;

    movingRight = true;
    wallDirection = 0;
    spiralAngle = 0;
    spiralRadius = 1;
    spiraling = true;
    wallFollowing = false;
    wallFollowCounter = 0;
    wallFollowing = atan2(velocity.y(), velocity.x());
    snakingMode = "vertical";
    snakingStepCounter = 0;
    snakingInitialized = false;
}

void Vacuum::setFloorType(const QString &type) {
    floorType = type;
    cleaningEfficiency = (type == "Hard") ? 0.9 :
                             (type == "Loop Pile") ? 0.75 :
                             (type == "Cut Pile") ? 0.7 :
                             (type == "Frieze-cut pile") ? 0.65 : 0.5;
    whiskerEfficiency = 0.3;

    // cleaningEfficiency = cleaningEfficiency;
    // whiskerEfficiency = whiskerEfficiency;
}

void Vacuum::setEfficiency(double vacuumEff, double whiskerEff) {
    cleaningEfficiency = vacuumEff;
    whiskerEfficiency = whiskerEff;

    cleaningEfficiency = vacuumEff;
    whiskerEfficiency = whiskerEff;
}

int Vacuum::getBatteryLife() const { return batteryLife; }
double Vacuum::getSpeed() const { return speed; }
QString Vacuum::getPathingAlgorithm() const { return pathingAlgorithm; }
QPointF Vacuum::getCurrentPosition() const { return currentPosition; }

void Vacuum::start() {
    stoppedByUser = false;
    movementTimer.start(16);
}

void Vacuum::stop() {
    stoppedByUser = true;
    movementTimer.stop();
}

void Vacuum::tick() {
    // 🔁 Check for stop by user or battery depleted
    if (stoppedByUser || remainingBattery <= 0) {
        stop();
        qDebug() << "Battery depleted!";
        emit batteryDepleted();
        return;
    }

    // ⏱ If running timed multi-algorithm mode ("All")
    if (timedExecutionEnabled) {
        algorithmTickCount++;
        globalTickCount++;

        if (globalTickCount >= maxGlobalTicks) {
            stop(); // ⛔️ Stops movementTimer
            timedExecutionEnabled = false;
            qDebug() << "🛑 Simulation ended after 10 minutes.";
            emit batteryDepleted();
            return; // ⛔️ Exit immediately
        }

        if (algorithmTickCount >= maxAlgorithmTicks) {
            completedAlgorithms.insert(pathingAlgorithm);
            algorithmTickCount = 0;

            int currentIndex = allAlgorithms.indexOf(pathingAlgorithm);
            if (currentIndex + 1 < allAlgorithms.size()) {
                QString nextAlg = allAlgorithms.at(currentIndex + 1);
                qDebug() << "🔄 Switching to next algorithm:" << nextAlg;
                setPathingAlgorithm(nextAlg);
            } else {
                stop();
                timedExecutionEnabled = false;
                qDebug() << "✅ All algorithms completed early.";
                emit batteryDepleted();
                return;
            }
        }
    }

    // Run the selected algorithm
    if (pathingAlgorithm == "Random") {
        moveRandomly();
    } else if (pathingAlgorithm == "Snaking") {
        moveSnaking();
    } else if (pathingAlgorithm == "WallFollow") {
        static bool activated = false;
        if (!activated) {
            moveRandomly();
            if (!checkBounds(currentPosition + velocity)) {
                activated = true;
            }
        } else {
            moveWallFollowing();
        }
    } else if (pathingAlgorithm == "Bounce") {
        moveBounce();
    } else if (pathingAlgorithm == "Spiral") {
        moveSpiral();
    }

    updatePosition();
    remainingBattery--;

    // 🕒 Time display
    int minutes = remainingBattery / 60;
    int seconds = remainingBattery % 60;
    qDebug().noquote() << QString("Battery Time Left: %1:%2")
                              .arg(minutes, 2, 10, QChar('0'))
                              .arg(seconds, 2, 10, QChar('0'));
}

void Vacuum::moveRandomly() {
    QPointF nextPos = currentPosition + velocity;

    if (!checkBounds(nextPos)) {
        double angle = (rand() % 360) * M_PI / 180.0;
        velocity.setX(std::cos(angle) * speed);
        velocity.setY(std::sin(angle) * speed);
    }
}

void Vacuum::moveSnaking() {
    const int margin = 10;
    const int rowStep = 3;

    double leftBound = roomRect.left() + margin;

    double rightBound = roomRect.right() - margin;
    double topBound = roomRect.top() + margin;
    double bottomBound = roomRect.bottom() - margin;

    if (!snakingInitialized) {
        currentPosition.setX(rightBound);
        currentPosition.setY(topBound);
        velocity = QPointF(-speed, 0);
        snakingInitialized = true;
        snakingDown = true;
    }

    QPointF nextPos = currentPosition + velocity;

    if (!checkBounds(nextPos)) {
        double nextY = currentPosition.y() + (snakingDown ? rowStep : -rowStep);

        if (nextY > bottomBound) {
            nextY = bottomBound;
            snakingDown = false;
        } else if (nextY < topBound) {
            nextY = topBound;
            snakingDown = true;
        }

        velocity.setX(-velocity.x());
        velocity.setY(0);
        currentPosition.setY(nextY);
        return;
    }

    velocity.setY(0);
}

void Vacuum::moveWallFollowing() {
    static double wallFollowAngle = 0.0;
    QPointF nextPos = currentPosition + velocity;

    if (checkBounds(nextPos)) return;

    for (int i = 0; i < 36; ++i) {
        wallFollowAngle += M_PI / 18;
        if (wallFollowAngle > 2 * M_PI) wallFollowAngle -= 2 * M_PI;

        QPointF tryVel(std::cos(wallFollowAngle) * speed,
                       std::sin(wallFollowAngle) * speed);
        QPointF tryPos = currentPosition + tryVel;

        if (checkBounds(tryPos)) {
            velocity = tryVel;
            return;
        }
    }

    velocity = -velocity;
}

void Vacuum::moveBounce() {
    QPointF nextPos = currentPosition + velocity;
    if (!checkBounds(nextPos)) handleWallCollision();
}

void Vacuum::moveSpiral() {
    if (roomRect.contains(currentPosition)) {
        spiralAngle += 0.15;
        spiralRadius += 0.05;
        velocity.setX(std::cos(spiralAngle) * spiralRadius);
        velocity.setY(std::sin(spiralAngle) * spiralRadius);
    } else {
        spiralAngle = 0;
        spiralRadius = 1;
        double angle = (rand() % 360) * M_PI / 180.0;
        velocity.setX(std::cos(angle) * speed);
        velocity.setY(std::sin(angle) * speed);
    }
}

void Vacuum::handleWallCollision() {
    double angle = atan2(velocity.y(), velocity.x());
    int sign = (rand() % 2 == 0) ? 1 : -1;
    angle += sign * (M_PI / 6);
    velocity.setX(std::cos(angle) * speed);
    velocity.setY(std::sin(angle) * speed);
}

void Vacuum::updatePosition() {
    QPointF newPosition = currentPosition + velocity;
    QRectF nextRect(newPosition.x(), newPosition.y(), 10, 10);
    bool throughDoor = doorRect.intersects(nextRect);

    // if (!roomRect.contains(nextRect) && !throughDoor) {
    //     if (nextRect.left() < roomRect.left() - 2 || nextRect.right() > roomRect.right() + 2 ||
    //         nextRect.top() < roomRect.top() - 2 || nextRect.bottom() > roomRect.bottom() + 2) {
    //         qDebug() << "Blocked move: outside room";
    //         return;
    //     }
    // }

    // if (checkBounds(newPosition)) {
    //     QPoint gridCoord(static_cast<int>(currentPosition.x()) / cellSize,
    //                      static_cast<int>(currentPosition.y()) / cellSize);

    //     //int &visits = cellVisitCount[gridCoord];
    //     //visits++;

    //     // int darkness = qBound(50, 255 - visits * 25, 200);
    //     // QColor grayShade(darkness, darkness, darkness);
    //     // QPen trailPen(grayShade);
    //     // trailPen.setWidth(2);

    //     // scene->addLine(QLineF(lastTrailPoint, currentPosition), trailPen);
    //     // lastTrailPoint = currentPosition;
    //     // cleanedCells.insert(gridCoord);

    //     // currentPosition = newPosition;
    //     // vacuumItem->setRect(currentPosition.x(), currentPosition.y(), 10, 10);
    // }
}

bool Vacuum::checkBounds(const QPointF &newPosition) {
    QRectF nextRect(newPosition.x(), newPosition.y(), 10, 10);

    if (!scene->sceneRect().contains(nextRect))
        return false;

    if (!roomRect.contains(nextRect) && !doorRect.intersects(nextRect))
        return false;

    for (auto *obstacle : std::as_const(obstacles)) {
        if (obstacle->rect().intersects(nextRect))
            return false;
    }

    return true;
}

void Vacuum::setSpeedMultiplier(int multiplier) {
    speedMultiplier = multiplier;

    // Adjust timer rate: higher speed = faster ticks
    int interval = 16 / multiplier;
    movementTimer.setInterval(qMax(1, interval));

    // Scale cleaning efficiency (higher speed = less effective)
    double scale = 1.0 / multiplier;
    cleaningEfficiency = cleaningEfficiency * scale;
    whiskerEfficiency = baseWhiskerEfficiency * scale;

    // Update velocity vector based on new speed and direction
    double angle = atan2(velocity.y(), velocity.x());
    velocity.setX(std::cos(angle) * speed * multiplier);
    velocity.setY(std::sin(angle) * speed * multiplier);
}

void Vacuum::advanceToNextAlgorithm() {
    for (const QString &alg : std::as_const(selectedAlgs)) {
        if (!completedAlgorithms.contains(alg)) {
            qDebug() << "Switching to next algorithm:" << alg;
            setPathingAlgorithm(alg);
            setBatteryLife(150);
            remainingBattery = batteryLife;
            start();
            return;
        }
    }

    qDebug() << "All algorithms completed. Simulation finished.";
    stoppedByUser = true;
    movementTimer.stop();
    emit batteryDepleted();
}

void Vacuum::rotateVacuum(double angleDelta) {
    currentRotation += angleDelta;
    vacuumItem->setRotation(currentRotation);
}

void Vacuum::setObstacles(const QList<QGraphicsRectItem*>& obs) {
    obstacles = obs;
}

QGraphicsEllipseItem* Vacuum::getVacuumItem() const {
    return vacuumItem;
}

double Vacuum::getCoveragePercent(int roomWidth, int roomHeight, const QList<QGraphicsRectItem*>& obstacles) const {
    QSet<QPoint> blockedCells;

    // for (QGraphicsRectItem* obs : obstacles) {
    //     QRectF rect = obs->rect();
    //     for (int x = rect.left(); x <= rect.right(); x += cellSize) {
    //         for (int y = rect.top(); y <= rect.bottom(); y += cellSize) {
    //             blockedCells.insert(QPoint(x / cellSize, y / cellSize));
    //         }
    //     }
    // }

    // int totalCells = (roomWidth / cellSize) * (roomHeight / cellSize);
    // int cleanableCells = totalCells - blockedCells.size();
    // int cleaned = 0;

    // for (const QPoint& cell : cleanedCells) {
    //     if (!blockedCells.contains(cell)) {
    //         cleaned++;
    //     }
    // }

    //return cleanableCells > 0 ? (cleaned * 100.0 / cleanableCells) : 0.0;

    return 0.0;
}

void Vacuum::runTimedAlgorithms() {
    timedExecutionEnabled = true;
    completedAlgorithms.clear();
    algorithmTickCount = 0;
    globalTickCount = 0;          // ⏱ Start fresh
    maxAlgorithmTicks = 36000;    // can still leave this, but won't be used here
    setPathingAlgorithm(allAlgorithms.first());
    start();
}

// MDELAUNE 4/22: This is where im getting errors, is not setting selected algs to selection. i prev had them j assigned instead of in a loop but that didnt work either.
void Vacuum::setSelectedAlgs(QStringList selection){
    for (int i = 0; i < selection.size(); i++){
        selectedAlgs.append(selection[i]);
    }
}
