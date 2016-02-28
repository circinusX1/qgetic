// Author: Marius Chincisan: 2003
//---------------------------------------------------------------------------------------
#ifndef __BSPTREEX_H__
#define __BSPTREEX_H__

#ifdef WINDOWS
#   pragma warning (disable: 4786)
#endif
//---------------------------------------------------------------------------------------

#include <basecont.h>
#include "poly.h"
#include "minibsp.h"


//---------------------------------------------------------------------------------------
class SceItem;
class CLeaf : public CMiniNode
{
public:
    CLeaf(MiniBsp* pBsp, u_int32_t dw=0):CMiniNode(pBsp, dw),
            _pNext(0),
            _pvsIdx(-1),
            _iModIndex(8),
            _portIdxes(8),
            _iItems(8)

    {
        _zonearea=(-1);
    }
    virtual ~CLeaf(){_portIdxes.clear();_iModIndex.clear();}
    void     AddPortalIdx(int32_t idx){
        _portIdxes.push_back(idx);
    };
    vvector<int32_t>&   GetPortalIdxes(){return _portIdxes;}

    CLeaf*          _pNext;
    int32_t				_pvsIdx;
    vvector<int32_t>    _iModIndex;          // linked module(bsp) index from compiler bsp's
    vvector<int32_t>    _portIdxes;          // in the port proceesor
    vvector<int32_t>    _iItems;
};

//---------------------------------------------------------------------------------------
// caries the colided point properties
class CBspTree;
struct CollPack
{
    CollPack(){
        ::memset(this,0,sizeof(*this));
        _dist = INFINIT;
    }
    CollPack& operator=(const CollPack& rc){
        if(this != &rc){
            ::memcpy(this, &rc,sizeof(*this));
        }
        return  *this;
    }
    v3d_t          _ip;        //  ip
    int32_t         _nLeaf;     //  hit leaf
    Plane*      _pPlane;    //  hit Plane
    Poly*       _pPoly;     //  hit Polygon
    CBspTree*   _pTree;     //  collided tree
    f32_t        _dist;
};

//---------------------------------------------------------------------------------------
class View2D;
class View3D;
class PortalPRC;
class CBspTree  : public MiniBsp
{
public:
    u_bool32 BuildPortals();
    CBspTree();
    virtual ~CBspTree();
    virtual void Clear();                       // virtual from base
    virtual u_bool32 Compile(vvector<Poly>& polys, u_int32_t brFlags, u_bool32 bremES=false, u_bool32 bTextCvx=false); // virtual from base
    CLeaf*  GetLeaf(u_int32_t idx){assert(idx < _leafs.size()); return _leafs[idx];}
    void    Draw3DBSPTree(View3D* pV, int32_t& leaf,int32_t curLeaf, u_int32_t mode);
    void    Draw2GlTree(View2D*,u_int32_t,int32_t,u_bool32);
    void    LinkDetailModel(CBspTree* pModelTree, u_bool32 bAtLeaf=true);
    int32_t		R_FindLeaf(int32_t nodeIdx,v3d_t& pos);
    int32_t     GetLeafsCount(){return _leafs.size();}
    vvector<CLeaf*>& GetLeafs() {return _leafs;}
    void    SetMainModel(){_treeprops |= BRSH_MAINBSP;}
    //
    int32_t     AddUniquePlane(vvector<Plane>&  allPlanes, map<int32_t,int32_t>& plIdxMap);
    void    ReindexPlanes(map<int32_t,int32_t>& pidxes);
    int32_t     CollectPNL(vvector<Poly*>&  allPolys, vvector<CMiniNode*>& allNodes, vvector<CLeaf*>& allLeafs);
    u_bool32    SegmentIntersect(v3d_t& a, v3d_t& b, u_bool32 bSubModels, CollPack& coll);
    void    LinkItem(SceItem* pItem, u_bool32 atLeaf);
    void	ConcaveConvex();
    virtual void    Finalize();
protected:
    // drawing
    void    Draw_Graph(View3D* pV);
    void    DrawPolys(Poly* pLeaf,int32_t ncount, u_int32_t mode,  u_rgba_t* lfc=0,
                                                               vvector<Poly*>* transPolys=0,
                                                               vvector <Poly*>* backDraw=0,
                                                               vvector <Poly*>* mirrors=0,
                                                               vvector <Poly*>* masked=0,
                                                               vvector <Poly*>* guround=0);
    void    Draw_BSP_WithLmapsAndTextures(View3D* pV, int32_t& leaf, int32_t curLeaf, u_int32_t mode);
    void    Draw_SelectedLeafByLeaf(View3D* pV, int32_t& leaf, int32_t curLeaf,u_int32_t mode);
    void    DoVisByLeafs();
    void    PatchPolys(vvector<Poly>&, vvector<Poly>&);
    u_bool32    SegmentIntersectPolygons(Poly* pStart, int32_t nCount, v3d_t& a, v3d_t& b, CollPack& col,u_bool32 faces=1);
    u_bool32    SegmentIntersectConvexLeaf(CMiniNode* pNode, v3d_t& a, v3d_t& b, CollPack& col);
    u_bool32    R_SegmentIntersect(v3d_t& a, v3d_t& b, int32_t node, CollPack& coll);

    virtual void UpdateNodeBB(CMiniNode* pNode, list<Poly>& polys);
    virtual void UpdateNodeBB(CMiniNode* pNode, vvector<Poly>& polys);
    void    R_LinkDetailModel(int32_t node, CBspTree* pModelTree);
    void    R_LinkItem(int32_t node, SceItem* pItem);
    void    R_DrawGraph(int32_t node, v3d_t pos, int32_t prevDir, View3D* pV);
    CMiniNode*  CreateNode(u_int32_t dw){
        if(dw & NODE_SOLID)
            return MiniBsp::CreateNode(dw);
        return new CLeaf(this, dw);
    }
    void   AddNode(CMiniNode* pNn){
        MiniBsp::AddNode(pNn);
        if(pNn->IsEmptyLeaf())
        {
            pNn->_leafIdx = _leafs.size();
            _leafs << (CLeaf*)pNn;
        }
    }
    void    R_CreateLeafClusters(int32_t nNode);
public:
    int32_t                 _thisIdx;
    PortalPRC*		    _prtProc;
    vvector<CLeaf*>     _leafs;
    vvector<CBspTree*>  _pSubModels;
    vvector<int32_t>        _sounds;
    v3d_t                  _rotSteps;
    int32_t				    _itemStartIdx;
    int32_t                 _motionsIdx;
    u_bool32                _patchPolys;
    u_int32_t               _zoneareea;
private:
    u_bool32                _userFlag;
    int32_t                 _clusterCnt;
};




#endif // !defined(AFX_BSPTREE_H__C358667F_7226_4799_BB82_E27CC4CA51B3__INCLUDED_)
