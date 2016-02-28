//----------------------------------------------------------------------------------------
// Unauthorized use or duplication is strictly prohibited.
// Copyright Zalsoft Inc 1999-2004.
//// Author: Marius C of Zalsoft Inc 2000-2004
//----------------------------------------------------------------------------------------
// Author: Marius Chincisan: 2003
//---------------------------------------------------------------------------------------
#include "stdafx.h"
#include "winwrap.h"
#include "compiler.h"
#include "scene.h"
#include "document.h"
#include "bsptree.h"
#include "view3d.h"
#include "view2d.h"
#include "geticapp.h"
#include "triggeritem.h"
#include "winwrap.h"
#include "cwaitcursor.h"
#include "progressdlg.h"


//---------------------------------------------------------------------------------------
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//---------------------------------------------------------------------------------------
Compiler* Compiler::PCompiler;

static int32_t GCOunter;

//---------------------------------------------------------------------------------------
class Cwc
{
public:
    Cwc(){}
    ~Cwc(){}
    void Step(const char* stp){
        if(ThrID != GetCurrentThreadId())
        {
            if(stp[0]!='-')
            {
                SBT(0, MKSTR("+%s Please Wait !", stp));
            }
        }
    }
};


//---------------------------------------------------------------------------------------
Compiler::Compiler()
{
    _errDetected       = 0;
    _done              = false;
    _bEsr              = true;
    _bPvsMethod        = 1;
    _bGeticBSP         = 1;//theApp->GetProfileInt("BSP","GETICBSP",1);
    _bSnapMainBsp      = 0;
    _bZones            = true;
    _bPvsMethod        = 1;
    _lmapIntens        = 4;
    _bMergeNarrowLeafs = 0;
    _narowLeaf         = 16;  /*cm*/
    _bAutoDetailSmall  = false;
    _bAutoDetailPolys  = false;
    _removeOnePolyLeaf = false;
    _portalCutFlag     = CUT_PORTAL_BY_NODE;
    _break             = 0;
    _bCompressPvs      = false;
    _bAABBTreeCsgTer   =  false;
    _bSaveImages       = false;
    _maxFacesLeaf      = 0;
    _bSaveCats         = false;
    _bSavePortals      = true;
    _bPolySortCrit     = 0;
    _bSaveSkyDom       = true;
    _bSaveBigTerrain   = true;
    _bSaveBigTerrainBSP= true;
    _bExportFile	   = false;

    _bAvoidNonOrt      = false;
    _bUseUserPriority  = false;

    _bConvexEarly      = 0;
    _bLightMaps        = true;
    _lmpowof2          = true;
    _lmConst          = false;
    _b3SampleLM        = true;
    _inThread          = false;
    _bSaveFinalBSP     = false;
    _szipit            = true;
    _bLmPatch          = false;
    _finalBSpFileName  = "";
    _exBspPlugin       = "";
    _balance           = 6;
    _curStep           = 0;
    _percentage        = 0;
    PCompiler          = this;
    _lmapMaxSize       = 16;
    _lMapLumelSz       = 64.0f;

};

//---------------------------------------------------------------------------------------
u_bool32 Compiler::Compile(Scene& scene)
{
#ifdef _DEBUG
    Pd->_testPoints.clear();
#endif //

    LOG(E_COMP, "Compiling BSP level...");

    Pd->Escape();
    Pd->DeselectAll();

    _errDetected=0;
    if(theApp->_returnVal==0)
    {
        PostQuitMessage(0);
        return false;
    }

    char   docName[_MAX_PATH];
    ::_ttcscpy(docName, Pd->GetTitles());
    PathHandler ph(docName);
    Break(0);

    _finalBSpFileName = theApp->BspsDir();
    _finalBSpFileName += ph.JustFile();
    _finalBSpFileName += ".gbt";


    Clear();
    scene.ClearBrushes();
    _pScene  = &scene;
    _lmProc.NTC_Clear();
    _break        = 0;
    _zonecount    = 0;
    _errDetected  = 0;
    FOREACH(PBrushes, (*Pd->_scene.GetPrimitives()), ppb)
    {
        if((*ppb)->IsZone())
        {
            (*ppb)->ZoneIdx()  = _zonecount;
            (*ppb)->VisData()  = 1<<_zonecount; // zone can se itself
            _zones[_zonecount] = (*ppb);
            ++_zonecount;
        }
    }
    _timeStart = ge_gettick();
    return start_thread();
}

//---------------------------------------------------------------------------------------
// see wich trigger points to this brush. Update triger index to this BSP witch is the
// brush bsp
void Compiler::EstablishTrigerLinks(Brush* pBrush, int32_t trrIndex)
{
    vvector<SceItem*>* pItems = _pScene->GetItems();
    FOREACH(vvector<SceItem*>, (*pItems), ppItem)
    {
        if((*ppItem)->_item != ITM_TRIGER)continue;
        if( ((TriggerItem*)(*ppItem))->_pBrush ==  pBrush)
        {
            ((TriggerItem*)(*ppItem))->_brIndex = trrIndex;
            break;
        }
    }
}

//---------------------------------------------------------------------------------------
int32_t Compiler::_CheckBrushes(u_bool32 hasTerrain)
{
    Q_UNUSED(hasTerrain);
    _curStep = 0;
AGAIN:
    FOREACH(PBrushes, (*_pScene->GetPrimitives()), ppBrsh)
    {
        if((*ppBrsh)->_polys.size() == 0)
        {
            Pd->DeleteBrush((*ppBrsh));
            goto AGAIN;
        }

        if((*ppBrsh)->IsSelection() ||
           ((*ppBrsh)->_brushflags & BRSH_BIGTERRAIN))
            continue;

        if(!((*ppBrsh)->_brushflags & BRSH_DETAIL))
        {
            _curStep = 1;
        }

        if((*ppBrsh)->_brushflags & BRSH_DISABLED && !((*ppBrsh)->_brushflags & BRSH_BIGTERRAIN))
        {
            Pd->SelectBrush((*ppBrsh)); // recut these
        }
    }
    return _curStep;
}

void Compiler::_RecutSelectedBrushes()
{

    int32_t nCount;
    Brush** ppBrushes = Pd->GetSelBrushes(nCount);
    if(nCount)
    {
        u_int32_t res = TAfxMessageBox(MKSTR("There are '%d' Brushe(s) disabled (Greyed). \r\n"
            "Do you like to enable them for the Compilation ?",nCount), MB_ICONQUESTION|MB_YESNOCANCEL);
        if(res == IDYES)
        {
            Pd->Recut();
            Pd->DeselectAll();
        }
        else if(res!=IDOK)
        {
            Pd->DeselectAll();
            LOG(E_COMP, "Info: User Abort. You can enable disabled brushes manually");
            throw E_COMP;
        }
    }
}

//---------------------------------------------------------------------------------------
void Compiler::thread_main()
{
    int32_t Gtask = BrushTask;
    //save lmpatch do not aplly it for ESR just for last BSP
    try
    {
        Cwc c;

        BrushTask = 0;

        _inThread       = 1;
        u_bool32 hasTerrain = SCENE().Terrain().Exist();
        PBrushes* pBrushes = _pScene->GetBrushes();

        if(_pScene->GetPrimitives()->size()==0)
        {
            LOG(E_COMP, "Error: the level has not brushes left to compile. They may be wiped out during CSG !");
            throw E_COMP;
        }

        _curStep        = 0;
        _percentage     = 0;
        _done           = false;


        int32_t hasMain = _CheckBrushes(hasTerrain);

        if(_curStep==0 && !hasTerrain)
        {
            Pd->DeselectAll();
            LOG(E_COMP, "Error: CMPL0001. The Scene does not have a Main Brush (All Brushes are details) !");
            throw E_COMP;
        }

        _RecutSelectedBrushes();

        if(hasTerrain)
            CompileTerrain();

        c.Step("HSR");
        //  HSR Step 1
        _curStep++;
        _percentage = 0;
        if(!PerformHSR( Pd->_scene.GetPrimitives()))
        {
            LOG(E_COMP, "Error: HSR0001. Error Performing Hidden Surface Removal !. Check if the level is sealed !");
            throw E_COMP;
        }
        if(pBrushes->size() == 0)
        {
            LOG(E_COMP, "Error: HSR0001. Scene has no brushes to build ! They may have been wiped out by CSG");
            throw E_COMP;
        }

        //

        if(hasMain)
        {
            c.Step("ESR");
            _curStep++;
            _percentage = 0;
            if(_bEsr)
            {
                if(!PerformESR(pBrushes) || _break)
                {
                    throw E_HSR;
                }
            }
        }

        //
        // reserve bsps as many brushes we have
        //
        _bTrees.reserve(pBrushes->size());
        CBspTree* pMainModel = 0;
        CBspTree* pSkyModel  = 0;

        //
        // step 3 compiling prepared geometry
        //
        c.Step("BSP Step 1");
        _curStep++;
        _percentage = 0;
        int32_t nCount = 0;
        v3d_t  bbex;

        //
        // places the main brush on pos 0 and sky on last
        //
        c.Step("Sorting");
        SortBrushes(*pBrushes);
        c.Step("BSP Compiling");
        FOREACH(PBrushes, (*pBrushes), ppBrsh)
        {

            c.Step("-");

            LOG(E_BSP,"BSP: Compiling Brush '%s'. ", (*ppBrsh)->_name);

            if(_break){
                throw E_BSP;
            }
            if((*ppBrsh)->IsZone())
            {
                continue;
            }

            Brush* pBrush = *ppBrsh;
            pBrush->Dirty(1);
            pBrush->Recalc(1/*with normals*/);


            bbex = pBrush->_box.GetExtends();
            if(bbex.x < 16 && bbex.y < 16 && bbex.z < 16)
            {
                LOG(E_BSP,"BSP: Brush: '%s' is to small. Brush minimum size is 16 units. Brush ignored", (*ppBrsh)->_name);
                continue;
            }

            PROG(nCount);
            nCount++;

            CBspTree* pTree    = new  CBspTree();
            if(0==pTree)
            {
                LOG(E_BSP,"BSP: Out of memory", (*ppBrsh)->_name);
                throw E_BSP;
            }
#ifdef WINDOWS
            #pragma message ("copy brush props into tree props")
#endif
            _ttcscpy(pTree->_name, pBrush->_name);

            pTree->_balance     = _balance;
            pTree->_treeflags   = pBrush->_brushflags;
            pTree->_treeprops   = pBrush->_brushprops ;
            pTree->_patchPolys  = Compiler::PCompiler->_bLmPatch;
            pTree->_motionsIdx  = Pd->_scene.GetIndex(pBrush->_pMotion);
            if(pTree->_motionsIdx>=0)
            {
                pTree->_treeprops|=MODEL_DYNAMIC;
            }
            else
            {
                pTree->_treeprops&=~MODEL_DYNAMIC;
            }
            pTree->_thisIdx     = _bTrees.size();

            c.Step("-");
            this->_bConvexEarly = 0; //portal fails
            u_bool32  success = pTree->Compile(pBrush->_polys, pBrush->_brushflags,true, this->_bConvexEarly);
            if(!success)
            {
                Pd->SelectBrush(pBrush);
                Pd->MoveCamera(pBrush->_box.GetCenter());
                Pd->Invalidate();
                delete pTree;
                throw E_BSP;
            }
            pTree->_treeprops   = pBrush->_brushprops ;       //for patching and materials (0xFF)
            pTree->_rotSteps    = pBrush->_rotSteps;
            // add on

            if( pBrush->_brushflags & BRSH_DETAIL ||
                pBrush->_brushflags & BRSH_CSGTERRAIN)
            {
                pTree->_treeprops  |= MODEL_IS_DETAIL;

                if(pBrush->_brushflags & BRSH_CSGTERRAIN)
                    pTree->_treeprops  |= MODEL_IS_TERRAIN;

                if(pBrush->_brushflags |= BRSH_TRIANGLES)
                    pTree->_treeprops  |= MODEL_TRIANGLES;

                if(pBrush->_brushflags |= BRSH_DONTBSP)
                    pTree->_treeprops  |= MODEL_IS_FLAT;

            }
            else if(pBrush->_brushprops & MODEL_IS_SKYDOM)
            {
                assert(0 == pSkyModel);
                pSkyModel = pTree;
            }
            else
            {
                assert(pMainModel  == 0);
                pMainModel         = pTree;
                pTree->_treeprops  |=  MODEL_IS_MAINBSP;
            }
            if(pBrush->_brushflags & BRSH_HASTRIG)
            {
                EstablishTrigerLinks(pBrush , pTree->_thisIdx);
            }
            _bTrees << pTree;
        }//each brush


        if((_bPvsMethod != 0 || _bZones) && pMainModel && !hasTerrain)
        {
            c.Step("Portals");
            _portPrc._mergeLeafs = _bMergeNarrowLeafs;
            if( _portPrc.Process(*pMainModel,1))
            {
                if(_pvsPrc.Process(pMainModel, &_portPrc))
                {
                    pMainModel->_prtProc = &_portPrc;
                }
                else
                {
                    throw E_PVS;
                }
            }
            else
            {
                throw E_PORT;
            }

            if(_bZones)
            {
                AutomateZoneVisibility(pMainModel);
                if(_bPvsMethod == 0)
                {
                    _portPrc.Clear();
                    _pvsPrc.Clear();
                }
            }
            else
                _ManuaIntersectZones();
        }
        else
            _ManuaIntersectZones();

        LinkDetailsToMainBsp();

        if(_bLightMaps)
        {
            if(false == PerformLmapCalculation(pMainModel))
            {
                throw E_LMAPS;
            }
        }

        // reduce trees. See if tree is concave or convex. reduce the leafs to 1
        // place a flag in the tree and collect all polys in that leaf
        c.Step("Concave Convex");
        FOREACH(vvector<CBspTree*>, _bTrees, ppTree)
        {
            (*ppTree)->ConcaveConvex();
        }
        _done = true;
        PROG(-1);
        _inThread  = 0;
    }
    catch(EXCOMP& ierr)
    {
        NOTIFY_FRAME(WM_USR_REPAINT,0,0);
        BrushTask=Gtask;
        NOTIFY_FRAME(WM_COMPILEDONE,ierr,0);
        PROG(-1);
        return ;
    }
#ifndef _DEBUG
    catch(...)
    {
        NOTIFY_FRAME(WM_USR_REPAINT,0,0);
        BrushTask=Gtask;
        NOTIFY_FRAME(WM_COMPILEDONE,E_COMPILER,0);
        PROG(-1);
        return ;
    }
#endif //
    NOTIFY_FRAME(WM_USR_REPAINT,0,0);
    BrushTask=Gtask;
    u_int32_t tsecs = (ge_gettick()-_timeStart)/1000;
    int32_t   tmin = tsecs/60;
    int32_t   secs = tsecs%60;
    LOG(E_BSP,"BSP: Compilation Time: %d tmin %d sec", tmin, secs);
    NOTIFY_FRAME(WM_COMPILEDONE,0,0);
    PROG(-1);
    return ;
}


//---------------------------------------------------------------------------------------
void    Compiler::ReevaluateBrush(Brush* pBrush)
{
    LOG(E_HSR,"HSR: Evaluating detail brushes '%s'",pBrush->_name);

    if(pBrush->_brushprops & MODEL_IS_SKYDOM)
    return;

    if(_bAutoDetailSmall)
    {
        v3d_t ex = pBrush->_box.GetExtends();
        if(ex.getmin() < 30.f && ex.getmax() < 200.0)
        pBrush->_brushflags |= BRSH_DETAIL;
    }

    if(_bAutoDetailPolys)
    {
        if(pBrush->_polys.size() > 32)
        pBrush->_brushflags |= BRSH_DETAIL;
    }
}

//---------------------------------------------------------------------------------------
u_bool32    Compiler::CompileTerrain()
{
    _terrTree.Compile(&SCENE().Terrain());
     return 1;
}

//---------------------------------------------------------------------------------------
// union all brushes and delete any poly ennding up in solid space
u_bool32    Compiler::PerformHSR(PBrushes* pBrushes )
{
    u_bool32 brv = false;
    _polysStats = 0;
    _cutsStats  = 0;

    if( 0 == pBrushes->size())
        return brv;

    LOG(E_HSR,"HSR: Performing hidden surface removal");

    PBrushes    brshSolids(256);
    PBrushes    brshSkydoms(64);

    CWaitCursor cwc;
    //
    // clear prev output brushes
    //
    int32_t nCount = 0;

    //scene.ClearBrushes();

    FOREACH(PBrushes, (*pBrushes), ppBrsh)
    {
        Brush* pBrushO =  (*ppBrsh);
        Brush* pBrush  =  pBrushO->_pResult;

        if((pBrush)->IsSelection() || (pBrush->_brushflags & BRSH_BIGTERRAIN) )
              continue;


        #pragma message("auto details based on number of polys and minimum extend")
        /*
        if(_bAutoDetailSmall || _bAutoDetailPolys)
        {
            ReevaluateBrush(pBrush); // preserve the skydoms
        }
        */

        // not touch detail brushes. add them to scene as they are
        if(pBrush->_brushflags & BRSH_DETAIL || pBrush->_brushflags & BRSH_CSGTERRAIN)
        {
            Brush* pResultSceneBrsh = new Brush(*pBrush);

            if(pBrush->_brushflags & BRSH_DETAIL)
            {
                CMotion* pMotion = pBrush->_pMotion ? pBrush->_pMotion : pBrushO->_pMotion;
                pResultSceneBrsh->_pMotion = pMotion;
            }


            Pd->_scene.AddBrush(pResultSceneBrsh);
            brv = true;
            continue;
        }

        // collect all skydom brushes
        if(pBrush->_brushprops & MODEL_IS_SKYDOM)
        {
            pBrush->Recalc();
            brshSkydoms.push_back(pBrush);
            continue;
        }

        if(pBrush->_brushflags & BRSH_SOLID)
        {
            pBrush->Recalc();
            brshSolids.push_back(pBrush);
        }
    }

    if(brshSolids.size() != 0)
    {
        Brush* pResultSceneBrsh = new Brush();

        pResultSceneBrsh->_brushflags = brshSolids[0]->_brushflags | BRSH_SOLID;

        if(brshSolids.size())
        {
            if(Union(brshSolids, *pResultSceneBrsh,1))
            {
                strcpy(pResultSceneBrsh->_name, brshSolids[0]->_name);
                Pd->_scene.AddBrush(pResultSceneBrsh);
                brv = true;
            }
            else
            {
                delete pResultSceneBrsh;
                pResultSceneBrsh = 0;
                return false;
            }
        }
        else
        {
            delete pResultSceneBrsh;
            pResultSceneBrsh = 0;
            return false;
        }
    }

    if(brshSkydoms.size() != 0)
    {
        Brush* pResultSceneBrsh = new Brush();
        pResultSceneBrsh->_brushflags = BRSH_SOLID;
        pResultSceneBrsh->_brushprops = MODEL_IS_SKYDOM;

        if(Union(brshSkydoms, *pResultSceneBrsh,1))
        {
            Pd->_scene.AddBrush(pResultSceneBrsh);
            brv = true;
        }
        else{
            delete pResultSceneBrsh;
            pResultSceneBrsh = 0;
            return false;
        }
    }

    cwc.Restore();

    LOG(E_HSR,"HSR: Successful");
    return brv ;
}

//---------------------------------------------------------------------------------------

u_bool32    Compiler::Union(PBrushes& pPrimitives, Brush& brush, u_bool32 sgow)
{
    Polys   firstPolys;
    Polys   pSplitters;
    Polys   splittedPolys;
    Polys   outPolys;
    int32_t     bClipIt  = false;
    Brush*  pB1      = NULL;
    Brush*  pB2      = NULL;

    int32_t counter = {0};

    int32_t         cntInPolys  = 0;
    int32_t         cntOutPolys = 0;
    int32_t         maxsteps = pPrimitives.size()*pPrimitives.size();


    FOREACH(PBrushes, pPrimitives, ppPrim)
    {
        bClipIt = false;
        pB1     = *ppPrim;

        if(pB1->IsSelection()|| (pB1->_brushflags & BRSH_BIGTERRAIN) )
        {
            continue;
        }

        pB1->GetTxPolysCopy(firstPolys);    // get translated polygons
        cntInPolys+=firstPolys.size();

        FOREACH(PBrushes, pPrimitives, ppPrim1)
        {
            if((*ppPrim1)->IsSelection()|| ((*ppPrim1)->_brushflags & BRSH_BIGTERRAIN))
            {
                continue;
            }

            (*ppPrim1)->Recalc();
            (*ppPrim)->Recalc();

            pB2 = *ppPrim1;
            if (pB1 == pB2)
            {
                bClipIt = true;
                ++counter;
                continue;
            }

            if(!pB1->_box.IsTouchesBox(pB2->_box))
            {
                continue;
                ++counter;
            }

            // animation by selection
            if((counter & 0xF) == 0xF)
            {

                if(GAnimCompilation)
                {
                    NOTIFY_FRAME(WM_USR_REPAINT,pB1,pB2);
                }


                int32_t percentage = (counter*100)/(maxsteps+1);
                if(PCompiler->_inThread==0)//LOG
                {
                    KEEP_WAIT(MKSTR("Performing Constructive Solid Geometry %d%%", percentage));
                }
                else
                {
                    PROG(percentage);
                }
            }

            do //scope
            {
                MiniBsp bsp;
                pB2->GetTxPolysCopy(pSplitters);

                if(!bsp.Compile(pSplitters,0,false))
                {
                    return false;
                }

                bsp.ClipBrPolys(firstPolys, splittedPolys, bClipIt,1,1);
                firstPolys.clear();
                firstPolys = splittedPolys;
                pSplitters.clear();
                splittedPolys.clear();

            }while(0);

            if(GAnimCompilation)
            {
                NOTIFY_FRAME(WM_USR_REPAINT,0,0);
                Sleep(0);
            }
            ++counter;
        }
        append(outPolys, firstPolys);
        firstPolys.clear();
    }

    if (outPolys.size())
    {
        FOREACH(Polys, outPolys, op)
        {
            op->Recalc();
            op->_pBrush = &brush;
            brush._polys.push_back(*op);
            cntOutPolys++;
        }
        brush.Recalc();
        brush._brushflags |= BRSH_SOLID;
    }
    PROG(-1);
    SBT(0,"+Ready");
    Compiler::PCompiler->_polysStats=cntInPolys;
    return true;
}


//---------------------------------------------------------------------------------------
void  Compiler::Render(View3D* pV, u_int32_t what)
{
    if(!_done)
    {
        if(pV)
            pV->TextOut(V0,"There is nothing to render",ZWHITE);
    }

    if(what & REND_BSP && _terrTree.Exist())
    {
        _terrTree.Render(pV!=0);
    }
    if((what & REND_PORTALS) && pV)
    {
        _portPrc.Render(pV);
    }

}

//---------------------------------------------------------------------------------------
int32_t FindLeaf(v3d_t& pct, CBspTree&  tmpTree)
{
    CMiniNode* pNode = tmpTree.Root();

    while(pNode->IsNode())
    {
        int32_t side = pNode->GetPlane().GetSide(pct);

        if(side >= 0)
        pNode = pNode->Front();
        else if(side < 0)
        pNode = pNode->Back();

    }
    return pNode->_leafIdx;

}

//---------------------------------------------------------------------------------------
// start on a leaf and perform a flood fill on the tree. wet all leafs we can go in from
// this by entering a portal
u_bool32  Compiler::PerformESR(PBrushes* pbrushes)
{
    int32_t         dummy;
    CBspTree    tmpTree;
    PortalPRC   tmpPPrc;
    int32_t         inPolys  = 0;
    int32_t         outPolys = 0;
    Brush*      pBrush   = 0;

    LOG(E_ESR," ESR: Performing Exterior Surface Removal");

    FOREACH(PBrushes, (*pbrushes), ppBrsh)
    {
        pBrush = *ppBrsh;
        if( pBrush->_brushprops & MODEL_IS_SKYDOM)
            continue;

        if( pBrush->_brushflags & BRSH_DETAIL)
            continue;

        if( pBrush->_brushflags & BRSH_CSGTERRAIN)
            continue;

        if( pBrush->IsSelection()|| ((pBrush)->_brushflags & BRSH_BIGTERRAIN))
            continue;
        break;
    }

    // ESR only on main tree
    if(pBrush->_brushflags & BRSH_CSGTERRAIN || pBrush->IsSelection() || ((pBrush)->_brushflags & BRSH_BIGTERRAIN))
    {
        return true;
    }

    ::_ttcscpy(tmpTree._name, pBrush->_name);
    if(0==tmpTree.Compile(pBrush->_polys, pBrush->_brushflags, false))
        return 0;

    int32_t savedFlag = Compiler::PCompiler->_portalCutFlag;
    tmpPPrc._testExterior = 0;
    Compiler::PCompiler->_portalCutFlag = savedFlag;
    if(0==tmpPPrc.Process(tmpTree))
    {
        Compiler::PCompiler->_portalCutFlag = savedFlag;
        return false;
    }
    Compiler::PCompiler->_portalCutFlag = savedFlag;
    //
    // reset del flags
    //
    FOREACH(vvector<Poly>, tmpTree.GetPolys(), ppoly)
    {
        ppoly->_polyflags &= ~POLY_DELETED;
        inPolys++;
    }


    // AUG 04
    // find a leaf around center
    //
    int32_t      kCount  = 128;
    v3d_t       vp      = V0;
    int32_t      ileaf   = -1;
    int32_t      j       = 0;
    ileaf = tmpTree.R_FindLeaf(0, vp);

    //
    // get first leaf center
    //
    if(-1 == ileaf)
    {
        LOGERR(E_ESR, V0,"Error: Center of the map must be empty!");
        _errDetected=1;
        return false;
    }



    u_int8_t*  byAlloc = new u_int8_t[tmpTree.GetLeafsCount()]; //alloc bits for each leaf

    if(0 == byAlloc)return false;
    ::memset(byAlloc,0,tmpTree.GetLeafsCount());


    _TRY{

        GCOunter = tmpTree._leafs.size();
        PROG(0);
        (0);
        R_FloodBSP(tmpTree, tmpPPrc, ileaf, byAlloc);
        PROG(100);

        //
        // delete unvisitted polys
        //
        vvector<CLeaf*>&     iLfsPtr = tmpTree.GetLeafs();
        int32_t j = 0;
        FOREACH(vvector<CLeaf*>, iLfsPtr, ppLeaf)
        {
            CLeaf*  pLeaf  = *ppLeaf;
            int32_t     nCount = 0;
            Poly*   pHead  = pLeaf->GetPolys(nCount);

            if(byAlloc[j]!=0)
            {
                j++;
                continue;
            }

            for(int32_t i=0; i<nCount;i++)
            {
                pHead->_polyflags |= POLY_DELETED;
                pHead++;
            }
            j++;
        }
    }
    _CATCHX()
    {
        delete[] byAlloc;
        return false;
    }
    delete[] byAlloc;

    if(_removeOnePolyLeaf) //remove leaking leafs
    {
        FOREACH(vvector<Portal>, tmpPPrc.GetPortals(), pPortal)
        {
            // if both leafs are 1 poly remove their polys
            int32_t    nfpp = pPortal->BackLeaf()->_nPolys;
            int32_t    nbpp = pPortal->FrontLeaf()->_nPolys;
            int32_t    nfpc = pPortal->FrontLeaf()->_portIdxes.size();
            int32_t    nbpc = pPortal->BackLeaf()->_portIdxes.size();

            if( nfpp ==1 && nbpp==1)
            {
                pPortal->BackLeaf()->GetPolys(dummy)->_polyflags |= POLY_DELETED;
                pPortal->FrontLeaf()->GetPolys(dummy)->_polyflags |= POLY_DELETED;
            }
        }
    }
    else
    {

    }

    //
    // place polys back into the brush
    //
    pBrush->_polys.clear();

    FOREACH(vvector<Poly>, tmpTree.GetPolys(), prpoly)
    {
        if(prpoly->_polyflags & POLY_DELETED)
        continue;

        //
        // clean splitter flags for next processing
        //
        prpoly->_polyflags &= ~SPLITTER_POLY;
        pBrush->_polys << (*prpoly);
    }

    // count the remaining polygons. if none thare means there is a hole
    // in the solid
    tmpTree.Clear();
    tmpPPrc.Clear();

    if(pBrush->_polys.size()==0 )
    {
        LOGERR(E_ESR, V0, MKSTR("Error: Leaf '%d' is Leaking outside.",ileaf));
        _errDetected=1;
        return false;
    }
    pBrush->Recalc();
    outPolys = pBrush->_polys.size();

    LOG(0, MKSTR(" ESR: %d polygons outside map were deleted",inPolys - outPolys));
    return true;
}

//---------------------------------------------------------------------------------------
// flood all leafs recusrivelly and mark the leafs visited in the array.
// anu non touched leafs
void  Compiler::R_FloodBSP(CBspTree&  tmpTree, PortalPRC& tmpPPrc, int32_t iLeaf, u_int8_t* wetBytes)
{
    wetBytes[iLeaf] = 1;

    CLeaf*          pLeaf = tmpTree.GetLeaf(iLeaf);
    vvector<int32_t>&   portalsIdxes = pLeaf->GetPortalIdxes();
    int32_t             percentage   = (100* (++_percentage))/GCOunter;

    FOREACH(vvector<int32_t>,  portalsIdxes, ppidx)
    {
        Portal&  portal =  tmpPPrc.GetPortal(*ppidx);
        CLeaf*   pNleaf = portal.FrontLeaf();

        if(wetBytes[pNleaf->_leafIdx] == 0)
        {
            v3d_t ex = pNleaf->_bbox.GetExtends();
            R_FloodBSP(tmpTree, tmpPPrc, pNleaf->_leafIdx, wetBytes);
        }
    }
}

//---------------------------------------------------------------------------------------
void    Compiler::LinkDetailsToMainSky()
{
    vvector<CBspTree*>::iterator itBegin =  _bTrees.begin();
    vvector<CBspTree*>::iterator itEnd   =  _bTrees.end();
    CBspTree* pMaster = _bTrees.back();

    assert(pMaster->_treeprops & MODEL_IS_SKYDOM);
    assert(!(pMaster->_treeprops & MODEL_IS_DETAIL));

    int32_t cnt = 0;
    for(;itBegin != itEnd; itBegin++)
    {
        if( ((*itBegin)->_treeprops & MODEL_IS_SKYDOM) &&
        ((*itBegin)->_treeprops & MODEL_IS_DETAIL))
        {
            pMaster->LinkDetailModel(*itBegin, false);
        }
        cnt++;
    }
}

//---------------------------------------------------------------------------------------
// Takes each detail bsp bsp and trow's them on the main bsp. [0]
// Updates the main bsp leaf with the detail bsp index as a link of details (_detaiIdxes).
void    Compiler::LinkDetailsToMainBsp() // LMAP DEPENDS OF IT
{
    LOG(E_COMP, "BSP: Linking detail models to main BSP)");
    vvector<CBspTree*>::iterator itBegin =  _bTrees.begin();
    vvector<CBspTree*>::iterator itEnd   =  _bTrees.end();
    CBspTree* pMaster = *itBegin++;

    ZoneAreea(pMaster, 1);
    //
    // link detail models
    //
    int32_t cnt = 0;
    for(;itBegin != itEnd; itBegin++)
    {
        if((*itBegin)->_treeprops & MODEL_IS_SKYDOM) // reject sky dom detail brushes
        {
            continue;
        }
        if((*itBegin)->_treeprops & MODEL_IS_DETAIL ||
            (*itBegin)->_treeprops & MODEL_IS_TERRAIN)
        {
            ZoneAreea(*itBegin, 0);
            pMaster->LinkDetailModel(*itBegin);
        }
        cnt++;
    }

    //
    // link items to main bsp
    //
    vvector<SceItem*>* pItems = _pScene->GetItems();
    FOREACH(vvector<SceItem*>, (*pItems), ppItem)
    {
        ZoneAreea(*ppItem);
        pMaster->LinkItem(*ppItem,1);
    }
    LOG(E_COMP, "BSP: %d detail models linked successfully", cnt);
}

//---------------------------------------------------------------------------------------
void    Compiler::ZoneAreea(SceItem* psi)
{
    psi->_zonearea = 0;
    Brush* pZone = GetZone(psi->_t);
    if(pZone)
    {
        psi->_zonearea = pZone->VisData();

    }
    else
        psi->_zonearea  = 0xFFFFFFFF;
}

//---------------------------------------------------------------------------------------
void    Compiler::ZoneAreea(CBspTree* pBsp, u_bool32 isMain)
{
    if(isMain)
        ZoneMainBSP(pBsp);
    else
    {
        pBsp->_zoneareea = 0;
        Brush* pZone = GetZone(pBsp->_nodesPtr[0]->_bbox.GetCenter());
        if(pZone)
        {
            pBsp->_zoneareea = pZone->VisData();

        }
        else
            pBsp->_zoneareea = 0xFFFFFFFF;
    }
}

//---------------------------------------------------------------------------------------
void    Compiler::ZoneMainBSP(CBspTree* pTree)
{

    FOREACH(vvector<CMiniNode*>, pTree->_nodesPtr,ppNode)
    {
        CMiniNode* pNode = *ppNode;
        Brush   *pZone= GetZone(pNode->_bbox.GetCenter());
        pNode->_zonearea = 0;
        if(pZone)
        {
            pNode->_zonearea = pZone->VisData();

        }
        else
            pNode->_zonearea = 0xFFFFFFFF;
    }
    FOREACH(vvector<CLeaf*>, pTree->_leafs, ppLeaf)
    {
        CLeaf* pLeaf = *ppLeaf;
        Brush* pZone = GetZone(pLeaf->_bbox);
        pLeaf->_zonearea = 0;
        if(pZone)
        {
            pLeaf->_zonearea = pZone->VisData();

        }
        else
            pLeaf->_zonearea = 0xFFFFFFFF;
    }
}

//---------------------------------------------------------------------------------------
Brush   *Compiler::GetZone(const v3d_t& vx)
{
    int32_t i=0;
    while(_zones[i])
    {
        if(_zones[i]->_box.ContainPoint(vx))
        {
            return _zones[i];
        }
        ++i;
    }
    return 0;
}

//---------------------------------------------------------------------------------------
Brush   *Compiler::GetZone(Box& box, int32_t* pindex)
{
    int32_t i=0;
    while(_zones[i])
    {
        if(_zones[i]->_box.ContainBox(box))
        {
            if(pindex)
                *pindex=i;
            return _zones[i];
        }
        ++i;
    }
    return 0;
}


//---------------------------------------------------------------------------------------
// places the main bsp on position 0. and aktdom brushes on last pos (thats all)
void    Compiler::SortBrushes(PBrushes& brushes)
{
    int32_t     skyIdx = -1;
    int32_t     elements = brushes.size();
    Brush*  pT;

    for(int32_t i=0; i<elements; i++)
    {
        if(brushes[i]->_brushflags & BRSH_DETAIL)
        continue;

        if(brushes[i]->_brushprops & MODEL_IS_SKYDOM)
        {
            skyIdx = i;
            continue;
        }
        if(0!=i)
        {
            pT = brushes[0];
            brushes[0] = brushes[i];
            brushes[i] = pT;
        }
    }
    //
    // places the skydom brush on last position
    //
    if(skyIdx>=0 && skyIdx != elements-1)
    {
        pT = brushes[skyIdx];
        brushes[skyIdx] = brushes[elements-1];
        brushes[elements-1] = pT;
    }

    brushes[0]->_brushflags = BRSH_MAINBSP;
}

//---------------------------------------------------------------------------------------
static u_bool32	Local_IsLeafVisFromLeaf(vvector<CLeaf*>& leafs, int32_t l1, int32_t l2, u_int8_t* ppvs)
{
    if(l1<0||l2<0)return 1;
    u_int8_t* pPvs1 = &ppvs[leafs[l1]->_pvsIdx];
    return (pPvs1[l2 >> 3] & (1<<(l2 & 7)) )!=0;
}

//---------------------------------------------------------------------------------------
void    Compiler::_ManuaIntersectZones()
{
    for(int32_t i=0;i<_zonecount;i++)
    {
        for(int32_t j=0;j<_zonecount;j++)
        {
            if(j==i)continue;

            Brush* pZone1 = _zones[i];
            Brush* pZone2 = _zones[j];
            if(pZone1->_box.IsTouchesBox(pZone2->_box))
            {
                pZone1->VisData() |= (1 << j);
                pZone2->VisData() |= (1 << i);
            }
        }
    }
}

//---------------------------------------------------------------------------------------
void    Compiler::AutomateZoneVisibility(CBspTree* pBsp)
{
    int32_t                         index;
    map<int32_t, vector<CLeaf*> >   leafsPerZone;

    FOREACH(vvector<CLeaf*>, pBsp->_leafs, ppLeaf) // assign to leafs zone index
    {
        index        = -1;
        CLeaf* pLeaf = *ppLeaf;
        Brush* pZone = GetZone(pLeaf->_bbox, &index);
        if(pZone)
        {
            leafsPerZone[index].push_back(pLeaf);
        }
    }

    // for each zone leafs test other lefas
    map<int32_t, vector<CLeaf*> >::iterator izB = leafsPerZone.begin();
    map<int32_t, vector<CLeaf*> >::iterator izE = leafsPerZone.end();
    for(;izB!=izE;izB++)
    {
        vector<CLeaf*>& vLeafs = (*izB).second;

        // for each leaf from this zone
        FOREACH(vector<CLeaf*>, vLeafs, ppLeaf)
        {
            // search in other zones leafs
            map<int32_t, vector<CLeaf*> >::iterator izB2 = izB;
            map<int32_t, vector<CLeaf*> >::iterator izE2 = leafsPerZone.end();
            izB2++;
            for(;izB2!=izE2;izB2++)
            {
                vector<CLeaf*>& vLeafs2 = (*izB2).second;

                FOREACH(vector<CLeaf*>, vLeafs2, ppLeaf2)
                {
                    if(Local_IsLeafVisFromLeaf(pBsp->_leafs,
                                               (*ppLeaf)->_leafIdx,
                                               (*ppLeaf2)->_leafIdx,
                                               _pvsPrc._pvs))
                    {
                        _zones[(*izB).first]->VisData()  |= (1 << (*izB2).first);
                        _zones[(*izB2).first]->VisData() |= (1 << (*izB).first);
                        break;
                    }
                }
            }//zone i2
        }
    }//zone i1
}

//---------------------------------------------------------------------------------------
u_bool32    Compiler::PerformLmapCalculation(CBspTree* pMainModel)
{
    LOG(E_HSR,"LM: processing light maps ");

    if(!_lmProc.AllocMemory(_bTrees))
    {
        return false;
    }

    if(false == _lmProc.Calculate(_pScene, _bTrees))
    {
        return false;
    }


    /*
    FOREACH(vvector<CBspTree*>,_bTrees, ppT)
    {
        if((*ppT)->_brushprops & MODEL_IS_SKYDOM)
        {
            continue;
        }

        if(false == _lmProc.Calculate(_pScene, _bTrees[0], *ppT))
        return false;
    }
    */
    return true;
}

//---------------------------------------------------------------------------------------
void Compiler::CommitChanges()
{
    CWaitCursor cwc;
    PROG(-1);
    if(_break){
        NOTIFY_FRAME(WM_COMPILEDONE,E_LMAPS,0);
        _inThread          = 0;
        return;
    }

    int32_t nIndex = 0;
    int32_t total       = _bTrees.size();
    int32_t percentage;

    START_WAIT("Generating Light Map Textures...");
    FOREACH(vvector<CBspTree*>,_bTrees, ppT)
    {
        _lmProc.NTC_BuildLmaps(*ppT);
        percentage  = (100*nIndex++)/(1+total);
        PROG(percentage);

        if(_break){
            END_WAIT("");
            NOTIFY_FRAME(WM_COMPILEDONE,E_LMAPS,0);
            _inThread= 0;
        }
    }
    _lmProc.NTC_BuildLmaps(&_terrTree);
    PROG(-1);
    END_WAIT("");

}

//---------------------------------------------------------------------------------------
u_int32_t Compiler::DoLightMaps()
{
    if(_bTrees.size() )
    {
        _lmProc.NTC_Clear();

        if(PerformLmapCalculation(_bTrees[0]))
        {
            CommitChanges();
            return E_LMAPS;
        }
    }
    return 0;
}
//---------------------------------------------------------------------------------------
void Compiler::Clear()
{
    memset(_zones,0,sizeof(_zones));
    _zonecount     = 0;
   _done           = false;


   _randombytes.deleteelements();

    FOREACH(vvector<CBspTree*>,_bTrees, ppT)
    {

        FOREACH(vvector<Poly>, (*ppT)->_polys, pPoly)
        {
            pPoly->_lmInfo._lmIndex = -1;
            if(pPoly->_ilmTex)
            {
                theApp->_TexSys.RemoveTexture(pPoly->_ilmTex);
            }
        }
    }
    if(_terrTree.Exist())
    {
        FOREACH( vvector<TerLeaf*>, _terrTree.p_leafs, ppl)
        {
            theApp->_TexSys.RemoveTexture((*ppl)->lm_Tex);
            (*ppl)->lm_info._lmIndex = -1;
        }
    }

    _pvsPrc.Clear();
    _bTrees.deleteelements();
    _lmProc.NTC_Clear();
    _portPrc.Clear();
    _terrTree.Clear();
    _done=false;
};

//---------------------------------------------------------------------------------------
void Compiler::FromRegistry(u_bool32 bRead)
{
    if(bRead)
    {
        //_finalBSpFileName  = theApp->GetProfileString("BSP","FILENAME");
        _bSaveFinalBSP     = theApp->GetProfileInt("BSP","SAVE",0);
        _szipit            = theApp->GetProfileInt("BSP","SZIPIT",0);
        _bSaveImages       = theApp->GetProfileInt("BSP","SAVETEX",1);
        _lmapIntens        = theApp->GetProfileInt("BSP","LMAPINTENS",4);
        _maxFacesLeaf      = theApp->GetProfileInt("BSP","MAXFACES",0);
        _bSaveCats         = theApp->GetProfileInt("BSP","SAVECATS",1);
        _bSavePortals      = theApp->GetProfileInt("BSP","SAVEPORT",1);
        _bPolySortCrit     = theApp->GetProfileInt("BSP","SORTCRIT",0);
        _bMergeNarrowLeafs = theApp->GetProfileInt("BSP","LEAFMERGE",1);
        _bSaveSkyDom       = theApp->GetProfileInt("BSP","SAVESKY",1);
        _bSaveBigTerrain   = theApp->GetProfileInt("BSP","SAVEBIGTERRAIN",1);
        _bSaveBigTerrainBSP= theApp->GetProfileInt("BSP","SAVEBIGTERRAINBSP",1);

        _bAvoidNonOrt      = theApp->GetProfileInt("BSP","AVOIDNONORT",0);
        _bUseUserPriority  = theApp->GetProfileInt("BSP","USEUSERPRIOR",0);



        _bConvexEarly      = theApp->GetProfileInt("BSP","CONVEXEARLY",0);
        _bLightMaps        = theApp->GetProfileInt("BSP","LMAPS",1);
        _lmpowof2          = theApp->GetProfileInt("BSP","LMAPSPOW2",1);
        _lmConst          = theApp->GetProfileInt("BSP","LMAASPECT",0);
        _b3SampleLM        = theApp->GetProfileInt("BSP","LMAPS3S",1);
        _bPvsMethod         = theApp->GetProfileInt("BSP","PVSMETH",1);


        _bGeticBSP         = theApp->GetProfileInt("BSP","GETICBSP",1);
        _bSnapMainBsp      = theApp->GetProfileInt("BSP","SNAPFULLBSP",0);
        _bZones            = theApp->GetProfileInt("BSP","AUTOZONES",0);

        _bCompressPvs      = theApp->GetProfileInt("BSP","COMPRESS",0);
        _bAABBTreeCsgTer   =  theApp->GetProfileInt("BSP","CGSTERAABBTREE",0);


        _removeOnePolyLeaf = theApp->GetProfileInt("BSP","ROPL",0);
        _portalCutFlag     = theApp->GetProfileInt("BSP","PORTCUTBYBOX",0);
        _balance           = theApp->GetProfileInt("BSP","BAL",6);
        _lmapMaxSize       = theApp->GetProfileInt("BSP","LM_XY",16);
        _bLmPatch          = theApp->GetProfileInt("BSP","LM_P",1);
        _bExportFile	   = theApp->GetProfileInt("BSP","EX_S",0);
        _lMapLumelSz       = Str2Real(theApp->GetProfileString("BSP","LMAP_LUMSZ","64.0"),64.0f);





    }
    else
    {
        //theApp->WriteProfileString("BSP","FILENAME",_finalBSpFileName);
        theApp->WriteProfileInt("BSP","SAVE",_bSaveFinalBSP);
        theApp->WriteProfileInt("BSP","SZIPIT",_szipit);
        theApp->WriteProfileInt("BSP","SAVETEX",_bSaveImages);
        theApp->WriteProfileInt("BSP","LMAPINTENS",_lmapIntens);
        theApp->WriteProfileInt("BSP","MAXFACES",_maxFacesLeaf);
        theApp->WriteProfileInt("BSP","SAVECATS",_bSaveCats);
        theApp->WriteProfileInt("BSP","SAVEPORT",_bSavePortals);
        theApp->WriteProfileInt("BSP","SORTCRIT",_bPolySortCrit);
        theApp->WriteProfileInt("BSP","LEAFMERGE",_bMergeNarrowLeafs);
        theApp->WriteProfileInt("BSP","SAVESKY",_bSaveSkyDom);
        theApp->WriteProfileInt("BSP","SAVEBIGTERRAIN",_bSaveBigTerrain);
        theApp->WriteProfileInt("BSP","SAVEBIGTERRAINBSP",_bSaveBigTerrainBSP);

        theApp->WriteProfileInt("BSP","CONVEXEARLY",_bConvexEarly);
        theApp->WriteProfileInt("BSP","LMAPS",_bLightMaps);
        theApp->WriteProfileInt("BSP","LMAPSPOW2",_lmpowof2);
        theApp->WriteProfileInt("BSP","LMAASPECT",_lmConst);



        theApp->WriteProfileInt("BSP","LMAPS3S",_b3SampleLM);

        theApp->WriteProfileInt("BSP","AVOIDNONORT",_bAvoidNonOrt);
        theApp->WriteProfileInt("BSP","USEUSERPRIOR",_bUseUserPriority);


        theApp->WriteProfileInt("BSP","PVSMETH",_bPvsMethod);
        theApp->WriteProfileInt("BSP","GETICBSP",_bGeticBSP);
        //_bGeticBSP         = theApp->GetProfileInt("BSP","GETICBSP",1);

        theApp->WriteProfileInt("BSP","SNAPFULLBSP",_bSnapMainBsp);
        theApp->WriteProfileInt("BSP","AUTOZONES",_bZones);

        theApp->WriteProfileInt("BSP","COMPRESS",_bCompressPvs);
        theApp->WriteProfileInt("BSP","CGSTERAABBTREE",_bAABBTreeCsgTer);


        theApp->WriteProfileInt("BSP","ROPL",_removeOnePolyLeaf);
        theApp->WriteProfileInt("BSP","PORTCUTBYBOX", _portalCutFlag);
        theApp->WriteProfileInt("BSP","BAL",_balance);

        theApp->WriteProfileInt("BSP","LM_XY",_lmapMaxSize);
        theApp->WriteProfileString("BSP","LMAP_LUMSZ",MKSTR("%3.2f",_lMapLumelSz));
        theApp->WriteProfileInt("BSP","LM_P",_bLmPatch);
        theApp->WriteProfileInt("BSP","EX_S",_bExportFile);
    }
}


