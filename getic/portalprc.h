// PortalPRC1.h: interface for the CPortalPRC class.
//
//---------------------------------------------------------------------------------------
#ifndef __PORTAL_PRC__
#define __PORTAL_PRC__

#include "bsptree.h"
#include "poly.h"


//---------------------------------------------------------------------------------------

#define PORT_INITAL     0x1
#define PORT_DUP        0x2
#define PORT_REVERSED   0x4
#define PORT_DELETED    0x8
#define PORT_PROCESSED  0x10

//---------------------------------------------------------------------------------------
class CLeaf;
class PortalPRC;
class Portal : public Plane
{
public:
    Portal(){};

    Portal(PortalPRC* pp):_idxThis(0),
                            _planeIdx(0),
                            _planeIdxFinal(-1),
                            _flags(0),
                            _pPortPrc(0)
    {
        Q_UNUSED(pp);
    }      // reduce fragmentation
    Portal(const Portal& r):Plane(_n,_c){
        _planeIdx = r._planeIdx;
        _planeIdxFinal = r._planeIdxFinal;
        _flags    = r._flags;
        _vxes     = r._vxes;
        _sideLIdx = r._sideLIdx;
        _pPortPrc = r._pPortPrc;
        _idxThis  = r._idxThis;
    }
    void Reverse(){
        _flags |= PORT_REVERSED;
        _n.negate();
        _c=-_c;
        _vxes.reverse();
    }
    Portal& operator=(const Portal& r){
        if(this !=&r){
            _planeIdx      = r._planeIdx;
            _planeIdxFinal = r._planeIdxFinal;
            _flags      = r._flags;
            _vxes       = r._vxes;
            _sideLIdx   = r._sideLIdx;
            _n          = r._n;
            _c          = r._c;
            _pPortPrc   = r._pPortPrc;
            _idxThis    = r._idxThis;
        }
        return *this;
    }
    void AdjustVxOrder()
    {
        Plane tp;

        GCalcNormal(&tp,_vxes[0],_vxes[1],_vxes[2]);

        if(!IsZero( Vdp(tp._n, _n)-1.0f,EPSMED))
        {
            _vxes.reverse();
            GCalcNormal(&tp,_vxes[0],_vxes[1],_vxes[2]);
        }
    }
    virtual ~Portal(){}
    // this against plane
    REL_POS Classify(Plane& plane)
    {
        int32_t		fronts  = 0, backs = 0, coinciss = 0;
        int32_t		vxes	= _vxes.size();
        f32_t	rdp;
        FOREACH(vvector<v3d_t>, _vxes, vx)
        {
            rdp = plane.DistTo(*vx);
            if(rdp > GEpsilon)
                fronts++;
            else if(rdp < -GEpsilon)
                backs++;
            else{
                coinciss++;
                backs++;
                fronts++;}
        }
        if (coinciss == vxes)
            return ON_PLANE;
        if (fronts == vxes)
            return ON_FRONT;
        if (backs  == vxes)
            return ON_BACK;
        return ON_SPLIT;
    }
    void CopyProps(Portal& pFrom){
        _planeIdxFinal = pFrom._planeIdxFinal; // is the depth
        _planeIdx      = pFrom._planeIdx;
        _flags         = pFrom._flags;
        _sideLIdx      = pFrom._sideLIdx;
        _n             = pFrom._n;
        _c             = pFrom._c;
        _idxThis       = pFrom._idxThis;
    }
    void    AddSideLeafIdx(int32_t idx){_sideLIdx<<idx;}
    void    Split(Plane& plane, Portal& a, Portal& b);
    INLN    CLeaf*  BackLeaf();
    INLN    CLeaf*  FrontLeaf();
    INLN    Plane&  GetPlane();
    void    Clear(){_vxes.clear();}
public:
    int32_t             _idxThis;
    int32_t             _planeIdx;       // index in planes index of the BSP wich ortal belons too
    int32_t             _planeIdxFinal;
    u_int32_t       _flags;
    vvector<v3d_t>     _vxes;
    vvector<int32_t>    _sideLIdx;
    vvector<int32_t>    _sideLIdxFinal;
    PortalPRC*      _pPortPrc;
};

//---------------------------------------------------------------------------------------
class CBspTree;
class View3D;
class Compiler;
class PortalPRC
{
public:
    friend class Portal;
    friend class Compiler;

    PortalPRC(){

        _pTree = 0;
        _testExterior = 0;
        _mergeLeafs = 0;

    }
    ~PortalPRC(){Clear();}
    u_bool32    Process(CBspTree& tree, u_bool32 testExterior=false);
    void    Clear();
    void    Render(View3D* pV);
    CLeaf*  GetLeaf(int32_t idx){
        CMiniNode* pL = _pTree->GetLeaf(idx);
        assert(pL->_nodeflags & NODE_LEAF);
        return (CLeaf*)pL;
    }
    Portal& GetPortal(int32_t idx){return _portals[idx];}
    vvector<Portal>& GetPortals(){return _portals;}
    u_bool32    HasPortals(){return _portals.size()>0;}
    void    MergeNarowLeafs();
private:
    void    CleanProcFlag(vvector<Portal>&  portals);
    void    MovePolysIdxInLeafs();
    void    R_CutPortalWithLeafs(Portal& portal, CMiniNode* pNode);
    void    MergeLeafs(CLeaf* ,CLeaf*, Portal& portal);
    u_bool32    TestExteriorPortals();
    void	CutPortalWithParents(Portal& portal, CMiniNode* pNode);
    void	CutPortalWithNodeBox(Portal& portal, Box& box,u_bool32 b=0);
    void    ReduceVertexes(vvector<Portal>& portals);
    u_bool32	CalculateInitialPortal(Portal&, CMiniNode* );
    u_bool32    BuildPortals();
    u_bool32	DuplicatePortals(vvector<Portal>& i, vvector<Portal>& o);
    void	R_ClipPortal(int32_t nodeIdx, Portal& pPortal, vvector<Portal>& portals);
    void	R_ClipPortalWithTree(int32_t nodeIdx, Portal& pPortal, vvector<Portal>& portals);
    u_bool32	CleanObsolettePortals(vvector<Portal>& );
    u_bool32	PortalHasDuplicate(vvector<Portal>::iterator pi, vvector<Portal>& plist);
    Plane&  GetPlane(int32_t idx){return _pTree->GetPlane(idx);}
    u_bool32    ReFlood();
    void    R_FloodBSP(CBspTree*  tmpTree, int32_t iLeaf, u_int8_t* wetLeafs);
    void    AddPortal(Portal& p){
                p._idxThis = _portals.size();
                _portals << p;
            }
    void	ClipWithLeafSides(CLeaf* pLeaf, Portal& portal);
public:
    vvector<Portal>    _portals;
    CBspTree*          _pTree;
    u_bool32               _testExterior;
    u_bool32               _mergeLeafs;
};

//----------------------------------------------------------------------------------------
INLN CLeaf*  Portal::BackLeaf(){
    assert(_sideLIdx.size()==2);
    return _pPortPrc->GetLeaf(_sideLIdx[0]);
}

INLN CLeaf*  Portal::FrontLeaf(){
    assert(_sideLIdx.size()==2);
    return _pPortPrc->GetLeaf(_sideLIdx[1]);
}

Plane&  Portal::GetPlane(){
    return _pPortPrc->GetPlane(_planeIdx);
}

#endif // !defined(AFX_PORTALPRC1_H__53BBF425_94DD_4BB0_9EE6_FB6AA639D797__INCLUDED_)
