// PVSPrc.cpp: implementation of the CPVSPrc class.
//
//---------------------------------------------------------------------------------------

#include "stdafx.h"
#include <baseutils.h>
#include "pvsprc.h"
#include "bsptree.h"
#include "portalprc.h"
#include "compiler.h"

//---------------------------------------------------------------------------------------
static int32_t CompressLeafPVS(u_int8_t* pDest,u_int8_t* pLeafPvs, int32_t bytesPvs);

int32_t AllocBitsRoundByLong(int32_t nSetCount)
{
    int32_t rval =   (nSetCount + 7) / 8;    // bytes
    rval = (rval * 3 + 3) & 0xFFFFFFFC;  // round by mod4
    return rval;
}


//---------------------------------------------------------------------------------------
u_bool32    PVSPrc::Process(CBspTree* pT, PortalPRC* pP)
{
    u_bool32 bret=true;
    LOG(E_PVS,"PVS: Processing Potentially View Sets");
    // get temp pointers
    _pBspTree = pT;
    _pPortPrc = pP;

    Clear();
    _nleafs  = pT->GetLeafsCount();

    if(pP->HasPortals())
    {
        vvector<Portal>& rPortals = pP->GetPortals();
        if(bret = InitalPortalVis(rPortals))
        {
            GaterLeafsVis();
            CompressPVS();
        }

        //// DoVisByLeafs();
    }
    else
    {
        assert(1 == _nleafs);
        if(0==_nleafs)
            return false;
        if(_pvs) delete[] _pvs; _pvs = 0;
        _cPvss  = 4;
        _pvs    = new u_int8_t[_cPvss];
        memset(_pvs,255,_cPvss);
        FOREACH(vvector<CLeaf*>, _pBspTree->_leafs, ppLeaf)
        {
            (*ppLeaf)->_pvsIdx = 0;
        }
    }
    if(0 == bret)
        LOG(E_PVS,"Error: PVS1022. Internal Error");
    else
        LOG(E_PVS,"PVS: Potentially View Sets '%s' successful build for '%d' leafs.",pT->_name, _pBspTree->_leafs.size());
    return bret;
}

//---------------------------------------------------------------------------------------
u_bool32    CompletteOnBack(Plane& pA, Portal& rP)
{
    int32_t count = rP._vxes.size();
    FOREACH(vvector<v3d_t>, rP._vxes, pvx)
    {
        if(pA.GetSide(*pvx) == 1)
            break;
        --count;
    }
    return (0==count);
}

//---------------------------------------------------------------------------------------
u_bool32    CompletteOnFront(Plane& pA, Portal& rP)
{
    int32_t count = rP._vxes.size();
    FOREACH(vvector<v3d_t>, rP._vxes, pvx)
    {
        if(pA.GetSide(*pvx) == -1)
            break;
        --count;
    }
    return (0==count);
}


//---------------------------------------------------------------------------------------
u_bool32    PVSPrc::InitalPortalVis(vvector<Portal>& rPortals)
{
    u_bool32    bRetVal= true;
    u_int8_t*   pVis   = new u_int8_t[rPortals.size()];
    int32_t     nCount = 0;
    int32_t     nTotal = rPortals.size();
    int32_t     percent;

    assert(pVis);
    _TRY
    {

        FOREACH(vvector<Portal>, rPortals, portItA)
        {
            Portal&     prtA = *portItA;
            PortalData* pD  = new PortalData(_nleafs);

            _portVs << pD;                                    // this prtA pvs
            ::memset(pVis, 0, rPortals.size());               // reset the array for this prtA
            int32_t nCount2 = 0;
            FOREACH(vvector<Portal>, rPortals, portItB)
            {
                Portal& prtB = *portItB;
                if(portItB == portItA)
                    continue;

                if(CompletteOnBack(prtA, prtB))    // complette on back we dont see this
                    continue;                       // so leave his byte on 0

                if(CompletteOnFront(prtB, prtA))  // are we complette on front
                    continue;

                pVis[prtB._idxThis] = 1;                // portal vis flag <<<<<<<<<<<<

            }
            // as int32_t we roll and we have temp vis sets we do here the visibile sets
            // I can stop here for very rough visibility
            pD->_possibleVisCount = 0;
            if(Compiler::PCompiler->_bPvsMethod==2)
            {
                PerformPVS(prtA, rPortals, pVis);
            }
            R_PortalFlood(prtA, pVis, prtA._sideLIdx[1]);   // flod in neghbour leaf
            if (Compiler::PCompiler->HasBreaked())
            {
                bRetVal = false;
                break;
            }
            percent = (100*nCount++) / (1+nTotal);
            PROG(percent);
        }
    }
    _CATCHX()
    {
        bRetVal = false;
    }
    delete []pVis;
    return bRetVal;
}

//---------------------------------------------------------------------------------------
void    PVSPrc::R_PortalFlood(Portal& srcP, u_int8_t* pVis, int32_t nLeaf)
{
    if(_portVs[srcP._idxThis]->_portArrPvs[nLeaf] == 1) //<<<<leaf vis bit
        return;

    _portVs[srcP._idxThis]->_portArrPvs.Set(nLeaf);
    _portVs[srcP._idxThis]->_possibleVisCount++;       // count how many portals this prtA can flood
    CLeaf*          pToLeaf = _pPortPrc->GetLeaf(nLeaf);
    vvector<int32_t>&   iporrts = pToLeaf->GetPortalIdxes();

    FOREACH(vvector<int32_t>, iporrts, pip)
    {
        #pragma message("unlimit limit here the flood")
        if(0 == pVis[*pip])                             // dont flood in this prtA
            continue;

        Portal& prtA = _pPortPrc->GetPortal(*pip);
        R_PortalFlood(srcP, pVis, prtA._sideLIdx[1]);
    }
}

//---------------------------------------------------------------------------------------
void    PVSPrc::PerformPVS(Portal& portA, vvector<Portal>& rPortals, u_int8_t* prevPvs)
{
    FOREACH(vvector<Portal>, rPortals, portItB)
    {
        Portal& prtB = *portItB;
        if(prtB == portA)
            continue;

        if(prevPvs[prtB._idxThis] == 0)
            continue;

        if(!PortalSeesPortal(portA,prtB))
            prevPvs[prtB._idxThis]=0;
    }
}

//---------------------------------------------------------------------------------------
void    PVSPrc::CompressPVS()
{
}

//---------------------------------------------------------------------------------------
void    PVSPrc::GaterLeafsVis()
{
    int32_t     dummy;
    if(_pvs) delete[] _pvs; _pvs = 0;
    int32_t      nPerLeaf    = AllocBitsRoundByLong(_nleafs);
    _cPvss  = _nleafs * nPerLeaf;                           // all leafs
    _pvs    = new u_int8_t[_cPvss];

    LOG(E_PVS,"PVS: PVS data has '%d' bytes ",_cPvss);

    ::memset(_pvs, 0, _cPvss);                              // serooo them
    u_int8_t*    pWalkPvsPtr       = _pvs;                            // head of oll bytes

    CBitArray pPerLeafPvs(_nleafs)  ;


    FOREACH(vvector<CLeaf*>, _pBspTree->_leafs, ppLeaf)
    {
        CLeaf*          pLeaf = (*ppLeaf);
        vvector<int32_t>&   pidxs = pLeaf->GetPortalIdxes();

        pPerLeafPvs.Reset();
        pLeaf->_pvsIdx  = pWalkPvsPtr - _pvs;

        pPerLeafPvs.Set((*ppLeaf)->_leafIdx);

        FOREACH(vvector<int32_t>,pidxs,pidx)
        {
            Portal& portal = _pPortPrc->GetPortal(*pidx);
            PortalData* pD = _portVs[portal._idxThis];
            pPerLeafPvs.Merge(pD->_portArrPvs);
        }
        assert(pPerLeafPvs.Size() == nPerLeaf);
        if(Compiler::PCompiler->_bCompressPvs)
        {
            pWalkPvsPtr += CompressLeafPVS( pWalkPvsPtr, pPerLeafPvs.Buffer(dummy) ,nPerLeaf);
        }
        else
        {
            ::memcpy(pWalkPvsPtr, pPerLeafPvs.Buffer(dummy), nPerLeaf);
            pWalkPvsPtr += nPerLeaf;
        }
    }
}



int32_t CompressLeafPVS(u_int8_t* pDest, u_int8_t* pLeafPvs, int32_t bytesPvs)
{
    u_int8_t*   pDestC = pDest;
    int32_t     repeatCnt;

    for(int32_t i=0; i < bytesPvs ; i++)    // for each octet
    {
        *pDestC++ = pLeafPvs[i];
        if(pLeafPvs[i])
            continue;      // all octet is !=0 do not compress

        repeatCnt = 1;
        for(i++; i < bytesPvs; i++)     // now count the 0 octets
        {
            if(pLeafPvs[i] || repeatCnt==255) // no more than 255 zeroes in 1 octet
                break;
            else
                repeatCnt++;
        }
        *pDestC++ = repeatCnt;
        i--;                            // repos the counter
    }
    return pDestC - pDest;
}



u_bool32    PVSPrc::PortalSeesPortal(Portal& a, Portal& b)
{
    v3d_t dummy;

    // test center
    v3d_t  paa;// = a._vxes[0]+ (a._vxes[1] - a._vxes[0])*.5f;
    v3d_t  pab;// = b._vxes[0]+ (b._vxes[1] - b._vxes[0])*.5f;


    FOREACH(vvector<v3d_t>, a._vxes ,pv)
        paa+=*pv;

    FOREACH(vvector<v3d_t>, b._vxes ,pv)
        pab+=*pv;

    paa/=(f32_t)a._vxes.size();
    pab/=(f32_t)b._vxes.size();

    CollPack   col;
    if(!_pBspTree->SegmentIntersect(paa, pab, 0, col))
    {
        return true;
    }

    v3d_t va,vb;
    FOREACH(vvector<v3d_t>, a._vxes, pva)
    {
        FOREACH(vvector<v3d_t>, b._vxes, pvb)
        {
            va.interpolate(*pva,paa,0.2);
            vb.interpolate(*pvb,pab,0.2);

            if(!_pBspTree->SegmentIntersect(va, vb, 0, col))
            {
                return true;
            }
        }
    }
    return false;
}

void PVSPrc::DoVisByLeafs()
{
    if(_pvs)
        delete[] _pvs;
    _pvs = 0;
    int32_t      nPerLeaf    = AllocBitsRoundByLong(_nleafs);
    _cPvss  = _nleafs * nPerLeaf;                           // all leafs
    _pvs    = new u_int8_t[_cPvss];
    ::memset(_pvs, 0, _cPvss);                              // zeroo them
    u_int8_t*    pWalkPvsPtr       = _pvs;                      // head of oll bytes
    CBitArray pPerLeafPvs(_nleafs)  ;

    // for each leaf
    FOREACH(vvector<CLeaf*>, _pBspTree->_leafs, ppLeafFrom)
    {

        pPerLeafPvs.Reset();

        for(vvector<CLeaf*>::iterator ppLeafTo = ppLeafFrom; ppLeafTo != _pBspTree->_leafs.end(); ppLeafTo++)
        {
            CLeaf*  pLFrom   = *ppLeafFrom;
            CLeaf*  pLTo     = *ppLeafTo;

            if(pLFrom == pLTo)
            {
                pPerLeafPvs.Set(pLTo->_leafIdx);
            }
            else
            {
                FOREACH(vvector<int32_t>,  pLFrom->_portIdxes, ppFrom)
                {
                    Portal&  pFrom =  _pPortPrc->GetPortal(*ppFrom);

                    FOREACH(vvector<int32_t>,  pLTo->_portIdxes, ppTo)
                    {
                        Portal&  pTo =  _pPortPrc->GetPortal(*ppTo);

                        if(CompletteOnBack(pFrom, pTo))    // complette on back we dont see this
                            continue;                       // so leave his byte on 0

                        if(CompletteOnFront(pTo, pFrom))  // are we complette on front
                            continue;

                        if(Compiler::PCompiler->_bPvsMethod==2)
                        {
                            if(PortalSeesPortal(pFrom, pTo))
                            {
                                pPerLeafPvs.Set(pLFrom->_leafIdx);
                                goto EX_PORTAL;
                            }
                        }
                        else
                        {
                            pPerLeafPvs.Set(pLFrom->_leafIdx);
                            goto EX_PORTAL;
                        }
                    }
                }
EX_PORTAL:;
            }
        }
        int32_t dummy;
        ::memcpy(pWalkPvsPtr, pPerLeafPvs.Buffer(dummy), nPerLeaf);
        pWalkPvsPtr += nPerLeaf;
    }
}


void    PVSPrc::Clear()
{
    delete[] _pvs;  _pvs=0;
    _portVs.deleteelements();
};
