
#include <QMessageBox>
#include <geticapp.h>
#include "winwrap.h"


pid_t ThrID;

int32_t TAfxMessageBox(const char* t, int32_t flag)
{
    QMessageBox msgBox;
    QString text = t;
    msgBox.setText(QCoreApplication::applicationName());
    msgBox.setInformativeText(text);
    //msgBox.setStandardButtons(flag);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    return msgBox.exec();
}


