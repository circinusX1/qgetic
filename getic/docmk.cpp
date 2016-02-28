// z_ed2Doc.cpp : implementation of the Document class
//

#include "stdafx.h"
#include <algorithm>
#include "geticplug.h"
#include "document.h"
#include "view2d.h"
#include "view3d.h"
#include "lightbulb.h"
#include "geticapp.h"


//---------------------------------------------------------------------------------------
static int32_t	GID=0;

v3d_t GetViewsDims()
{

    View2D* pTX = VIEW2D('z');
    View2D* pTY = VIEW2D('y');
    View2D* pTZ = VIEW2D('x');

    v3d_t	boxdim = v3d_t(  pTZ->_vport.right-pTX->_vport.left,
                      pTX->_vport.bottom-pTZ->_vport.top,
                      pTX->_vport.right-pTX->_vport.left);
    boxdim *= .3f;

    return boxdim;
}
//---------------------------------------------------------------------------------------
// set the flags from setting dialog into the brush
void Document::ApplyFlags(Brush* pBrush)
{
    _docDirty = 1;
    View2D* pTX = VIEW2D('z');
    View2D* pTY = VIEW2D('y');
    View2D* pTZ = VIEW2D('x');
    View3D*  pT3 = VIEW3D(0,0);

    pBrush->Move(v3d_t(pTY->_shiftx, pTZ->_shifty, -pTY->_shifty));
    pBrush->Recalc();

    if(_cachedSett._splitter)
    {
        pBrush->_brushflags = BRSH_SPLITTER;
    }
    else if(_cachedSett._zone)
    {
        pBrush->_brushflags = BRSH_ZONE;
    }
    else
    {
        if(_cachedSett._isSolid) // cut
        {
            pBrush->_brushflags |= BRSH_SOLID;
            if(_cachedSett._detail)
            {
                pBrush->_brushflags |= BRSH_DETAIL;
                if(_cachedSett._detached)
                {
                    pBrush->_brushflags |= BRSH_DETACHED;
                }
            }
        }
        else
        {
            pBrush->FlagFaces(POLY_FROM_CUTB);
            if(_cachedSett._cutAll)
            {
                pBrush->_brushflags |= BRSH_CUTALL;
            }
            pBrush->_brushflags &= ~BRSH_DETAIL;
            pBrush->_brushflags &= ~BRSH_DETACHED;
        }
    }


    if(_cachedSett._reversed)
    {
        pBrush->Reverse();

        if(_cachedSett._skyDom)
        {
            pBrush->_brushprops |= MODEL_IS_SKYDOM;
        }
    }
    pBrush->_group = _curCreatGroup;
    pBrush->_brushflags |= BRSH_NEW;


    if(GDetEnabled && (pBrush->_brushflags & BRSH_SOLID))
    {
        pBrush->Recalc();
        v3d_t ex = pBrush->_box.GetExtends();
        if(ex.getmax() < GMaxDetDim &&
           ex.getmin() < GMinDetDim)
        {
            pBrush->_brushflags |= BRSH_DETAIL;
            if(_cachedSett._detached)
            {
                pBrush->_brushflags |= BRSH_DETACHED;
            }

        }
        else
        {
            pBrush->_brushflags &= ~BRSH_DETAIL;
            pBrush->_brushflags &= ~BRSH_DETACHED;
        }

        if(pBrush->_polys.size() > (u_int32_t)GMinDetPolys)
        {
            pBrush->_brushflags |= BRSH_DETAIL;
        }
    }


    pBrush->Reverse();

    //
    // gens lmap tex cooord and stores initial texture coords
    //
    if(pBrush->_cf._tm == TM_LIN)
    {
        pBrush->ResetTexCoords(-1);	//FOR LIGHT MAP
    }
    pBrush->ResetTexCoords(TX_LM);	//FOR LIGHT MAP

    FOREACH(vvector<Poly>,pBrush->_polys,pPoly)
    {
        Poly& poly = *pPoly;

        if(poly.GetHtex(GUtex) <= 0)
        {
            poly.SetTex(BARDLG()->GetCurSelTexture(), GUtex , BARDLG()->GettexGenFlag());
        }
        FOREACH(vvector<Vtx2>,poly._vtci,pvtci)
        {
            pvtci->_rgb         = ZWHITE;
            pvtci->_uv[TX_1]    = pvtci->_uv[TX_0];
            pvtci->_uv[TX_2]    = pvtci->_uv[TX_0];
        }
    }


    _pSelBrushes.push_back(pBrush); //place in sel brushes witht eh new flag set
    Invalidate();
    pT3->setFocus();
    // assert(0);
    _selmode		= SEL_BRUSH;
    _bShowCsg		= 0;
    _compviewmode   = C_NA;

    _maxUsedGrp     = tmax(_maxUsedGrp, _curCreatGroup);
    // TODO SaveTempFile();
    _brmode = BR_SCALE;
}


//---------------------------------------------------------------------------------------
// makes a box by setings dialog

//---------------------------------------------------------------------------------------
void Document::MkCylinder()
{
    /*
//	OnFileSave();
    OnEscape() ;    //cancel any action because we've begin a new one
    if(_cachedSett._radstrips<0)
        _cachedSett._radstrips=16;
    if(_cachedSett._vstrips<0)
        _cachedSett._vstrips=1;

    _cachedSett._boxdim = GetViewsDims();

    _newBrshDlg._brshSett = _cachedSett;
    _newBrshDlg._hideZone=true;
    if(QMessageBox::Cancel == _newBrshDlg.DoModal())
        return;
    _cachedSett = _newBrshDlg._brshSett;
    _selmode = SEL_BRUSH;

    if(_cachedSett._radstrips<3  || _cachedSett._radstrips>32 )
    {
        TAfxMessageBox("Cylinder Radial Strips are between 3 and 32",MB_OK|MB_ICONINFORMATION);
        return;
    }


    f32_t    rad        = tmin(_cachedSett._boxdim.x,_cachedSett._boxdim.z) / 2;
    f32_t    height     = _cachedSett._boxdim.y;

    Brush* pBrush = new Brush();
    pBrush->MakeCylinder(rad, height, _cachedSett._radstrips, _cachedSett._vstrips, _cachedSett._tm);

    if(_cachedSett._thikness != 0)
    {
        pBrush->StartSecGrp(_cachedSett._thikness);

        Brush tb;
        tb.MakeCylinder(rad-_cachedSett._thikness,
                        height-_cachedSett._thikness, _cachedSett._radstrips, _cachedSett._vstrips, _cachedSett._tm);


        tb.Reverse();
        FOREACH(Polys,tb._polys,pp)
        {
            pp->_pBrush = pBrush;
            pBrush->_polys.push_back(*pp);
        }
    }
    ApplyFlags(pBrush);

    pBrush->_cf._tm      = _cachedSett._tm;
    pBrush->_cf._brushflags   = pBrush->_brushflags;
    pBrush->_cf._hstrips = _cachedSett._radstrips;
    pBrush->_cf._vstrips = _cachedSett._vstrips;
    pBrush->_cf._thick   = _cachedSett._thikness;
    pBrush->_cf._t    = E_CYL;

*/

}


void     Document::InvokePlugIn(const char* dllname, int32_t ncmd)
{
    /*
    char szFilters[MAX_PATH];// = _T(" Ms3dFile (*.ms3d)|*.ms3d|All Files (*.*)|*.*||");
    theApp->ReDir();	

    SaveTempFile();
    _TRY
    {
        PlugInDll<IGeticPlug> plDll(MKSTR("plugins\\%s",dllname));
        if(plDll.IsValid())
        {
            char   menuName[128];

            u_int32_t   typo;
            plDll->GetMenuStringAndType(menuName,&typo);
            if(_ttcschr(menuName,_G(',')))
            {
                char*  pFName = _ttcstok(menuName,",");
                char*  pExt   = _ttcstok(0,",");   ;

                if((ncmd & 0x1)==0)
                {

                    if(!theApp->get_modifiers(Qt::Key_Control))
                        AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_FILE_NEW,0);

                    qDebug(__LINE__,__FILE__);
                }

                _tstprintf(szFilters, _T("%s (*.%s)|*.%s|All Files (*.*)|*.*||"),pFName,pExt,pExt);
                CFileDialog dlg(((ncmd & 0x1)==0) ? true : false, MKSTR(".%s",pExt), 0, OFN_HIDEREADONLY, szFilters);
                dlg.m_ofn.lpstrInitialDir = theApp->DocsDir();
                if (theApp->DoFileDialog(dlg) != IDOK)
                {
                    theApp->ReDir();	
                    return;
                }
                PathHandler ph(dlg.m_ofn.lpstrFile);
                theApp->DocsDir(ph.Path());



                if(typo & PLUG_IMPORTER)
                    START_WAIT("Importing File");
                else
                    START_WAIT("Exporting File");

                AfxGetMainWnd()->Invalidate();
                AfxGetMainWnd()->UpdateWindow();
                _TRY
                {
                    if(typo & PLUG_IMPORTER && (ncmd & 0x1)==0)
                    {
                        ImportFile(plDll, dlg.m_ofn.lpstrFile);
                    }
                    else if ( (ncmd & 0x1) == 1)
                        ExportFile(plDll, dlg.m_ofn.lpstrFile);
                }
                _CATCHX()
                {
                    END_WAIT("");
                    TAfxMessageBox(MKSTR("%s PlugIn Error: %d.",GetLastError(), dllname),MB_OK|MB_ICONWARNING);
                }
                END_WAIT("");

                PathHandler ph2(dlg.m_ofn.lpstrFile);
                theApp->DocsDir(ph2.Path());

                theApp->ReDir();	

                _TRY{
                    theApp->_gEditor.Clear();
                }_CATCHX()
                {
                    TAfxMessageBox("Exception: Please don't delete texture buffers!",MB_OK|MB_ICONWARNING);
                }
            }
            else{
                TAfxMessageBox(MKSTR("'%s' menu string of '%s' Is Not Well Formated ",menuName,dllname),MB_OK|MB_ICONWARNING);
            }
            plDll.Free();

            VIEW3D(0,0)->SetFocus();
            OnEnter();
            OnEscape();
        }
        else
        {
            TAfxMessageBox(MKSTR("'%s' Plug-In Error. Cannot Find Exported Functions",dllname),MB_OK|MB_ICONWARNING);
        }
    }
    _CATCHX()
    {
        TAfxMessageBox(MKSTR("'%s' Plug-In Error. ",dllname),MB_OK|MB_ICONWARNING);
    }
    */

}

void      Document::ExportFile(PlugInDll<IGeticPlug>& plgDll, const char* pszName)
{
    Plg_Scene       scene;
    map<int32_t,int32_t>    texids;
    int32_t             cntTex = 0;

    // store the whole stuff.
    PBrushes* pbRsh = _scene.GetPrimitives();
    scene.nBrushes  = pbRsh->size();
    scene.pBrushes  = new Plg_Brush[scene.nBrushes];

    scene.nTextures = Inl_GetTexCount();
    scene.pTextures = new Plg_Texture[scene.nTextures];

    // texture stuff
    TexSys::iterator bt = theApp->_TexSys.begin();
    TexSys::iterator et = theApp->_TexSys.end();

    for(;bt != et; bt++)
    {
        Texture& tex      = (*bt).second;
        if(!strncmp(tex.filename, "@L",2))
            continue;

        texids[(int32_t)tex.hTex] = cntTex;         // map index with texid

        ::_ttcscpy(scene.pTextures[cntTex].filename, tex.filename);
        scene.pTextures[cntTex].target = tex.hTex.glTarget;
        ++cntTex;
    }

    int32_t brIdx = 0;
    FOREACH(PBrushes, (*pbRsh), ppbb)
    {
        Brush* pB = &(*ppbb)->GetResult();
        Brush* pB0 = *ppbb;

        scene.pBrushes[brIdx].pPolys = 0;
        scene.pBrushes[brIdx].nPolys = 0;

        //Brush* pB = (*ppbb);
        if(pB0->IsCut())
        {
            ++brIdx;
            continue;
        }

        scene.pBrushes[brIdx].flags = pB0->_cf._brushflags;

        if(pB==pB0)
            scene.pBrushes[brIdx].flags |= BRSH_UNCUT;
        else
            scene.pBrushes[brIdx].flags &= ~BRSH_UNCUT;


        SCopy((char*)scene.pBrushes[brIdx].name, pB->_name, sizeof(scene.pBrushes[brIdx].name));
        scene.pBrushes[brIdx].nPolys = pB->_polys.size();
        scene.pBrushes[brIdx].pPolys = new Plg_Poly[scene.pBrushes[brIdx].nPolys];

        // store polygons
        int32_t idxPoly = 0;
        FOREACH(vvector<Poly>,pB->_polys,pPoly)
        {
            Poly&       poly = *pPoly;
            Plg_Poly&   plPl = scene.pBrushes[brIdx].pPolys[idxPoly];

            plPl.flags     = poly._polyflags;
            plPl.combine   = poly.Combine();
            poly.BuildTexAxes(plPl.texax[0], plPl.texax[1]);
            plPl.texIdx[0] = texids[poly.GetHtex(0)];
            plPl.texScale  = v2d_t(poly._texcoord.scalex, poly._texcoord.scaley);
            plPl.texShift  = v2d_t(poly._texcoord.shiftx, poly._texcoord.shifty);
            plPl.nvXes     = poly._vtci.size();
            plPl.vXes      = new v_vunc_t[plPl.nvXes];

            int32_t idxvx = 0;
            FOREACH(vvector<Vtx2>, poly._vtci, pvtci)
            {
                plPl.vXes[idxvx] = *pvtci;
                ++idxvx;
            }
            ++idxPoly;
        }

        ++brIdx;
    }

    _TRY
    {
        plgDll->ExportFile(&theApp->_gEditor, (char*)theApp->InstallDir(), (char*)pszName, &scene);

        if(scene.pTextures)
            delete[] scene.pTextures;

        for(int32_t i=0; i< scene.nBrushes;i++)
        {
            Plg_Brush& b = scene.pBrushes[i];
            for(int32_t j=0; j< b.nPolys;j++)
            {
                Plg_Poly& p = b.pPolys[j];
                delete[] p.vXes;
            }
            delete[]b.pPolys;
        }
        delete[] scene.pBrushes;
    }
    _CATCHX()
    {
        TAfxMessageBox(MKSTR("Error:%d in plugin",GetLastError()),MB_OK|MB_ICONWARNING);
    }

}

static void __CalcTexCoord(Poly& p, v3d_t tax[2], v2d_t& shift, v2d_t& scale, SIZE& bmpSz, int32_t layer)
{
    v3d_t  texO = (shift.u * tax[0]) / tax[0].len();
        texO -= (shift.u * tax[1]) / tax[1].len();

    // fix passed in scalation
    if(scale.u == 0) scale.u = 1;
    if(scale.v == 0) scale.v = 1;

    v3d_t  ss = tax[0] / (f32_t)bmpSz.cx; // xbitmap
    v3d_t  tt = tax[1] / (f32_t)bmpSz.cy; //ybitmap
    f32_t scaleS = ss.len();
    f32_t scaleT = tt.len();
    ss /= scaleS;
    tt /= scaleT;

    for(u_int32_t i=0; i < p._vtci.size() ;i++)
    {
        v3d_t& v  = p._vtci[i]._xyz;
        v2d_t& t  = p._vtci[i]._uv[layer];
        t.u = Vdp((p._vtci[i]._xyz - texO) , ss) * scaleS * scale.u;
        t.v = Vdp((p._vtci[i]._xyz - texO) , tt) * scaleT * scale.v;
    }
}


void      Document::ImportFile(PlugInDll<IGeticPlug>& plgDll, const char* pszdllname)
{

    Plg_Scene* pScene = 0;
    char	   texture[_MAX_PATH];
    int32_t         k;

    QString resDir = theApp->HomeDir();
    resDir += "\\res\\";
    Box     wholething;

    InitDocVars();
    _cachedSett._splitter   = 0;
    _cachedSett._isSolid    = 1;
    _cachedSett._reversed   = 0;
    _cachedSett._cutAll     = 0;
    _cachedSett._skyDom     = 0;
    _cachedSett._splitter   = 0;
    _cachedSett._zone       = 0;
    _cachedSett._detached   = 0;
    _cachedSett._zone       = 0;
    _cachedSett._detail     = 0;
    _cachedSett._reversed   = 0;


    if(0== plgDll->ImportFile(&theApp->_gEditor, (char*)theApp->InstallDir(), (char*)pszdllname, &pScene) && pScene)
    {

        for(k=0;k<pScene->nBrushes;k++)
        {

            ///KEEP_WAIT(MKSTR("Processing Brushes %d", k));

            Plg_Brush*	pBrush = &pScene->pBrushes[k];
            Brush*      pB     = new Brush();


            for(int32_t i=0; i< pBrush->nPolys;i++)
            {
                Poly    p;
                for(int32_t j=0; j < pBrush->pPolys[i].nvXes; j++)
                {
                    Vtx2   vtc;
                    vtc._xyz    = pBrush->pPolys[i].vXes[j]._xyz;
                    vtc._nrm    = pBrush->pPolys[i].vXes[j]._nrm;
                    vtc._pPoly  = &p;
                    int32_t maxtex  = 1;

                    wholething.AddPoint(vtc._xyz);

                    for(int32_t ts = 0; ts < maxtex; ts++)
                    {
                        if(pBrush->pPolys[i].vXes[j]._uv[ts].u != NAF)
                        {
                            vtc._uv[ts]  = pBrush->pPolys[i].vXes[j]._uv[ts];
                        }
                    }
                    p._vtci.push_back(vtc);
                }

                p._pBrush     = pB;
                p._colorD     = u_rgba_t(pBrush->pPolys[i].color);
                p._polyflags  = 0;
                p._polyprops  = 0;


                int32_t ito = 0;
                for(int32_t t=0; t<4; t++)
                {
                    if(t==1)//loght map
                        continue;

                    if(pBrush->pPolys[i].texIdx[t]==-1)
                        continue;


                    u_int32_t tmpVal = pBrush->pPolys[i].texapply[t];

                    p.SetApply(tmpVal,ito);
                    p.SetCombine(pBrush->pPolys[i].combine);

                    if(pBrush->pPolys[i].texIdx[t] >= 0 && pScene->nTextures > pBrush->pPolys[i].texIdx[t])
                    {
                        u_int32_t target = pScene->pTextures[pBrush->pPolys[i].texIdx[t]].target;


                        char* pFileName = (char*)pScene->pTextures[pBrush->pPolys[i].texIdx[t]].filename;
                        if(pFileName[0] != 0)
                        {
                            SCopy(texture, pFileName, sizeof(texture));

                             u_bool32        bWorking = false;
/*
                            CFileFind   finder;



                            if(strchr(texture,'.'))
                                bWorking=finder.FindFile( pFileName);
                            else
                            {
                                if(bWorking=finder.FindFile( MKSTR("%s.jpg",pFileName)))
                                {
                                    SCopy(texture, (char*)MKSTR("%s.jpg",pFileName), sizeof(texture));
                                }
                                else if (bWorking=finder.FindFile( MKSTR("%s.tga",pFileName)))
                                {
                                    SCopy(texture, (char*)MKSTR("%s.tga",pFileName), sizeof(texture));
                                }
                                else if(bWorking=finder.FindFile( MKSTR("%s.bmp",pFileName)))
                                {
                                    SCopy(texture, (char*)MKSTR("%s.bmp",pFileName), sizeof(texture));
                                }
                            }
*/
                            assert(0);

                            if(bWorking)
                            {
                                //finder.Close();

                                PathHandler ph(texture);

//                                CopyFile(texture, MKSTR("%s%s",resDir, ph.File()), 0);
                                assert(0);
                                p.SetTex(ph.File(), ito);
                            }

                        }
                    }
                    ito++;
                }//4 tex stages

                pB->_polys.push_back(p);
            }

            // for the commit
            _cachedSett._zone = 0;
            if(pBrush->flags & BRSH_SOLID)
            {
                _cachedSett._isSolid = 1;
                _cachedSett._cutAll  = 0;
            }
            else
            {
                _cachedSett._isSolid = 0;
                _cachedSett._cutAll  = 1;
            }
            _cachedSett._splitter=0;
            _cachedSett._skyDom=0;

            CommitBrush(pB);
            SCopy(pB->_name, pBrush->name, sizeof(pB->_name));
            pB->_group = 0;
        }

        for(k=0;k<pScene->nItems;k++)
        {
            Plg_Item*	pItem = &pScene->pItems[k];

            CLightBulb* pCurItem = new CLightBulb();


            pCurItem->_t           = pItem->position;
            pCurItem->_colorD.r    = pItem->_U._Light.color[0];
            pCurItem->_colorD.g    = pItem->_U._Light.color[1];
            pCurItem->_colorD.b    = pItem->_U._Light.color[2];
            pCurItem->_radius      = pItem->_U._Light.radius;
            pCurItem->_lmIntensity =  pItem->_U._Light.intensity;

            pCurItem->_flags|= BRSH_NEW;
            pCurItem->_group = _curCreatGroup;

            Pd->AddItem(pCurItem);
            //Pd->OnEnter();

        }

        //plgDll->ReleaseScene(pScene);
    }

    wholething.Expand(256);


    Brush*    pBrush   = new Brush();
    pBrush->MakeCube(wholething.GetExtends(), _cachedSett._tm);
    pBrush->Move(wholething.GetCenter());
    pBrush->_brushflags = BRSH_SOLID|BRSH_NEW;
    pBrush->ResetTexCoords();
    pBrush->Recalc();
    pBrush->_cf._tm             = _cachedSett._tm;
    pBrush->_cf._brushflags     = BRSH_SOLID;
    pBrush->_cf._hstrips        = 0;
    pBrush->_cf._vstrips        = 0;
    pBrush->_cf._thick          = 0;
    pBrush->_cf._t              = E_CUBE;

    _pSelBrushes.push_back(pBrush); //place in sel brushes witht eh new flag set

    _selmode = SEL_NONE;


    _docDirty = 0;
}



void Document::CommitBrush(Brush* pBrush)
{
    if(_cachedSett._splitter)
    {
        pBrush->_brushflags = BRSH_SPLITTER;
    }
    else if(_cachedSett._zone)
    {
        pBrush->_brushflags = BRSH_ZONE;
    }
    else
    {
        if(_cachedSett._isSolid) // cut
        {
            pBrush->_brushflags |= BRSH_SOLID;
            if(_cachedSett._detail)
            {
                pBrush->_brushflags |= BRSH_DETAIL;
            }
        }
        else
        {
            pBrush->FlagFaces(POLY_FROM_CUTB);
            if(_cachedSett._cutAll)
            {
                pBrush->_brushflags |= BRSH_CUTALL;
            }
            pBrush->_brushflags &= ~BRSH_DETAIL;
        }
    }
    if(_cachedSett._reversed)
    {
        pBrush->Reverse();
        if(_cachedSett._skyDom)
        {
            pBrush->_brushprops |= MODEL_IS_SKYDOM;
        }
    }
    pBrush->_group = _curCreatGroup;
    pBrush->_brushflags |= BRSH_NEW;
    pBrush->Reverse();
    pBrush->ResetTexCoords();	//FOR LIGHT MAP
    pBrush->Recalc();
    _pSelBrushes.push_back(pBrush); //place in sel brushes witht eh new flag set
    _selmode		= SEL_BRUSH;
    _bShowCsg		= 0;
    _compviewmode   = C_NA;
    _maxUsedGrp     = tmax(_maxUsedGrp, _curCreatGroup);
}


