#ifndef WINWRAP_H
#define WINWRAP_H

#ifndef WINDOWS


#include <pthread.h>
#include <QMessageBox>
#include <unistd.h>

int32_t TAfxMessageBox(const char* t, int32_t flag=0);


typedef u_int32_t WPARAM;
typedef u_int32_t LPARAM;

inline pid_t GetCurrentThreadId(){ return (pid_t)pthread_self();}
inline void Sleep(int32_t ms) {usleep(ms*1000);}


#define Beep(x,y)
inline void PostQuitMessage(int32_t x){Q_UNUSED(x);}


#define WM_USER         0xF0000
#define WM_COMPILEDONE  WM_USER+1001
#define WM_USR_REPAINT  WM_USER+1002
#define WM_POSTINIT		WM_USER+1006
#define WM_TEXTURE		WM_USER+1007
#define WM_SBT_MSG      WM_USER+1008
#define WM_VIEW_DESTROY WM_USER+1009
#define WM_GOTOZALSOFT  WM_USER+1010
#define WM_SPLASHGONE	WM_USER+1011


#define MB_CANCELTRYCONTINUE    QMessageBox::Cancel|QMessageBox::Retry|QMessageBox::Ok
#define MB_ABORTRETRYIGNORE     QMessageBox::Abort|QMessageBox::Retry|QMessageBox::Ignore
#define MB_HELP                 QMessageBox::Help
#define MB_OK                   QMessageBox::Ok
#define MB_OKCANCEL             QMessageBox::Cancel
#define MB_RETRYCANCEL          QMessageBox::Retry|QMessageBox::Cancel
#define MB_YESNO                QMessageBox::Yes|QMessageBox::No
#define MB_YESNOCANCEL          QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel
#define MB_ICONEXCLAMATION      QMessageBox::Warning
#define MB_ICONWARNING          QMessageBox::Warning
#define MB_ICONINFORMATION      QMessageBox::Information
#define MB_ICONASTERISK         QMessageBox::Warning
#define MB_ICONQUESTION         QMessageBox::Question
#define MB_ICONSTOP             QMessageBox::Critical
#define MB_ICONERROR            QMessageBox::Critical
#define MB_ICONHAND             QMessageBox::Critical


#define IDABORT             QMessageBox::Abort
#define IDCANCEL            QMessageBox::Cancel
#define IDCONTINUE          QMessageBox::Ok
#define IDIGNORE            QMessageBox::Ignore
#define IDNO                QMessageBox::No
#define IDOK                QMessageBox::Ok
#define IDRETRY             QMessageBox::Retry
#define IDTRYAGAIN          QMessageBox::Retry
#define IDYES               QMessageBox::Yes


extern pid_t ThrID;

typedef int32_t BOOL;
typedef  const char* LPCTSTR;


/*
struct FPOINT
{
    float x;
    float y;

};

struct FRECT
{
    float left;
    float top;
    float right;
    float bottom;
};


#define C_NA    0
#define C_FULL  0x1
#define C_PORT  0x2
#define C_LEAFS 0x4
#define C_LBYL  0x8
#define C_LMAP  0x10
#define C_GRAPH 0x20
#define C_ZONES 0x40
#define C_WHITE 0x80
#define C_BYFAR 0x100


typedef enum _BR_MODE{BR_VIEW, BR_MOVE, BR_SCALE, BR_ROTATE, BR_RISE}BR_MODE;
typedef enum _EL_SEL{SEL_NONE, SEL_BRUSH, SEL_FACE, SEL_VERTEX, SEL_EDGE, SEL_ITEMS, SEL_BR_ITEM}EL_SEL;

enum
{
    CUR_NONE=0,
    CUR_SCROLL,
    CUR_MOVE,
    CUR_SCALE,
    CUR_ROTATE,

    CUR_NS,
    CUR_EW,
    CUR_NWSE,
    CUR_NESW,
};

*/
#endif //!WINDOWS

#endif // HEADER
