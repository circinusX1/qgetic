/**
    File:   irender.h
    Author: Marius Chincisan
    Desc:   Interfaces definition for rendering DLL's
    History:
*/
#ifndef __IRENDER_H__
#define __IRENDER_H__

#include <baseutils.h>
#include <basecont.h>

//---------------------------------------------------------------------------------------
#define CB_WNDMSG               0x1
//---------------------------------------------------------------------------------------
// Used to respond and manage the texture on main thread when used form different threads
#define WM_DELETE_TEXTURE       WM_USER+1988
#define WM_STORE_TEXTURE        WM_USER+1987
#define WM_OPENGLNOTY           WM_USER+1986

//---------------------------------------------------------------------------------------
// object runtime flags
#define OBJ_LOADIDENTITY        0x1                         
#define OBJ_ROTATE              0x2
#define OBJ_TRANSLATE           0x4
#define OBJ_SCALE               0x8
#define OBJ_ROTATE_EX           0x10
#define OBJ_BB_CYL              0x20
#define OBJ_BB_SPH              0x40
#define OBJ_BB_ALL              0x80
#define OBJ_HIDE	            0x100
#define OBJ_SHOW_ALLWAYS        0x200
#define OBJ_INVIEW				0x400
#define OBJ_LOCAL_INPUT         0x800
#define OBJ_NOCOLLIDE			0x1000
#define OBJ_USE_SOFT_TR		    0x8000	    //software trnasformation	
#define OBJ_INPVS               0x10000
#define OBJ_BILLBOARD           (OBJ_BB_CYL|OBJ_BB_SPH|OBJ_BB_ALL)
#define OBJ_TRANSFORM			(OBJ_ROTATE_EX|OBJ_SCALE|OBJ_ROTATE|OBJ_BB_ALL|OBJ_BB_SPH|OBJ_BB_CYL|OBJ_LOADIDENTITY|OBJ_TRANSLATE)
#define OBJ_TRX                 (OBJ_TRANSLATE|OBJ_ROTATE_EX)
#define OBJ_TR                  (OBJ_TRANSLATE|OBJ_ROTATE)
#define OBJ_TS                  (OBJ_TRANSLATE|OBJ_SCALE)
#define OBJ_TRXS                (OBJ_TRANSLATE|OBJ_ROTATE_EX|OBJ_SCALE)

//--DRAW STYLES-------------------------------------------------------------------------

#define DS_TEX_CNT_MASK         0xF          
#define DS_BLEND_DECAL          0x10
#define DS_BLEND_HITMARK        0x20    
#define DS_BLEND_LIGHT          0x40
#define DS_BLEND_HALLO          0x80
#define DS_BLEND_MASK           0x100
#define DS_SHOW_BACK            0x200
#define DS_DEEPTEST_OFF         0x400
#define DS_COLOR			    0x800
#define DS_NODEEPTEST           0x1000
#define DS_TEXT_BIT 		    0x2000		


#define GET_TEXSTAGE_BITS(x_)   (x_ & 0xF)

//----------------------------------------------------------------------------------------
#if 0
      combiners    ***X****   texture stages (1 2 4 8) as VX_TX1
#endif //0
//----------------------------------------------------------------------------------------
// howwhat
#define VX_COLOR                0x00010000   
#define VX_NORMAL               0x00020000
#define VX_BINORM               0x00040000
#define VX_TANGENT              0x00080000
// gap
#define VTXLATERS               0x00F00000  //texture layers bits
#define VX_TX1                  0x00100000
#define VX_TX2                  0x00200000
#define VX_TX3                  0x00400000
#define VX_TX4                  0x00800000

//---------------------------------------------------------------------------------------
#define FACE_TRANSP             0x1000000
#define FACE_BMASK              0x2000000
#define FACE_MIRRORS            0x4000000

#define ALL_FACES               0xF000000
//---------------------------------------------------------------------------------------
#define REND_PRIM(x_)           (x_ & 0x3F)

//---------------------------------------------------------------------------------------
#define HAS_TEX_0(x_)           (x_ & VX_TX1)
#define HAS_TEX_LM(x_)          (x_ & VX_TX2)
#define HAS_TEX_DET(x_)         (x_ & VX_TX3)
#define HAS_TEX_AUX(x_)         (x_ & VX_TX4)

//---------------------------------------------------------------------------------------
// BLENDS
#define DS_BLEND 			    (DS_BLEND_HITMARK|DS_BLEND_LIGHT|DS_BLEND_HALLO)
#define	DS_BL				    (DS_BLEND_LIGHT|DS_DEEPTEST_OFF)
#define	DS_HL				    (DS_BLEND_HALLO|DS_DEEPTEST_OFF)
#define	DS_HLB				    (DS_BLEND_HALLO|DS_DEEPTEST_OFF|DS_SHOW_BACK)


//----------------------------------------------------------------------------------------
#if 0
      HOWWAT    ---Y--XX   Y=genST XX=how
#endif //0
//----------------------------------------------------------------------------------------

//---TEXTURE_TARGET----------------------------------------------------------------------
//target
#define	TGET_TARGET(dw_)                (dw_&0xF)
#define GEN_TEX_2D_MAP			        0x0             // default
#define GEN_TEX_CUBE_MAP	            0x1		   
#define GEN_TEX_3D_MAP			        0x2		  

//---------------------------------------------------------------------------------------
// filter
#define	TGET_FILTER(dw_)                (dw_&0xF0)
#define	GEN_TEX_MM_LINEAR		        0x00            // default
#define	GEN_TEX_MM_BILINEAR		        0x10	
#define	GEN_TEX_MM_TRILINEAR	        0x20
#define	GEN_TEX_MM_NONE	                0x40
//gap
//---------------------------------------------------------------------------------------
// target 2
#define	TGET_WRAP(dw_)                  (dw_&0xF00)
#define GEN_REPEAT                      0x000           // default
#define GEN_CLAMP                       0x100   
#define GEN_CLAMP_TO_BORDER		        0x200
#define GEN_TEX_HAS_CUBE_T			    0x300	// 6 images T shaped
#define GEN_TEX_HAS_CUBE_M			    0x400   // 6 images on 2 rows by 3

//gap
//---------------------------------------------------------------------------------------
#define	TGET_GENST(dw_)                 (dw_&0xF000)
#define GEN_TEX_GEN_NA                  0x0000         // default no generation
#define	GEN_TEX_GEN_ST_SPH              0x1000         
#define	GEN_TEX_GEN_ST_CUBE	            0x2000

//gap
//---------------------------------------------------------------------------------------
#define	TGET_ENV(dw_)                   (dw_&0xFF0000)
//pnames
#define TPN_TEXTURE_ENV_MODE             0x010000  
#define TPN_TEXTURE_ENV_COLOR            0x020000                  
#define TPN_COMBINE_EXT                  0x030000  
#define TPN_COMBINE_RGB_EXT              0x040000  
#define TPN_COMBINE_ALPHA_EXT            0x050000  
#define TPN_RGB_SCALE_EXT                0x060000  
#define TPN_ADD_SIGNED_EXT               0x070000  
#define TPN_INTERPOLATE_EXT              0x080000  
#define TPN_CONSTANT_EXT                 0x090000  
#define TPN_PRIMARY_COLOR_EXT            0x0A0000  
#define TPN_PREVIOUS_EXT                 0x0B0000  
#define TPN_SOURCE0_RGB_EXT              0x0C0000  
#define TPN_SOURCE1_RGB_EXT              0x0D0000  
#define TPN_SOURCE2_RGB_EXT              0x0E0000  
#define TPN_SOURCE0_ALPHA_EXT            0x0F0000  
#define TPN_SOURCE1_ALPHA_EXT            0x100000  
#define TPN_SOURCE2_ALPHA_EXT            0x110000  
#define TPN_OPERAND0_RGB_EXT             0x120000  
#define TPN_OPERAND1_RGB_EXT             0x130000  
#define TPN_OPERAND2_RGB_EXT             0x140000  
#define TPN_OPERAND0_ALPHA_EXT           0x150000  
#define TPN_OPERAND1_ALPHA_EXT           0x160000  
#define TPN_OPERAND2_ALPHA_EXT           0x170000  
// p-params
#define TPP_MODULATE                    0x800000
#define TPP_DECAL                       0x810000
#define TPP_BLEND                       0x820000 
#define TPP_REPLACE                     0x830000
#define TPP_COMBINE_EXT                 0x840000
#define TPP_COMBINE_RGB_EXT             0x850000
#define TPP_COMBINE_ALPHA_EXT           0x860000
#define TPP_RGB_SCALE_EXT               0x870000
#define TPP_ADD_SIGNED_EXT              0x880000
#define TPP_INTERPOLATE_EXT             0x890000
#define TPP_CONSTANT_EXT                0x8A0000
#define TPP_PRIMARY_COLOR_EXT           0x8B0000
#define TPP_PREVIOUS_EXT                0x8C0000
#define TPP_SOURCE0_RGB_EXT             0x8D0000
#define TPP_SOURCE1_RGB_EXT             0x8E0000
#define TPP_SOURCE2_RGB_EXT             0x8F0000
#define TPP_SOURCE0_ALPHA_EXT           0x910000
#define TPP_SOURCE1_ALPHA_EXT           0x920000
#define TPP_SOURCE2_ALPHA_EXT           0x930000
#define TPP_OPERAND0_RGB_EXT            0x940000
#define TPP_OPERAND1_RGB_EXT            0x950000
#define TPP_OPERAND2_RGB_EXT            0x960000
#define TPP_OPERAND0_ALPHA_EXT          0x970000
#define TPP_OPERAND1_ALPHA_EXT          0x980000
#define TPP_OPERAND2_ALPHA_EXT          0x990000
//gap

//---------------------------------------------------------------------------------------
#define TEX_ALPHABW                     0x80000000


//---------------------------------------------------------------------------------------
#define TEX_PRIMARY(flt_)			     (flt_|GEN_TEX_2D_MAP|TPP_REPLACE)
#define TEX_LIGHTMAP(flt_)			     (flt_|GEN_TEX_2D_MAP|TPP_MODULATE)
#define TEX_CUBEMAP(flt_,wrap_)			 (flt_|wrap_|GEN_TEX_CUBE_MAP|TPP_REPLACE)
#define TEX_CUBELMAP(flt_,wrap_)		 (flt_|wrap_|GEN_TEX_CUBE_MAP|TPP_MODULATE)

// dummy predefined textures-------------------------------------------------------------
#define TEX_LOWREZ      			     (GEN_TEX_MM_LINEAR|GEN_TEX_2D_MAP|TPP_REPLACE)
#define TEX_NORMAL      			     (GEN_TEX_MM_TRILINEAR|GEN_TEX_2D_MAP|TPP_REPLACE)
#define TEX_NORMAL_LM     			     (GEN_TEX_MM_TRILINEAR|GEN_TEX_2D_MAP|TPP_MODULATE|GEN_CLAMP)

//---Contexts ---------------------------------------------------------------------------
// Htex flags are the creation flags for the texture. The apply flags are caried out by 
// each item/face
#ifndef __HTEX
    typedef u_int32_t HTEX;
    #define __HTEX
#endif //

//--------------------------------------------------------------------------------------
// texture handler. texture id handler
typedef  u_int32_t    $Htex[4];
#pragma pack(push, 8)

struct Htex
{
public:
    Htex():hTex(0),glTarget(0),envMode(0),genST(0){}
    ~Htex(){}
    Htex(const Htex& r):hTex(r.hTex),glTarget(r.glTarget),envMode(r.envMode),genST(r.genST){}
    u_bool32  operator==(const Htex& r){return hTex==r.hTex;};
    Htex& operator=(const Htex& r){if(this!=&r) {hTex=r.hTex; glTarget=r.glTarget; envMode = r.envMode; genST=r.genST;} return *this; }
    operator u_int32_t(){return hTex; };
    void    Clear(){hTex=0;glTarget=0;envMode=0;genST=0;}
    HTEX        hTex;                             // texture unique ID
    u_int32_t   glTarget;							// set by render to GL_TEXTURe_XX
    u_int32_t   envMode;                          // additional data not used
    u_int32_t   genST;                            // align to 16 bytes
};
#pragma pack(pop)

//---RENDER_STYLE------------------------------------------------------------------------
#define	USE_CCW			    0x1
#define	USE_CW				0x2
#define	SHOW_FRONT			0x4
#define	SHOW_BACK			0x8
#define	SHOW_BOTH			0x10

//---------------------------------------------------------------------------------------

#define LIGHT_QPoint         1
#define LIGHT_DIRECT        2
#define LIGHT_SPOT          4

//hardware light
struct RenderLight 
{
    u_bool32 operator==(const RenderLight& r){return _colorDiffuse.r==r._colorDiffuse.r;}
    u_int32_t   _flags;
    v3d_t      _pos;
    v3d_t      _direction;
    f32_t    _radius;
    u_rgba_t     _colorAmbient;                 
    u_rgba_t     _colorDiffuse;
    u_rgba_t     _colorSpecular;
    f32_t    _intens;
    f32_t    _cutOff;
    f32_t    _fallOff;
    f32_t    _constAttenuation;
    f32_t    _linearAttenuation;
    f32_t    _quadratAttenuation;
	int32_t	_userdata;
};

//---------------------------------------------------------------------------------------
// hardware material
struct RenderMaterial 
{
    u_bool32 operator==(const  RenderMaterial& r){return _colorDiffuse.r==r._colorDiffuse.r;}
    u_int32_t   _flags;
    u_rgba_t     _colorAmbient;                 
    u_rgba_t     _colorDiffuse;
    u_rgba_t     _colorEmmisive;
    u_rgba_t     _colorSpecular;
    f32_t    _shiness;
};

//---------------------------------------------------------------------------------------
//hardware fog
struct RenderFog
{
    f32_t    _gnear;
    f32_t    _gfar;
    f32_t    _dens;
    u_rgba_t     _clr;
    u_int32_t   _flag;
    Plane   _clip;
};


//---------------------------------------------------------------------------------------
// Renderer generic flags. We pass in our array of gPrims with these indexes and the
// plugin DLL is responsable to copy it's internal flags on specific offsets.
enum
{
	ZERO = 0,
	Z_TEXTURE_2D, 
	Z_TEXTURE_3D, 
	Z_TEXTURE_CUBE_MAP,
    Z_FRONT_AND_BACK,
	Z_LINE,
    Z_FILL,
    Z_CULL_FACE,
    Z_FRONT,
    Z_SHININESS,
    Z_BACK,//10
	Z_BLEND,
	Z_QPointS,       //draw mode
	Z_LINES,
	Z_LINE_STRIP,
	Z_LINE_LOOP,
	Z_TRIANGLES,
	Z_TRIANGLE_STRIP,
	Z_TRIANGLE_FAN,
	Z_QUADS,
	Z_QUAD_STRIP,//20
	Z_POLYGON,
	Z_ZERO,         //alpha
	Z_ONE,
	Z_SRC_COLOR,
	Z_ONE_MINUS_SRC_COLOR,
	Z_SRC_ALPHA,
	Z_ONE_MINUS_SRC_ALPHA,
	Z_DST_ALPHA,
	Z_ONE_MINUS_DST_ALPHA,
	Z_DST_COLOR,
	Z_ONE_MINUS_DST_COLOR,
	Z_SRC_ALPHA_SATURATE,
    Z_TEXTURE_GEN_S,
    Z_TEXTURE_GEN_T,
    Z_TEXTURE_GEN_R,
    Z_TEXTURE_GEN_Q,
    Z_TEXTURE_MAG_FILTER,
    Z_TEXTURE_MIN_FILTER,
    Z_TEXTURE_WRAP_S,
    Z_TEXTURE_WRAP_T,
    Z_TEXTURE_ENV_MODE,
    Z_TEXTURE_ENV_COLOR,
    Z_TEXTURE_ENV,
    Z_CLAMP,
    Z_REPEAT,
    Z_AMBIENT,
    Z_DIFFUSE,
    Z_SPECULAR,
    Z_EMISSION,
    Z_LAST_ITEM
};

//---------------------------------------------------------------------------------------
// We pass this in to configure the initial state of the rendering system
struct RndStruct
{
    RndStruct()
    {
        retainedDraw  = false;
        hInstance	  = 0;
        fNear		  = 8.0;
        fFar		  = 131072.0;
        hFov		  = 64.0;
		xPolyMode     = USE_CW|SHOW_BACK;
		wndStyle	  = 0;
		wndParent	  = 0;
        pfDepthBits   = 32;
        pfStencilBits = 1;
        bFullScreen	  = false;
        bgColor       = 0;
        clipCursor    = 0;
        hideCurosr    = 1;
        bsetCapture    = 0;
		dmDisplayFrequency=60;
        dmPelsWidth   = 640; 
		dmPelsHeight  =	480;
		dmBitsPerPel  = 16;

        bIsActivated = false;
        fWidth	   = 0.0;
        fHeight    = 0.0;
        nWidth	   = 0;
        nHeight	   = 0;
        dwVpStyle  = 3; //clsoe and maximize button
        szError[0] = 0;        
    }

	void*		    wndParent;
    u_int32_t          	wndStyle;
    void*	    hInstance;
    u_bool32		retainedDraw;
    
    f32_t		fNear;
    f32_t		fFar;
    f32_t		hFov;
    
    
    
    u_int32_t       bgColor;
    int32_t         dmPelsWidth; 
    int32_t         dmPelsHeight;
    int32_t         dmBitsPerPel;
    int32_t         dmDisplayFrequency;

    u_int32_t		xPolyMode;
    u_bool32		bFullScreen;
    int32_t         pfDepthBits;
    int32_t         pfStencilBits;

    f32_t		fAspect;
    f32_t		fWidth;
    f32_t		fHeight;
    int32_t			nWidth;
    int32_t			nHeight;
    u_int32_t       dwVpStyle;
    u_bool32		bIsActivated;

    u_bool32        bsetCapture;
    u_bool32        hideCurosr;
    
    u_bool32        clipCursor;


    char        szError[64];

};


//---------------------------------------------------------------------------------------
// array of vtxes
struct RenderVx
{
    RenderVx():_v(0),_n(0),_b(0),_c(0),_sz(0){_u[0]=_u[1]=_u[2]=_u[3]=0;}
    RenderVx(int32_t sz, u_int32_t what):_v(0),_n(0),_b(0),_c(0),_sz(0){
        _u[0]=_u[1]=_u[2]=_u[3]=0;
        _what = what;
        _sz = sz;
        _v  = new v3d_t[sz];
        if(what & VX_TX1)       _u[0] = new v2d_t[sz];
        if(what & VX_TX2)       _u[1] = new v2d_t[sz];
        if(what & VX_TX3)       _u[2] = new v2d_t[sz];
        if(what & VX_TX4)       _u[3] = new v2d_t[sz];
        if(what & VX_NORMAL)    _n  = new v3d_t[sz];    
        if(what & VX_BINORM)    _b  = new v3d_t[sz];    
        if(what & VX_COLOR)     _c  = new u_rgba_t[sz];
    }

    ~RenderVx(){
        delete[] _v;
        delete[] _n;
        delete[] _b;
        delete[] _c;
        delete[] _u[0];
        delete[] _u[1];
        delete[] _u[2];
        delete[] _u[3];
    }
    v3d_t*  getv(int32_t idx){return &_v[idx];}
    v2d_t*  getu(int32_t idx, int32_t layer){ return &_u[layer][idx];}
    v3d_t*  getn(int32_t idx){return &_n[idx];}
    v3d_t*  getb(int32_t idx){return &_b[idx];}
    u_rgba_t* getc(int32_t idx){return &_c[idx];}
    
    void  setv(int32_t idx, const v3d_t* v){_v[idx] = *v;}
    void  setu(int32_t idx, const v2d_t* u_int32_t, int32_t layer){ _u[layer][idx]=*u_int32_t;}
    void  setn(int32_t idx, const v3d_t* v){_n[idx]=*v;}
    void  setb(int32_t idx, const v3d_t* v){_b[idx]=*v;}
    void  setc(int32_t idx, const u_rgba_t* c){_c[idx]=*c;}

    v3d_t* _v;
    v3d_t* _n;
    v3d_t* _b;
    u_rgba_t* _c;
    int32_t _sz;
    u_int32_t _what;
    v2d_t* _u[4];

};



//----------RENDERER PLUGIN INTERFACE---------------------------------------------------------
typedef int32_t (*PR_C)();
class ISystem;
class NO_VT Irender  
{
public:
    Irender(){};
    virtual ~Irender(){};

    virtual u_bool32 CreateRender(const ISystem* pe, RndStruct* prndStates, u_int32_t* gRconsts, HWND useThis=0)=0;
    virtual u_bool32 SetSystem(const ISystem* pe)=0;
    virtual void Destroy()=0;
    virtual HDC  Hdc()=0;
    virtual HWND Hwnd()=0;
    virtual void Swap()=0;
    virtual void SwitchMode()=0;
	virtual void ClearColor(f32_t, f32_t, f32_t, f32_t)=0;
    virtual int32_t  UseFontBitmaps(HFONT hf, int32_t a, int32_t b, int32_t c)=0;
    virtual u_int32_t Blend(u_int32_t rCtx)=0;
    virtual void UnBlend(u_int32_t context)=0;
    virtual void UnBlendv()=0;
		
    virtual void SelectTrMattrix(const Pos* pos, u_int32_t what)=0;
	virtual void ReleaseTrMattrix()=0;
    	

    virtual void SetArray(const RenderVx* pxyz)=0;
	virtual void ResetArray(const RenderVx* pxyz)=0;

    virtual void SetArrayPtrs(const v_vunc_t* pxyz, int32_t what)=0;
	virtual void ResetArrayPtrs(int32_t what)=0;
    virtual void DrawArrays(int32_t nStart, int32_t vxCnt, u_int32_t howWhat)=0;
   	
    virtual PR_C Gpa(const char* fn) = 0;
    virtual Htex GenTexture(const char* texFile, u_int32_t creaFlags)=0;
    virtual Htex ReGenTexture(const Htex& tex, int32_t x, int32_t y, int32_t bpp, const u_int8_t* pBuff, u_int32_t creaFlags)=0;
    virtual Htex GenTexture(int32_t x, int32_t y, int32_t bpp, const u_int8_t* pBuff, u_int32_t creaFlags)=0;
    virtual void AlterTexture(const Htex& itex, const u_int8_t* psub, int32_t s, int32_t t, int32_t u_int32_t, int32_t v)=0;
    virtual void SetCanonicalState(void)=0;
    virtual u_int32_t BindAllTextures(const Htex* pTxStg, u_int32_t comb)=0;
    virtual u_int32_t BindTexture(const Htex& txStg,  int32_t stage)=0;
	virtual void UnBindTexture(const Htex& txStg, int32_t stage)=0;
    virtual void DisableTextures(u_bool32 force=false)=0;
    virtual void RemoveAllTextures()=0;
    virtual void RemoveTexture(u_int32_t index,u_bool32=true)=0;
    virtual void RemoveTextures(const u_int32_t* ptex, int32_t count)=0;
    virtual void  SetCurrent()=0;
    virtual u_int32_t BindTex1(const Htex*)=0;
    virtual u_int32_t BindTex2(const Htex*, const Htex* )=0;
    virtual u_int32_t BindTex3(const Htex*, const Htex*,const Htex*  )=0;
    
    virtual void Render(int32_t font, f32_t x, f32_t y, f32_t z, const char* text, const u_rgba_t& clr)=0;
    virtual void Render(const Pos* pOp, u_int32_t trCtx, u_int32_t dCtx, const Htex* txCtx,  u_int32_t comb, const v_vunc_t* pVtx, int32_t vxCnt, u_int32_t how)=0;
    virtual void Render(const Pos* pOp, u_int32_t trCtx, const Htex* txCtx , u_int32_t comb,  const v_vunc_t* pVtx, int32_t vxCnt , u_int32_t how)=0;
    virtual void Render(const Pos* pOp, u_int32_t trCtx, const v_vunc_t* pVtx, int32_t vxCnt , u_int32_t how)=0;
    virtual void Render(u_int32_t dCtx, const Htex* txCtx, u_int32_t comb, const v_vunc_t* pVtx, int32_t vxCnt , u_int32_t how)=0;
    virtual void RenderFace(const Htex* dwctx, u_int32_t comb, const v_vunc_t* pvx, int32_t count, u_int32_t how)=0;
    virtual void Render(const v_vunc_t* pVx, int32_t first, int32_t  vxCnt, u_int32_t how)=0;
    virtual void Render(const u_int8_t* pV, int32_t start, int32_t count, u_int32_t what)=0;
    virtual void Render(int32_t strps, int32_t grps, int32_t* prims, v3d_t* vxes, v3d_t* norms, v2d_t* uvs1, v2d_t* uvs2, int32_t* idxes)=0;
    virtual void RenderFontList(f32_t x, f32_t y, const char* text, int32_t fontlist, const u_rgba_t& color)=0;
    virtual void RenderFontList3(const v3d_t& pos, const char* text, int32_t fontlist, const u_rgba_t& color)=0;
    virtual void TextOut(int32_t fontlist, f32_t xpos, f32_t ypos, const char* text, const u_rgba_t& color)=0;
    virtual void EnableStencil(int32_t how)=0;
    virtual void MatrixMode(int32_t m)=0;
    virtual void RenderVertex(const v_vunc_t* pvtci, u_int32_t dww=0xF)=0;
    virtual void Vertex2(f32_t x, f32_t y)=0;
	virtual void Vertex3(f32_t x, f32_t y, f32_t z)=0;
	virtual void Texture2(f32_t u_int32_t, f32_t v)=0;
    virtual void DbgLine(const v3d_t& a,const  v3d_t& b)=0;
    virtual void DeepTest(u_bool32 enable)=0;
    virtual void EnableBlend(u_bool32 enable)=0;
    virtual void EnableFog(f32_t fnear, f32_t ffar, f32_t dens, const u_rgba_t& color)=0;
    virtual void BlendFunc(int32_t a, int32_t b)=0;
    virtual void AlphaFunc(int32_t a, int32_t b)=0;
    virtual void Color4(char r, char g, char b, char a)=0;
    virtual void Color(u_rgba_t& r)=0;
    virtual void SetNormal(const v3d_t& v)=0;
    virtual void SetWndMinimizeFlag(u_bool32 bEnable)=0;
    virtual void RefreshModelView(f32_t ffov)=0;
    virtual void Clear(int32_t quakeTrick=0)=0;
    virtual void SetMatrix(const m_16t& m)=0;
    virtual void MulMatrix(const m_16t& m)=0;
    virtual void SetMaterial(u_int32_t fb, u_int32_t what, f32_t r)=0;
    virtual void SetMaterials(u_int32_t fb, u_int32_t what, f32_t* r)=0;
    virtual void SetViewMtx(const m_16t& m)=0;
    virtual void SetGamma(const f32_t* gamma, const f32_t* contrast, const int32_t* ilum)=0;
    virtual u_int32_t* GetPrimitives()=0;
	virtual void CleanTexCache(int32_t i=-1)=0;
    virtual void Push()=0;
    virtual void Pop()=0;
    virtual void GetViewMatrix(f32_t* m)=0;
	virtual void LoadIdentity()=0;
    virtual void PolygonMode(u_int32_t what,u_int32_t mode)=0;
    virtual void CullFace(u_int32_t what)=0;
    virtual void Enable(u_int32_t what)=0;
    virtual void Disable(u_int32_t what)=0;
    virtual void Rotate(const v3d_t& xyz)=0;
    virtual void RotateTo(const  v3d_t& xyz, f32_t angle)=0;
    virtual void Scale(const v3d_t& xyz)=0;
    virtual void Translate(const v3d_t& xyz)=0;
    virtual void Rotatef(f32_t, f32_t, f32_t, f32_t)=0;
    virtual void Scalef(f32_t, f32_t, f32_t)=0;
    virtual void Translatef(f32_t, f32_t, f32_t)=0;
    virtual void Begin(u_int32_t what)=0;
    virtual void BeginPrim(u_int32_t what)=0;
    virtual void End()=0;
    virtual u_int32_t Compile(const v_vunc_t* pm, int32_t count)=0;
    virtual void NewList(int32_t list)=0;
	virtual int32_t  GenLists(int32_t num)=0;
    virtual void EndNewList()=0;
    virtual void DeleteList(u_int32_t id,u_int32_t many=1)=0;
    virtual void CallList(u_int32_t ID)=0;
    virtual void CallLists(u_int32_t a, u_int32_t b, const void* ptr)=0;// states
    virtual void EnableLighting(u_bool32 bEnable, u_int32_t c)=0;
    virtual void EnableRenderMaterialLighting(u_int32_t amb, u_bool32 b)=0;
    virtual void SetLights(const RenderLight** pl, int32_t ncount);
    virtual int32_t  GetMaxHwLights()=0;
    virtual void SetMaterial(const RenderMaterial& mat);
    virtual const char*   GetExtensions(u_int32_t b)=0;
    virtual RndStruct* GetStruct()=0;
    virtual u_bool32 InitShader()=0;
    virtual void SetTransform(const Pos* pos, u_int32_t dCtx )=0;
	virtual void ResetTransform()=0;

};


#endif //__IRENDER_H__

