#include "editwindow.h"
#include "./ui_editwindow.h"

#include <QDebug>
#include <QStyleFactory>
#include <QFileDialog>

EditWindow::EditWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::EditWindow)
{
    ui->setupUi(this);
}

EditWindow::~EditWindow()
{
    delete ui;
    delete m_menu;
    delete scene;
    delete house;
    delete edit;
}

void EditWindow::setupMenu()
{

    // setup menu icons
    ui->action_New->setIcon(QIcon::fromTheme("document-new"));
    ui->action_Open->setIcon(QIcon::fromTheme("document-open"));
    ui->action_Save->setIcon(QIcon::fromTheme("document-save"));
    ui->action_SaveAs->setIcon(QIcon::fromTheme("document-save-as"));
    ui->action_Quit->setIcon(QIcon::fromTheme("application-exit"));
    ui->action_About->setIcon(QIcon::fromTheme("help-about"));


    m_menu = new Menu(house);
    connect(ui->action_New, &QAction::triggered, m_menu, &Menu::menuNew);
    connect(ui->action_Open, &QAction::triggered, m_menu, &Menu::menuOpen);
    connect(ui->action_Save, &QAction::triggered, m_menu, &Menu::menuSave);
    connect(ui->action_SaveAs, &QAction::triggered, m_menu, &Menu::menuSaveAs);
    connect(ui->action_Quit, &QAction::triggered, m_menu, &Menu::menuQuit);
    connect(ui->action_About, &QAction::triggered, m_menu, &Menu::menuAbout);

}

void EditWindow::setupScene()
{
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    house = new House(scene);
    QString default_plan = ":/Default/default_plan.json";
    house->loadPlan(default_plan);
    house->setNewID();
    setupMenu();
    setupToolButtons();
    ui->floorID->setText("Floorplan ID: " + QString::number(house->getFloorplanId()));
}

bool EditWindow::setupSceneFromFile(){
    QString plan = QFileDialog::getOpenFileName(this, "Select Floorplan File", "C://", "JSON (*.json)");

    QFile planFile(plan);
    if (planFile.open(QIODevice::ReadOnly)){
        scene = new QGraphicsScene(this);
        ui->graphicsView->setScene(scene);
        house = new House(scene);
        house->setScene(scene);
        house->loadPlan(plan);
        setupMenu();
        setupToolButtons();
        ui->floorID->setText("Floorplan ID: " + QString::number(house->getFloorplanId()));
        return true;
    }
    else{
        return false;
    }

}

void EditWindow::setupToolButtons()
{

    ui->btn_squareRoom->setIcon(QIcon(":/Images/Images/square.png"));
    ui->btn_rectangleRoom->setIcon(QIcon(":/Images/Images/rectangle.png"));
    ui->btn_door->setIcon(QIcon(":/Images/Images/door.png"));
    ui->btn_vacuum->setIcon(QIcon(":/Images/Images/vacuum.png"));


    ui->btn_Chest->setIcon(QIcon(":/Images/Images/chest.png"));
    ui->btn_Table->setIcon(QIcon(":/Images/Images/table.png"));
    ui->btn_Chair->setIcon(QIcon(":/Images/Images/chair.png"));

    ui->btn_hardFloor->setIcon(QIcon(":/Images/Images/hardfloor.png"));
    ui->btn_loopPile->setIcon(QIcon(":/Images/Images/looppile.png"));
    ui->btn_cutPile->setIcon(QIcon(":/Images/Images/cutpile.png"));
    ui->btn_friezeCut->setIcon(QIcon(":/Images/Images/frieze.png"));

    ui->btn_squareRoom->setIconSize(QSize(32, 32));
    ui->btn_rectangleRoom->setIconSize(QSize(32,32));
    ui->btn_door->setIconSize(QSize(32,32));
    ui->btn_vacuum->setIconSize(QSize(32,32));

    ui->btn_Chest->setIconSize(QSize(32,32));
    ui->btn_Table->setIconSize(QSize(32,32));
    ui->btn_Chair->setIconSize(QSize(32,32));

    ui->btn_hardFloor->setIconSize(QSize(32,32));
    ui->btn_loopPile->setIconSize(QSize(32,32));
    ui->btn_cutPile->setIconSize(QSize(32,32));
    ui->btn_friezeCut->setIconSize(QSize(32,32));

    edit = new Draw(house, scene);
    connect(ui->btn_squareRoom, &QToolButton::clicked, edit, &Draw::addSquareRoom);
    connect(ui->btn_rectangleRoom, &QToolButton::clicked, edit, &Draw::addRectRoom);
    connect(ui->btn_door, &QToolButton::clicked, edit, &Draw::addDoor);
    connect(ui->btn_vacuum, &QToolButton::clicked, edit, &Draw::addVacuum);

    connect(ui->btn_clear, &QPushButton::clicked, house, &House::clear);
    connect(ui->btn_delete, &QPushButton::clicked, house, &House::deleteItem);
    connect(ui->btn_rotate, &QPushButton::clicked, house, &House::rotate);

    connect(ui->btn_Chest, &QToolButton::clicked, edit, &Draw::addChest);
    connect(ui->btn_Table, &QToolButton::clicked, edit, &Draw::addTable);
    connect(ui->btn_Chair, &QToolButton::clicked, edit, &Draw::addChair);

    connect(ui->btn_hardFloor, &QToolButton::clicked, edit, &Draw::changeFlooring);
    connect(ui->btn_loopPile, &QToolButton::clicked, edit, &Draw::changeFlooring);
    connect(ui->btn_cutPile, &QToolButton::clicked, edit, &Draw::changeFlooring);
    connect(ui->btn_friezeCut, &QToolButton::clicked, edit, &Draw::changeFlooring);

}
