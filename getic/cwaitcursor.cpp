
#include <QApplication>
#include "cwaitcursor.h"

CWaitCursor::CWaitCursor()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
}


CWaitCursor::~CWaitCursor()
{
    Restore();
}

void CWaitCursor::Restore()
{
    QApplication::restoreOverrideCursor();
}
