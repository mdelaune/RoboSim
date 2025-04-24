#ifndef EDIT_H
#define EDIT_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QString>
#include "menu.h"
#include "house.h"
#include "draw.h"

namespace Ui {
class edit;
}

class edit : public QMainWindow
{
    Q_OBJECT

public:
    explicit edit(QWidget *parent = nullptr);
    ~edit();
    void setupScene();
    void setupSceneFromFile();
    void setupMenu();
    void setupToolButtons();

private:
    Ui::edit *ui;
    Menu *m_menu;
    House *house;
    Draw *draw;
    QGraphicsScene *scene;
};

#endif // EDIT_H
