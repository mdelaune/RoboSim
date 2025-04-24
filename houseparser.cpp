#include "houseparser.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

HouseParser::HouseParser(const QString &filePath)
{
    parseJson(filePath);
}

void HouseParser::parseJson(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return;

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return;

    QJsonObject obj = doc.object();

    QJsonArray roomArray = obj["rooms"].toArray();
    for (int i = 0; i < roomArray.size(); ++i) {
        QJsonObject room = roomArray[i].toObject();
        QRectF rect(
            QPointF(room["x_topLeft"].toDouble(), room["y_topLeft"].toDouble()),
            QPointF(room["x_bottomRight"].toDouble(), room["y_bottomRight"].toDouble())
            );
        rooms2.append(rect.normalized());
    }

    QJsonArray doorArray = obj["doors"].toArray();
    for (int i = 0; i < doorArray.size(); ++i) {
        QJsonObject door = doorArray[i].toObject();
        doors2.append(QPointF(door["x"].toDouble(), door["y"].toDouble()));
    }

    QJsonArray obstructionArray = obj["obstructions"].toArray();
    for (int i = 0; i < obstructionArray.size(); ++i) {
        QJsonObject obs = obstructionArray[i].toObject();
        QRectF rect(
            QPointF(obs["x_topLeft"].toDouble(), obs["y_topLeft"].toDouble()),
            QPointF(obs["x_bottomRight"].toDouble(), obs["y_bottomRight"].toDouble())
            );
        bool isChest = obs["is_chest"].toBool();
        obstructions2.append({ rect.normalized(), isChest });
    }
}

QList<QRectF> HouseParser::getRooms2() const { return rooms2; }
QList<QPointF> HouseParser::getDoors2() const { return doors2; }
QList<Obstruction2> HouseParser::getObstructions2() const { return obstructions2; }
