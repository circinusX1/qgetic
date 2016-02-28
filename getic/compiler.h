#ifndef __CCOMPILER_H__
#define __CCOMPILER_H__

#include "winwrap.h"
#include "geticmainwnd.h"
#include <baseutils.h>
#include <brush.h>
#include "bsptree.h"
#include "portalprc.h"
#include "pvsprc.h"
#include "flowpath.h"
#include "lmproc.h"
#include "bsphook.h"
#include "tertree.h"



//---------------------------------------------------------------------------------------
#define REND_BSP        0x1
#define REND_PORTALS    0x2
enum {
    CUT_PORTAL_BY_NODE,
    CUT_PORTAL_BY_PARENTS,
    CUT_PORTAL_BY_LEAFS,
};

//---------------------------------------------------------------------------------------
struct SBytes
{
    typedef enum _SType{S_IMAGE, S_SOUND, S_SCRIPT, S_UNKNOWN}SType;
    SBytes():ptr(0){}
    ~SBytes(){delete[] ptr;}
    u_int8_t*   ptr;
    int32_t    size;
    int32_t    offset;
    SType   stype;
};

//---------------------------------------------------------------------------------------
class View3D;
class Scene;
class CDlgCompProgress;
class Compiler  : public OsThread
{
public:
    Compiler();
    virtual ~Compiler(){Clear();};

    u_bool32     Compile(Scene& scene);
    void     CommitChanges();
    u_bool32     Save(const char* pFIleName);
    u_bool32     ExportBSP(const char* pFIleName);
    u_bool32     PerformHSR(PBrushes* pBrushes );
    void     Clear();
    u_bool32     Done(){return _done;}
    void     Render(View3D* pV, u_int32_t what);
    static   u_bool32 Union(PBrushes& pPrimitives, Brush& brush, u_bool32 sgow=0);
    void     Break(int32_t st=1){_break=st;};
    int32_t     HasBreaked(){return _break;}
    void     FromRegistry(u_bool32 bRead);
    void     Export(const char* bspFile);
    u_int32_t	 DoLightMaps();
    u_bool32     CompileTerrain();
    Brush   *GetZone(Box& box, int32_t* pindex=0);
    Brush   *GetZone(const v3d_t& vx);

    void    thread_main();

private:
    void    _ManuaIntersectZones();
    int32_t     _CheckBrushes(u_bool32 hasTerrain);

    void    _RecutSelectedBrushes();

    u_bool32    PerformESR(PBrushes* pbrushes);
    void    ReevaluateBrush(Brush* pBrush);
    void    R_FloodBSP(CBspTree&  tmpTree, PortalPRC& tmpPPrc, int32_t iLeaf, u_int8_t* wetBytes);
    void    LinkDetailsToMainBsp();
    void    LinkDetailsToMainSky();
    void    SortBrushes(PBrushes& pBrushes);
    u_bool32    PerformLmapCalculation(CBspTree* pMainModel);

    void	EstablishTrigerLinks(Brush* pBrush, int32_t trrIndex);
    void    ZoneAreea(SceItem*);
    void    ZoneAreea(CBspTree*, u_bool32 isMain);
    void    ZoneMainBSP(CBspTree*);
    void    AutomateZoneVisibility(CBspTree*);
    void    Clean();

private:    //saving
    void    WriteHeader(FlStream& fw);
    void    WriteSceneInfo(FlStream& fw);
    void    WriteSkyDomInfo(FlStream& fw);
    void    WriteScripts(FlStream& fw);
    void    WriteScriptsData(FlStream& fw);
    void    WriteImages(FlStream& fw);
    void    WriteSounds(FlStream& fw);
    void    WriteZones(FlStream& fw);
    void    KeepSecttions(u_int32_t cs, u_int32_t cend, BTF_Section& s, FlStream& fw);
    void    UpdateServerIndex();
    void	WriteXML(const char *xmlFname,
                     const char* bspNameFinalZ,
                     const char* subDir,
                     vvector<tstring>& dependencies,
                     const vvector<u_int32_t>& lengths);
    void    WriteGameInit(FlStream& fw);
    void    WriteLMInfoAndData(FlStream& fw);
    void    WriteVertexes(vvector<v3d_t>& vp, FlStream& fw);
    void    WritePvs(FlStream& fw)    ;
    void    WriteBsps(map<int32_t,int32_t>& texids, FlStream& fw);
    void    WritePortals(FlStream& fw);
    void	WriteTexInfo(map<int32_t,int32_t>& texids, FlStream& fw);
    void    WriteSndInfo(map<int32_t,int32_t>& sndInfo, FlStream& fw);
    void    WriteItemsInfo(map<int32_t,int32_t>& sndidx, map<int32_t,int32_t>& texids, FlStream& fw);
    void    WriteV3s(vvector<v3d_t>& vertexPool,FlStream& fw, u_int32_t sect);
    void    WritePlanes(vvector<Plane>& allPlanes, FlStream& fw, u_int32_t sect);
    void    WriteNodes(vvector<CMiniNode*>& allNodes, FlStream& fw, u_int32_t sect);
    void    WriteLeafs(vvector<CLeaf*>& allLeafs,   FlStream& fw, u_int32_t sect);
    void    WriteModels(FlStream& fw, u_int32_t sect);
    void    WritePolys(vvector<Poly*>& allPolys,
                             map<int32_t,int32_t>& texids,
                             FlStream& fw, u_int32_t sect);
    void    WriteMotions(FlStream& fw);
    void    WritItemsCathegory(FlStream& fw);
    void    WriteTerrain(FlStream& fw, map<int32_t,int32_t>& texids);
    void    WriteDetails(FlStream& fw);
    void    WriteRandomBits(FlStream& fw);

public:
    vvector<CBspTree*>  _bTrees;
    TerTree             _terrTree;
    CFlowPath           _flowPath;
    PortalPRC           _portPrc;
    LMProc              _lmProc;

    PVSPrc              _pvsPrc;
    Scene*              _pScene;
    u_bool32                _done;

    u_bool32                _bAutoDetailSmall;
    u_bool32                _bAutoDetailPolys;
    u_bool32                _esrErr;
    u_bool32                _removeOnePolyLeaf;
    u_bool32                _bCompressPvs;

    u_bool32                _bAABBTreeCsgTer;
    u_bool32                _bEsr;
    int32_t                 _bPvsMethod;
    u_bool32                _bGeticBSP;//         = theApp->GetProfileInt("BSP","GETICBSP",1);
    u_bool32                _bSnapMainBsp;
    u_bool32                _bZones;
    int32_t                _break;
    u_bool32				_bSaveImages;
    int32_t                 _lmapIntens;
    int32_t                _maxFacesLeaf;
    u_bool32				_bSaveCats;
    u_bool32                _bPolySortCrit;
    u_bool32				_bSavePortals;
    u_bool32                _bMergeNarrowLeafs;
    u_bool32				_bSaveSkyDom;
    u_bool32                _bSaveBigTerrain;
    u_bool32                _bSaveBigTerrainBSP;
    u_bool32                _bAvoidNonOrt;
    u_bool32                _bUseUserPriority;
    vvector<SBytes*>    _randombytes;

    u_bool32                _bConvexEarly;
    u_bool32                _bLightMaps;
    u_bool32                _lmpowof2;
    u_bool32                _lmConst;
    u_bool32                _b3SampleLM;
    u_bool32                _bSaveFinalBSP;
    u_bool32                _szipit;
    f32_t                _narowLeaf;
    int32_t                 _portalCutFlag;
    u_bool32                _bExportFile;

    QString             _exBspPlugin;
    QString             _finalBSpFileName;
    int32_t                 _lmapMaxSize;
    f32_t                _lMapLumelSz;
    u_bool32                _bLmPatch;
    int32_t                 _balance;
    int32_t                 _curStep;
    int32_t                 _percentage;
    int32_t                _inThread;
    int32_t				_polysStats;
    int32_t				_cutsStats;
    u_bool32                _errDetected;
    Brush*              _zones[32];
    int32_t                 _zonecount;
    u_int32_t               _timeStart;

public:
    static Compiler* PCompiler;
};

//---------------------------------------------------------------------------------------
template <class T, class U> void NOTIFY_FRAME(u_int32_t m, const T& a, const U& b)
{
    emit(_Hmain->onUpdate(m,(void*)a,(void*)b));
}


//---------------------------------------------------------------------------------------
#define COMP()          Compiler::PCompiler

#endif // !__CCOMPILER_H__
