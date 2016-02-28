#include "qgemenu.h"

QGeMenu::QGeMenu(QWidget *parent) :
    QMenuBar(parent)
{
}


void QGeMenu::triggered(QAction *action)
{

    QMenuBar::triggered(action);

}
