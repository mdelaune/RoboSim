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
    m_house->clear();
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

    QTextStream out(&file);
    out << m_house->toJson().toJson(QJsonDocument::Indented);
    file.close();
}

void Menu::menuSave()
{
    QString filename = getFilename("Name", true);
    if (!filename.isEmpty()) {
        saveToFile(filename);
        qDebug() << "SAVE";
    }
}

void Menu::menuSaveAs()
{
    QString filename = getFilename("Name", true);
    if (!filename.isEmpty()) {
        saveToFile(filename);
        qDebug() << "SAVE AS";
    }
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
    qDebug() << "ABOUT";
}
