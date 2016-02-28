//---------------------------------------------------------------------------------------

#ifndef __BRUSH_H__
#define __BRUSH_H__

//---------------------------------------------------------------------------------------
#ifdef WINDOWS
#   pragma warning (disable: 4786)
#endif
#include "poly.h"
#include "minibsp.h"
#include "baseentity.h"
#include <bspfilestr.h>


//--------------------------------------------------------------------------------------
typedef enum _TEX_MAP
{
    TM_LIN,
    TM_CYL,
    TM_SPH,
    TM_ENV,
}TEX_MAP;

#define BRSH_CLEAN (BRSH_DIRTY|BRSH_NEW)
//--------------------------------------------------------------------------------------
typedef enum _P_TYPE{
                E_CUBE=1,
                E_CONE,
                E_CYL,
                E_SPHERE,
                E_SHEET,
                E_STAIRS,
                E_TERRAIN,
                E_PREFAB}P_TYPE;
// struct Iniaial flags of this brush creation
struct  CreaFlags
{
    f32_t    _thick;
    u_int32_t   _brushflags;
    int32_t     _vstrips;
    int32_t     _hstrips;
    TEX_MAP _tm;
    P_TYPE  _t;
};

//---------------------------------------------------------------------------------------
class	Brush  ;
typedef vvector<Brush*>            PBrushes;
typedef vvector<Brush*>::iterator  PPBrush;

//---------------------------------------------------------------------------------------
class CMotion ;
class Scene;
class Brush  : public BaseEntity, public CanProps
{

public:
    Brush*   _pResult;
    Brush*   _pUnrotated;
    v3d_t		 _t;
    v3d_t		 _r;
    v3d_t		 _s;
    v3d_t		 _f;	//shift
    m_16t       _mt;
    int32_t		 _unicID;
    Polys	 _polys;
    Box      _box;
    u_bool32     _selected;
    u_bool32	 _copyed;
    u_int32_t    _brushflags;
    u_int32_t    _brushprops;
    v3d_t       _physicsA;         // stickness, rejection etc...
    v3d_t       _rotSteps;         // rotation
    v3d_t       _physicsD;
    CMotion* _pMotion;        // index to dynaics movement equation (details only)

    PBrushes  _pCutBrshes;
    PBrushes  _pSldBrushes;
    PBrushes  _pProjBrushes;
    u_int32_t	  _polySec;
    int32_t       _group;            // group them by user design time
    f32_t	  _thikness;
    TEX_MAP   _texmap;
    CreaFlags _cf;
    Box       _cbox;			 // box of decupated area from a solid. Updated ech cut

    void*     _pUsrData;
    int32_t		  _nZoneNo;
    int32_t      _intask;

public:
    Brush();
    Brush(const Brush& b);
    virtual ~Brush();
    // make them
    void    FindNormal(Vtx2& tv, Polys& polys);
    u_bool32    CopyDiametral(const Brush& b);
    void    MakeStairs(v3d_t& dims, int32_t radstrips, int32_t vstrips, f32_t thikness, TEX_MAP tm = TM_LIN);
    void    MakeCone(f32_t rad, f32_t height, int32_t radstrips, int32_t vstrips, TEX_MAP tm = TM_LIN);
    void    MakeCylinder(f32_t rad, f32_t height, int32_t radstrips, int32_t vstrips, TEX_MAP tm = TM_LIN);
    void    MakeCube(const v3d_t& dims, TEX_MAP tm = TM_LIN);
    void    MakeSheet(v3d_t& dims, int32_t vstrips, int32_t hstrips, TEX_MAP tm = TM_LIN);
    void    MakeSphere(f32_t dims, int32_t vstrips, int32_t hstrips, TEX_MAP tm = TM_LIN);
    void    MakeTerrain(v3d_t& meshDim, int32_t dimX, int32_t dimY, u_int8_t* pBuffer);
    void    CalctexCoord(int32_t idx);
    void    ModifyTexCoord(int32_t idx);
    void    CalcVxNormals();
    int32_t     CalcVxTask();
    f32_t    GetVolume();
    void	StartSecGrp(f32_t t){_polySec = _polys.size();_thikness=t;}
    Brush&  operator<<(const Brush& b);
    Brush&  operator=(const Brush& b);
    Brush&  operator<<(Poly& p);
    Brush&  operator<<(vvector<Poly>& p);
    void    RotateTCOneStep();
    Brush&  GetWorldPos();
    Brush&	GetResult();
    Brush&	GetPrimitive();
    u_bool32    IsDisabled(){return _brushflags & BRSH_DISABLED;};
    void    RecalcRotation();
    void    Reinit();
    void    GlTransform();
    void    ResetTexCoords(int32_t ti=-1);
    void    TriangulateDeplanetizedFaces();
    void    Snap2grid(f32_t gs,u_bool32 m);
    void	SBoxTr(Box& newBox);
    void    Reverse();
    void    HalloUnHallo();
    m_16t&     GetTrMatrix();
    void    DeSelect(){_selected=false;}
    void    Select(){_selected=true;}
    u_bool32    IsSelected(){return _selected;}
    u_bool32    IsSpecial();
    void	MakeTm();
    void    Recalc(u_bool32 withnormals=false);
    void    Replan();
    void	GetTxPolysCopy(Polys& pl);
    void	GetTxReversedPolysCopy(Polys& pl);
    void    Clear();
    void    Mirror();
    u_bool32    IsSealed();
    void    Seal();
    void    AddCutter(Brush* pCutter);
    void    RemoveCutter(Brush* pCutter);
    void    AddBrush(Brush* pBrush);
    void    AppendPrim(Brush& brush);
    void    RemoveBrush(Brush* pBrush);
    void    AddProjection(Brush* pCutter);
    void    RemoveProjection(Brush* pCutter);
    u_bool32    ApplyCuts();
    u_bool32    Split(Brush& splBrush, Brush& frontB, Brush& backB);
    void    Equal(const Brush& b,u_bool32 polys=true);
    void	Move(const v3d_t& v);
    void	Scale(const v3d_t& v, const v3d_t& t,f32_t zoom);
    void	ScaleTr(const v3d_t& v,const  v3d_t& t);
    void	Raise(const v3d_t& v, f32_t zoom);
    u_bool32    LimitScaleVector(v3d_t& v,f32_t);
    void	Rotate(v3d_t& v, u_bool32 aroundOrigin=false);
    void	SetPos(const v3d_t& center);
    void	SetSize(const Box& extends);
    void    FlagFaces(u_int32_t f);
    void	Deleteface(Poly* p);
    u_bool32    HasPolys(){return _polys.size()>0;}
    u_bool32    IsSolid(){return _brushflags & BRSH_SOLID;}
    u_bool32    IsTouched(){return this != _pResult;}
    void	DeletePolys();
    u_bool32    HasFlag(u_int32_t f);
    u_bool32    IsCut();
    u_bool32    IsProjector();
    u_bool32    IsSplitter();
    u_bool32    IsZone();
    u_bool32    IsSelection();
    u_bool32	IsDirty(){return _brushflags & BRSH_DIRTY;};
    void	Dirty(u_bool32 b){if(b) _brushflags|=BRSH_DIRTY; else  _brushflags&=~BRSH_DIRTY;};
    u_bool32    HasMotion(){return _pMotion!=0;}
    Poly&   FirstPoly(){return _polys[0];}
    void    TriFanIt();
    int32_t     Triangulate();
    void    CopyPolysFrom(Brush& br);
    const Poly&	GetTerrainPoly(int32_t x, int32_t y, int32_t tridx)const;
    void    AddPoly(Poly& p);
    void    AddPolys(vvector<Poly>& polys);
    void    ErasePoly(Poly& p);
    void    UpdateCutsTouches();
    void    UpdateSolidsTouches();
    void    UndoLastTr();
    void    ExplodeInConvexBrushes(PBrushes& pResults);
    u_bool32    IsConvex();
    int32_t     SPlitByPlane(Plane& p0, Brush* pNewA, Brush* pNewB);
    int32_t    &ZoneIdx(){return _nZoneNo;} // fo zone hold here this index
    u_int32_t  &VisData(){return _polySec;} // fo zone hold here this index
    u_bool32    RecalcBoxAndNormals(u_bool32 withnormals);
    int32_t     GetPolyCount(u_int32_t flags, u_int32_t props = (u_int32_t)-1);
    Poly *  GetNextPoly(class Poly *);
    Poly *  GetPrevPoly(class Poly *);
private:
    void	StoreInitiaTexCoord();
    void    RescalePolys(int32_t s, int32_t e,const  v3d_t& sc,const  v3d_t& tr, Box& b,const Box& b1);
    void	CalcEnvTc(int32_t stage = 0);
    void    R_Explode(PBrushes& pResults);

public:
    static int32_t GID;

private:
    void    AlocateResultPtr();
    void	DeleteResultPtr();
    void	RecalcTM();
    void	Shift(const v3d_t& v);

};


#endif //
