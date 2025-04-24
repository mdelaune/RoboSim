#include "edit.h"
#include "./ui_edit.h"

#include <QDebug>
#include <QStyleFactory>
#include <QFileDialog>

edit::edit(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::edit)
{
    ui->setupUi(this);
}

edit::~edit()
{
    delete ui;
}

void edit::setupMenu()
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
    connect(ui->action_About, &QAction::triggered, m_menu, &Menu::menuQuit);

}
void edit::setupScene()
{
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    house = new House(scene);
    QString default_plan = "../../default_plan.json";
    house->loadPlan(default_plan);
    setupMenu();
    setupToolButtons();
}

void edit::setupSceneFromFile(){
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    house = new House(scene);
    //house->file_name =
    QString plan = QFileDialog::getOpenFileName(this, "Select Floorplan File", "C://", "JSON (*.json)");
    house->loadPlan(plan);
    setupMenu();
    setupToolButtons();
}

void edit::setupToolButtons()
{
    ui->btn_squareRoom->setIcon(QIcon("../../Images/square.png"));
    ui->btn_rectangleRoom->setIcon(QIcon("../../Images/rectangle.png"));
    ui->btn_door->setIcon(QIcon("../../Images/door.png"));

    ui->btn_Chest->setIcon(QIcon("../../Images/chest.png"));
    ui->btn_Table->setIcon(QIcon("../../Images/table.png"));
    ui->btn_Chair->setIcon(QIcon("../../Images/chair.png"));

    ui->btn_hardFloor->setIcon(QIcon("../../Images/hardfloor.png"));
    ui->btn_loopPile->setIcon(QIcon("../../Images/looppile.png"));
    ui->btn_cutPile->setIcon(QIcon("../../Images/cutpile.png"));
    ui->btn_friezeCut->setIcon(QIcon("../../Images/frieze.png"));

    ui->btn_squareRoom->setIconSize(QSize(32, 32));
    ui->btn_rectangleRoom->setIconSize(QSize(32,32));
    ui->btn_door->setIconSize(QSize(32,32));

    ui->btn_Chest->setIconSize(QSize(32,32));
    ui->btn_Table->setIconSize(QSize(32,32));
    ui->btn_Chair->setIconSize(QSize(32,32));

    ui->btn_hardFloor->setIconSize(QSize(32,32));
    ui->btn_loopPile->setIconSize(QSize(32,32));
    ui->btn_cutPile->setIconSize(QSize(32,32));
    ui->btn_friezeCut->setIconSize(QSize(32,32));

    draw = new Draw(house, scene);
    connect(ui->btn_squareRoom, &QToolButton::clicked, draw, &Draw::addSquareRoom);
    connect(ui->btn_rectangleRoom, &QToolButton::clicked, draw, &Draw::addRectRoom);
    connect(ui->btn_door, &QToolButton::clicked, draw, &Draw::addDoor);

    connect(ui->btn_clear, &QPushButton::clicked, house, &House::clear);
    connect(ui->btn_delete, &QPushButton::clicked, house, &House::deleteItem);

    connect(ui->btn_Chest, &QToolButton::clicked, draw, &Draw::addChest);
    connect(ui->btn_Table, &QToolButton::clicked, draw, &Draw::addTable);
    connect(ui->btn_Chair, &QToolButton::clicked, draw, &Draw::addChair);

    connect(ui->btn_hardFloor, &QToolButton::clicked, draw, &Draw::changeFlooring);
    connect(ui->btn_loopPile, &QToolButton::clicked, draw, &Draw::changeFlooring);
    connect(ui->btn_cutPile, &QToolButton::clicked, draw, &Draw::changeFlooring);
    connect(ui->btn_friezeCut, &QToolButton::clicked, draw, &Draw::changeFlooring);

}
