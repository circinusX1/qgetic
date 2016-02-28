#ifndef STDAFXX_H
#define STDAFXX_H



//#include <QApplication>
#include <GL/gl.h>
#include <GL/glu.h>
#include <_irender.h>
#include "qgttypes.h"


#define DOC_VERSION2     2
#define DOC_VERSION3     3
#define DOC_VERSION4     4
#define DOC_VERSION5     200  // terrain details
#define DOC_VERSION6     201  // sound position bug
#define DOC_VERSION7     202  // early convex
#define DOC_VERSION8     203  // lmap custom intensity
//----------------------------------------------------
#define DOC_VERSION10    300  // linux 64 bit pointers

extern u_int32_t    UTimer;
extern Htex		    _selTex;
extern Htex		    _dumptex;
extern Htex			_lbulbtex;
extern Htex			_itmqtex;
extern Htex			_sndtex;
extern Htex			_startPosTex;
extern Htex			_tbtex;
extern Htex         _wLmap;
extern Htex         _ambLmap;


typedef enum _EXCOMP{
    E_COMP  = 1  ,
    E_HSR      ,
    E_ESR      ,
    E_BSP      ,
    E_PORT     ,
    E_PVS      ,
    E_TJU      ,
    E_LMAPS    ,
    E_COMPILER,
    I_DONE,
}EXCOMP;


#define __noop (void)(0)



#endif // STDAFX_H
