
#include "stdafx.h"
#include <baselib.h>
#include "geticapp.h"
#include "crck.h"


extern u_bool32 _Ginitializing;
u_bool32    GFrustCull   = 0;
u_bool32    BShowLCirc   = 1;
u_bool32    BShowTrPolys = 1;
u_int32_t   PaintCounter = 0;
u_bool32    GNewUID     = 1;
f32_t	GEpsilon	= .125f;
u_bool32    GDetEnabled = FALSE;
int32_t     GMinDetDim  = 64;
int32_t     GMaxDetDim  = 100;
int32_t     GCullTerrCell = 0;
u_bool32    BShowCb      = 0;
u_bool32    GDisableGrpCSG=1;
u_bool32    GKeepProjections=0;
int32_t     GMinDetPolys= 32;
f32_t    GGridSnap   = 32;
u_bool32	GBackFace	= TRUE;
u_bool32	GLight		= FALSE;
u_bool32    GShowCuts   = FALSE;
f32_t    GMeters     = 1.0f;
u_bool32	GForceBSP	= FALSE;
u_bool32    GCarving    = FALSE;
u_bool32	GUpdateImm  = FALSE;
u_bool32    GAutoSave   = 1;
u_int32_t    GUtex       = 0;
u_int32_t    GAnimCompilation = FALSE;
u_bool32    GSelTexFace = 1;
u_bool32    BrushTask   = 1;
u_bool32    BVxNormals  = 0;
u_bool32    CompileTask = 0;//CK_TASK
u_bool32    CSGTask     = 0;
u_bool32    GNormals    = 0;
int32_t     TerLeafCelTiles = 16;
int32_t     TerrTiles       = 64;
int32_t     SnapToXY      = 16;

RGBA	CLR_NORMAL	= {182,182,182,255};
RGBA	CLR_DARK	= {96,96,96,255};
RGBA	CLR_CUT		= {180,0,0,255};
RGBA	CLR_WHITE	= {255,255,255,255};
RGBA	CLR_SELECT	= {162,255,182,192};
RGBA	CLR_SELECTDIMED	= {142,235,162,128};
RGBA	CLR_SELBOX	= {0,64,200,222};
RGBA	CLR_SELBOX_TCH = {0,96,255,255};
RGBA	CLR_CAMSTART={0,0,255,255};
RGBA	CLR_CAMEND  ={0,0,64,255};
RGBA	CLR_CAMFRUST={0,0,255,255};
RGBA	CLR_SCRSURFACE={0,0,255,75};
RGBA	CLR_PRJDBRUSH={192,100,0,128};
RGBA	CLR_CUT3DBRUSH={255,0,0,64};
RGBA	CLR_CUT3DBRUSHREV={255,128,0,54};
RGBA	CLR_PORTALS={0,0,128,200};
RGBA	CLR_PORTALS1={0,128,0,200};
RGBA	CLR_PORTALSLINE={255,255,192,255};
RGBA	CLR_BLOOR={255,255,255,128};
RGBA	CLR_DETAIL={128,128,0,255};
RGBA	CLR_TERRAIN={84,72,60,255};
RGBA	CLR_DETAILTRIG={192,255,0,255};
RGBA	CLR_SKYDOM={0,200,200,255};
RGBA	CLR_LIGHTBULB={64,0,0,255};
RGBA	CLR_MOTION={46,150,220,255};
RGBA	CLR_TRIGGER={250,84,84,255};
RGBA	CLR_FACEDETAIL={192,192,100,255};
RGBA    CLR_NEW_BRITEM={255,147,149,255};
RGBA    CLR_ZONE={160,60,160,255};
RGBA    CLR_GRIDF		= {52,52,54,255};
RGBA    CLR_FINEGRIDF	= {42,42,42,255};
RGBA    CLR_GRIDD		= {38,38,38,255};
RGBA    CLR_FINEGRIDD	= {28,28,28,255};
RGBA    CLR_AXES_X		= {111,64,64,255};
RGBA    CLR_AXES_Y		= {64,111,64,255};
RGBA    CLR_AXES_Z		= {64,64,111,255};




char    GeticPrimitiveFile[]   = "GPGF";   // getic primitive geometry file
char    GeticGeometryRawFile[] = "GGRF";   // getic primitive geometry file
char    GeticGeometryBeamTree[] = "GGBT";   // getic primitive geometry file


const char* MakeBinStr(u_int32_t dw)
{
    static char ret[512]={0};
    u_int32_t   mask = 0x1;
    int32_t     j = 0;
    memset(ret,0,512);
    for(int32_t i=0; i <31; i++)
    {
        if(dw & mask)
        {
            strcat(ret, MKSTR("%x (%d) \r\n", mask,mask));
        }
        mask<<=1;
    }
    return ret;

}

