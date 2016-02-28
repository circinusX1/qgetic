#ifndef CDLGCOMPPROGRESS_H
#define CDLGCOMPPROGRESS_H

#include <QWidget>
#include <map>
#include <baselib.h>
#include <QProgressDialog>
#include <QFutureWatcher>


#define LOG_WARNING 0x100000
#define LOG_ERROR   0x200000

using namespace std;

class CDlgCompProgress : public QWidget
{
    Q_OBJECT
public:
    explicit CDlgCompProgress(QWidget *parent = 0);
    ~CDlgCompProgress();
    void    Progress(int32_t pos=-1);
    void    LogPos(u_int32_t flag, const v3d_t& pos, const _tchar* lpszFormat,...);
    void    Log(u_int32_t flag, const _tchar*,...);
signals:
    
public slots:
    void Cancel();

private:
    QProgressDialog* _dlg;
    QFutureWatcher<void> _futureWatcher;

    int32_t             _idx;
    int32_t             _progValue;
    map<int32_t,v3d_t>     _mapidPoint;
    char            _progTxt[128];
};

#endif // CDLGCOMPPROGRESS_H
