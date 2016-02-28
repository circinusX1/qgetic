#ifndef __POLY_H__
#define __POLY_H__

//---------------------------------------------------------------------------------------
#include <basecont.h>
#include <bspfilestr.h>
#include <baseentity.h>
#include "texadapter.h"


// Structures passed between plugins and main application
//---------------------------------------------------------------------------------------
// internals
#define	POLY_FROM_CUTB	    0x1
#define	SPLITTER_POLY	    0x2
#define	POLY_DELNAILED      0x4
#define	POLY_DELETED	    0x8
//gap
#define POLY_PORTAL0        0x20
#define POLY_PORTAL1        0x40
#define	POLY_PROCESSED		0x80
#define POLY_HIDDEN         0x100
//
#define POLY_SEP_LMAP       0x400       // trat lmap individually for this poly
#define POLY_DIRTY          0x800
#define POLY_EXTRUDED       0x1000      // allows rotation and moving cose any vertex of
#define POLY_TERRAIN        0x2000      // allows rotation and moving cose any vertex of
#define POLY_NOPROJ         0x4000
#define POLY_INSERTED       0x10000

//---------------------------------------------------------------------------------------
INLN u_bool32 IsOrtogonal(const v3d_t& v)
{
    return v.x==1 ||v.y==1||v.z==1||v.x==-1||v.y==-1||v.z==-1;
}

//---------------------------------------------------------------------------------------
// holds the texture coord transformation applyed in the editor only
struct TexCoords
{
    TexCoords& operator=(const TexCoords& tc){
        if(this != &tc){
            ::memcpy(this,&tc,sizeof(TexCoords));
        }
        return *this;
    }
    u_bool32 operator==(const TexCoords& tc){
        return !memcmp(this,&tc,sizeof(TexCoords));
    }

    int32_t 	rotate;
    f32_t	scalex;
    f32_t	scaley;
    f32_t	shiftx;
    f32_t	shifty;
    int32_t	flipV;
    int32_t	flipH;
    f32_t	b;
    f32_t	c;
    f32_t	d;
    f32_t	e;
};

//---------------------------------------------------------------------------------------
#define	SIZE_0	    0x0
#define	SIZE_N	    0x1
#define	SIZE_S	    0x2
#define	SIZE_E	    0x4
#define	SIZE_W	    0x8
#define	SIZE_MOVE	0x10

//---------------------------------------------------------------------------------------
#define	SIZE_SW	(SIZE_S|SIZE_W)
#define	SIZE_SE	(SIZE_S|SIZE_E)
#define	SIZE_NW	(SIZE_N|SIZE_W)
#define	SIZE_NE	(SIZE_N|SIZE_E)
#define SAFE_TIDX 3     // 3th tex stage


class Poly;
//---------------------------------------------------------------------------------------
// extended vertex that allows selection. reuses the _cmd because all will be POLYGONS
//---------------------------------------------------------------------------------------
/*
 *     int		_index;		// just for final save is kept here
    UV		_tcCrea;
    Poly*	_pPoly;		// !=0 whenb vertex is selected
    DWORD	_extra1;
   */
//---------------------------------------------------------------------------------------
struct Fl_Vtx2_Old : public v_vunc_t
{
    Fl_Vtx2_Old():_index(-1),_pPoly(0),_extra1(0){}
    int32_t		_index;		// just for final save is kept here
    v2d_t		_tcCrea;
    int32_t     _pPoly;		// !=0 whenb vertex is selected
    u_int32_t	_extra1;

};

class Poly;
struct Fl_Vtx2 : public v_vunc_t
{
    Fl_Vtx2():v_vunc_t(),_index(-1),_pPoly(0),_extra1(0){

    }
    Fl_Vtx2(const Fl_Vtx2_Old& r):_index(r._index),_tcCrea(r._tcCrea),
                                    _pPoly((Poly*)r._pPoly),
                                    _extra1(r._extra1)
    {
        _nrm   = r._nrm;
        _rgb   = r._rgb;
        _user  = r._user;
        _uv[0] = r._uv[0];
        _uv[1] = r._uv[1];
        _uv[2] = r._uv[2];
        _uv[3] = r._uv[3];
        _xyz   = r._xyz;

    }
    int32_t		_index;		// just for final save is kept here
    v2d_t		_tcCrea;
    Poly*       _pPoly;		// !=0 whenb vertex is selected
    u_int32_t  	_extra1;

};



class Vtx2 : public v_vunc_t
{
public:
    Vtx2(){_pPoly=0;_extra1=0;}

    Vtx2(v3d_t& v){_xyz=v;_pPoly=0;_extra1=0;}
    Vtx2(const Fl_Vtx2& v){
        _tcCrea = v._tcCrea;
        _pPoly  = (Poly*)v._pPoly;
        _extra1 = v._extra1;
        _xyz = v._xyz;    // 12 ...
        _nrm = v._nrm;    // 12 ...
        _uv[0] = v._uv[0];	 // 32 .. .. .. ..
        _uv[1] = v._uv[1];	 // 32 .. .. .. ..
        _uv[2] = v._uv[2];	 // 32 .. .. .. ..
        _uv[3] = v._uv[3];	 // 32 .. .. .. ..
        _rgb = v._rgb;	 // 4  .
        _user = v._user;	 // 4 .
    }
    Vtx2(const Vtx2& v):v_vunc_t(v){
        _tcCrea = v._tcCrea;
        _pPoly  = v._pPoly;
        _extra1 = v._extra1;
        _xyz = v._xyz;    // 12 ...
        _nrm = v._nrm;    // 12 ...

    }
    ~Vtx2(){
        ;
    }
     Vtx2& operator = (const Vtx2& r){
        if(this != &r){
                _xyz = r._xyz;
                _nrm = r._nrm;
                _uv[0] = r._uv[0];
                _uv[1] = r._uv[1];
                _uv[2] = r._uv[2];
                _uv[3] = r._uv[3];
                _rgb   = r._rgb  ;
                _user  = r._user ;

            _tcCrea = r._tcCrea;
            _pPoly  = r._pPoly;
            _extra1 = r._extra1;
        }
        return *this;
    }
    void  Select(Poly* pB){_pPoly = pB;}
    u_bool32  IsSelected(){return _pPoly!=0;}

    int32_t		_index;		// just for final save is kept here
    v2d_t		_tcCrea;
    Poly*       _pPoly;		// !=0 whenb vertex is selected
    u_int32_t	_extra1;
};

class Poly;



typedef vvector<Vtx2>             Vertexes;
typedef vvector<Vtx2*>            PVertexes;
typedef vvector<Vtx2>::iterator   PVertex;
typedef vvector<Vtx2>::const_iterator   CPVertex;
typedef vvector<Vtx2*>::iterator  PPVertex;

//---------------------------------------------------------------------------------------
class Brush;
class nvtclass Poly : public Plane, public TexCarrier, public CanProps
{
public:
    int32_t             _idxSplitter;   // temp durring CSG
    int32_t             _idxParent;     // temp durring CSG
    int32_t             _planeIdx;      // bsp plane index
    int32_t             _planeIdxFinal; // final index BSP prior to save BSP
    int32_t				_fragments;     // number of fragments (CSG)
    int32_t             _use2splitidx;
    u_bool32            _selected;      // poly is selected
    Box             _box;       // poly bounding box
    TexCoords		_texcoord;  // holds  the texture coord transformation applyed in the editor only
    LmInfo          _lmInfo;    // light  map index size, flags and axes
    Htex            _ilmTex;    // used only in editor for light map index
    Htex            _nonProjTex;
    Brush*          _pBrush;     // brush this poly is on
    Vertexes        _vtci;	    // vertexex array
    f32_t            _area;
    void*           _pUsrData;   // bsp node Idx;

    v2d_t              _tanimA;    // texture animation v2d_t
    v2d_t              _tanimB;    // texture animation v2d_t
    v3d_t              _texV1;     // not used
    v3d_t              _texV2;     // not used

    u_int32_t           _polyflags;     // polygon flags (in editor)
    u_int32_t           _polyflags2;    // extra flags
    u_int32_t           _polyprops;     // polygon properties in BspFIleStr.h

    u_rgba_t             _colorS;   // spec color MATERIAL
    u_rgba_t             _colorD;   // diffuse color MATERIAL
    u_rgba_t             _colorE;   // emmisive color MATERIAL
    f32_t            _shiness;  // shiness
    f32_t            _bump;
    f32_t            _friction;


    static  int32_t    _TexCopy;
    static  v3d_t     __Max;
public:
    Poly();
    ~Poly();
            Poly(const Poly& p);
    u_bool32    operator<(const Poly& p){return _area < p._area;}
    u_bool32    operator>(const Poly& p){return _area > p._area;}
    friend u_bool32   operator>(const Poly& p, const Poly& q){return p._area > q._area;}
    void    Clear();
    void    Create(int32_t points, v3d_t* pPoints, Brush* pBrush);
    void    Create(int32_t points, Vtx2* pPoints, Brush* pBrush);
    void	Transform(m_16t m);
    void    GetWorldPos(m_16t& m, Poly& pr);
    void	ApplyNewTexCoord();
    void    Recalc();
    u_bool32    ContainPoint(v3d_t& pi);
    void    Reverse();
    void    SnaptoPoint(int32_t ammnt);
    void    RotateTC(int32_t steps=0, int32_t ti=1);
    void    RotateTCRad(f32_t grads, int32_t ti=0);
    void    ResetTexCoords(int32_t ti=-1);
    void    DeSelect();
    void    Select();
    void	SortVertexes();
    void	InitCalcTC(int32_t ti=-1);
    void    CopyTc0InAll();

    void	RescaleTexCoord(u_bool32 bNew, int32_t ti=0);
    void    ReApplyTexture(u_int32_t creaFlags);
    u_bool32    IsSelected(){return _selected;}
    void    CopyProps(Poly& p);
    u_bool32    IntersectsPoly(Poly& p2);
    void    Split(Plane& plane, Poly& a, Poly& b, u_bool32 bAllways=false);
    void    Clip(Poly& plane, Poly& p);
    REL_POS Classify(Plane& plane);
    int32_t     Classify(v3d_t& point);
    v3d_t      GetCenter()const;
    f32_t    GetAreea();
    u_bool32	IsSplitter(){return (_polyflags & SPLITTER_POLY);}
    void	SetSplitter(){_polyflags |= SPLITTER_POLY;}
    Poly&   operator=(const Poly& p);
    void    CalcNormal();
    void    FlipTC(char cd);
    Poly&   operator<<(Vtx2& v);
    Poly&   operator<<(v3d_t& v);
    u_bool32    operator == (const Poly& p);
    u_bool32    IsInnerPoint(v3d_t& pi);
    u_bool32    IsDeplanetized();
    void    BuildTexEdges(v3d_t& v0, v3d_t& v1 ,v3d_t& v2);
    void    BuildTexAxes(v3d_t& v0, v3d_t& v1 );
    int32_t     TrifanIt(vvector<Poly>&   fragments);
    int32_t     Triangulate(vvector<Poly>&   fragments);
    int32_t     AddVertex(const v3d_t &vertex, vvector<Poly> &fragments);
    size_t  GetVxIndex(const Vtx2& vtx);
    void    MinMaxUV(int32_t layer, v2d_t& minUV,v2d_t& maxUV, v3d_t* vnorm = 0);
};


u_bool32    PolyTouchesPoly(Poly* a,Poly* b);

INLN    int32_t GetBrushes(vvector<Poly*>& polys, set<Brush*>& brushes)
{
    FOREACH(vvector<Poly*>, polys, pp)
    {
        brushes.insert((*pp)->_pBrush);
    }
    return brushes.size();
}

typedef vvector<Poly>             Polys;
typedef vvector<Poly>::iterator   PPoly;

typedef vvector<Poly*>             PPolys;
typedef vvector<Poly*>::iterator   PPPoly;

//--------------------------------------------------------------------------
struct PolyEdge
{
      Vtx2  *_edge[2];  //points in 2 vxes of the poly
      Poly  *_poly;
};


#endif //__POLY_H__
