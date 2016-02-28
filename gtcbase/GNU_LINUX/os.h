/*
    Author: Marius Octavian Chincisan, Jan-Aug 2012
    Copyright: Marius C.O.
*/
#ifndef __OS_H__
#define __OS_H__

#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/time.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>

#ifdef MA_UNICODE
#   include <wchar.h>
#   define _ttfopen      _wfopen
#   define _ttstrchr     wcschr
#   define _ttstrstr     wcsstr
#   define _ttsnprintf   wsnprintf
#   define _ttatoi       _wtoi
#   define _ttstrtok     wcstok
#   define _ttgetenv     _wgetenv
#   define _ttsystem     _wsystem
#   define _ttasctime    _wasctime
#   define _ttremove     _wremove
#   define _ttrename     _wrename
#   define _tchar        wchar_t
#   define tstring     basic_string<wchar_t>
#   define tchar        wchar_t
#else
#   define _ttfopen      fopen
#   define _ttstrchr     strchr
#   define _ttstrstr     strstr
#   define _ttsnprintf   snprintf
#   define _ttatoi       atoi
#   define _ttstrtok     strtok
#   define _ttgetenv     getenv
#   define _ttsystem     system
#   define _ttasctime    asctime
#   define _ttremove     remove
#   define _ttrename     rename
#   define _ttcscpy      strcpy
#   define _ttcscat      strcat
#   define _ttcschr      strchr
#   define _tcsstr       strstr
#   define _ttgetcwd     getcwd
#   define _ttcslen      strlen
#   define _ttchdir      chdir
#   define _tstprintf    sprintf
#   define _tstricmp     strncasecmp
#   define _stscanf      sscanf
#   define _getcwd       getcwd
#   define _ttricmp       strncasecmp
#   define _tfopen      fopen
#   define _tchar        char
#   define tstring      string
#   define tchar        char
#   define _ttcstok     strtok
#endif

#define UNUS(x_)   (void)(x_)


#define INLN    inline
#define REG
#define NO_VT
#define VIRT
#define NAKED_  __attribute__ ((naked))
#define ASM_    __asm
#define nvtclass
#define OUT_
#define OVERWRITE
#define TYPENAME    typename
#define OPBS  '/'
#define BS    '\\'
#define BSS   "\\"


#define _MAX_PATH           256
#define _MAX_FNAME          256
#define _MAX_EXT            32
#define _MAX_DRIVE          8

#define MAX_PATH           256
#define MAX_FNAME          256
#define MAX_EXT            32
#define MAX_DRIVE          8

#define __cdecl     __attribute__((stdcall))
#define __stdcall     __attribute__((stdcall))

//-----------------------------------------------------------------------------
typedef void* HANDLE;
#define DECLARE_HANDLE(name) typedef HANDLE name
typedef HANDLE *PHANDLE;
typedef int32_t         u_bool32; // to keep BOOL alignment from windows

DECLARE_HANDLE (HWND);
DECLARE_HANDLE (HDC);
DECLARE_HANDLE (HFONT);
DECLARE_HANDLE (HGLRC);
DECLARE_HANDLE (HINSTANCE);
//-----------------------------------------------------------------------------
#pragma pack(push, 1)
typedef struct _FILETIME {
  u_int32_t dwLowDateTime;
  u_int32_t dwHighDateTime;
} FILETIME, *PFILETIME;

typedef struct tagBITMAPFILEHEADER {
  u_int16_t  bfType;
  u_int32_t bfSize;
  u_int16_t  bfReserved1;
  u_int16_t  bfReserved2;
  u_int32_t bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;


typedef struct tagBITMAPINFOHEADER {
  u_int32_t biSize;
  int32_t  biWidth;
  int32_t  biHeight;
  u_int16_t  biPlanes;
  u_int16_t  biBitCount;
  u_int32_t biCompression;
  u_int32_t biSizeImage;
  int32_t  biXPelsPerMeter;
  int32_t  biYPelsPerMeter;
  u_int32_t biClrUsed;
  u_int32_t biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct tagRGBQUAD {
  u_int8_t rgbBlue;
  u_int8_t rgbGreen;
  u_int8_t rgbRed;
  u_int8_t rgbReserved;
} RGBQUAD;

#pragma pack(pop)

//-----------------------------------------------------------------------------
typedef u_int32_t COLORREF;

inline u_int8_t GetRValue(COLORREF crf) {return ((crf>>16) & 0xFF);}
inline u_int8_t GetGValue(COLORREF crf) {return ((crf>>8) & 0xFF);}
inline u_int8_t GetBValue(COLORREF crf) {return (crf & 0xFF);}


//-----------------------------------------------------------------------------
class mutex;
class condition
{
public:
    condition()
    {
        pthread_cond_init(&_cond, NULL);
        pthread_mutex_init(&_mutex  ,NULL);
    }
    ~condition()
    {
        pthread_cond_signal(&_cond);
        pthread_mutex_unlock(&_mutex);

        pthread_cond_destroy(&_cond);
        pthread_mutex_destroy(&_mutex);
    }
    void lock()
    {
        pthread_mutex_lock(&_mutex);
    }
    void signal()
    {
        pthread_cond_signal(&_cond);
    }
    void broadcast()
    {
        pthread_cond_broadcast(&_cond);
    };

    void wait()
    {
        pthread_cond_wait(&_cond, &_mutex);
    }
    void unlock()
    {
        pthread_mutex_unlock(&_mutex);
    }
private:

    pthread_cond_t _cond;
    pthread_mutex_t _mutex;
};
//-------;----------------------------------------------------------------------
class mutex
{
    mutable pthread_mutex_t _mut;
public:
    mutex()
    {
        pthread_mutexattr_t     attr;

        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&_mut,&attr);
        pthread_mutexattr_destroy(&attr);
    }

    virtual ~mutex()
    {
        pthread_mutex_unlock(&_mut);
        pthread_mutex_destroy(&_mut);
    }

    int32_t mlock() const
    {
        int32_t err = pthread_mutex_lock(&_mut);
        return err;
    }

    int32_t try_lock() const
    {
        int32_t err =pthread_mutex_trylock(&_mut);
        return err;
    }

    int32_t munlock() const
    {
        int32_t err =pthread_mutex_unlock(&_mut);
        return err;
    }
};

//-----------------------------------------------------------------------------
class semaphore
{
    sem_t _sem;

public:
    semaphore( int32_t init = 0 )
    {
        sem_init(&_sem,0,init);
    }

    virtual ~semaphore()
    {
        sem_destroy(&_sem);
    }

    int32_t swait(int32_t to=-1) const
    {
        UNUS(to);
        sem_wait((sem_t *)&_sem);
        return 1;
    }

    int32_t stry_wait(int32_t to=-1) const
    {
        UNUS(to);
        return (sem_trywait((sem_t *)&_sem)?errno:0);
    }

    int32_t snotify() const
    {
        return (sem_post((sem_t *)&_sem) ? errno : 0);
    }

    int32_t value() const
    {
        int32_t val = -1;
        sem_getvalue((sem_t *)&_sem,&val);
        return val;
    }
    void reset( int32_t init = 0 )
    {
        sem_destroy(&_sem);
        sem_init(&_sem,0,init);
    }
};


//-----------------------------------------------------------------------------
class AutoLock
{
public:
    AutoLock(mutex* m):_mutex(m)
    {
         _mutex->mlock();
    }
    AutoLock(const mutex* m):_mutex((mutex*)m)
    {
         _mutex->mlock();
    }
    ~AutoLock()
    {
        _mutex->munlock();
    }
private:
    mutex* _mutex;
};

//-----------------------------------------------------------------------------
class Mvi1Wr
{
    pthread_rwlock_t       rwlock;
public:
    Mvi1Wr()
    {
        memset(&rwlock,0,sizeof(rwlock));//=PTHREAD_RWLOCK_INITIALIZER;
    }
    ~Mvi1Wr()
    {
        pthread_rwlock_destroy(&rwlock);
    }

    void paint()
    {
        int32_t rc = pthread_rwlock_wrlock(&rwlock);
        assert(0 ==rc);
    }

    void done()
    {
         pthread_rwlock_unlock(&rwlock);
    }

    int32_t view()
    {
        int32_t k = 65535;//avoid dead infinitte
        int32_t rc = pthread_rwlock_tryrdlock(&rwlock);
        while(rc == EBUSY && k--)
        {
           usleep(0x1FF);
           rc = pthread_rwlock_tryrdlock(&rwlock);
        }
        return rc;
    }


    void viewed(int32_t rc)
    {
        if(rc==0)
            pthread_rwlock_unlock(&rwlock);
    }

    class Read
    {
        const Mvi1Wr& _mv;
        int32_t rc;

    public:

        Read(const Mvi1Wr &v):_mv(v)
        {
            rc = ((Mvi1Wr &)v).view();
        }
        ~Read()
        {
            ((Mvi1Wr&)_mv).viewed(rc);
        }
    };

private:
    mutex       _mv, _m, _mp;
    u_int32_t   _viewers;

};

//-----------------------------------------------------------------------------
class OsThread
{
public:
    OsThread()
    {
        _bstop   = 1;
        _hthread = 0;
        _joined  = false;
        _init = -1;
    }

    virtual ~OsThread()
    {
        if(!_stopped)
        {
            stop_thread();
        }

        if(_joined && _hthread)
        {
            //printf("\ndetaching thread %p \n", _hthread);//mem leak fix valgrind
            pthread_detach(_hthread);
        }
        if(_init==0)
            pthread_attr_destroy(&_attr);
        _hthread = 0;

    }
    virtual void _post_thread_foo()
    {
        ;
    }
    virtual u_bool32 _pre_thread_foo()
    {
        return true;
    }
    virtual int32_t  start_thread()
    {
        _bstop   = 0;

        _init=pthread_attr_init(&_attr);
        pthread_attr_setdetachstate(&_attr, PTHREAD_CREATE_JOINABLE);

        if (pthread_create(&_hthread, &_attr, SFoo, this) != 0)
        {
            pthread_attr_destroy(&_attr);
            return errno;
        }
        pthread_attr_destroy(&_attr);
        usleep(1000);
        _start.swait();
        return 0;
    }

    virtual void signal_to_stop()
    {
        _bstop = 1;
    }
    virtual void    stop_thread()
    {
        if(!_stopped)
        {
            _bstop = 1;
            usleep(100000);
        }
        t_join();
    }

    u_bool32  is_stopped()
    {
        return _bstop;
    }

    int32_t t_join()
    {
        if(!_joined)
        {
            _joined = true;
            //printf("\njoining thread %p \n", _hthread);//mem leak fix valgrind
            return pthread_join(_hthread,0);
        }
        return 0;
    }

    int32_t kill()
    {
        return 0;
    }
    int32_t detach()
    {
        return 0;
    }

    void set_prio(int32_t boost)
    {
        (void)boost;
        if(_hthread)
        {
            // pthread_setschedprio(_hthread, 32);
        }
    }
protected:
    virtual void thread_main() {};
    int32_t         _bstop;
private:
    int32_t         _init;
    mutex       _mutex;
    semaphore   _start;
    pthread_attr_t  _attr;
    pthread_t     _hthread;
    int32_t         _stopped;
    u_bool32        _joined;
    static void* SFoo(void* pData)
    {
        OsThread* pT = reinterpret_cast<OsThread*>(pData);
        pT->_stopped = 0;
        pT->_start.snotify();
        if(pT-> _pre_thread_foo())
        {
            pT->thread_main();
            pT->_stopped = 1;
            pT->_post_thread_foo();
            // no member access couse can be deleted
        }
        return 0;
    }
};


template < typename T, class P>struct AutoCall
{
    AutoCall(T f, P p):_f(f),_p(p) {}
    ~AutoCall()
    {
        _f(_p);
    }
    T _f;
    P _p;
};



INLN void    ge_sleep(u_int32_t milisecs){usleep(milisecs * 1000);};
INLN void    ge_idle(){usleep(8000);};
INLN u_int32_t   ge_gettick()
{
        static u_bool32 sgiInit = false;
        static int32_t         sg_secsOffset;
        timeval     t;

        if (sgiInit == false)
        {
            sgiInit = true;
            ::gettimeofday(&t, NULL);
            sg_secsOffset = t.tv_sec;
        }
        ::gettimeofday(&t, NULL);
        u_int32_t secs  = t.tv_sec - sg_secsOffset;
        u_int32_t uSecs = t.tv_usec;
        return (secs * 1000) + (uSecs / 1000);
}
INLN int32_t    ge_error(){return errno;};
INLN void    ge_error(int32_t err){errno=err;};



#define _PLATFORM_ "LINUX"
#endif // !__OS_H__

