#include <QDebug>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>

#include "menu.h"

Menu::Menu(House *house)
{
    m_house = house;
}

void Menu::menuNew()
{
    m_house->createNewFloorplan();
    qDebug() << "NEW";
}

QString Menu::getFilename(const QString& dialogTitle, bool forSaving)
{
    QString filename;

    if (forSaving) {
        filename = QFileDialog::getSaveFileName(nullptr, dialogTitle, "", "JSON Files (*.json)", nullptr);
    } else {
        filename = QFileDialog::getOpenFileName(nullptr, dialogTitle, "", "JSON Files (*.json)");
    }

    if (filename.isEmpty()) {
        qDebug() << "No file selected";
        return QString();
    }

    return filename;
}

void Menu::menuOpen()
{
    QString filename = getFilename("Open File", false);
    if (!filename.isEmpty()) {
        qDebug() << "Selected file:" << filename;
        m_house->loadPlan(filename);
        qDebug() << "OPEN";
    }
}

void Menu::saveToFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "Error Opening File",
                             "Unable to open file. Please make sure the file you selected still exists.");
        qDebug() << "Error opening file for writing:" << file.errorString();
        return;
    }

    m_house->set_floorplanName(filename);

    QTextStream out(&file);
    out << m_house->toJson().toJson(QJsonDocument::Indented);
    file.close();
}

void Menu::menuSave()
{
    if(!errorChecks())
    {
        return;
    }

    QString filename = m_house->get_floorplanName();
    if(filename == "Untitled")
    {
        menuSaveAs();
        return;
    }

    if (!filename.isEmpty()) {
        saveToFile(filename);
        qDebug() << "SAVE";
    }
}

void Menu::menuSaveAs()
{
    if(!errorChecks())
    {
        return;
    }

    QString filename = getFilename("Name", true);
    if (!filename.isEmpty()) {
        saveToFile(filename);
        qDebug() << "SAVE AS";
    }
}

bool Menu::errorChecks()
{
    if(m_house->validateTotalAreaBeforeSave() > 0)
    {
        QMessageBox errorBox;
        errorBox.setWindowTitle("Floor Plan Error");
        errorBox.setIcon(QMessageBox::Warning);
        errorBox.setText("Invalid Floor Plan Area");
        errorBox.setInformativeText("The total floor plan area is outside the acceptable range.\n\nPlease ensure your floor plan is not too large.");
        errorBox.setStandardButtons(QMessageBox::Ok);
        errorBox.exec();
        return false;
    }
    else if(m_house->validateTotalAreaBeforeSave() < 0)
    {
        QMessageBox errorBox;
        errorBox.setWindowTitle("Floor Plan Error");
        errorBox.setIcon(QMessageBox::Warning);
        errorBox.setText("Invalid Floor Plan Area");
        errorBox.setInformativeText("The total floor plan area is outside the acceptable range.\n\nPlease ensure your floor plan is not too small.");
        errorBox.setStandardButtons(QMessageBox::Ok);
        errorBox.exec();
        return false;
    }

    if(!m_house->validateRoomConnectivity())
    {
        QMessageBox errorBox;
        errorBox.setWindowTitle("Floor Plan Error");
        errorBox.setIcon(QMessageBox::Warning);
        errorBox.setText("Invalid Floor Plan");
        errorBox.setInformativeText("Each room must share a wall with at least one other room. No room may be isolated or intersect with another room.");
        errorBox.setStandardButtons(QMessageBox::Ok);
        errorBox.exec();
        return false;
    }

    if(!m_house->validateNoRoomIntersections())
    {
        QMessageBox errorBox;
        errorBox.setWindowTitle("Floor Plan Error");
        errorBox.setIcon(QMessageBox::Warning);
        errorBox.setText("Invalid Floor Plan");
        errorBox.setInformativeText("No room may intersect with another room.");
        errorBox.setStandardButtons(QMessageBox::Ok);
        errorBox.exec();
        return false;
    }

    if(!m_house->validateDoorsOnWalls())
    {
        QMessageBox errorBox;
        errorBox.setWindowTitle("Floor Plan Error");
        errorBox.setIcon(QMessageBox::Warning);
        errorBox.setText("Invalid Floor Plan");
        errorBox.setInformativeText("Every door must be on a wall.");
        errorBox.setStandardButtons(QMessageBox::Ok);
        errorBox.exec();
        return false;
    }

    if(!m_house->validateEveryRoomHasDoor())
    {
        QMessageBox errorBox;
        errorBox.setWindowTitle("Floor Plan Error");
        errorBox.setIcon(QMessageBox::Warning);
        errorBox.setText("Invalid Floor Plan");
        errorBox.setInformativeText("Every room must have a door.");
        errorBox.setStandardButtons(QMessageBox::Ok);
        errorBox.exec();
        return false;
    }

    if(!m_house->validateObstructionPlacements())
    {
        QMessageBox errorBox;
        errorBox.setWindowTitle("Floor Plan Error");
        errorBox.setIcon(QMessageBox::Warning);
        errorBox.setText("Invalid Floor Plan");
        errorBox.setInformativeText("Invalid obstruction placement. Obstructions can not intersect rooms, block doors, or exist outside a room.");
        errorBox.setStandardButtons(QMessageBox::Ok);
        errorBox.exec();
        return false;
    }

    if(!m_house->validateNoObstructionIntersections())
    {
        QMessageBox errorBox;
        errorBox.setWindowTitle("Floor Plan Error");
        errorBox.setIcon(QMessageBox::Warning);
        errorBox.setText("Invalid Floor Plan");
        errorBox.setInformativeText("Obstructions can not intersect each other.");
        errorBox.setStandardButtons(QMessageBox::Ok);
        errorBox.exec();
        return false;
    }

    if(!m_house->isVacuumPositionValid())
    {
        QMessageBox errorBox;
        errorBox.setWindowTitle("Floor Plan Error");
        errorBox.setIcon(QMessageBox::Warning);
        errorBox.setText("Invalid Floor Plan");
        errorBox.setInformativeText("Vacuum position is invalid. Please make sure the vacuum is inside a room and not overlapping with walls, doors, or obstructions.");
        errorBox.setStandardButtons(QMessageBox::Ok);
        errorBox.exec();
        return false;
    }

    return true;
}

void Menu::menuQuit()
{
    qDebug() << "QUIT";
}

void Menu::menuUndo()
{
    qDebug() << "UNDO";
}

void Menu::menuRedo()
{
    qDebug() << "REDO";
}

void Menu::menuAbout()
{
    QMessageBox aboutBox;
    aboutBox.setWindowTitle("About");
    aboutBox.setIcon(QMessageBox::Information);
    aboutBox.setText("<h3>Clean Sweep</h3>");
    aboutBox.setInformativeText(
        "Version 1.0<br><br>"
        "A robot vacuum simulation.<br><br>"
        "Authors: <br>"
        "Maddie Delaune<br>"
        "Allan Benjume<br>"
        "Quinton Pouncy<br>"
        "Vasilisa Douglas<br>"
        );
    aboutBox.setStandardButtons(QMessageBox::Ok);
    aboutBox.exec();
    qDebug() << "ABOUT";
}
