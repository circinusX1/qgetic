#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <list>
#include "winwrap.h"
#include "scene.h"
#include "camera.h"
#include "view2d.h"
#include "bsptree.h"
#include "compiler.h"
#include "motion.h"
#include "undo.h"
#include "cfont.h"
#include "dialognewbrush.h"
#include "cdialogevaldets.h"

class SceItem;
class CLightBulb;
class View2D;
class View3D;
class IGeticPlug;
class CArchive;

#define BIT_SPECULAR    0x4
#define BIT_DIFFUSE     0x2
#define BIT_AMBIENT     0x1


struct CCmdUI
{
    void Enable(){}
    void Disable(){}
};

// DOC_VERSION9
//---------------------------------------------------------------------------------------
class CDocSaver : public FileWrap
{
public:
    CDocSaver(const char* szFile, u_bool32 bsave, u_bool32 pref=0):_pref(pref),_version(DOC_VERSION10),_filename(szFile),_bsave(bsave),_maxspliterIdx(0),_selection(0),_pTerBrush(0){}
    u_bool32    Open(){
        return this->FileWrap::Open(_filename.c_str(),_bsave ? "wb" : "rb");
    }
    void    SerializeDoc(u_bool32 selection);
    void    SerializeHeader();
    void    SerializeScene();
    void    SerializeSettings();
    void    SerializeCompSettings();
    void    SerializeTextures();
    void    SerializeBrushes();
    void    SerializeItems();
    void    SerializeMotions();
    void    SerializeScripts();
    void    SerializeCatFiles();
    void    SerializeTerrain();
    void    SerializeDescriptions();

    u_bool32    IsPerf(){return _pref;}

    void SerializeBrush(Brush* pB);
    void SerializePolygon(Brush* pB, Poly* pp);
    SceItem* SerializeItem(SceItem* pi);

private:
    u_int32_t               _version;
    int32_t                 _maxspliterIdx;
    map<int32_t,int32_t>    _texassoc;
    vvector<Texture>        _texNames;
    map<int32_t,Brush*>     _dynaBrushes;
    u_bool32                _bsave;
    tstring                 _filename;
    u_bool32                _pref;
    u_bool32                _selection;
    Brush*                  _pTerBrush;
};


class Document
{


public:
    Document();
    virtual ~Document();

    int32_t OnCompileDone(WPARAM,LPARAM);
    void Repos20();
    void EndEditor();
    u_bool32 StartEditor();
    u_bool32 InitGL(int32_t i3d,HDC hDC, HGLRC hrc);
    void SetShift(View2D* pSrc);
    void SetZoom(View2D* pSrc);
    void ReposCamLight(u_int32_t enableBits, int32_t availLght, f32_t offset=128.0, u_rgba_t cdif=ZBLACK);



    void DisableAll(u_bool32 bd);
    void RayIntersectBrush(v3d_t& s, v3d_t& end,_EL_SEL sel);
    void RayIntersectItems(v3d_t& s, v3d_t& end);
    void SetErrorPoint(const v3d_t& v){_errPoint = v;};
    void SetActiveWiew(QWidget* pWnd){_pActiveView = pWnd;};
    u_int32_t SetFaceMode(u_bool32 resetBlends=false);
    void  SetLightMode(u_bool32 inv=true);
    void  SbarShowCurMode();
    void  MoveCamera(const v3d_t& p);
    QString GetTitle()const{return _title;}
    void SetTitle(const char* t){_title=t;};
    void SetTitle(const QString& t){_title=t;};
    const char* GetTitles()const{return _title.toUtf8();}
    void  DrawSolidBrush(Brush& rb,
                          vvector<Poly*>&,
                          vvector<Poly*>&,
                          vvector<Poly*>&,
                          vvector<Poly*>&,
                          vvector<Poly*>&,
                          u_bool32 usesel=false);
    void DrawSpecialPolygons(View3D *pV,
                             u_int32_t lMaps,
                             vvector<Poly*>& backDraw,
                             vvector<Poly*>& transPolys,
                             vvector<Poly*>& mirrors,
                             vvector<Poly*>& masked,
                             vvector<Poly*>& guround);
    void   DrawRect(Htex& texture, const Box& box);
    Brush* GetLastSelBrush();
    Brush* GetFirstSelBrush();
    u_bool32   DeleteBrush(Brush* pBrush, u_bool32 undo=true);
    void  DeleteBrushMotion(Brush*);
    u_bool32  AddBrushMotion(Brush* _pCurBrush, CMotion* pNewMotion);
    void  AddBrush(Brush* pB);
    void  CameraPointTo(v3d_t& v);
    void  DetachTrigger(Brush* pB);
    void  DrawGridLines();
    void    ProjectTexture(Brush& projBrush);
    Htex    RecreateAmbLmap();
    QString ResFile(const char* filenme);
    u_bool32    DuplicateFile(const char* src, u_bool32 over = true);
    void    CopyResources(const char* srcFile);
// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(Document)
public:
    void Serialize(CArchive& ar);
    void DeleteContents();
protected:
    //}}AFX_VIRTUAL

// Implementation
public:

// Generated message map functions
public:


public:
    void     Escape(u_bool32 resetSels=true);
    void     RoundToNear(v3d_t &v, int32_t snp=16);
    void     TestBackupFile();
    void     SaveTempFile();
    //debug
    void     DrawAxis(v3d_t& org, f32_t ex);
    void     InvokePlugIn(const char* dllname, int32_t ncmd);
    u_bool32     AddItem(SceItem* pItem);
    void     DelItem(SceItem* pItem);

    void	AddDbgPoly(Poly& p){
        _debugPolys << p;
    }
    u_bool32    OnStartMoveRotScale();
    void    OnStartMoveRotScale(int32_t operation, Brush** ppBrushes, int32_t count);
    void    OnStartMoveRotScale(int32_t operation, SceItem** ppItems, int32_t count);
    void    OnFinishMoveRotScale(int32_t operation, vvector<Brush*>* pBrushes=0);
    u_bool32    UpdateTouches(Brush* pPrim, Brush* pMovedBrush, map<Brush*, int32_t>& accList);
    void    UpdateCutsCollisionList(Brush** ppB, int32_t brushes );
    void    SplitBySplitterBrush(Brush* pb);
    void    DeleteUncomittedNewBrushes();
    void    DeleteUncomittedNewItems();
    void    SetViewMode(){_brmode=BR_VIEW;};
    void    GLDrawBox(Box&,u_bool32, RGBA );
    void    Draw2DScene(View2D* pV);
    void    Draw3DScene(View3D* pV);
    void    Invalidate(u_bool32 update=false) ;
    void    Invalidate3D(u_bool32 fromMM=false) ;
    void    DoSelection3D(const QPoint& point, View3D* pV, _EL_SEL sel);
    void    DoSelection2D(f32_t x, f32_t y, View2D* pV, _EL_SEL sel);
    void    CreateSelBox(QPoint& pt);
    void    HideSelBox();
    void    Make3dCursorPos(View3D* pV,const QPoint& pt, QRect rt, v3d_t& out);
    u_bool32    BrushIntersect(v3d_t& vstart, v3d_t& end, int32_t&, int32_t& , int32_t&);

    u_bool32    SelectItem(SceItem* pItemFnd, u_bool32 notyBar=true);
    void    DeselectItem(SceItem* pItemFnd,u_bool32 notyBar=true);
    void    DeselectBrushes();
    void    DeselectPolys(u_bool32 desel=1);
    void    DeselectVertexes();
    void    SelectEdge(int32_t brSel, int32_t nPoly, int32_t vxSel);
    void	Recut(vvector<Brush*>* extra=0);
    void    DrawErrorPoint();
    void    DrawTerrain();
    u_bool32    HasSomeSelected(){
        return (_pSelBrushes.size()!=0 ||_pSelPolys.size()!=0 || _pSelItems.size()!=0) ;
    }
    void    SelectByBox();
    u_bool32    SelectBrush(Brush* pB);
    void    DeSelectBrush(Brush* pB);
    u_bool32    SelectFace(Poly* pB, Brush* pb=0);
    void    DeSelectFace(Poly* pB);
    void    DrawCompiledStates(View3D* pV);
    void    DeselectItems(u_bool32 bNoty=true);

    Brush** GetSelBrushes(int32_t& count){
        count = _pSelBrushes.size();
        if(count)
            return &_pSelBrushes[0];
        return 0;
    }

    SceItem** GetSelItems(int32_t& count){
        count = _pSelItems.size();
        if(count)
            return &_pSelItems[0];
        return 0;
    }

    Vtx2** GetSelVxes(int32_t& count){
        count = _pSelVertexes.size();
        if(count)
            return &_pSelVertexes[0];
        return 0;
    }


    Poly** GetSelPolys(int32_t& count){
        count = _pSelPolys.size();
        if(count)
            return &_pSelPolys[0];
        return 0;
    }

    int32_t HasForms(){
        if(_bShowCsg)
        {
              return _scene.GetBrushes()->size();
        }
        return _scene.GetPrimitives()->size();
    }

    vvector<SceItem*>*  GetItems(){
        return _scene.GetItems();
    }

    PBrushes* GetForms(){
        if(_bShowCsg)
        {
              return _scene.GetBrushes();
        }
        return _scene.GetPrimitives();
    }

    Brush* GetForm(int32_t index){
        if(_bShowCsg)
            return _scene.GetBrush(index);
        return _scene.GetPrimitive(index);
    }
    PBrushes* GetPrimitives(){
        return _scene.GetPrimitives();
    }
    u_bool32      IsSelBoxOn(){
        return _selBox._brushflags & BRSH_SELVIS;
    }
    void	  SelectTabBrush(int32_t direction, u_bool32 cumulate);
    void      RotateByAngleCw(f32_t rad);
    void      SetSelMode(EL_SEL selmode ){_selmode = selmode ; Invalidate() ;}

    u_bool32	  ResolveTexturesOnHRC(Brush* pB, Brush** pBRet,
                                vvector<Texture >&  texNames,
                                vvector<Texture >*  pTexNames);
    void      ExportFile(PlugInDll<IGeticPlug>&,const char*);
    void      ImportFile(PlugInDll<IGeticPlug>&,const char*);
    void      CleanProjections(Brush& projBrush, u_bool32 onlyFlags=0);
    void	  LocalDrawBspTreeGraph(Compiler& compiler, int32_t& leafNo, View3D* pV);
    void	  LocalDrawReadyBSP(Compiler&  compiler, Scene& scene, View3D* pV);
    u_bool32      LoadPrefFile(const char* szFileName, Brush** pBRet=0);
    void      DrawPolyVertexes(u_int32_t how, const Poly& poly, int32_t texCoord, u_int32_t what=0);
    void      DrawPolyLines(Poly& poly);
    void      DrawPolygon(Poly& poly, u_bool32 wireMode);
    void      DrawOrtho(const v3d_t& pos, const v3d_t& dir, f32_t len, f32_t clratn=1.0);
    void      BlockSelections(u_bool32 bl){_blocksel = bl;}
    void      ReadFromFile(const char* szFilename);
    void      MkCylinder();
    void      DoHSR();


protected:
    void AddVertex(Poly* ps, const v3d_t& v);
    void SaveInFile(const QString& pSzName, u_bool32 prims=false);
    void SavePrefFile();
    void SaveInXML(const char* pSzName, u_bool32 bBrshes=false);
    void ReadFromBSP(const char* szFilename);
    void _BSP_ReadPolys(const FileWrap& fw, int32_t elements);


    void SaveFinalBSP();
    void ModifyBrushes(vvector<Brush*>& pChanged);
    void RedoVxSelection(Brush* pBrush);
    void DrawDebugPointsAndPolys();
    void Draw3DRawScene(View3D* pV);
    void UpdateBrushLinks(Brush* pBrush);
    void DeleteBrushItemLinks(Brush* pbrush);
    void UpdateDynamicObjPos(Brush* pb);
    void EvaluateDetailbrsh(Brush* );
    void ApplyFlags(Brush* b);
    void DoBoxSelSelection();
    void DrawCameraRay(View2D *pV);
    void DrawProjectorBrush(Brush& rb);
    void DrawCamFrustrum(View2D *pV);
    void SelectBrush(int32_t brSel, int32_t faceSel);
    Poly* SelectFace(int32_t brSel, int32_t faceSel);
    void SelectVertex(Vtx2*,Poly*);
    void SelectAllBrushVertexes(Brush& b, Vtx2& x);
    void DeselectVertex(Vtx2*);
    void SelectVertex(int32_t,int32_t,int32_t);
    void Draw2DBrushes(View2D *pV);
    void Draw2dItems(View2D *pV);
    void DrawTextured(View3D *pV);
    void DrawSelBrushes(View3D *pV);
    void DrawSelFaces(View3D *pV);
    void DrawCutBrush(Brush& rb);
    void DrawMotionsPaths();
    void OnMkShellBox(v3d_t& vdim) ;
    Box  GetReferenceBrushBox();
    v3d_t   GetViewXYZWidths();
    void InitDocVars();
    void ProjectTexture();
    void MarkDetPolys(int32_t percent);
    void CommitBrush(Brush* pBrush);
    void RWCompSettings(FileWrap    &fw, u_bool32 save);
    void ApplyCloserLights(const Poly& poly);
    void DrawItems();
    void RecalcBrushes();
    u_bool32 HasExtension(const char* szex); //{"_GL_EXT_separate_specular_color")
    void TestFreeVersion(int32_t polys);
    void _DrawCamera();
    void _SeColor(COLORREF crf, PPolys& pSelPolys);
    void _SeColor(COLORREF crf, vvector<Vtx2*>& pSelVertexes);
    void _SeColor(COLORREF crf, vvector<SceItem*>& pSelItems);
    void _SeColor(COLORREF crf, PBrushes& pSelBrushes);


public:



    /////////////////// commands
    ///
public slots:

    void OnButselbrush();
    void OnEscape();
    void OnBurotate();
    void OnBumove();
    void OnBUScale();
    void OnBuriselow();
    void OnFileNew();




    //=========================================================

    void    DeselectAll(u_bool32 killDets=true);

    u_bool32 OnNewDocument();
    void OnFileSave();
    void OnFileSaveAs();
    void OnFileOpen();
    u_bool32 OnOpenDocument();

    //=====================================================
    // MK
    void OnMkBox();
    void OnEnter();


#ifdef DOCCMD_MFC

    int32_t OnCompileDone(WPARAM,LPARAM);
    void OnUpdateTbLockxyz(CCmdUI* pCmdUI);
    void OnUpdateBurotate(CCmdUI* pCmdUI);
    void OnUpdateBumove(CCmdUI* pCmdUI);
    void OnUpdateBUScale(CCmdUI* pCmdUI);
    void OnUpdateBushcam(CCmdUI* pCmdUI);
    void OnUpdateBusnapgrid(CCmdUI* pCmdUI);
    void OnUpdateButselbrush(CCmdUI* pCmdUI);
    void OnUpdateBufacesel(CCmdUI* pCmdUI);
    void OnUpdateBuwireframe(CCmdUI* pCmdUI);
    void OnUpdateBushowback(CCmdUI* pCmdUI);
    void OnTbLockxyz();

    void OnBushcam();
    void OnBusnapgrid();
    void OnBureverse();
    void OnButselbrush();
    void OnBufacesel();
    void OnFileSave();
    void OnBuwireframe();
    void OnEditCopy();
    void OnEditPaste();
    void OnBushowback();
    void OnBushowcsg();
    void OnUpdateBushowcsg(CCmdUI* pCmdUI);
    void OnDelete();
    void OnCutBrush();
    void OnUpdateCutBrush(CCmdUI* pCmdUI);
    void OnTabNextsel();

    void OnUpdateBuriselow(CCmdUI* pCmdUI);
    void OnUpdateEditCopy(CCmdUI* pCmdUI);
    void OnCut();
    void OnUpdateCut(CCmdUI* pCmdUI);
    void OnUpdateEditPaste(CCmdUI* pCmdUI);
    void OnUpdateDelete(CCmdUI* pCmdUI);
    void OnEscape();
    void OnUpdateMirror(CCmdUI* pCmdUI);
    void OnMirror();
    void OnUpdateEnter(CCmdUI* pCmdUI);
    void OnXalign();
    void OnUpdateXalign(CCmdUI* pCmdUI);
    void OnYalign();
    void OnUpdateYalign(CCmdUI* pCmdUI);
    void OnZalign();
    void OnUpdateZalign(CCmdUI* pCmdUI);
    void OnXinter();
    void OnUpdateXinter(CCmdUI* pCmdUI);
    void OnYinter();
    void OnUpdateYinter(CCmdUI* pCmdUI);
    void OnZinter();
    void OnUpdateZinter(CCmdUI* pCmdUI);
    void OnXdim();
    void OnUpdateXdim(CCmdUI* pCmdUI);
    void OnYdim();
    void OnUpdateYdim(CCmdUI* pCmdUI);
    void OnZdim();
    void OnUpdateZdim(CCmdUI* pCmdUI);
    void OnXyzdim();
    void OnUpdateXyzdim(CCmdUI* pCmdUI);
    void OnEditUndo();
    void OnUpdateEditUndo(CCmdUI* pCmdUI);
    void OnFileSaveAs();
    void OnBusplitable();
    void OnUpdateBusplitable(CCmdUI* pCmdUI);
    void OnBuredcuts();
    void OnBucone();
    void OnBustairs();
    void OnTabPrevsel();
    void OnSelectall();
    void OnTabSelnext();
    void OnTabSelprev();
    void OnCompbsp();
    void OnDelface();
    void OnBucompile();
    void OnUpdateBucompile(CCmdUI* pCmdUI);
    void OnViewPortals();
    void OnViewByFar();
    void OnViewByFarUpdate(CCmdUI* pCmdUI);
    void OnUpdateOnViewPortals(CCmdUI* pCmdUI);
    void OnViewColorLeafs();
    void OnUpdateOnViewColorLeafs(CCmdUI* pCmdUI);
    void OnViewWhite();
    void OnViewWhiteUpdate(CCmdUI* pCmdUI);
    void OnSelsameasselface();
    void OnAlignRightHit();
    void OnAlighLefthit();
    void OnAlighBothit();
    void OnAlighTophit();
    void OnSelectgroup();
    void OnSelcurid();
    void OnUpdateSelcurid(CCmdUI* pCmdUI);
    void OnUpdateSelectgroup(CCmdUI* pCmdUI);
    void OnUpdateSelsameasselface(CCmdUI* pCmdUI);
    void OnUpdateSelectall(CCmdUI* pCmdUI);
    void OnUpdateBureverse(CCmdUI* pCmdUI);
    void OnUpdateBucsg(CCmdUI* pCmdUI);
    void OnLr();
    void OnUpdateLr(CCmdUI* pCmdUI);
    void OnTb();
    void OnUpdateTb(CCmdUI* pCmdUI);
    void OnFb();
    void OnUpdateFb(CCmdUI* pCmdUI);
    void OnCtx2dRotatecw30();
    void OnCtx2dRotatecw60();
    void OnCtx2dRotatecw90();
    void OnCtx2dRotatecw45();
    void OnBusheet();
    void OnMatchwidth();
    void OnSelitem();
    void OnUpdateSelitem(CCmdUI* pCmdUI);
    void OnViewFullBSP();
    void OnUpdateOnViewFullBSP(CCmdUI* pCmdUI);
    void OnInsitem();
    void OnAddModel();
    void OnUpdateAddModel(CCmdUI* pCmdUI);
    void OnInspref();
    void OnUpdateInsertMesh(CCmdUI* pCmdUI);
    void OnBusphere();
    void OnBspcursel();
    void OnWpncfg();
    void OnCompileroptions();
    void OnUpdateCompileroptions(CCmdUI* pCmdUI);
    void OnImport();
    void OnSelvertex();
    void OnUpdateSelvertex(CCmdUI* pCmdUI);
    void OnTexrefdlg();
    void OnShowselbbox();
    void OnTerian();
    void OnUpdateTerian(CCmdUI* pCmdUI);
    void On2dshape();
    void OnUpdate2dshape(CCmdUI* pCmdUI);
    void OnReclmaps();
    void OnUpdateReclmaps(CCmdUI* pCmdUI);
    void OnFileMerge();
    void OnUpdateParskin(CCmdUI* pCmdUI);
    void OnParskin();
    void OnTriangulate();
    void OnTriFan();
    void OnUpdateTriFan(CCmdUI* pCmdUI);
    void OnSametexface();
    void OnSamenormface();
    void OnInsgameitem();
    void OnUpdateInsgameitem(CCmdUI* pCmdUI);
    void OnSpline();
    void OnUpdateSpline(CCmdUI* pCmdUI);
    void OnClosedspline();
    void OnUpdateClosedspline(CCmdUI* pCmdUI);
    void OnXoy();
    void OnUpdateXoy(CCmdUI* pCmdUI);
    void OnXoz();
    void OnUpdateXoz(CCmdUI* pCmdUI);
    void OnZoy();
    void OnUpdateZoy(CCmdUI* pCmdUI);
    void OnExtrude();
    void OnUpdateExtrude(CCmdUI* pCmdUI);
    void OnHallounhallo();
    void OnUpdateHallounhallo(CCmdUI* pCmdUI);
    void OnMerge();
    void OnUpdateMerge(CCmdUI* pCmdUI);
    void OnProjBrush();
    void OnUpdateProjBrush(CCmdUI* pCmdUI);
    void OnPermcut();
    void OnUpdatePermcut(CCmdUI* pCmdUI);
    void OnUndo();
    void OnUpdateUndo(CCmdUI* pCmdUI);
    void OnUpdateTervol(CCmdUI* pCmdUI);
    void OnTervol();
    void OnManTex();
    void OnUpdateManTex(CCmdUI* pCmdUI);
    void OnToXml();
    void OnUpdateToXml(CCmdUI* pCmdUI);
    void OnExtractfaces();
    void OnUpdateExtractfaces(CCmdUI* pCmdUI);
    void OnEvaldets();
    void OnUpdateEvaldets(CCmdUI* pCmdUI);
    void OnHome();
    void OnCreatesb();
    void OnUpdateCreatesb(CCmdUI* pCmdUI);
    void OnLoadlastbackip();
    void OnUpdateLoadlastbackip(CCmdUI* pCmdUI);
    void OnShowgrid();
    void OnUpdateShowgrid(CCmdUI* pCmdUI);
    void OnWbck();
    void OnTrpolys();
    void OnRepoint();
    void OnUpdateRepoint(CCmdUI* pCmdUI);
    void OnUpdateSave(CCmdUI* pCmdUI);
    void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
    void OnAlbybox();
    void OnUpdateAlbybox(CCmdUI* pCmdUI);
    void OnZones();
    void OnUpdateZones(CCmdUI* pCmdUI);
    void OnViewLeafByLeaf();
    void OnUpdateViewLeafByLeaf(CCmdUI* pCmdUI);
    void OnViewLightmaps();
    void OnUpdateViewLightmaps(CCmdUI* pCmdUI);
    void OnViewGraph();
    void OnUpdateViewGraph(CCmdUI* pCmdUI);
    void OnCamLspot();
    void OnFileSaveselAs();
    void OnUpdateFileSaveselAs(CCmdUI* pCmdUI);
    void OnInsface();
    void OnUpdateInsface(CCmdUI* pCmdUI);
    void OnCutwithbrush();
    void OnUpdateCutwithbrush(CCmdUI* pCmdUI);
    void OnUpdateTriangulate(CCmdUI* pCmdUI);
    void OnTerianbig();
    void OnUpdateTerianbig(CCmdUI* pCmdUI);
    void OnSnapvxes();
    void OnUpdateSnapvxes(CCmdUI* pCmdUI);
    void OnInsvx();
    void OnUpdateInsvx(CCmdUI* pCmdUI);
    void OnPaintimer();
    void OnUpdateCombo(CCmdUI* pCmdUI);
    void OnUpdateCombots(CCmdUI* pCmdUI);
    void OnSeledge();
    void OnUpdateSeledge(CCmdUI* pCmdUI);
    void OnRepointbrsh();
    void OnUpdateRepointbrsh(CCmdUI* pCmdUI);
    void OnImpp();
    void OnUpdateImpp(CCmdUI* pCmdUI);
    void OnExpp();
    void OnUpdateExpp(CCmdUI* pCmdUI);
    void OnLeafs();
    void OnUpdateLeafs(CCmdUI* pCmdUI);
    void OnSelnondetbrushes();
    void OnDetbrushes();
    void OnColor();
    void OnUpdateColor(CCmdUI* pCmdUI);
#endif //MFC



public:
    u_int32_t            _frmTimer;
    u_bool32            _blocksel;	 //blocks selection when evaluate mode is on
    QWidget        *_pActiveView;
    BR_MODE         _brmode;
    BR_MODE         _brmodePrev;
    EL_SEL          _selmode;
    EL_SEL          _prevSelmode;
    u_int32_t           _compviewmode;
    u_bool32            _bSnapGrid;
    Camera		    _cam;
    u_bool32            _alignment;
    u_bool32            _snapToGrid;
    int32_t             _gridSize;
    int32_t             _gridStep;
    CFont           _cfont;
    u_bool32            _xyz_lock;
    u_bool32            _selBoxByTouch;
    int32_t             _iselbrush;
    v3d_t              _shift;
    v3d_t              _zoom;
    u_bool32            _bDrawCamRay;
    u_bool32			b_showgrid;
    int32_t             b_whitebk;
    Scene           _scene;
    v3d_t			    _camray[3];
    v3d_t			    _frustDots[5];
    v3d_t              _ip[2];
    u_bool32		    _vmodefire;
    u_bool32		    _vmodeback;
    int32_t             _curgroup;
    int32_t             _maxUsedGrp;
    int32_t             _curCreatGroup; //current group of items we create
    Brush           _selBox;
    v3d_t              _camsave[2];

    PBrushes			_pClipboard;
    vvector<SceItem*>	_pClipboardItm;

    vvector<PolyEdge>   _selEdges;
    vvector<SceItem*>	_pSelItems;
    vvector<Vtx2*>      _pSelVertexes;
    int32_t                 i_geomfilever;

#ifdef _DEBUG
    vvector<v3d_t>         _testPoints;
#endif

    PBrushes        _pSelBrushes;
    PPolys          _pSelPolys;
    Compiler        _compiler;
    int32_t             _leafNo;
    u_bool32            _compiling;     //set when we compile BSP
// child ctrls

    SNewBrush         _cachedSett;

    u_bool32            _bShowCsg;
    u_bool32            _closing;
    int32_t             _undodirt;
    u_bool32            _docDirty;
//
    vvector<Poly>	_debugPolys;
    v3d_t              _errPoint;
    u_bool32			_cmdMsg;
    int32_t				_exViewMode;
    int32_t             _lastItmSel;    //selection in item insert dialog
    int32_t             _viewChanged;
    QPoint          _scrPoint;
    u_bool32           _doneReceived;
    u_bool32           _selShowSelBox;
    Brush*         _pLastMakedBrush;
    HGLRC          _hrc;
    u_bool32           _userBool;
    u_bool32           _allselected;
    tstring         _docdir;
    tstring         _lasterror;
    int32_t         _viewCount;
    u_int32_t		_lvupdate;
    CDialogEvalDets	_devdet;
    u_bool32        _clearing;
    QWidget*        _lastFocusView;
    QString         _tempfileName;
    u_bool32        _complBreaking;
    u_int32_t       _polycount;
    int32_t         _use2splitidx;
    int32_t         _maxLights;
    int32_t         _numLights;
    int32_t         _maxMirrorsDraws;
    u_bool32        _camSpot;
    string          _extensions;
    CUndo           _undo;
    u_bool32        _tildselect;
    QString         _title;


};



#define SCENE() Pd->_scene
#define TERRIAN()  Pd->_scene.Terrain()

extern u_int8_t	CLR_SEL[4];
extern u_int8_t	CLR_NORM[4];
extern u_int8_t	CLR_BBOX[4];
extern Document*   Pd;

#define DOC()   Pd
#define AKEY    theApp->isKeyDown

class NO_VT GlMatrix
{
public:
    GlMatrix(){
        glPushMatrix();
    }
    ~GlMatrix(){
        glPopMatrix();
    }
};

class SelModeSave
{
public:
    SelModeSave(BR_MODE& pmode, EL_SEL& psel):_pmode(&pmode),_psel(&psel),
                                              _vmode(pmode), _vsel(psel)
    {
    }
    ~SelModeSave()
    {
        *_pmode = _vmode;
        *_psel  = _vsel;
    }

    BR_MODE* _pmode;
    EL_SEL*  _psel;
    BR_MODE  _vmode;
    EL_SEL   _vsel;

};


v3d_t GetViewsDims();

#endif // DOCUMENT_H
