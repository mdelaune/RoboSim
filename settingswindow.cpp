#include "settingswindow.h"
#include "ui_settingswindow.h"

#include <QCheckBox>
#include<QMessageBox>
#include <QDebug>

SettingsWindow::SettingsWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    setupAlgorithmList();
    this->setFocus();

    // Connect START button to handle the final selection
    connect(ui->pushButtonSave, &QPushButton::clicked, this, &SettingsWindow::handleSaveClicked);
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::setupAlgorithmList()
{
    QStringList algorithms = {"Random", "Snaking", "Wall Follow", "Spiral"};

    QFont font = ui->selectAlgorithms->font();
    font.setPointSize(20);
    ui->selectAlgorithms->setFont(font);

    ui->selectAlgorithms->clear();

    for (const QString &algo : algorithms) {
        // Create a QWidget to hold the checkbox and label
        QWidget *widget = new QWidget();
        QHBoxLayout *layout = new QHBoxLayout(widget); // Horizontal layout for checkbox + label
        layout->setAlignment(Qt::AlignCenter);  // Align everything in the center
        layout->setContentsMargins(0, 0, 0, 0); // Remove margins to avoid cutoff

        // Create the checkbox for each algorithm
        QCheckBox *checkbox = new QCheckBox(algo);
        checkbox->setChecked(true);  // All checked by default
        checkbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);  // Allow the checkbox to expand
        layout->addWidget(checkbox);

        // Set the widget's size to adjust properly within the list
        widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        // Create a QListWidgetItem and set it as user-checkable
        QListWidgetItem *item = new QListWidgetItem(ui->selectAlgorithms);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);

        // Set the custom widget inside the item
        ui->selectAlgorithms->setItemWidget(item, widget);
    }

    // Adjust the QListWidget to handle proper resizing
    ui->selectAlgorithms->setSpacing(10);  // Set spacing between items
    ui->selectAlgorithms->setUniformItemSizes(true);  // Ensure uniform item sizes
}

void SettingsWindow::setupLineEdits()
{
    QFont font;
    font.setPointSize(20);

    ui->batteryLifeEdit->setFont(font);
    ui->vacuumEfficiencyEdit->setFont(font);
    ui->whiskerEfficiencyEdit->setFont(font);
    ui->speedEdit->setFont(font);

    ui->batteryLifeEdit->setText("150");

    qDebug() << floorCovering;

    if (floorCovering == "hard_floor") ui->vacuumEfficiencyEdit->setText("90");
    else if (floorCovering == "loop_pile") ui->vacuumEfficiencyEdit->setText("75");
    else if (floorCovering == "cut_pile") ui->vacuumEfficiencyEdit->setText("70");
    else if (floorCovering == "frieze_cut") ui->vacuumEfficiencyEdit->setText("65");

    ui->whiskerEfficiencyEdit->setText("30");
    ui->speedEdit->setText("12");

    ui->batteryLifeEdit->setValidator(new QIntValidator(1, 200, this));
    ui->vacuumEfficiencyEdit->setValidator(new QIntValidator(10, 90, this));
    ui->whiskerEfficiencyEdit->setValidator(new QIntValidator(10, 50, this));
    ui->speedEdit->setValidator(new QIntValidator(6, 18, this));
}

bool SettingsWindow::validateInputs()
{
    bool hasError = false;
    QString errorMessages;

    int batteryLife = ui->batteryLifeEdit->text().toInt();
    if (batteryLife < 1 || batteryLife > 200) {
        errorMessages += "Battery life must be between 90–200\n";
        hasError = true;
    }

    int vacuumEfficiency = ui->vacuumEfficiencyEdit->text().toInt();
    if (vacuumEfficiency < 10 || vacuumEfficiency > 90) {
        errorMessages += "Vacuum Efficiency must be between 10–90\n";
        hasError = true;
    }

    int whiskerEfficiency = ui->whiskerEfficiencyEdit->text().toInt();
    if (whiskerEfficiency < 10 || whiskerEfficiency > 50) {
        errorMessages += "Whisker Efficiency must be between 10-50\n";
        hasError = true;
    }

    int speed = ui->speedEdit->text().toInt();
    if (speed < 6 || speed > 18) {
        errorMessages += "Speed must be between 6–18\n";
        hasError = true;
    }

    if (hasError) {
        QMessageBox::critical(this, "Input Error", errorMessages.trimmed());
    }

    return hasError;
}

void SettingsWindow::handleSaveClicked()
{

    if (validateInputs()) {
        return;
    }

    QStringList selectedAlgorithms;

    for (int i = 0; i < ui->selectAlgorithms->count(); ++i) {
        QListWidgetItem *item = ui->selectAlgorithms->item(i);
        QWidget *widget = ui->selectAlgorithms->itemWidget(item);
        QCheckBox *checkbox = widget->findChild<QCheckBox *>();

        if (checkbox && checkbox->isChecked()) {
            selectedAlgorithms << checkbox->text();
        }
    }

    if (selectedAlgorithms.isEmpty()) {
        QMessageBox::warning(this, "No Algorithms Selected", "Please select at least one algorithm before proceeding.");
        return;
    }

    emit settingsUpdated(ui->batteryLifeEdit->text().toInt(),
                        ui->vacuumEfficiencyEdit->text().toInt(),
                        ui->whiskerEfficiencyEdit->text().toInt(),
                        ui->speedEdit->text().toInt(),
                        selectedAlgorithms);
    this->close();
}

void SettingsWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    setupLineEdits();
}
