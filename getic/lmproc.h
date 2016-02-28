// LMProc.h: interface for the LMProc class.
//
//-----------------------------------------------------------------------------
#ifndef __LMPROC_H__
#define __LMPROC_H__

#include <basecont.h>

//-----------------------------------------------------------------------------
struct LmInfo;
class Scene;
class CBspTree;
class CLightBulb;
class CLeaf;
class Poly;
class TerTree;
//-----------------------------------------------------------------------------
struct LmSizeBuff
{
    LmSizeBuff()
    {
        pBuff = 0;
        index = -1;
        lmName[0] = 0;
        flags = 0;
        sz.cx = 0;
        sz.cy = 0;
        realsz.cx = 0;
        realsz.cy = 0;

    }
    LmSizeBuff(int32_t x, int32_t y, int32_t realx, int32_t realy, int32_t bp)
    {
        index = -1;
        lmName[0] = 0;
        flags = 0;
        sz.cx = x;
        sz.cy = y;
        realsz.cx = realx;
        realsz.cy = realy;
        bpp   = bp;
        pBuff = new u_int8_t[x*y*bp];
        if(!pBuff){
            sz.cx=sz.cy=0;
        }
        index=-1;       // not saved yet. if saed has the offset in bsp file buffer
    }
    LmSizeBuff(const LmSizeBuff& r){
        index = r.index;
        sz.cy = r.sz.cy;
        sz.cx = r.sz.cx;
        realsz.cx = r.realsz.cx;
        realsz.cy = r.realsz.cy;
        bpp   = r.bpp;
        flags = r.flags;
        ::strcpy(lmName, r.lmName);
        pBuff = new u_int8_t[sz.cx*sz.cy*bpp];
        if(pBuff){
            ::memcpy(pBuff, r.pBuff, sz.cx*sz.cy*bpp);
        }
    }
    LmSizeBuff& operator = (const LmSizeBuff& r)
    {
        if(this != &r){
            index = r.index;
            sz.cy = r.sz.cy;
            sz.cx = r.sz.cx;
            realsz.cx = r.realsz.cx;
            realsz.cy = r.realsz.cy;

            bpp   = r.bpp;
            flags = r.flags;
            ::strcpy(lmName, r.lmName);
            if(pBuff)delete[]pBuff;
            pBuff = new u_int8_t[sz.cx*sz.cy*bpp];
            if(pBuff){
                ::memcpy(pBuff, r.pBuff,sz.cx*sz.cy*bpp);
            }
        }
        return *this;
    }
    ~LmSizeBuff(){
        delete[] pBuff;
        pBuff = 0;
    }
    void Reset(){
        pBuff = 0;
    }
    int32_t     index;
    SIZE    sz;
    SIZE    realsz;
    int32_t     bpp;
    u_int32_t   flags;
    u_int8_t*   pBuff;
    char    lmName[16];
};

//-----------------------------------------------------------------------------
class LMProc
{
public:
    friend class Compiler;

    LMProc():_pTree(0)
    {
        _pPoolLmap = 0;
        _szPool = 0;
    }
    virtual ~LMProc();
    void    NTC_BuildLmaps(CBspTree* pTree);//no thread call
    void    NTC_BuildLmaps(TerTree* pTree);
    void    NTC_Clear();     //no thread call
    u_bool32	AllocMemory(vvector<CBspTree*>& bspTrees);
    u_bool32    Calculate(Scene* pScene, vvector<CBspTree*>&);
    u_bool32    GetLmapBytes(const char* name, LmSizeBuff& lnB);
private:
    u_bool32    Calculate(Scene* pScene, CBspTree* pTreeMain, CBspTree* pTree, vvector<CLightBulb*>&);
    void    CalcLMapOnPolys(vvector<Poly*>& polyGroup, vvector<CLightBulb*>&);
    void    CalcLMapOnPolysConst(vvector<Poly*>& polyGroup, vvector<CLightBulb*>& lights, u_bool32 bConst);

    void    GouraudShade(vvector<Poly*>& polyGroup, vvector<CLightBulb*>& lights);
    void	CalcMinMaxUVs(vvector<Poly*>& polyGroup, v2d_t& minUV,v2d_t& maxUV);
    void    BuildLmapEdges(f32_t d2p, v3d_t& normal, v2d_t& minUV, v2d_t& maxUV, v3d_t& v0, v3d_t& v1 ,v3d_t& v2);
//	u_bool32    R_Snd2Tree(CLightBulb* pLB, int32_t nodeIdx)  ;
    void    IluminatePolygons(CLightBulb* pLB, CLeaf*  pLeaf);
    SIZE    CalcBmpDims(const v2d_t& edgesLength);
    u_int8_t*   GetPrevLumels(Poly& poly, v3d_t* pLumels, SIZE sz);
    void    StoreLumels(Poly& poly, v3d_t* pLumels, SIZE sz);
    void    CalculateTtLm(TerTree& tt, vvector<CLightBulb*>& lights);
    void    _GatherSamePolys(vvector<Poly>&, vvector<Poly>::iterator&, vvector<Poly*>&, Box&);
    int32_t    _YolTtLight(TerTree& tt,int32_t x, int32_t z, const v3d_t& vWalk1, const v3d_t& vWalk2, const v3d_t& vWalk3, CLRNOA* glight, int32_t xtiles, vvector<CLightBulb*>&);
    int32_t    _GetLumColor(v3d_t& lumel,  v3d_t& lumelPos, const v3d_t& norm, vvector<CLightBulb*>&);
    int32_t    _RoundToClosestPow2(SIZE& sz, int32_t updown);
private:
    f32_t                 _lmUnit;
    Scene*               _pScene;
    CBspTree*            _pMainTree;
    CBspTree*            _pTree;
    vvector<LmSizeBuff*> _lMaps;
    int32_t                  _totpolys;
    int32_t                  _curPolys;

    u_int8_t*				_pPoolLmap;
    int32_t					_szPool;
    int32_t					_lastAllocIdx;
};

#endif // __LMPROC_H__
