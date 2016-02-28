#ifndef QGEMENU_H
#define QGEMENU_H

#include <QMenuBar>

class QGeMenu : public QMenuBar
{
    Q_OBJECT
public:
    explicit QGeMenu(QWidget *parent = 0);

signals:

    void triggered(QAction *action);
public slots:

};

#endif // QGEMENU_H
