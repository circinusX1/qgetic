// MiniBsp.h: interface for the MiniBsp class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __MINIBSP_H__
#define	__MINIBSP_H__

#include <list>
#include "brush.h"
#include <bspfilestr.h>

class MiniBsp ;
class CMiniNode
{
public:
    CMiniNode(MiniBsp* pBsp, u_int32_t dw=0)
    {
        _cluster      = -1;
        _idxNodeThis  = -1;
        _nodeidx[0]	  = -1;
        _nodeidx[1]	  = -1;
        _planeIdx     = -1;
        _nPolys       = 0;
        _polyIdx      = -1;
        _nodeflags    = dw;
        _pBsp         = pBsp;
        _leafIdx	  = -1;
        _idxParent	  = -1;
        _planeIdxFinal =-1;
        _origPolyIndex = -1;
        _zonearea      = -1;
        _selected	   = 0;
        _content       = 0;
    }
    virtual ~CMiniNode(){}

    CMiniNode* Back();
    CMiniNode* Front();
    CMiniNode* Parent();
    int32_t     FontIdx(){return _nodeidx[1];}
    int32_t     BackIdx(){return _nodeidx[0];}
    u_bool32	IsEmptyLeaf()  {return  (_nodeflags & NODE_LEAF) && !(_nodeflags & NODE_SOLID);}
    u_bool32	IsLeaf()  {
        return  (_nodeflags & NODE_LEAF);
    }
    u_bool32	IsSolid() {
        return (_nodeflags & NODE_SOLID);
    }
    u_bool32	IsSpace() {return !(_nodeflags & NODE_SOLID) && (_nodeflags & NODE_LEAF);}
    u_bool32	IsNode()  {
        return !(_nodeflags & NODE_LEAF);
    }
    INLN    Plane&  GetPlane()const;//{
    INLN    Poly*   GetPolys(int32_t& count);
    INLN    Poly&   GetOrigPoly();
    int32_t     AddUniqueVertexes(vvector<v3d_t>& globPool);

public:
    int32_t         _idxNodeThis;  // this idx in bsp array
    int32_t         _idxParent;    // this idx in bsp array
    int32_t		    _nodeidx[2];   // front back
    u_rgba_t         _ilum;         // leaf ilum
    int32_t         _planeIdx;     // spliter plane index in bsp plane arrray
    int32_t			_leafIdx;
    int32_t         _planeIdxFinal;    // spliter plane index in bsp global plane arrray
    int32_t         _origPolyIndex;    // polygon index just in case;
    u_int32_t	    _nodeflags;
    int32_t		    _nPolys;       // leaf polys
    int32_t		    _polyIdx;      // start poly leaf index
    int32_t         _cluster;
    MiniBsp*    _pBsp;
    Box         _bbox;
    u_int32_t       _zonearea;
    u_int32_t       _flags;
    u_int32_t       _selected;
    u_int32_t       _content;
};



struct	CompileRcs
{
    int32_t				noode;
    list<Poly>		polys;
};


class Compiler;
class MiniBsp
{
public:
    typedef list<Poly>  PolyList;
    enum _CUT_FLAG{CUT_POLYS=0, PART_ON_FRONT, PART_ONBOTH, IS_HEIGHTMAP};
    enum _BSP_TYPE{BSP_LEAFY=0, BSP_TERRAIN, BSP_AABB_LEAFY, BSP_NODE};
public:
    friend class CMiniNode;
    friend class Compiler;
    MiniBsp(int32_t expected=128){
        _polys.reserve(expected);
        _nodesPtr.reserve(expected*2);
        _bRemUS     =  false;
        _bRemCoplan = true;
        _bClipSolid = true;
        _bremES     = false;
        _polyStartIdx = 0;
        _nodeStartIdx = 0;
        _leafStartIdx = 0;
        _pdroppedpolys  = 0;
        _timeStart    = 0;
        _dlgWaitOn    = 0;
        _balance      = 6;
        _treeflags   = 0;
        _name[0]	  = '.';
        _name[1]	  = 0;
        _treeprops     = 0;
        _cutflag      = CUT_POLYS;
        _bsptype      = BSP_LEAFY;
    };
    virtual ~MiniBsp(){
        Clear();
    };
    void            SetType(_BSP_TYPE bt, _CUT_FLAG cf ){
        _cutflag    = cf;
        _bsptype    = bt;
    }
    virtual void	Clear();
    vvector<Poly>&  GetPolys() {return _polys;};
    virtual u_bool32 Compile(vvector<Poly>& polys, u_int32_t brFlags, u_bool32 bRemES=false, u_bool32 bTextCvx=false); // virtual from base
    int32_t    AddUniqueVertexes(vvector<v3d_t>& globPool);
    void    ClipBrPolys(vvector<Poly>&, vvector<Poly>& , u_bool32 , u_bool32 ,u_bool32);
    CMiniNode*  Root(){return _nodesPtr[0];}//       _pRoot;
    vvector<CMiniNode*>& GetNodes(){return _nodesPtr; }
    CMiniNode* GetNode(int32_t idx){assert(idx < (int32_t)_nodesPtr.size()); return _nodesPtr[idx];}
    Plane&     GetPlane(int32_t iplane){assert(iplane < (int32_t)_planes.size()); return _planes[iplane];}
    Poly&      GetPoly(int32_t idx) {assert(idx < (int32_t)_polys.size()); return _polys[idx];}
    u_bool32       GetError(){return _bComplErr;}
    void       WantDroppedPolys(vvector<Poly>* polys){_pdroppedpolys = polys;}
    u_bool32       IsDummy(){return _nodesPtr.size() ? (_nodesPtr[0]->_planeIdx ==-1) : true;}
    virtual void    Finalize(){};
    u_bool32    IsMainModel(){return _treeflags & BRSH_MAINBSP;}
protected:
    virtual void   AddNode(CMiniNode* pNn){pNn->_idxNodeThis = _nodesPtr.size(); _nodesPtr << pNn;}
    virtual void UpdateNodeBB(CMiniNode* pNode, PolyList& polys);
    virtual void UpdateNodeBB(CMiniNode* pNode, vvector<Poly>& polys);
    u_bool32 AllSpliters(PolyList& polyList);
    u_bool32    _CheckSafeStack(int32_t backSize);
    u_bool32    BuildDummyBSP(vvector<Poly>& polys);
    int32_t     FindPLane(const Plane& p);
    void    MakeRoot(){assert(_nodesPtr.size()==0); AddNode(new CMiniNode(this));};
    void	BuildPlaneArray(PolyList& polys, u_int32_t flags);
    void    R_ClipPolyList(int32_t,  vvector<Poly>& , vvector<Poly>&);
    void	R_Compile(int32_t nodeIdx, PolyList&);
    void	R_TerrainCompile(int32_t nodeIdx, PolyList&);
    int32_t 	GetBestSplitter(PolyList& polys, Poly* pWantPoly=0);
    void	RepairSplits(vvector<Poly>& , vvector<Poly>& );
    void	Trace(int32_t idxnode);
    virtual CMiniNode*  CreateNode(u_int32_t dw){return new CMiniNode(this, dw);}
    int32_t		Partition(CMiniNode* pNode, PolyList&, PolyList&,PolyList&);
    void	PartitionCSGTerrain(CMiniNode* pNode, PolyList&, PolyList&,PolyList&);
    Plane   GetCSGTerOptimSpliterPlane(PolyList& polys, int32_t moe=1);
    void    FindCsgTerrSplitterPlane(PolyList& polys, CMiniNode* pNode);
    u_bool32	_FormConvex(PolyList& frontPolys);
public:
    vvector<CMiniNode*>	_nodesPtr;
    vvector<Poly>	    _polys;
    vvector<Plane>	    _planes;
    vvector<Poly>*	    _pInPolys;
    vvector<Poly>*      _pdroppedpolys;
    vvector<Plane*>     _pSplPlanes;

    int32_t		     _polyStartIdx;
    int32_t          _nodeStartIdx;
    int32_t          _leafStartIdx;

    // where the leafs are
    _CUT_FLAG    _cutflag;
    _BSP_TYPE    _bsptype;

    u_bool32         _bRemUS;
    u_bool32		 _bRemCoplan;
    u_bool32		 _bClipSolid;
    u_bool32         _bComplErr;
    u_bool32         _bremES;
    Box			 _addedPolysBox;
    u_int32_t        _timeStart;
    u_bool32         _dlgWaitOn;
    int32_t			 _reachedDepth;
    int32_t          _balance;
    char		 _name[64];
    u_int32_t        _treeprops;
    u_int32_t        _treeflags;
    int32_t          _lefmost;
    u_bool32		 _btextconvex;
    int32_t          _usersplitters;
    int32_t          _safeStack[64];
    int32_t          _nSafeStack;
    static       Poly*      _pWorkingPoly[2];
};

//------------------------------------------------------------------------------------------
INLN Plane& CMiniNode::GetPlane()const
{
    assert(_planeIdx>=0 && _planeIdx < (int32_t)_pBsp->_planes.size());
    if(_planeIdx>=0 && _planeIdx < (int32_t)_pBsp->_planes.size())
        return _pBsp->_planes[_planeIdx];
    return _pBsp->_planes[0];
};

INLN Poly*   CMiniNode::GetPolys(int32_t& count){
    vvector<Poly>& polys = _pBsp->GetPolys();
    count = _nPolys;
    return &polys[_polyIdx];
};

INLN    Poly&   CMiniNode::GetOrigPoly()
{
    vvector<Poly>& polys = _pBsp->GetPolys();
    assert(_origPolyIndex>=0);
    return polys[_origPolyIndex];
}

#endif // !__MINIBSP_H__
