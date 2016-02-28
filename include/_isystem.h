#ifndef __ISYSTEM_H__
#define __ISYSTEM_H__

#include "../baselib/baselib.h"
#include "../baselib/baseutils.h"
#include "../baselib/basecont.h"
#include "_irender.h"
#include "_isound.h"
#include "_iinput.h"
using namespace baseg;

//---------------------------------------------------------------------------------------
#ifndef WM_USER
    #define WM_USER 32768
#endif //

//---------------------------------------------------------------------------------------
// _flags
#define SM_NOMAPPING    1       // systemmode no map keys

//---------------------------------------------------------------------------------------
#define SYS_0               (40000)  // for windows
//---------------------------------------------------------------------------------------
                            // msg    wparam, lparam
#define HANDLE_SCRIPT       0 
#define SYS_CONFIGURE       (SYS_0)   // StartStruct, RndStruct
#define SYS_START           (SYS_0+1) // render struct, system data
#define SYS_TOGLESCRREN     (SYS_0+2) //screen full/windowed, sysdata  
#define SYS_ESCAPE          (SYS_0+3) //0, sysdata  
#define SYS_EXIT            (SYS_0+4) //0, sysdata
#define SYS_CLOSEAPP        (SYS_0+5) //0, sysdata
#define SYS_INPUT           (SYS_0+6)
#define SYS_UICOMMAND       (SYS_0+7) //cmd, 0
#define SYS_UIMOUSEENTER    (SYS_0+8) //cms, 1 enter 0 exit
#define SYS_RENDER_2D       (SYS_0+9) //0, paint prim plan
#define SYS_RENDER_3D       (SYS_0+10) //IRender, (int32_t)&_sysData
#define SYS_ANIMATE         (SYS_0+11) //IRender, (int32_t)&_sysData
#define SYS_SETERROR        (SYS_0+12) //0, sysdata


//---------------------------------------------------------------------------------------
#define SYS_BSP_READ        (SYS_0+100)
#define SYS_LOADMAP         (SYS_0+101)
#define SYS_BSPITEM_CAT     (SYS_0+102)
#define SYS_BSPITEM         (SYS_0+103)

//---------------------------------------------------------------------------------------
#define SYS_STATUSTEXT      (SYS_0+200)

//---------------------------------------------------------------------------------------
#define SYS_GAMESRVLIST      (SYS_0+300) // game server found
#define SYS_NET_REC          (SYS_0+301)
#define SYS_NET_OPENED       (SYS_0+302) 
#define SYS_NET_CLOSED       (SYS_0+302) 
#define SYS_NET_SETID        (SYS_0+303) //1 , id
#define SYS_NET_PLAYNOW      (SYS_0+304) //0 , 0
#define SYS_NET_PLAYERON     (SYS_0+305) //0 , buff
#define SYS_NET_PLAYEROFF    (SYS_0+305) //0 , buff
#define SYS_NET_POSITION     (SYS_0+306) //id, pos
#define SYS_NET_ACTION       (SYS_0+307) //id, pos
#define SYS_NET_MAPCHANGE    (SYS_0+308)   
#define SYS_NET_LOADMAP      (SYS_0+309) 
#define SYS_NET_PING         (SYS_0+310) //id, pos
#define SYS_NET_AUTH         (SYS_0+311) //id, pos
#define SYS_PROGRESS         (SYS_0+312) //id, pos
#define SYS_PROGRESSPAINT    (SYS_0+313) //0, sysdata

//return codes
//---------------------------------------------------------------------------------------
#define OS_NOMSG            0
#define OS_TOGGLESCR        0x1
#define OS_ESCAPE           0x2
#define OS_EXITAPP          0x4
#define OS_MSG              0x8

//---------------------------------------------------------------------------------------
#define RET_NOT_PROC        0x0
#define NO_IN_SPIN          0x1
#define NO_NET_SPIN         0x2
#define NO_CAM_SPIN         0x4
#define NO_RENDER           0x8
#define NO_PAINT            0x10
#define NO_SQ_CALL          0x20


//---------------------------------------------------------------------------------------
class NO_VT  ISystem;
typedef int32_t  (*NotifyProc)(ISystem*, int32_t, int32_t, int32_t);
typedef void  (*pdestruct)(void* , int32_t size);
//---------------------------------------------------------------------------------------

struct SystemData 
{
    f32_t         _fps;          
    f32_t         _curtime;
    f32_t         _ticktime;     
    f32_t         _pauseTime;    
    u_int32_t        _physFrame;   
    u_int32_t        _drawFrame;   
    u_int8_t         *_pKeys;
    int32_t          *_pMouse;
    ISystem*     _pSystem;
    RndStruct*   _pRndStruct;
    int32_t          Key(int32_t index){return *(_pKeys+index);}
    u_int8_t         MouseBut(int32_t index){return *(_pMouse+index);}
};

//---------------------------------------------------------------------------------------

struct StartStruct
{
    StartStruct(){  
                    memset(this,0,sizeof(*this));
                    _size = sizeof(*this);
                    strcpy(_renderSel,"rendergl.dll");
                    strcpy(_inputSel, "inputdx.dll");    
                    strcpy(_audioSel,"sounddx.dll");
                    strcpy(_fontFile,"fontfile.bmp");
                    _enableScript = 1;
        }
    int32_t           _size;                 // size of this structure
    tchar       _renderSel[64];        
    tchar       _inputSel [64]; 
    tchar       _audioSel [64];
    tchar       _fontFile [64];
    u_bool32          _enableScript;
};

//---------------------------------------------------------------------------------------
#define MSG_REMOVE      0
#define MSG_NOREMOVE    1
typedef struct _SysMsg
{
    int32_t    msg;
    int32_t    pw;
    int32_t    pl;
    int32_t    time;
}SysMsg;

//---------------------------------------------------------------------------------------
typedef enum _sysMODES
{
    PLAY_MODE,
    SET_MODE,
    DOWNLOADING_MODE,
}sysMODES;


//PSystem->_hudMode = 0;----------------------------------------------------------------
#define HUD_ARROW   0x1
#define HUD_HELP    0x2
#define HUD_TRACE   0x4
#define HUD_CONSOLE 0x8


typedef struct _EKEY
{
    int32_t     _et;   
    int32_t*    _pidx;
}EKEY;

//---------------------------------------------------------------------------------------
class    ISystem;
__declspec(thread) extern   ISystem* PISystem;
class    IGc;              //gabrbage collected.
class    ISystem
{
public:
    
    ISystem(){PISystem=this;}
    virtual ~ISystem(){PISystem=0;}
    
    virtual int32_t    Run(void)=0;
    virtual int32_t    Stop(void) = 0;
    
    virtual void   RegisterDestructor(pdestruct, void*)=0;

// access plugs functions
    virtual Irender*  Render()=0;
    virtual Isound*   Sound() = 0;
    virtual Iinput*   Input() = 0;
    virtual const     u_int32_t*  Primitives() = 0;

    virtual void      SetMode(int32_t ss)=0;
    virtual int32_t      GetMode()=0;
    virtual void      TextOut(int32_t id, const tchar*,...)=0;
    virtual void      SetHudMode(u_int32_t ss)=0;
    virtual u_int32_t   GetHudMode()=0;

    virtual void      Printf(const tchar*,...)=0;
    
    virtual int32_t      Default(int32_t, int32_t, int32_t )=0;
    virtual int32_t      SendMessage(int32_t, int32_t, int32_t )=0;
    virtual int32_t      PostProc(int32_t, int32_t, int32_t, int32_t delay=0)=0;

    virtual SysMsg*   GetMessage(int32_t )=0;
    virtual int32_t       PumpMessage()=0;
    virtual void      Spin(f32_t&)=0;
    virtual int32_t       UpdateWindow(u_bool32 camtoo=0);
    virtual void      Wait(int32_t percent)=0;

// texture functions
    virtual Htex* GenTexGetBuff(const tchar* , u_int32_t, char**, int32_t*, int32_t*, int32_t*)=0;
    virtual Htex* GenTex(const tchar* name, int32_t x, int32_t y, int32_t bpp, const char* buff, u_int32_t mip);
    virtual Htex* GenTexFile(const tchar* , u_int32_t)=0;
    virtual Htex* GenTexFont(f32_t x, f32_t y, f32_t scale, const tchar* fname) = 0;
    virtual void  DeleteTexFont(Htex*, u_bool32 allrefs=0)=0;
    virtual void  DeleteTex(Htex*, u_bool32 allrefs=0)=0;
    virtual void  DeleteTexts(Htex*,int32_t, u_bool32 allrefs=0)=0;

//sound functions
    virtual Itape* AddSoundFile(const tchar* pFile)=0;
	virtual void   RemoveSoundFile(const tchar* pFile)=0;
	virtual void   RemoveSound(Itape* pt)=0;
	virtual Itape* GetTape(const tchar* fname)=0;
    virtual void   PlayPrimary(const Itape* pt, u_int32_t mode=1)=0;
   	virtual void   StopPlay(const Itape* pt )=0;
    virtual Itape* PlayPrimaryFile(const tchar* fname, u_int32_t mode=1)=0;
	virtual void   StopPlayFile(const tchar* fname)=0;

//input functions
    virtual void    SetSpeeds(f32_t ts, f32_t rs)=0;
	virtual u_bool32    IsKeyDown(int32_t key)=0;
    virtual int32_t *   Moves()=0;
    virtual u_int8_t*   Keys()=0;
    virtual u_int8_t    GetPressedKey()=0;
    virtual void    IgnoreInput(f32_t secs)=0;


//
    virtual HINSTANCE   Hinstance()=0;
    virtual HWND        Hwnd()=0;

//
    virtual void        RegisterGc(IGc* pigc)=0;
    virtual void        UnregisterGc(IGc* pigc)=0;

};

//---------------------------------------------------------------------------------------
class ICamera : public Pos
{
public:
    ICamera (){TRACEX("ICamera()\r\n");}
    virtual ~ICamera (){TRACEX("~ICamera()\r\n");}
    virtual int32_t         Animate(SystemData* ps=0)=0;
    virtual u_bool32		CanSee(const Box& aabbMin, int32_t i=-1)const=0;
    virtual u_bool32		CanSeeCamera(const v3d_t& center,  f32_t rad)=0;
    virtual f32_t        Fov()=0;
    virtual void        SetPos(const v3d_t& eye, const v3d_t& euler)=0;
    virtual void        SetPosMat(const m_16t& vmatrix)=0;
    virtual void        LookAt(const v3d_t& eye,const  v3d_t& up,const  v3d_t& fwd, const v3d_t& rght)=0;
    virtual void        SetFarAndFov(f32_t rfar, f32_t rfov)=0;
    virtual const m_16t&	ViewMatrix()=0;
    virtual void        SetUserData(void* ud)=0;
    virtual void*       GetUserData()=0;
};


//---------------------------------------------------------------------------------------
class NO_VT IGc  //gabrbage collected.
{
public:
    IGc(){TRACEX("IGc()\r\n"); PISystem->RegisterGc(this);}
    virtual ~IGc(){TRACEX("~IGc()"); PISystem->UnregisterGc(this);}
    virtual void    Clear()=0;
};


//---------------------------------------------------------------------------------------
#include <windows.h>

#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct _GUID {          // size is 16
    u_int32_t Data1;
    u_int16_t  Data2;
    u_int16_t  Data3;
    u_int8_t  Data4[8];
} GUID;
#endif //GUID_DEFINED

//---------------------------------------------------------------------------------------
INLN GUID* EngineIID()
{
    static GUID ENGINE_IID = { 0x646741ef, 0x2cc5, 0x4b68, { 0x93, 0x3d, 0xcd, 0x36, 0x51,0xfd, 0x61, 0xea } };
    return &ENGINE_IID;
}

#endif //__IENGINE_H__


