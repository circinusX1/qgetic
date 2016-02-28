// PVSPrc.h: interface for the CPVSPrc class.
// By: Marius Chincisan
//---------------------------------------------------------------------------------------

#ifndef __PVS_PRC__
#define __PVS_PRC__

//---------------------------------------------------------------------------------------
#include <basecont.h>

//---------------------------------------------------------------------------------------
class CBspTree;
class CLeaf;
class Portal;
class PortalPRC;

//---------------------------------------------------------------------------------------
// paralel array of data assocuated to each portal. The portal index idxThis will
// be used to point tin this array
struct PortalData
{
    PortalData(int32_t sz):_portArrPvs(sz)
    {
        _possibleVisCount = 0;
        _size   = _portArrPvs.Size();
    }
    ~PortalData(){}

    int32_t			_size;
    int32_t			_possibleVisCount;
    CBitArray   _portArrPvs;
};

//---------------------------------------------------------------------------------------

class PVSPrc
{
public:
    PVSPrc():_nleafs(0),_pvs(0),_cPvss(0){}
    virtual ~PVSPrc(){
        Clear();
    };

    void    Clear();
    u_bool32    Process(CBspTree* pT, PortalPRC* pP);

private:
    u_bool32    InitalPortalVis(vvector<Portal>& rPortals);
    void    R_PortalFlood(Portal& portal, u_int8_t* pVis, int32_t nLeaf);   // flod in neghbour leaf
    void    PerformPVS(Portal& pOrigin, vvector<Portal>& rPortals, u_int8_t* prevPvs);
    void    CompressPVS();
    void    GaterLeafsVis();
    u_bool32    PortalSeesPortal(Portal& a, Portal& b);
    void    DoVisByLeafs();
private:
    CBspTree*   _pBspTree;
    PortalPRC*  _pPortPrc;
public:
    int32_t                   _nleafs;
    vvector<PortalData*>  _portVs;
public:
    u_int8_t*				  _pvs;
    int32_t					  _cPvss;
};

#endif // !defined(AFX_PVSPRC_H__AC44DB74_60E2_4611_97C6_B7AB50079388__INCLUDED_)
