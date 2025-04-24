#ifndef SIMWIDGET_H
#define SIMWIDGET_H

#include <QWidget>

namespace Ui {
class simWidget;
}

class simWidget : public QWidget
{
    Q_OBJECT

public:
    explicit simWidget(QWidget *parent = nullptr);
    ~simWidget();

private:
    Ui::simWidget *ui;
};

#endif // SIMWIDGET_H
