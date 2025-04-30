#ifndef EDITWINDOW_H
#define EDITWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QString>
#include "menu.h"
#include "house.h"
#include "draw.h"

namespace Ui {
class EditWindow;
}

class EditWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EditWindow(QWidget *parent = nullptr);
    ~EditWindow();
    void setupScene();
    bool setupSceneFromFile();
    void setupMenu();
    void setupToolButtons();

    House *house;

private:
    Ui::EditWindow *ui;
    Menu *m_menu;
    Draw *edit;
    QGraphicsScene *scene;
    // void setupMenu();
    // void setupToolButtons();
};

#endif // EDITWINDOW_H
