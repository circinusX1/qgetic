#include "stdafx.h"
#include "cdlgcompprogress.h"
#include "geticapp.h"
#include "geticmainwnd.h"
#include "document.h"

CDlgCompProgress::CDlgCompProgress(QWidget *parent) :
    QWidget(parent),_idx(0),_progValue(0)
{
    _dlg = new QProgressDialog();
    _dlg->setRange(0,100);

    QObject::connect(_dlg, SIGNAL(canceled()), &_futureWatcher, SLOT(cancel()));
    //_dlg->exec();
}

CDlgCompProgress::~CDlgCompProgress()
{
    _futureWatcher.waitForFinished();
}


void    CDlgCompProgress::LogPos(u_int32_t flag, const v3d_t& pos, const _tchar* lpszFormat,...)
{
    va_list args;
    va_start(args, lpszFormat);

    int32_t  nBuf;
    char szBuffer[512];
    nBuf = snprintf(szBuffer, sizeof(szBuffer) / sizeof(char), lpszFormat, args);
    assert(nBuf < sizeof(szBuffer));
    if(szBuffer[0]=='W')
    {
        flag |= LOG_WARNING;
        _mapidPoint[_idx++] = pos;
    }
    else if(szBuffer[0]=='E')
    {
        flag |= LOG_ERROR;
        _mapidPoint[_idx++] = pos;

    }
    _dlg->setLabelText(szBuffer);
    va_end(args);
}

void    CDlgCompProgress::Log(u_int32_t flag, const _tchar* lpszFormat,...)
{
    va_list args;
    va_start(args, lpszFormat);

    int32_t  nBuf;
    char szBuffer[512];
    nBuf = snprintf(szBuffer, sizeof(szBuffer) / sizeof(char), lpszFormat, args);
    assert(nBuf < sizeof(szBuffer));

    _dlg->setLabelText(szBuffer);
    va_end(args);
}

void CDlgCompProgress::Cancel()
{
   _mapidPoint.clear();
//   _repLB.ResetContent();
   Pd->SetErrorPoint(V0);
   assert(0);
   //Pd->OnBucompile();
   Sleep(1000);
   delete _dlg;
}


void    CDlgCompProgress::Progress(int32_t pos)
{
    if(pos==-1)
    {
        _progValue = 0;
    }
    else if(_progValue != pos)
    {
        _progValue = pos;
    }
    emit _dlg->setValue(_progValue);
}
