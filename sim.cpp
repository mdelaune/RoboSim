#include "sim.h"
#include "ui_sim.h"

sim::sim(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::sim)
{
    ui->setupUi(this);
}

sim::~sim()
{
    delete ui;
}
