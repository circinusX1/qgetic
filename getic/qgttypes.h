#ifndef QGTTYPES_H
#define QGTTYPES_H

#include <baselib.h>
#include <QString>
#include <QWidget>
#include <QWidget>
#include <QLineEdit>

struct FQPoint
{
    f32_t x;
    f32_t y;

};

struct FRECT
{
    f32_t left;
    f32_t top;
    f32_t right;
    f32_t bottom;
};


struct SIZE
{
    u_int32_t cx;
    u_int32_t cy;
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


struct SHCUR
{
    int32_t hc;
    int32_t scDir;
};

//=======================================================================================




#define		WM_USRKEYDOWN	WM_USER+1002

//globals
extern f32_t GEpsilon;

//#define	theApp->get_modifiers(k_)   (GetAsyncKeyState(k_) & 0x8000)
typedef	u_int8_t		RGBA[4];

extern u_bool32     Gtrial;
extern u_bool32     GFrustCull;
extern u_bool32     BShowLCirc;
extern u_bool32     BShowCb;
extern u_bool32     BShowTrPolys;
extern u_int32_t    PaintCounter;
extern int32_t		GCullTerrCell;
extern u_bool32     GDisableGrpCSG;
extern u_bool32     GKeepProjections;
extern u_bool32     GNewUID;
extern f32_t		GGridSnap;
extern u_bool32		GDetEnabled;
extern int32_t		GMinDetDim;
extern int32_t		GMaxDetDim;
extern int32_t		GMinDetPolys;
extern u_bool32		GBackFace;
extern u_bool32		GLight;
extern u_bool32     GShowCuts;
extern f32_t     GMeters;
extern u_bool32		GForceBSP;
extern u_bool32     GCarving;
extern u_bool32	    GUpdateImm;
extern  u_bool32    GAutoSave;
extern u_int32_t     GUtex;
extern u_int32_t		GAnimCompilation;
extern u_bool32     GSelTexFace;
extern  u_bool32    BrushTask;
extern u_bool32     BVxNormals;
extern  u_bool32    CompileTask;
extern  u_bool32    CSGTask;
extern  u_bool32    GNormals;
extern int32_t      TerLeafCelTiles;
extern int32_t      TerrTiles;
extern int32_t      SnapToXY;

extern	RGBA	CLR_NORMAL;
extern	RGBA	CLR_DARK;
extern	RGBA	CLR_CUT;
extern	RGBA	CLR_WHITE;
extern	RGBA	CLR_SELECT;
extern	RGBA	CLR_SELECTDIMED;
extern	RGBA	CLR_SELBOX;
extern	RGBA	CLR_SELBOX_TCH;
extern	RGBA	CLR_CAMSTART;
extern	RGBA	CLR_CAMEND;
extern	RGBA	CLR_CAMFRUST;
extern	RGBA	CLR_SCRSURFACE;
extern	RGBA	CLR_CUT3DBRUSH;
extern  RGBA	CLR_PRJDBRUSH;
extern  RGBA	CLR_BLOOR;
extern	RGBA	CLR_CUT3DBRUSHREV;
extern	RGBA	CLR_PORTALS;
extern	RGBA	CLR_PORTALS1;
extern	RGBA	CLR_PORTALSLINE;
extern	RGBA	CLR_DETAIL;
extern	RGBA	CLR_TERRAIN;
extern	RGBA	CLR_DETAILTRIG;
extern	RGBA	CLR_LIGHTBULB;
extern	RGBA    CLR_SKYDOM;
extern	RGBA    CLR_MOTION;
extern	RGBA    CLR_TRIGGER;
extern	RGBA    CLR_FACEDETAIL;
extern	RGBA    CLR_NEW_BRITEM;
extern	RGBA    CLR_ZONE;
extern	RGBA    CLR_GRIDF		;
extern	RGBA    CLR_FINEGRIDF	;
extern	RGBA    CLR_GRIDD		;
extern	RGBA    CLR_FINEGRIDD	;
extern	RGBA    CLR_AXES_X		;
extern	RGBA    CLR_AXES_Y		;
extern	RGBA    CLR_AXES_Z		;


extern char    GeticPrimitiveFile[];//   = "GPGF";   // getic primitive geometry file
extern char    GeticGeometryRawFile[];// = "GGRF";   // getic primitive geometry file
extern char    GeticGeometryBeamTree[];// = "GGRF";   // getic primitive geometry file

INLN char*	StripSpaces(char* psz)
{
    char* ps = psz;
    char* pd = psz;
    while(*ps){
        if(*ps==' '||*ps=='*'||*ps==';'||*ps==':'||*ps=='\t'||*ps=='.'||*ps=='\r'||*ps=='\n')
        {	ps++;continue;}
        *pd++=*ps++;
    }
    *pd=0;
    return psz;
}

INLN char*	Replace(char* psz, char a, char b)
{
    char* ps = psz;
    char* pd = psz;
    while(*ps){
        if(*ps==a)
        {
            *pd++=b;
            ps++;
        }
        else
            *pd++=*ps++;
    }
    *pd=0;
    return psz;
}

INLN char*	StripChar(char* psz, char ch)
{
    char* ps = psz;
    char* pd = psz;
    while(*ps){
        if(*ps==ch)
        {	ps++;continue;}
        *pd++=*ps++;
    }
    *pd=0;
    return psz;
}

INLN char*	AnsiStripSpaces(char* psz)
{
    char* ps = psz;
    char* pd = psz;
    while(*ps){
        if(*ps==' ' ||*ps=='\t' ||*ps=='\r'||*ps=='\n')
        {	ps++;continue;}
        *pd++=*ps++;
    }
    *pd=0;
    return psz;
}



INLN char*	StripCrlf(char* psz)
{
    char* ps = psz;
    char* pd = psz;
    while(*ps){
        if(*ps=='\r'||*ps=='\n')
        {	ps++;continue;}
        *pd++=*ps++;
    }
    *pd=0;
    return psz;
}

INLN char*	StripDigits(char* psz)
{
    char* ps = psz;
    char* pd = psz;
    while(*ps){
        if(isdigit(*ps))
        {	ps++;continue;}
        *pd++=*ps++;
    }
    *pd=0;
    return psz;
}

INLN float    RoundNear(float fValue, float fFactor)
{
    if(0== fFactor)fFactor=EPSILON;
     return (float)((int32_t)((fValue + fFactor/2.0f) / fFactor) * fFactor);
}

INLN v3d_t GCalcNormal(const v3d_t& p0, const  v3d_t& p1, const  v3d_t& p2)
{
    v3d_t vV1 = p0-p1;
    v3d_t vV2 = p0-p2;
    v3d_t n = Vcp(vV1, vV2);
    n.norm();
    return n;
}

INLN void	GCalcNormal(Plane* p, v3d_t& a, v3d_t& b, v3d_t& c)
{
    p->CalcNormal(c,b,a);
}

#ifdef WINDOWS

#endif


INLN f32_t ParseREAL(QLineEdit& edid)
{
    const QString sz = edid.text();
    if(sz.length())
        return (f32_t)sz.toFloat();
    return 0.0f;
}

INLN v3d_t ParseV3(QLineEdit& edid)
{
    v3d_t   ret;
    int32_t comma = 0;

    const QString sz = edid.text();
    if(sz.length())
    {
        for(int i=0;sz.length();i++)
        {
            if(sz.at(i) == ',')
                comma++;
        }
        if(2== comma )
            ::_stscanf((const char*)sz.toUtf8(),"%f,%f,%f",&ret.x,&ret.y,&ret.z);
    }
    return ret;
}

INLN v2d_t ParseUV(QLineEdit& edid)
{
    v2d_t   ret;
    int32_t comma = 0;

    const QString sz = edid.text();
    if(sz.length())
    {
        for(int i=0;sz.length();i++)
        {
            if(sz.at(i) == ',')
                comma++;
        }
        if(1== comma )
            ::_stscanf((const char*)sz.toUtf8(),"%f,%f",&ret.u,&ret.v);
    }
    return ret;
}


INLN u_rgba_t ParseCLR(QLineEdit& edid)
{
    u_rgba_t    ret;
    int32_t     comma = 0;

    const QString sz = edid.text();
    if(sz.length())
    {
        for(int i=0;i<sz.length();i++)
        {
            if(sz.at(i) == ',')
                comma++;
        }
        if(3== comma )
        {
            int32_t r,g,b,a;
            ::_stscanf((const char*)sz.toUtf8(),"%d,%d,%d,%d",&r,&g,&b,&a);
            ret.r=r;ret.g=g;ret.b=b;ret.a=a;

        }
    }
    return ret;
}


INLN void Sct(QLineEdit& edid, f32_t value)
{
    edid.setText(QString::number(value));
}

INLN void Sct(QLineEdit& edid, v3d_t& value)
{
    QString s;
    s.sprintf("%.2f,%.2f,%.2f",value.x,value.y,value.z);
    edid.setText(s);
}

INLN void Sct(QLineEdit& edid, u_rgba_t& value)
{
    QString s;
    s.sprintf("%d,%d,%d,%d",value.r,value.g,value.b,value.a);
    edid.setText(s);
}


INLN const char* strchrrev( char* pstr, char tok)
{
    if(pstr==0 || *pstr)
        return 0;
    char* pend = pstr + strlen(pstr)-1;
    while(pend != pstr)
    {
        if(*pstr == tok)
            return pstr;
        --pend;
    }
    return 0;
}

INLN int32_t Str2Int(const char* psz, int32_t def)
{
    if(psz[0])
    {
        int32_t retval = 0;
        ::_stscanf(psz,"%d",&retval);
        return retval;
    }
    return def;
}


INLN f32_t Str2Real(const QString& psz, f32_t def)
{
    if(!psz.isEmpty())
    {
        return psz.toFloat();
    }
    return def;
}

INLN f32_t Str2Real(const char* psz, f32_t def)
{
    if(psz[0])
    {
        f32_t retval = 0;
        ::_stscanf(psz,"%f",&retval);
        return retval;
    }
    return def;
}


INLN int32_t GetLastError()
{
    return errno;
}

const char* MakeBinStr(u_int32_t dw);

#define DI(v_)  GetDlgItem(v_)

#define	IS_WINDOW(w_)	(w_ && ::IsWindow(w_))
#define	IS_VISIBLE(w_)	(w_ && ::IsWindowVisible(w_))
#define IS_K            IsDlgButtonChecked
#define DI_             GetDlgItem
#define CK_             CheckDlgButton
#define EN_(u_,f_)      GetDlgItem(u_)->EnableWindow(f_)
#define	EV_(ex_)		((ex_)?BST_CHECKED : BST_UNCHECKED)

#define     SCOPE(val_)     if(val_)

u_bool32    G_CopyFolder(const char* dst, const char* src, const char* filter, u_bool32 recurs=0);

const char* BrowseForFolder(char* title,  const char* cs);
void    DelayUpdateWindow(HWND);

#ifdef _DEBUG
    #define   _TRY      do
    #define   _CATCHX()  while(0);if(0)
#else
    #define   _TRY         try
    #define   _CATCHX()    catch(...)
#endif


u_bool32    IsValidUL(char*);


//=======================================================================================
#define     BI_RGB  0x00000000
#define     RGB 	0x32424752
#define     BI_RLE8 0x00000001
#define     RLE8 	0x38454C52
#define     BI_RLE4 0x00000002
#define     RLE 	0x34454C52

#define LERROR      errno

//=======================================================================================
//#define GEpsilon 0.128

//=======================================================================================


#endif // QGTTYPES_H
