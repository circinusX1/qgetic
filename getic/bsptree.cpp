//----------------------------------------------------------------------------------------
// Unauthorized use or duplication is strictly prohibited.
// Copyright Zalsoft Inc 1999-2004.
//// Author: Marius C of Zalsoft Inc 2000-2004
//----------------------------------------------------------------------------------------

#include "stdafx.h"
#include "scene.h"
#include "bsptree.h"
#include <assert.h>
#include "view3d.h"
#include "portalprc.h"
#include "document.h"
#include "texadapter.h"


#define     BAD_GEOMERTY    6
#define     BOGUS_VALUE     524288
static int32_t Sign = 0;
//--| Box GetVxesBbox|--------------------------------------------------------------------
static Box GetVxesBbox(vvector<Vtx2>& pcts)
{
    Box b;

    FOREACH(vvector<Vtx2>, pcts, pp)
    {
        b.AddPoint(pp->_xyz);
    }
    return b;
}


//--| void SetPvsBit |--------------------------------------------------------------------
static void SetPvsBit (u_int8_t *pVisArray, int32_t nDestLeaf)
{
    pVisArray[nDestLeaf>>3] |= (1<<(nDestLeaf&7));
}

//--| u_bool32 GetPvsBit |--------------------------------------------------------------------
static u_bool32 GetPvsBit (u_int8_t *pVisArray, int32_t pDestLeaf)
{
    return pVisArray[pDestLeaf>>3] & (1<<(pDestLeaf&7));
    //[B>>3]&1<<[B&7]
}


#define PON_FRONT    1
#define PON_BACK  2
#define PON_BOTH  (PON_FRONT|PON_BACK)
#define PON_FACE  4

//--|   u_int32_t ClassifyBox|----------------------------------------------------------------
static   u_int32_t ClassifyBox(Plane& plan, Box& box)
{
    v3d_t   corners[2];
    GetCornersOfPlane(box._min, box._max, plan._n, corners);
    f32_t d1 = plan.DistTo(corners[0]);
    f32_t d2 = plan.DistTo(corners[1]);
    u_int32_t side = 0;
    if (d1 >= 0)
       side |= PON_FRONT;
    if (d2 < 0)
       side |= PON_BACK;
    return side;
}

CBspTree::CBspTree():_prtProc(0),_motionsIdx(-1),_patchPolys(false),_zoneareea(-1)
{
}

//--|    CBspTree::Clear|-----------------------------------------------------------------
void    CBspTree::Clear()
{
    _sounds.clear();
    _pSubModels.clear();
    _leafs.clear();
    MiniBsp::Clear();
}

//--|CBspTree::~CBspTree|-----------------------------------------------------------------
CBspTree::~CBspTree()
{
    Clear();
}

/**
Compile the bsp
*/
//--| CBspTree::Compile|------------------------------------------------------------------
u_bool32 CBspTree::Compile(vvector<Poly>& polys, u_int32_t brFlags, u_bool32 bLog, u_bool32 btextConvex) // virtual from base
{

    u_bool32 b = false;
    if(polys.size()==0)
        return true;
    _TRY{

        if(IsMainModel() && 1)
        {
            vvector<Poly>   patchedPolys;
            PatchPolys(polys, patchedPolys);//_bSnapMainBsp
            b =  MiniBsp::Compile(patchedPolys, brFlags, bLog, btextConvex);
        }
        else
        {
            b =  MiniBsp::Compile(polys, brFlags, bLog, btextConvex);
        }
    }
    _CATCHX()
    {
        TAfxMessageBox("Patch Exception. Please Disable Polygon Patching For The Time Being",MB_OK|MB_ICONHAND);
        return false;
    }

    if(b == false)
    {
        LOG(E_BSP,"Error BSP1023: Bad geometry detected. Please review the geometry");
        return false;
    }

    if(bLog)
    {
        LOG(E_BSP,"BSP: Ouput summary results for BSP %s: ",_name);
        LOG(E_BSP, "BSP: Polygons: %d, Planes: %d, Nodes: %d, Leafs: %d", _polys.size(),
           _planes.size(),
           _nodesPtr.size(),
           _leafs.size());
        /*
        FOREACH(vvector<CLeaf*>, _leafs, pp)
        {
            LOG(E_BSP, "-Leaf: %03d has %03d polyons\r\n",(*pp)->_leafIdx,(*pp)->_nPolys);
        }
        */

    }
    Finalize();
    return b;
}

//--| CBspTree::UpdateNodeBB|-------------------------------------------------------------
void CBspTree::UpdateNodeBB(CMiniNode* pNode, vvector<Poly>& polys)
{
    MiniBsp::UpdateNodeBB(pNode, polys);
}

//--| CBspTree::UpdateNodeBB|-------------------------------------------------------------
void CBspTree::UpdateNodeBB(CMiniNode* pNode, list<Poly>& polys)
{
    MiniBsp::UpdateNodeBB(pNode, polys);
}

//--|    CBspTree::LinkItem|--------------------------------------------------------------
void    CBspTree::LinkItem(SceItem* pItem, u_bool32 atLeaf)
{
    R_LinkItem(0, pItem);
}

//--|    CBspTree::R_LinkItem|------------------------------------------------------------
void    CBspTree::R_LinkItem(int32_t node, SceItem* pItem)
{
    CMiniNode* pNode = _nodesPtr[node];

    if(pNode->IsLeaf()) // leaf
    {
        if(!pNode->IsSolid())
        {
            pItem->_leafIdx = pNode->_leafIdx;
        }
        return;
    }

    int32_t side = pNode->GetPlane().GetSide(pItem->_t);

    if(side > 0 && pNode->_nodeidx[N_FRONT]>=0) // && item out of BSP
        R_LinkItem(pNode->_nodeidx[N_FRONT], pItem);
    else if( pNode->_nodeidx[N_FRONT]>=0)       // && item out of BSP
        R_LinkItem(pNode->_nodeidx[N_BACK], pItem);
}


//--|    CBspTree::LinkDetailModel|-------------------------------------------------------
void    CBspTree::LinkDetailModel(CBspTree* pModelTree, u_bool32 atLeaf)
{
    _pSubModels << pModelTree;
    if(atLeaf)
        R_LinkDetailModel(0, pModelTree);
}

//--|    CBspTree::R_LinkDetailModel|-----------------------------------------------------
void    CBspTree::R_LinkDetailModel(int32_t node, CBspTree* pModelTree)
{
    CMiniNode* pNode = _nodesPtr[node];

    if(pNode->IsLeaf()) // leaf
    {
        if(!pNode->IsSolid())
        {
            ((CLeaf*)pNode)->_iModIndex.push_back(pModelTree->_thisIdx);
        }
        return;
    }

    Box& box = pModelTree->Root()->_bbox;

    u_int32_t rp = ClassifyBox(pNode->GetPlane(), box);

    if(rp & PON_FRONT && pNode->_nodeidx[N_FRONT]!=-1) // if model get's out
      R_LinkDetailModel(pNode->_nodeidx[N_FRONT], pModelTree);

    if(rp & PON_BACK && pNode->_nodeidx[N_BACK]!=-1)
      R_LinkDetailModel(pNode->_nodeidx[N_BACK], pModelTree);

}


//--|  CBspTree::R_FindLeaf|--------------------------------------------------------------
int32_t  CBspTree::R_FindLeaf(int32_t nodeidx, v3d_t& pos)
{
    if(nodeidx == -1)
        return -1;
    CMiniNode* pNode = _nodesPtr[nodeidx];

    if(pNode->IsLeaf()) // leaf
    {
        if(!pNode->IsSolid())
        {
            return ((CLeaf*)pNode)->_leafIdx;
        }
        return -1;
    }

    int32_t side = pNode->GetPlane().GetSide(pos);

    if(side > 0)
        return R_FindLeaf(pNode->_nodeidx[N_FRONT], pos);
    else
        return R_FindLeaf(pNode->_nodeidx[N_BACK], pos);
}


//--|   CBspTree::AddUniquePlane|---------------------------------------------------------
int32_t   CBspTree::AddUniquePlane(vvector<Plane>&  allPlanes, map<int32_t,int32_t>& reindexMap)
{
    int32_t nIndex = 0;
    FOREACH(vvector<Plane>, _planes, pPlane)
    {
        Plane& rPlane = *pPlane;

        vvector<Plane>::iterator fp = allPlanes.findelement(rPlane);

        if(fp == allPlanes.end())
        {
            reindexMap[nIndex] = allPlanes.size();
            allPlanes << rPlane;
        }else
        {
            reindexMap[nIndex] = fp - allPlanes.begin();
        }
        nIndex++;
    }
    return 0;
}

//--|    CBspTree::ReindexPlanes|---------------------------------------------------------
void    CBspTree::ReindexPlanes(map<int32_t,int32_t>& pidxes)
{
    FOREACH(vvector<CMiniNode*>, _nodesPtr,ppNode)
    {
        CMiniNode* pNode = *ppNode;
        if(pNode->_planeIdx>=0)
          pNode->_planeIdxFinal = pidxes[pNode->_planeIdx];
        else
            pNode->_planeIdxFinal = -1; // DUMMY-TREE
    }

    FOREACH(vvector<Poly>,  _polys, pPoly)
    {
        Poly& rP = *pPoly;
        if(rP._planeIdx>=0)
          rP._planeIdxFinal = pidxes[rP._planeIdx];
        else
            rP._planeIdxFinal = -1;// DUMMY-TREE
    }

    FOREACH(vvector<CLeaf*>, _leafs, ppLeaf)
    {
        CLeaf* pLeaf = *ppLeaf;
        if(pLeaf->_planeIdx>=0)
          pLeaf->_planeIdxFinal = pidxes[pLeaf->_planeIdx];
        else
            pLeaf->_planeIdxFinal = -1;// DUMMY-TREE
    }

    if(_prtProc)
    {
        FOREACH(vvector<Portal>, _prtProc->_portals, pPort)
        {
            Portal& rP = *pPort;
            if(rP._planeIdx>=0)
              rP._planeIdxFinal = pidxes[rP._planeIdx];
            else
                rP._planeIdxFinal = -1;// DUMMY-TREE
        }
    }

}

//--|  CBspTree::CollectPNL|--------------------------------------------------------------

struct S_SortByTexture
{
    /*
    u_bool32 operator()(Poly* p1, Poly* p2)
    {
        LONG texA[4]    = { p1->_textures[0] > 0 ? p1->_textures[0] * (1000000000000) : 0,
                            p1->_textures[1] > 0 ? p1->_textures[0] * (100000000) : 0,
                            p1->_textures[2] > 0 ? p1->_textures[0] * (10000) : 0,
                            p1->_textures[3] > 0 ? p1->_textures[0] * (1) : 0};
        LONG   texB[4]  = { p2->_textures[0] > 0 ? p2->_textures[0] * (1000000000000) : 0,
                            p2->_textures[1] > 0 ? p2->_textures[0] * (100000000) : 0,
                            p2->_textures[2] > 0 ? p2->_textures[0] * (10000) : 0,
                            p2->_textures[3] > 0 ? p2->_textures[0] * (1) : 0};
        return (texA < texB);
    }
    */

};

//--|  CBspTree::CollectPNL|--------------------------------------------------------------
int32_t  CBspTree::CollectPNL(vvector<Poly*>&        allPolys,
                          vvector<CMiniNode*>&   allNodes,
                          vvector<CLeaf*>&       allLeafs)
{
    _polyStartIdx = allPolys.size();
    _nodeStartIdx = allNodes.size();
    _leafStartIdx = allLeafs.size();

    // collect polys, nodes and leafs. when save add the index with the start indexa as int32_t
    // they are ofset in th allXXx collection
    FOREACH(vvector<Poly>,  _polys, pPoly)
    {
        Poly& rP = *pPoly;
        allPolys << &rP;
    }

    FOREACH(vvector<CMiniNode*>, _nodesPtr,ppNode)
    {
        CMiniNode* pNode = *ppNode;
        allNodes << pNode;
    }

    FOREACH(vvector<CLeaf*>, _leafs, ppLeaf)
    {
        CLeaf* pLeaf = *ppLeaf;
        allLeafs << pLeaf;

        // sort polys by texture in each leaf
        vvector<Poly*>::iterator t0S = allPolys.begin() + pLeaf->_polyIdx + _polyStartIdx;
        vvector<Poly*>::iterator t0E = allPolys.begin() + pLeaf->_polyIdx + _polyStartIdx + pLeaf->_nPolys;

        //std::sort(t0S, t0E, S_SortByTexture);
    }
    return 0;
}

//--|    CBspTree::SegmentIntersect|------------------------------------------------------
u_bool32    CBspTree::SegmentIntersect(v3d_t& a, v3d_t& b, u_bool32 bSubModels, CollPack& coll)
{
    Box       segBox;
    f32_t       dTmp;

    if(_treeprops & MODEL_NOCOLIDE)
    {
        return false;
    }


    _userFlag  = false; // collpack set
    v3d_t   iP    = b;  // keep this for submitting it to models
    coll._ip   = b;  // last ip is the end. keep this
    coll._dist = 999999.99;
    coll._pTree=0;
    u_bool32 bRet = R_SegmentIntersect(a, b, 0, coll);
    //
    // we always go forward. This Segitersect is not only for light
    //
    if(/*!bRet &&  */bSubModels && _pSubModels.size())
    {
        CollPack   best;

        FOREACH(vvector<CBspTree*>, _pSubModels, ppSubBsp)
        {
            CBspTree* pBsp = (*ppSubBsp);

            //
            // ignore dynamic models for now
            //
            if(pBsp->_treeprops & MODEL_DYNAMIC)
                continue;

            if(pBsp->_treeprops & MODEL_NOCOLIDE)
                continue;

            if(pBsp->_treeprops & MODEL_HIDDEN)
                continue;

            if(pBsp->_treeprops & MODEL_IS_SKYDOM)
                continue;

            if(GET_CONTENT(pBsp->_treeprops) == CONT_PLASMA ||
               GET_CONTENT(pBsp->_treeprops) == CONT_GAS)
                continue;



            segBox.FromSegment(a,iP);                          // do a fast seg box intersection
            if(segBox.IsTouchesBox(pBsp->GetNode(0)->_bbox))
            {
                if(!pBsp->SegmentIntersect(a, iP, 0/*no submodels*/, best))
                {
                    continue;
                }
                //
                // keep the best ip
                //
                bRet = true;
                dTmp = vdist(best._ip, a);
                if(dTmp < coll._dist)
                {
                    coll       = best;
                    coll._dist = dTmp;
                    coll._pTree = pBsp;
                }
            }
        }
    }
    return bRet ;
}

//--|    CBspTree::R_SegmentIntersect|----------------------------------------------------
u_bool32    CBspTree::R_SegmentIntersect(v3d_t& a, v3d_t& b, int32_t nodeIdx, CollPack& col)
{
    #pragma message("happend with lmap  128x128 and lumelsz = 8")
    if(nodeIdx < 0 || nodeIdx > BOGUS_VALUE)
    {
        return 1;
    }
    u_bool32        rVal = 0;
    CMiniNode* pNode  = _nodesPtr[nodeIdx];
    if(pNode->IsLeaf())
    {
        u_bool32 isSolid = pNode->IsSolid();
        if(_treeprops  & MODEL_IS_TERRAIN && Compiler::PCompiler->_bAABBTreeCsgTer)
        {
            assert(pNode->_polyIdx>=0);
            if(pNode->_polyIdx<0)
            {
                return 1;
            }
            return SegmentIntersectPolygons(&_polys[pNode->_polyIdx], pNode->_nPolys, a, b, col);
        }

        if(_treeflags & BRSH_DONTBSP)
        {
            assert(pNode->_polyIdx>=0);
            assert(pNode->_nodeflags  &  NODE_LEAF);
            if(_polys.size())
                return SegmentIntersectPolygons(&_polys[0], pNode->_nPolys, a, b, col);
        }
        if(_btextconvex && !isSolid)
        {
            //return SegmentIntersectPolygons(&_polys[0], pNode->_nPolys, a, b, col);
            return SegmentIntersectConvexLeaf(pNode, a, b, col);
        }
        col._nLeaf = pNode->_leafIdx;
        col._pTree = this;
        col._ip    = a;
        return isSolid;
    }

    Plane& plane = _planes[pNode->_planeIdx];
    f32_t   da    = plane.DistTo(a);
    f32_t   db    = plane.DistTo(b);

    if(da > -EPSILON  && db > -EPSILON )
        return R_SegmentIntersect(a, b, pNode->_nodeidx[N_FRONT],col);
    else if(da < EPSILON  && db < EPSILON )
        return R_SegmentIntersect(a, b, pNode->_nodeidx[N_BACK],col);

    int32_t  side   = da >= 0;
    f32_t dist   = da / (da - db);
    v3d_t  midPct  = a+(b-a) * dist;

    if(R_SegmentIntersect(a, midPct, pNode->_nodeidx[side],col))
         return true;
    else if (R_SegmentIntersect(midPct, b, pNode->_nodeidx[!side],col))
    {
        if(!_userFlag )// collpack set
        {
            _userFlag   = true;
            col._ip     = midPct;
            col._pPlane = &plane;
            col._pTree  = this;
        }
        return true;
    }
    return false;
}

u_bool32    CBspTree::SegmentIntersectConvexLeaf(CMiniNode* pNode, v3d_t& a, v3d_t& b, CollPack& col)
{
    u_bool32        hit = 0;
    set<int32_t>    indexes;
    f32_t        d2p = INFINIT;
    Poly*       pStart = &_polys[pNode->_polyIdx];// pNode->_nPolys
    int32_t         nCount = pNode->_nPolys;
    for(int32_t i=0;i<nCount;i++,pStart++)
    {
        Plane& plane = _planes[pStart->_planeIdx];
        if(indexes.find(pStart->_planeIdx) == indexes.end())
        {
            f32_t da = plane.DistTo(a);
            f32_t db = plane.DistTo(b);
            if(db < -1.0)
            {
                f32_t ratio   = da / (da - db);
                v3d_t   midPct  = a+(b-a) * ratio;
                f32_t dist    = vdist(a, midPct);
                if(dist < d2p)
                {
                    //_userFlag   = true; // collfound
                    col._pPlane = &plane;
                    col._pTree  = this;
                    col._ip     = midPct;
                    col._dist   = dist;
                    d2p         = dist;
                    hit         = 1;
                }
            }
            indexes.insert(pStart->_planeIdx);
        }
    }
    return hit;
}

//--| CBspTree::SegmentIntersectPolygons|-------------------------------------------------
u_bool32 CBspTree::SegmentIntersectPolygons(Poly* pStart,
                                       int32_t nCount,
                                       v3d_t& a,
                                       v3d_t& b,
                                       CollPack& col, u_bool32 faces)
{
    u_bool32            bHit = 0;
    v3d_t vdir = a;    vdir-=b; vdir.norm();
    Plane           plane;
    f32_t            minDist = INFINIT;

    for(int32_t i=0;i<nCount;i++,pStart++)
    {
        if(pStart->_planeIdx>=0)
        {
            plane = _planes[pStart->_planeIdx];
        }
        else
        {
            assert(0);
            plane._n = pStart->_n;
            plane._c = pStart->_c;
        }

        f32_t da = plane.DistTo(a);
        f32_t db = plane.DistTo(b);

        if(da >= 0 &&  db < -2.0)
        {
            f32_t ratio   = da / (da - db);
            v3d_t   midPct  = a+(b-a) * ratio;
            f32_t dist    = vdist(a, midPct);

            if(faces)
            {
                if(pStart->IsInnerPoint(midPct))
                {
                    if(dist < minDist)
                    {
                        minDist = dist;

                        if(pStart->_planeIdx>=0)
                            col._pPlane = &_planes[pStart->_planeIdx];
                        else
                            col._pPlane = 0;   // DUMMY TREE
                        col._pPoly  = pStart;
                        col._pTree  = this;
                        col._ip     = midPct;
                        bHit        = 1;
                        _userFlag   = 1;
                    }
                }
            }
            else
            {
                if(dist < minDist)
                {
                    minDist     = dist;
                    col._pPlane = &_planes[pStart->_planeIdx];
                    col._pTree  = this;
                    col._ip     = midPct;
                    bHit        = 1;
                    _userFlag   = 1;
                }
            }
        }
    }
    return bHit;
}

//--| CBspTree::ConcaveConvex|------------------------------------------------------------
void CBspTree::ConcaveConvex()
{
    return;//
    /**
    // empty convex (remove all nodes)
    if(_leafs.size()==1)
    {
        _props |= MODEL_CONVEX_EMPTY;
    }
    if(_leafs.size() == this->_polys.size())
    {
        _props |= MODEL_CONVEX_SOLID;
    }

    if(_props & MODEL_CONVEX_SOLID ||
      _props & MODEL_CONVEX_EMPTY)
    {
        //delete nodes
        _nodesPtr.deleteelements();
        _leafs.clear();

        //add new root
        AddNode(new CMiniNode(this));
        UpdateNodeBB(Root(), _polys);

        //
        // create front node. place all trees plygons here
        // there is no need to walk to bottom as lkong
        // all are facing in
        //
        CMiniNode* fn = CreateNode(NODE_LEAF);
        Root()->_nodeidx[N_FRONT] = _nodesPtr.size();
        fn->_polyIdx    =  0;
        fn->_nPolys     =  _polys.size();
        fn->_idxParent           =  0;
        UpdateNodeBB(fn, _polys);
        AddNode(fn);

        //
        // create back node witch is solid
        //
        CMiniNode* bn = CreateNode(NODE_LEAF|NODE_SOLID);
        Root()->_nodeidx[N_BACK]=_nodesPtr.size();
        bn->_idxParent = 0;
        AddNode(bn);
    }
    */


}
//PROP_PATCHES
//--| void CutByWalkingPlane|-------------------------------------------------------------
static void CutByWalkingPlane(Poly& rp, f32_t treshVal, vvector<Poly>& outPolys)
{
    v3d_t      majorAx      = GetMaxAx(rp._n);
    Box     box          = rp._box;
    f32_t    coverExtends = box.GetMaxExtend() * 2.200f;

    v3d_t      vStart  =   rp._vtci[0]._xyz;
    v3d_t      d1      =   (rp._vtci[1]._xyz - rp._vtci[0]._xyz).norm();
    v3d_t      d2      =   Vcp(d1, rp._n) ;

    /** Debug points for patching the polygons
        for(f32_t x = 0 ; x < coverExtends; x+=treshVal)
    {
        for(f32_t y = 0 ; y < coverExtends; y+=treshVal)
        {

            v3d_t dp = vStart + (d1*x) + (d2*y);

            Pd->_testPoints << dp;

        }
    }

    */

    Plane           divPlane;
    vvector<Poly>   frags;
    Poly            toCut = rp;

    for(f32_t x = 0 ; x < coverExtends; x+=treshVal)
    {
        v3d_t      dp = vStart + (d1*x);
        Poly    pf;
        Poly    pb;

        //Pd->_testPoints << dp;

        divPlane.Set(d1, dp);

        toCut.Split(divPlane, pf, pb, true);

        if(pb._vtci.size()>=3)
        {
            pb._polyflags |= POLY_SEP_LMAP;
            frags << pb;
        }

        if(pf._vtci.size()>=3)
        {
            toCut.Clear();
            toCut = pf;
        }
        else
        {
            break;
        }
    }

    // now cut all frags
    FOREACH(vvector<Poly>, frags, p2c)
    {
        toCut = *p2c;

        for(f32_t x = 0 ; x < coverExtends; x+=treshVal)
        {
            v3d_t      dp = vStart + (d2*x);
            Poly    pf;
            Poly    pb;

            //Pd->_testPoints << dp;

            divPlane.Set(d2, dp);

            toCut.Split(divPlane, pf, pb, true);
            if(pb._vtci.size()>=3)
            {
                pb._polyflags |= POLY_SEP_LMAP;
                outPolys << pb;
            }
            if(pf._vtci.size()>=3)
            {
                toCut.Clear();
                toCut = pf;
            }
            else
            {
                break;
            }
        }
    }
}

//--|    CBspTree::PatchPolys|------------------------------------------------------------
void    CBspTree::PatchPolys(vvector<Poly>& inPolys, vvector<Poly>& outPolys)
{
    #ifdef _DEBUG
        Pd->_testPoints.clear();
    #endif //

    // tmax xy dimension unpached -is tmax lmap size on that dim x  _lMapLumelSz
    f32_t    maxDimPoly = Compiler::PCompiler->_lmapMaxSize * Compiler::PCompiler->_lMapLumelSz;

    FOREACH(vvector<Poly>, inPolys, pp)
    {
        if(Compiler::PCompiler->_bSnapMainBsp)
        {
            pp->SnaptoPoint(SnapToXY);
        }

        if(pp->_box.GetMaxExtend() > maxDimPoly && pp->_polyprops & FACE_LM_PATCH)
        {
            CutByWalkingPlane(*pp, maxDimPoly, outPolys);
            continue;
        }
        outPolys<<*pp;continue;
    }
}

static GLUquadricObj* go;

//--|    CBspTree::Draw2GlTree|------------------------------------------------------------
void    CBspTree::Draw2GlTree(View2D* pV, u_int32_t mode, int32_t leafNo, u_bool32 bvisible)
{
    Ta.Disable();
    int32_t nCount = 0;
    int32_t nLeaf = 0;
    u_rgba_t leafcolor = ZRED;
    FOREACH(vvector<CLeaf*>, _leafs, pNode)
    {
        Poly*   pPoly = (*pNode)->GetPolys(nCount);

        if(mode & (C_FULL|C_LMAP|C_GRAPH|C_ZONES))
        {
            if(pPoly->_polyflags & POLY_HIDDEN || !bvisible)
            {
                glColor4ubv(CLR_DARK);
            }
            else
            {
                glColor4ubv(CLR_WHITE);
            }
        }
        else if(IsMainModel())
        {
            if(mode & C_LBYL)
            {
                if(nLeaf == leafNo)
                    glColor4ubv(CLR_WHITE);
                else
                    glColor4ubv(CLR_DARK);
            }
            else if(mode & C_LEAFS)
            {
                int32_t treebits = nLeaf & 0x1F;

                if(treebits & 0x10)
                {
                    leafcolor.r = !!(treebits&1)*80;
                    leafcolor.g = !!(treebits&2)*80;
                    leafcolor.b = !!(treebits&4)*80;
                }
                else if(treebits & 0x8)
                {
                    leafcolor.r = !!(treebits&1)*162;
                    leafcolor.g = !!(treebits&2)*162;
                    leafcolor.b = !!(treebits&4)*162;
                }
                else
                {
                    leafcolor.r = !!(treebits&1)*250;
                    leafcolor.g = !!(treebits&2)*250;
                    leafcolor.b = !!(treebits&4)*255;
                }
                glColor4ubv(leafcolor);
            }
        }
        else
        {
            glColor4ubv(CLR_NORMAL);
        }

        for(int32_t i=0;i<nCount;i++, pPoly++)
        {
            glBegin(GL_LINE_LOOP);
            PVertex vb = pPoly->_vtci.begin();
            PVertex ve = pPoly->_vtci.end();
            for(;vb!=ve;vb++)
            {
                Vtx2& vtci = *vb;
                glVertex3fv((f32_t*)vtci._xyz);
            }
            glEnd();
        }
        ++nLeaf;
    }
    Ta.Restore();
}

//--|    CBspTree::Draw3DBSPTree|------------------------------------------------------------
static u_bool32	Local_IsLeafVisFromLeaf(vvector<CLeaf*>& leafs, int32_t l1, int32_t l2, u_int8_t* ppvs)
{
    if(l1<0||l2<0)return 1;
    if(ppvs)
    {
        u_int8_t* pPvs1 = &ppvs[leafs[l1]->_pvsIdx];
        return (pPvs1[l2 >> 3] & (1<<(l2 & 7)) )!=0;
    }
    return 1;
}

//--|    CBspTree::Draw_Graph|------------------------------------------------------------
static int32_t NDepth;
void    CBspTree::Draw_Graph(View3D* pV)
{
    int32_t node = 0 ;
    v3d_t  pos  = v3d_t(0.,400.,0.);
    int32_t dir = 0;
    NDepth = 0;

    Ta.Disable();

    go = gluNewQuadric();
    Sign=0;
    R_DrawGraph(0, pos, dir, pV);
    if(IsMainModel())
    {
        pV->TextOut(pos, MKSTR("BSP: %d",_thisIdx) ,ZYELLOW);
    }
    else
    {
        pV->TextOut(pos, MKSTR("Model: %d",_thisIdx),ZYELLOW);
    }
    gluDeleteQuadric(go);
    Ta.Restore();
}
//--|    CBspTree::|------------------------------------------------------------
//--|    CBspTree::|------------------------------------------------------------
void    CBspTree::DrawPolys(Poly* pPoly,int32_t ncount, u_int32_t mode,    u_rgba_t* leafcolor,
                                                                       vvector<Poly*>* transPolys,
                                                                       vvector <Poly*>* backDraw,
                                                                       vvector <Poly*>* mirrors,
                                                                       vvector <Poly*>* masked,
                                                                       vvector <Poly*>* guround)

{
    u_bool32 bind;

    for(int32_t i=0;i<ncount;i++, pPoly++)
    {
        bind = 0;
        pPoly->_polyflags &= ~POLY_HIDDEN;

        if(pPoly->_polyprops & FACE_MIRROR)
        {
            if(mirrors) mirrors->push_back((Poly*)pPoly);
            continue;
        }
        if(pPoly->_polyprops & FACE_SHOWBACK)
        {
            if(backDraw) backDraw->push_back((Poly*)pPoly);
            continue;
        }
        if(pPoly->_polyprops & FACE_TRANSPARENT)
        {
            if(transPolys) transPolys->push_back((Poly*)pPoly);
            continue;
        }

        if(pPoly->_polyprops & FACE_BLACKMASK)
        {
            if(masked) masked->push_back((Poly*)pPoly);
        }

        if(pPoly->_polyprops & FACE_GSHADE)
        {
            if(guround) guround->push_back((Poly*)pPoly);
            continue;
        }


        if(mode & C_WHITE || mode & C_BYFAR)
        {
            Pd->DrawPolyVertexes(GL_POLYGON, *pPoly, 0);
        }
        else
        {

            if((mode & C_LEAFS) && leafcolor)
            {
                leafcolor->r-=5;
                leafcolor->b-=5;
                glColor4ubv((u_int8_t*)*leafcolor);
                Pd->DrawPolyVertexes(GL_POLYGON, *pPoly, 1);
            }
            else
            {
                glColor4ubv((u_int8_t*)pPoly->_colorD);
                Texs& tex = pPoly->GetTexs();
                if(mode & C_LMAP)
                {
                    if(pPoly->_polyprops & FACE_FULLBRIGHT)
                        bind = Ta.Bind(_wLmap);
                    else
                    {
                        if(pPoly->_polyprops & FACE_NOLIGTMAP)
                        {
                            __noop;
                        }
                        else
                        {
                            if(pPoly->_polyprops & FACE_CUSTLM)
                                bind=Ta.Bind(tex._texts[1], tex._texApply[1]);
                            else
                                bind=Ta.Bind(pPoly->_ilmTex, 0);
                        }
                    }
                    if(bind)
                    {
                        Pd->DrawPolyVertexes(GL_POLYGON, *pPoly, 1);
                    }
                }
                else
                {
                    u_bool32 binded;

                    for(int32_t ts=0;ts<4;++ts)
                    {
                        binded=0;

                        switch(ts)
                        {
                            case 0:
                                Ta.Bind(tex._texts[0], tex._texApply[0]);
                                Pd->DrawPolyVertexes(GL_POLYGON, *pPoly, 0);
                                break;
                            case 1:
                                if(pPoly->_polyprops & FACE_FULLBRIGHT)
                                {
                                    Ta.Bind(_wLmap, 0);
                                    binded=1;
                                }
                                else
                                {
                                    if(pPoly->_polyprops & FACE_NOLIGTMAP)
                                    {
                                        __noop;
                                    }
                                    else
                                    {
                                        if(pPoly->_polyprops & FACE_CUSTLM && pPoly->GetHtex(1))
                                        {
                                            Ta.Bind(tex,1);
                                            binded=1;
                                        }
                                        else if(pPoly->_ilmTex)
                                        {
                                            Ta.Bind(pPoly->_ilmTex, 0);
                                            binded=1;
                                        }
                                    }
                                }
                                if(binded)
                                {
                                    TexRef::GlLmapMode(1);
                                    Pd->DrawPolyVertexes(GL_POLYGON, *pPoly, 1);
                                    TexRef::GlLmapMode(0);
                                }
                                break;
                            case 2:
                            case 3:
                                if(Ta.Bind(tex,ts))
                                {
                                    TexRef::GlDetailMode(1);
                                    Pd->DrawPolyVertexes(GL_POLYGON, *pPoly, ts);
                                    TexRef::GlDetailMode(0);
                                }
                                break;
                        }
                    }
                }
            }
        }
    }

}

//--|    CBspTree::|------------------------------------------------------------
void    CBspTree::Draw_BSP_WithLmapsAndTextures(View3D* pV, int32_t& leaf, int32_t curLeaf, u_int32_t mode)
{
    u_bool32            frustCull = 0;
    int32_t             nCount;
    vvector <Poly*> transPolys;         // draw these last
    vvector <Poly*> backDraw;
    vvector <Poly*> mirrors;
    vvector <Poly*> masked;
    vvector <Poly*> guround;

    glDisable(GL_BLEND);

    u_rgba_t     leafcolor = ZRED;
    u_bool32    pvsHidden ;
    int32_t     lfCount = 1;


    FOREACH(vvector<CLeaf*>, _leafs, pNode)
    if((*pNode)->_nodeflags & LEAF_HAS_OCCLUDERS)
    {
        // add them to camera antifrustrum
        Poly*   pPoly = (*pNode)->GetPolys(nCount);
        for(int32_t i=0;i<nCount;i++, pPoly++)
        {
            if(pPoly->_polyprops & FACE_OCCLUDER)
            {
                Pd->_cam.AddOccluder(*pPoly);
            }
        }
    }

    // PVS the leafs
    FOREACH(vvector<CLeaf*>, _leafs, pNode)
    {
        ++lfCount;
        if(((lfCount & 0x8)==0) || ((lfCount & 0x10)==0) )++lfCount;
        if(mode & C_LEAFS && IsMainModel())
        {
            int32_t treebits = lfCount & 0x1F;

            if(treebits & 0x10)
            {
                leafcolor.r = !!(treebits&1)*80;
                leafcolor.g = !!(treebits&2)*80;
                leafcolor.b = !!(treebits&4)*80;
            }
            else if(treebits & 0x8)
            {
                leafcolor.r = !!(treebits&1)*162;
                leafcolor.g = !!(treebits&2)*162;
                leafcolor.b = !!(treebits&4)*162;
            }
            else
            {
                leafcolor.r = !!(treebits&1)*250;
                leafcolor.g = !!(treebits&2)*250;
                leafcolor.b = !!(treebits&4)*255;
            }
        }

        pvsHidden  = false;


        if(curLeaf >= 0 && (*pNode)->_pvsIdx >=0 && (*pNode)->_leafIdx != curLeaf)
        {
            u_int32_t zonevis = _leafs[curLeaf]->_zonearea;
            u_int32_t nodezone = (*pNode)->_zonearea;
            if(!(nodezone & zonevis))
            {
                pvsHidden = true;
            }
            else
            {
                if(!Local_IsLeafVisFromLeaf(_leafs, (*pNode)->_leafIdx, curLeaf,Compiler::PCompiler->_pvsPrc._pvs))
                {
                    pvsHidden =true;
                }
            }
            frustCull =  (GFrustCull &&   Pd->_cam.IsCulled(&(*pNode)->_bbox._min,&(*pNode)->_bbox._max));
            // frust cull leafs and nodes
            if(pvsHidden || frustCull)
            {
                Poly*   pPoly = (*pNode)->GetPolys(nCount);
                for(int32_t i=0;i<nCount;i++, pPoly++)
                {
                    pPoly->_polyflags |= POLY_HIDDEN;
                }
                continue;
            }

        }

        glColor4ubv(CLR_WHITE);
        Poly*   pPoly = (*pNode)->GetPolys(nCount);
        DrawPolys(pPoly, nCount, mode,  &leafcolor,
                                        &transPolys,         // draw these last
                                        &backDraw,
                                        &mirrors,
                                        &masked,
                                        &guround);
    }
    glColor4ubv(CLR_WHITE);
    Pd->DrawSpecialPolygons(pV,  1, backDraw, transPolys, mirrors, masked, guround);
}

//--|    CBspTree::Draw_SelectedLeafByLeaf|-----------------------------------------------
void    CBspTree::Draw_SelectedLeafByLeaf(View3D* pV, int32_t& leaf, int32_t curLeaf, u_int32_t mode)
{
    CLeaf* pSelLeaf = 0;
    int32_t    lfCount = 0;

    if(-1==curLeaf)
        curLeaf=0;


    glColor4ubv(CLR_WHITE);
    FOREACH(vvector<CLeaf*>, _leafs, pNode)
    {
        if(leaf == (*pNode)->_leafIdx)
        {
            pSelLeaf = (*pNode);
            continue;
        }
        else
        {
            glColor4ubv(CLR_DARK);
        }

        // draw selected leaf only
        int32_t     nCount;
        Poly*   pPoly = (*pNode)->GetPolys(nCount);
        DrawPolys(pPoly, nCount, mode);
    }

    if(pSelLeaf )
    {
        glColor4ubv(CLR_WHITE);

        int32_t     nCount;
        Poly*   pPoly = pSelLeaf->GetPolys(nCount);

        for(int32_t i=0;i<nCount;i++)
        {
            if(!Pd->_vmodefire)
                Ta.Bind((const TexCarrier&)*pPoly);
            Pd->DrawPolyVertexes(GL_POLYGON, *pPoly, GUtex);
            ++pPoly;
        }
        glColor4ubv(CLR_WHITE);
    }
    if(leaf == _leafs.size())
        leaf=0;
}

//--|    CBspTree::Draw3DBSPTree|------------------------------------------------------------
void    CBspTree::Draw3DBSPTree(View3D* pV, int32_t& leaf, int32_t curLeaf, u_int32_t mode)
{
    glColor4ubv(CLR_WHITE);
    Pd->SetFaceMode();

    if(mode == C_GRAPH)
    {
        Draw_Graph(pV);
        return;
    }
    if(mode & (C_LMAP|C_FULL|C_LEAFS)) // lmaps
    {
        u_rgba_t leafcolor = ZRED;
        if(_treeflags & BRSH_DONTBSP)
        {
            int32_t nCount;
            vvector <Poly*> transPolys;         // draw these last
            vvector <Poly*> backDraw;
            vvector <Poly*> mirrors;
            vvector <Poly*> masked;
            vvector <Poly*> guround;

            Poly*   pPoly = Root()->GetPolys(nCount);
            DrawPolys(pPoly, nCount, mode,  &leafcolor,
                                            &transPolys,         // draw these last
                                            &backDraw,
                                            &mirrors,
                                            &masked,
                                            &guround);
            glColor4ubv(CLR_WHITE);
            Pd->DrawSpecialPolygons(pV, 1,backDraw, transPolys, mirrors, masked, guround);
        }
        else
        {
            Draw_BSP_WithLmapsAndTextures(pV, leaf, curLeaf, mode);
        }
        return;
    }
    else  if(mode & C_LBYL)
    {
        Draw_SelectedLeafByLeaf(pV, leaf, curLeaf, mode);
    }
}

//----------------------------------------------------------------------------------------
#define DIST_SPHERE(rdt,dir) (((rdt-dir+1)*12))

//--|    CBspTree::R_DrawGraph|----------------------------------------------------------
void    CBspTree::R_DrawGraph(int32_t node, v3d_t pos, int32_t prevDir, View3D* pV)
{
    ++NDepth;
    Sign=!Sign;

    CMiniNode* pNode = _nodesPtr[node];
    if(pNode->IsLeaf())
    {
        glPushMatrix();
        {
            glTranslatef(pos.x, pos.y, pos.z);
            if(pNode->_selected==0)
                glColor4ub(96,96,96,255);
            else
                glColor4ub(192,192,192,255);
            if(pNode->IsSolid())
            {
                if(pNode->_selected==0)
                {
                    pV->TextOut(V0, MKSTR("  x%d",node ) , u_rgba_t(255,255,0));
                    glColor4ub(128,0,0,255);
                }
                else
                {
                    pV->TextOut(V0, MKSTR("  x%d",node) , u_rgba_t(192,0,192));
                    glColor4ub(255,0,0,255);
                }
            }
            else
            {
                if(pNode->_selected==0)
                {
                    pV->TextOut(V0, MKSTR("  %d/%d ",pNode->_leafIdx, pNode->_nPolys) , ZYELLOW);
                    glColor4ub(0,128,96,255);
                }
                else
                {
                    pV->TextOut(V0, MKSTR("  %d/%d ",pNode->_leafIdx, pNode->_nPolys) , u_rgba_t(192,0,192));
                    glColor4ub(0,255,255,255);
                }
            }
            gluSphere(go, 2.0,6,6);
        }
        glPopMatrix();
        --NDepth;
        return;
    }
    v3d_t lr;

    if(pNode->_nodeidx[0]>=0) // dir -1
    {
        lr = pos;
        int32_t rdw = _reachedDepth*2 - 12;
        int32_t sign = rdw%2?1:-1;
        int32_t depl = rdw - NDepth;

        lr.y -= (rdw - NDepth) * 32;
        lr.x -= (rdw - NDepth) * 16;
        lr.z += (Sign * depl) * 24;


        if(pNode->_selected==0)
            glColor4ub(128,0,0,254);
        else
            glColor4ub(255,255,0,254);


        glBegin(GL_LINES);
        glVertex3f(pos.x,pos.y,pos.z);
        glVertex3f(lr.x,lr.y,lr.z);
        glEnd();

        if(pNode->_selected==1)
            pV->TextOut(pos, MKSTR("%d",node) , ZYELLOW);
        else
            pV->TextOut(pos, MKSTR("%d",node) , u_rgba_t(192,0,192));

        R_DrawGraph(pNode->_nodeidx[0], lr, 32 , pV);
    }

    if(pNode->_nodeidx[1]>=0)// - > +1
    {
        lr = pos;

        int32_t rdw = _reachedDepth*2 - 9;

        int32_t depl = rdw - NDepth;
        lr.y -= (depl) * 32;
        lr.x += (depl) * 16;
        lr.z += (-Sign * depl) * 24;

        if(pNode->_selected==0)
            glColor4ub(0,128,0,254);
        else
            glColor4ub(0,255,0,254);

        glBegin(GL_LINES);
        glVertex3f(pos.x,pos.y,pos.z);
        glVertex3f(lr.x,lr.y,lr.z);
        glEnd();

        if(pNode->_selected==1)
            pV->TextOut(pos, MKSTR("%d",node) , ZGREEN);
        else
            pV->TextOut(pos, MKSTR("%d",node) , u_rgba_t(64,64,64));


        R_DrawGraph(pNode->_nodeidx[1], lr, 32 , pV);
    }
    --NDepth;
}

// cluster the nodes
void    CBspTree::Finalize()
{
    //_cluster
    _clusterCnt = 0;
    R_CreateLeafClusters(0);
}


void    CBspTree::R_CreateLeafClusters(int32_t nNode)
{
    if(nNode==-1)
        return;
    CMiniNode* pNode = _nodesPtr[nNode];

    if(pNode->_leafIdx==-1) //still node
    {
        R_CreateLeafClusters(pNode->_nodeidx[0]);
        R_CreateLeafClusters(pNode->_nodeidx[1]);
        return;
    }

    CLeaf* pLeaf = _leafs[pNode->_leafIdx];
    if(pLeaf->IsSolid())
        pNode->_cluster = -1;
    else
        pNode->_cluster = _clusterCnt;
    ++_clusterCnt;
}
