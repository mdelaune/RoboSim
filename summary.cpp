#include "summary.h"
#include "ui_summary.h"

summary::summary(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::summary)
{
    ui->setupUi(this);
}

summary::~summary()
{
    delete ui;
}
