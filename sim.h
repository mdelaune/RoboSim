#ifndef SIM_H
#define SIM_H

#include <QWidget>

namespace Ui {
class sim;
}

class sim : public QWidget
{
    Q_OBJECT

public:
    explicit sim(QWidget *parent = nullptr);
    ~sim();

private:
    Ui::sim *ui;
};

#endif // SIM_H
