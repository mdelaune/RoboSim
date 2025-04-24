#ifndef SUMMARY_H
#define SUMMARY_H

#include <QWidget>

namespace Ui {
class summary;
}

class summary : public QWidget
{
    Q_OBJECT

public:
    explicit summary(QWidget *parent = nullptr);
    ~summary();

private:
    Ui::summary *ui;
};

#endif // SUMMARY_H
