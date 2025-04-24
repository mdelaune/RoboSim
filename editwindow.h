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
    void setupSceneFromFile();
    void setupMenu();
    void setupToolButtons();

private:
    Ui::EditWindow *ui;
    Menu *m_menu;
    House *house;
    Draw *edit;
    QGraphicsScene *scene;
    // void setupMenu();
    // void setupToolButtons();
};

#endif // EDITWINDOW_H
