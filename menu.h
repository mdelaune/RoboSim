#ifndef MENU_H
#define MENU_H

#include <QObject>
#include "house.h"

class Menu : public QObject
{
public:
    Menu(House *house);
    void menuNew();
    void menuOpen();
    void menuSave();
    void menuSaveAs();
    void menuQuit();
    void menuUndo();
    void menuRedo();
    void menuAbout();

    QString getFilename(const QString& dialogTitle, bool forSaving = false);
    void saveToFile(const QString& filename);

private:
    House *m_house;
};

#endif // MENU_H
