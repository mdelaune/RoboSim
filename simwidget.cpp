#include "simwidget.h"
#include "ui_simwidget.h"

simWidget::simWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::simWidget)
{
    ui->setupUi(this);
}

simWidget::~simWidget()
{
    delete ui;
}
