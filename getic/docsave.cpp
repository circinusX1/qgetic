// z_ed2Doc.cpp : implementation of the Document class
//

#include "stdafx.h"
#include "texref.h"
#include "document.h"
#include "view2d.h"
#include "view3d.h"
#include <algorithm>
#include "lightbulb.h"
#include "triggeritem.h"
#include "sounditem.h"
#include "gameitem.h"
#include "geticapp.h"
#include "cwaitcursor.h"
#include "progressdlg.h"


#define     CHECK_COUNT(c_,max_,_expsion)    if(c_>max_ ||c_<0)throw _expsion;
int32_t         Version = 0;
//---------------------------------------------------------------------------------------
u_bool32 __FixBadName(char* pszname)
{
    char		invalids[] ="!@#$%^&^*()_+~|}{():\"\'.,<>?/-=\\][";
    char* pinvalid = invalids;
    while(*pinvalid)
    {
        StripChar(pszname,*(pinvalid++));
    }
    return 0;
}

//---------------------------------------------------------------------------------------
void CDocSaver::SerializeDoc(u_bool32 selection)
{
    theApp->ReDir();	

    theApp->_errors.clear();

    if(!this->FileWrap::Open(_filename.c_str(),  _bsave ? "wb": "rb"))
    {
        return;
    }

    CWaitCursor cwc;

    _pTerBrush = 0;
    _selection = (_filename.find(".grs") != -1);
    _pref      = (_filename.find(".gpm") != -1);


    SerializeHeader();

    if(_selection)
    {
        SerializeTextures();
        SerializeBrushes();
        return;
    }

    if(!_pref)
    {
        SerializeScene();
        SerializeSettings();
        SerializeCompSettings();
    }
    test();
    SerializeTextures();
    SerializeBrushes();
    SerializeTerrain();

    if(!_pref)
    {
        SerializeItems();
        SerializeMotions();
        SerializeScripts();
        SerializeCatFiles();
        SerializeDescriptions();

        if(!_bsave)
        {
            Pd->_use2splitidx = tmax(_maxspliterIdx+1, Pd->_use2splitidx);
        }
    }

    Pd->RecreateAmbLmap();

    if(!theApp->_errors.empty())
    {
        char cd[255];
        string all;
        int32_t lines = 0;

        getcwd(cd,254);
        all = "\r\n";
        all += cd;

        all += "\r\nCannot Find: \r\n";
        FOREACH(vvector<string>, theApp->_errors, pps)
        {
            all += *pps;
            all += "\r\n";
            if(++lines>32)
            {
                all += "...";
                break;
            }
        }
        theApp->_errors.clear();
        TAfxMessageBox(all.c_str(), MB_OK|MB_ICONWARNING);
    }
}

//----------------------------------------------------------------------------------------
void CDocSaver::SerializeHeader()
{
    char        szSig[5];

    if(_pref)
        ::strcpy(szSig, GeticPrimitiveFile);
    else
        ::strcpy(szSig, GeticGeometryRawFile);

    this->Serialize(szSig,4);
    if(!_bsave)
    {
        if(!strcmp(szSig,GeticPrimitiveFile))
        {
            _pref=1;
        }
        else
        if(strcmp(szSig,GeticGeometryRawFile))
        {
            throw "Invalid File Signature";
        }
    }
    this->test();
    this->Serialize(_version);
    Version = _version;
    if(_version < DOC_VERSION4)
    {
        throw "Invalid File Version. ";
    }
}

//----------------------------------------------------------------------------------------
void    CDocSaver::SerializeScene()
{
    size_t sz = sizeof(SCENE()._si);
    assert(this->Serialize(SCENE()._si));
    if(!_bsave)
    {
        if(_version < DOC_VERSION5)
        {
            SCENE()._si.fogNear     = 0;
            SCENE()._si.fogFar      = 0;
            SCENE()._si.fogDens     = 0;
            SCENE()._si.fogColor    = ZWHITE;
        }
    }
}

//----------------------------------------------------------------------------------------
void CDocSaver::SerializeSettings()
{
    View2D* pTX = VIEW2D('z');
    View2D* pTY = VIEW2D('y');
    View2D* pTZ = VIEW2D('x');

    this->test();

    this->Serialize(GCarving);
    this->Serialize(pTX->_zoom);
    this->Serialize(pTX->_shifty);
    this->Serialize(pTX->_shiftx);
    this->Serialize(pTY->_zoom);
    this->Serialize(pTY->_shifty);
    this->Serialize(pTY->_shiftx);
    this->Serialize(pTZ->_zoom);
    this->Serialize(pTZ->_shifty);
    this->Serialize(pTZ->_shiftx);
    this->Serialize(Pd->_cam._pos);
    this->Serialize(Pd->_cam._euler);

}

//----------------------------------------------------------------------------------------
void CDocSaver::SerializeCompSettings()
{
    this->Serialize(COMP()->_bSaveFinalBSP);
    this->Serialize(COMP()->_szipit);
    this->Serialize(COMP()->_bSaveImages);
    this->Serialize(COMP()->_maxFacesLeaf);
    this->Serialize(COMP()->_bSaveCats);
    this->Serialize(COMP()->_bSavePortals);
    this->Serialize(COMP()->_bPolySortCrit);
    this->Serialize(COMP()->_bMergeNarrowLeafs);
    this->Serialize(COMP()->_bSaveSkyDom);
    this->Serialize(COMP()->_bLightMaps);
    this->Serialize(COMP()->_b3SampleLM);
    this->Serialize(COMP()->_bAvoidNonOrt);
    this->Serialize(COMP()->_bUseUserPriority);
    this->Serialize(COMP()->_bPvsMethod);
    this->Serialize(COMP()->_bZones);
    this->Serialize(COMP()->_bPvsMethod); // dup
    this->Serialize(COMP()->_removeOnePolyLeaf);
    this->Serialize(COMP()->_portalCutFlag);
    this->Serialize(COMP()->_balance);
    this->Serialize(COMP()->_lmapMaxSize);
    this->Serialize(COMP()->_lMapLumelSz);
    this->Serialize(COMP()->_bLmPatch);
    this->Serialize(COMP()->_bExportFile);
    if(_version >= DOC_VERSION5)
    {
        this->Serialize(COMP()->_bSaveBigTerrain);
        this->Serialize(COMP()->_bSaveBigTerrainBSP);
    }
    if(_version >= DOC_VERSION7)
    {
        this->Serialize(COMP()->_bConvexEarly);
        // additional data added
        this->Serialize(COMP()->_bCompressPvs);
        this->Serialize(COMP()->_bAABBTreeCsgTer);
        this->Serialize(COMP()->_lmpowof2);
        this->Serialize(COMP()->_lmConst);
        this->Serialize(COMP()->_bGeticBSP );
        this->Serialize(COMP()->_bPolySortCrit); // dup
        this->Serialize(COMP()->_bConvexEarly);  //here next used var
        this->Serialize(COMP()->_bConvexEarly);
    }
    if(_version >= DOC_VERSION8)
    {
        int32_t dummy;
        this->Serialize(COMP()->_lmapIntens);
        this->Serialize(dummy); // dummy
        this->Serialize(dummy);
        this->Serialize(dummy);
    }
}


//----------------------------------------------------------------------------------------
void CDocSaver::SerializeTextures()
{
    this->test();
    int32_t nCount = 0;
    if(_bsave)
    {
        //
        // count textures (excluding lightmaps). we dont save lmaps at all
        //

        TexSys::iterator bt = theApp->_TexSys.begin();
        TexSys::iterator et = theApp->_TexSys.end();
        for(;bt != et; bt++)
        {
            Texture& tex      = (*bt).second;
            if(!strncmp(tex.filename, "@L",2))
                continue;
            ++nCount;
        }

        this->Write(nCount);

        int32_t cntTex = 0;

        _texassoc[_dumptex.hTex]  =  -1;
        bt = theApp->_TexSys.begin();
        for(;bt != et; bt++)
        {
            Texture& tex      = (*bt).second; //80 is sieze of T
            if(!strncmp(tex.filename, "@L",2))
            {
                continue;
            }
            _texassoc[tex.hTex.hTex] = cntTex++;      // map index with texid
            this->Write(tex);
        }
        return;
    }

    size_t sz = 0;


    if(_version < DOC_VERSION10)
        sz = sizeof(Fl_TextureOld);
    else
        sz = sizeof(Fl_Texture);

    Fl_Texture        tex_new ;
    Fl_TextureOld     tex ;

    //TODO DANGER SIZE FT_
    this->Read(nCount);
    for(int32_t i=0; i < nCount; i++)
    {
        CHECK_COUNT(nCount,65535,"To many textures: Limit is 65535");

        if(_version < DOC_VERSION10)
        {
            this->Read(tex);
            if(tex.cx==0 ||
               tex.cy==0)
                continue;
            if(tex.cx>=2048 ||
               tex.cy>=2048)
                continue;
            if(_version < DOC_VERSION5)
            {
                tex.hTex.glTarget= 0;
                tex.hTex.envMode = 0;
                tex.hTex.genST   = 0;
            }
            Fl_Texture tex2(tex);
            Texture ttx(tex2);
            _texNames.push_back(ttx);
        }
        else
        {
            this->Read(tex_new);
            if(tex_new.cx==0 ||
               tex_new.cy==0)
                continue;
            if(tex_new.cx>=2048 ||
               tex_new.cy>=2048)
                continue;
            if(_version < DOC_VERSION5)
            {
                tex_new.hTex.glTarget= 0;
                tex_new.hTex.envMode = 0;
                tex_new.hTex.genST   = 0;
            }
            Texture ttx(tex_new);
            _texNames.push_back(ttx);

        }
    }
}

//----------------------------------------------------------------------------------------
void CDocSaver::SerializeBrushes()
{
    if(_bsave)
    {
        PBrushes* brshses = 0 ;
        if(_selection)
            brshses = &Pd->_pSelBrushes;
        else
            brshses = SCENE().GetPrimitives();

        int32_t	nBrushes = brshses->size();
        this->Write(nBrushes);
        FOREACH(PBrushes, (*brshses), ppb)
        {
            Brush* pB = (*ppb);
            pB->Recalc();
            SerializeBrush(pB);
        }
    }
    else
    {
        int32_t	nBrushes;
        if(_pref)
            nBrushes=1;
        else
        {
           this->Read(nBrushes);
        }

        CHECK_COUNT(nBrushes,32768 ,"To many brushes: Limit is 32768");

        for(int32_t ib=0; ib<nBrushes;ib++)
        {
            Brush* pB = new Brush;
            SerializeBrush(pB);

            Pd->AddBrush(pB);

            if(!pB->IsSolid() || pB->IsProjector())
            {
                Pd->SelectBrush(pB);
            }
        }
        ++Brush::GID;
    }
}

//----------------------------------------------------------------------------------------
void CDocSaver::SerializeBrush(Brush* pB)
{
    this->test();

    this->Serialize(pB->_unicID   );
    this->Serialize(pB->_t        );
    this->Serialize(pB->_r        );
    this->Serialize(pB->_s        );
    this->Serialize(pB->_brushflags    );
    this->Serialize(pB->_brushprops    );
    this->Serialize(pB->_physicsA);
    this->Serialize(pB->_rotSteps);
    this->Serialize(pB->_rotSteps);
    this->Serialize(pB->_physicsD);
    size_t sss = sizeof(pB->_cf);
    this->Serialize(pB->_cf);              // creation flags
    this->Serialize(pB->_polySec);
    this->Serialize(pB->_thikness);
    memset(pB->_name, 0, sizeof(pB->_name));
    this->Serialize(pB->_name, 16);
    this->Serialize(pB->_group    );

    if(_bsave)
    {
        int32_t motionIdx = SCENE().GetIndex(pB->_pMotion);
        this->Write(motionIdx);

        int32_t nCountPolys = pB->_polys.size();
        this->Write(nCountPolys);

        FOREACH(Polys, pB->_polys, pp)
        {
            SerializePolygon(pB, &(*pp));
        }
    }
    else
    {
        pB->_group += Pd->_maxUsedGrp;
        int32_t motionIdx ;
        this->Read(motionIdx);
        if(motionIdx>=0)	// for redoing the pointer in the motion object
        {
            _dynaBrushes[pB->_unicID] = pB;	// store this brush here, process later
        }
        pB->_pMotion = 0;

        int32_t     nCountPolys ;

        this->Read(nCountPolys );
        CHECK_COUNT(nCountPolys,1024, MKSTR("To many polygons per brush %s: Limit is 1024",pB->_name));
        for(int32_t vx=0; vx < nCountPolys ; vx++)
        {
            do{
                Poly	pl;
                SerializePolygon(pB, &pl);
                pB->_polys << pl;
                pl.Clear();
            }while(0);
        }
        if(pB->_brushflags & BRSH_BIGTERRAIN)
        {
            _pTerBrush = pB;
        }
        pB->Dirty(1);
        pB->Recalc(1);
        Brush::GID = tmax(Brush::GID, pB->_unicID);
    }
    KEEP_WAIT(MKSTR("Processing Brush %d ",pB->_unicID));
}

//----------------------------------------------------------------------------------------

void CDocSaver::SerializePolygon(Brush* pB, Poly* pp)
{
    for(int32_t k=0;k<4;k++)
    {
        if(_bsave)
        {
            map<int32_t, int32_t>::iterator fi = _texassoc.find((int32_t)pp->GetHtex(k));
            if(fi != _texassoc.end())
            {
                int32_t fisec = fi->second;
                this->Write(fisec);
            }
            else
            {
                int32_t it = -1;
                this->Write(it);
            }
            this->Write(pp->Apply(k));
        }
        else
        {
            int32_t texID, applyTex;

            this->test();

            this->Read(texID);
            this->Read(applyTex);

            if(texID >=0 && (u_int32_t)texID < _texNames.size())
            {
                pp->SetApply(applyTex,k);
                pp->SetTex(_texNames[texID].filename, k, _texNames[texID].hTex.glTarget);

                if(_dumptex == pp->GetHtex(k))
                {
                    string serr = _texNames[texID].filename;
                    theApp->_errors.push_unique(serr);
                }
            }
        }
    }
    u_int32_t utempval = pp->Combine();
    this->Serialize(utempval);
    this->Serialize(pp->_polyflags);
    this->Serialize(pp->_polyflags2);
    this->Serialize(pp->_polyprops);
    this->Serialize(pp->_use2splitidx);
    this->Serialize(pp->_texcoord );
    this->Serialize(pp->_lmInfo);
    this->Serialize(pp->_colorD);
    this->Serialize(pp->_colorE);
    this->Serialize(pp->_colorS);
    this->Serialize(pp->_shiness);
    this->Serialize(pp->_tanimA);
    this->Serialize(pp->_tanimB);
    this->Serialize(pp->_texV1);
    this->Serialize(pp->_texV2);

    this->Serialize(pp->_bump);
    this->Serialize(pp->_friction);

    if(_bsave)
    {
        //TODO DANGER SIZE FT_
        int32_t  nCount = (*pp)._vtci.size();
        this->Write(nCount);
        FOREACH(vvector<Vtx2>, pp->_vtci, pv)
        {
            this->Write((*pv));//sizeof is 96
        }
    }
    else
    {
        _maxspliterIdx = tmax(_maxspliterIdx, pp->_use2splitidx);
        pp->_pBrush = pB;


        int32_t  nVxes = 0;
        this->Read(nVxes);

        CHECK_COUNT(nVxes,128, MKSTR("Too many vertexes per polygon: %s",pB->_name));
        Fl_Vtx2_Old vtx_old;
        Fl_Vtx2 vtx;

        size_t sznew = sizeof(vtx);

        for(int32_t v=0; v < nVxes; v++)
        {
            if(_version < DOC_VERSION10)
            {
                this->Read(&vtx_old, sizeof(vtx_old)); //84

                Fl_Vtx2 new_vx(vtx_old);
                Vtx2 fvtx(new_vx);
                pp->_vtci << fvtx;
            }
            else
            {
                this->Read(&vtx, sizeof(vtx));
                Vtx2 fvtx(vtx);
                pp->_vtci << fvtx;
            }
        }
        pp->Recalc();
    }
}

//---------------------------------------------------------------------------------------
void CDocSaver::SerializeItems()
{
    int32_t nItemsCount;
    if(_bsave)
    {
        nItemsCount = Pd->GetItems()->size();
        this->Serialize(nItemsCount);
        FOREACH(vvector<SceItem*>, (*Pd->GetItems()), ppi)
        {
            SceItem* pi = *ppi;
            SerializeItem(pi);
        }
    }
    else
    {
         this->Serialize(nItemsCount);
         CHECK_COUNT(nItemsCount,2048,"Too many items per scene: Limit is 2048");

         for(int32_t vx=0; vx < nItemsCount ; vx++)
         {
             SceItem* pi = SerializeItem(0);

             SceItem::_GItemID = tmax(SceItem::_GItemID, (int32_t)pi->_id);

             pi->_flags &= ~BRSH_NEW;
             pi->Commit();
             SCENE().AddItem(pi);
             BARDLG()->AddItem(pi);
         }
         ++SceItem::_GItemID;
    }
}

//----------------------------------------------------------------------------------------
SceItem* CDocSaver::SerializeItem(SceItem* pi)
{
    if(!pi) //load
    {
        int32_t itemItem;

        this->Read(itemItem);
        pi = SceItem::CreateSci(itemItem);
    }
    else
    {
        this->Write(pi->_item);
    }
    pi->Serialize(this, _texNames, _texassoc);
    return pi;
}

//----------------------------------------------------------------------------------------
void CDocSaver::SerializeMotions()
{
    if(_bsave)
    {
        int32_t nCount = SCENE().GetMotions()->size();
        this->Write(nCount);

        FOREACH(vvector<CMotion*>,  (*SCENE().GetMotions()), ppMotion)
        {
            CMotion* pM = *ppMotion;

            this->Write(pM->_bLoop);
            this->Write(pM->_center);
            this->Write(pM->_pBrush->_unicID);

            int32_t nCount = pM->GetNodesCount();
            this->Write(nCount);

            for(int32_t i=0; i<nCount; i++)
            {
                MState* pS = pM->GetState(i);

                this->Write(pS->_pos);
                this->Write(pS->_euler);
                this->Write(pS->_trTimes[0]);
                this->Write(pS->_trTimes[1]);
                this->Write(pS->_trRots[0]);
                this->Write(pS->_trRots[1]);
            }
        }
    }
    else
    {
        int32_t nCount = 0,nIndex=0;
        this->Read(nCount);

        CHECK_COUNT(nCount,128,"Too many motions: Limit is 128");

        for (int32_t i=0;i<nCount;i++)
        {
            CMotion* pM = new CMotion(0, 0, 0);

            this->Read(pM->_bLoop);
            this->Read(pM->_center);
            //
            // link them back
            //
            this->Read(nIndex);					// uuid of brush
            pM->_pBrush = _dynaBrushes[nIndex];	// take it from array
            if(pM->_pBrush)
            {
                pM->_pBrush->_pMotion = pM;			// link back
            }
            this->Read(pM->_iNodes);
            for(int32_t i=0; i < pM->_iNodes; i++)
            {
                MState  state;

                this->Read(state._pos);
                this->Read(state._euler);
                this->Read(state._trTimes[0]);
                this->Read(state._trTimes[1]);
                this->Read(state._trRots[0]);
                this->Read(state._trRots[1]);
                pM->_states << state;
            }
            if(pM->_pBrush)
                SCENE().AddMotion(pM);
            else
                delete pM;

        }
    }
}

//----------------------------------------------------------------------------------------
void CDocSaver::SerializeScripts()
{
    if(_bsave)
    {
        int32_t tmpVal = 0;
        int32_t nCount = SCENE().GetScripts()->size();

        this->Write(nCount);
        FOREACH(vvector<ScriptItem*>,  (*SCENE().GetScripts()), ppScript)
        {
            ScriptItem*	pSi = *ppScript;

            tmpVal = pSi->_name.length();
            this->Write(tmpVal);
            this->Write((void*)pSi->_name.c_str(),pSi->_name.length());

            tmpVal = pSi->_script.length();
            this->Write(tmpVal);
            this->Write((void*)pSi->_script.c_str(), pSi->_script.length());
        }
    }
    else
    {
        int32_t     nCount = 0;
        int32_t     nLength = 0;
        char   szBuff[_MAX_PATH];
        char*   pszScr;

        this->Read(nCount);
        CHECK_COUNT(nCount,256, "Too many scipt items: Limit is 256");
        for(int32_t i=0;  i<nCount; i++)
        {
            ScriptItem* pSci = new ScriptItem();

            // script file name
            this->Read(nLength);
            memset(szBuff,0,sizeof(szBuff));
            this->Read(szBuff, nLength);
            pSci->_name = szBuff;

            // script content
            this->Read(nLength);
            pszScr = new char[nLength];
            if(pszScr)
            {
                memset(pszScr,0,sizeof(nLength));
                this->Read(pszScr, nLength);
                pSci->_script = pszScr;
                SCENE().AddScript(pSci);
                delete[] pszScr;
            }
            else
            {
                delete pSci;
            }
        }
    }
}

//----------------------------------------------------------------------------------------
void CDocSaver::SerializeCatFiles()
{
    char sFileName[_MAX_PATH] = {0};
    if(_bsave)
    {
        if(theApp->XsdCatItmFile()[0])
            strcpy(sFileName, theApp->XsdCatItmFile());
        this->Write((void*)sFileName,_MAX_PATH);
    }
    else
    {
        this->Read((void*)sFileName,_MAX_PATH);
        if(sFileName[0])
        {
            if(SCENE().SearchCats((char*)sFileName))
            {
                theApp->XsdCatItmFile((char*)sFileName);
            }
        }
    }
}

//----------------------------------------------------------------------------------------
void CDocSaver::SerializeTerrain()
{
    CBigTerrain& terr = SCENE().Terrain();

    if(_bsave)
    {
        terr.Serialize(this, _texNames, _texassoc);
    }
    else if(_version >= DOC_VERSION5) // load these sections
    {
        terr.Serialize(this, _texNames, _texassoc);
        terr.p_dummyBrush = _pTerBrush;
        if(_pTerBrush)
            _pTerBrush->_pUsrData = &terr;
    }
}

//----------------------------------------------------------------------------------------
void CDocSaver::SerializeDescriptions()
{
    BTF_Details             detcell;
    int32_t                     ncount  = 0;
    map<string ,string>&    descs   = SCENE().Descriptions();

    if(_bsave)
    {
        ncount = descs.size();
        this->Write(ncount);
        for(map<string ,string>::iterator b = descs.begin(); b != descs.end(); b++)
        {
            SCopy(detcell.name, (char*)(*b).first.c_str(), sizeof(detcell.name));
            detcell.length = (*b).second.length();
            this->Write(detcell);
            this->WriteTextAsIs((*b).second.c_str());
        }
    }
    else if(_version >= DOC_VERSION5)// load these sections
    {
        char  tmp[16384];
        this->Read(ncount);
        CHECK_COUNT(ncount,1024, "Too many detail descriptors. Limit is 1024");
        for(int32_t i=0; i < ncount; i++)
        {
            this->Read(detcell);
            this->Read(tmp, detcell.length);
            tmp[detcell.length] = 0;
            descs[detcell.name] = tmp;
        }
    }
}


//----------------------------------------------------------------------------------------
void      Document::SavePrefFile()
{
/*
    theApp->ReDir();	
    DlgCpxPrimnames d;
    if(d.DoModal() == IDOK)
    {
        theApp->ReDir();	
        CDirChange  cdd(d._curPath.c_str());
        cdd.UseObj();

        PBrushes* pBrushes = _scene.GetBrushes();

        FOREACH(PBrushes, (*pBrushes), ppCpxBrush)
        {
            do{
                Brush brush = *(*ppCpxBrush);
                brush.Recalc();

                v3d_t  center = brush._box.GetCenter();
                brush.Move(-center);

                char locName[64];
                _tcscpy(locName, brush._name);
                if(__FixBadName(locName))
                    continue;

                CDocSaver   ds(MKSTR("%s.gpm",locName), true, true);

                if(ds.Open())
                {
                    ds.SerializeHeader();
                    ds.SerializeTextures();
                    ds.SerializeBrush(&brush);
                    ds.Close();
                }

            }while(0);
        }
    }
    (VIEW3D(0,0))->SetFocus();
    */

}

//----------------------------------------------------------------------------------------
u_bool32 Document::LoadPrefFile(const char* szFileName, Brush** pBRet)
{
    Brush*			  pB = 0;

    CDirChange  cdd("");
    cdd.UseObj();

    CDocSaver   ds(szFileName, false); // load, pref

    try
    {
        if(ds.Open())
        {
            ds.SerializeHeader();
            if(ds.IsPerf())
            {
                ds.SerializeTextures();

                if(pB = new Brush())
                {
                    do{
                        CDirChange cdc("");
                        theApp->ReDir();	
                        ds.SerializeBrush(pB);
                        pB->_unicID = Brush::GID++;
                    }while(0);

                    pB->_pMotion=0;

                    if(!(pB->_brushflags & BRSH_SOLID) &&
                       !pB->IsProjector())
                    {
                        SelectBrush(pB);
                        Recut();
                    }
                }
            }
            ds.Close();
        }
    }
    catch(const string ex)
    {
        TAfxMessageBox(ex.c_str(),MB_OK|MB_ICONHAND);
        delete pB;
        pB = 0;
    }
    catch(...)
    {
        TAfxMessageBox("File Is Malformated or file version is too old.",MB_OK|MB_ICONHAND);
        delete pB;
        pB = 0;
    }
    if(pBRet)
    {
        *pBRet = pB;
    }
    return pB!=0;
}

//----------------------------------------------------------------------------------------
class NO_VT VRMLWritter
{
public:
    VRMLWritter(FileWrap* pfw):_pfw(pfw),_tabs(0){}
    void Write(const char* p,...)
    {
        va_list args;
        va_start(args, p);
        ::snprintf(_s, sizeof(_s) / sizeof(char), p, args);
        va_end(args);

        for(int32_t i=0; i<_tabs; i++)
            _pfw->WriteTextAsIs("   ");
        _pfw->WriteTextAsIs(_s);
        _pfw->WriteTextAsIs("\r\n");
    }

    void Tab(){
        for(int32_t i=0; i<_tabs; i++)
            _pfw->WriteTextAsIs("   ");
    }
    void WriteValNoCR(const char* p,...)
    {
        va_list args;
        va_start(args, p);
        ::snprintf(_s, sizeof(_s) / sizeof(char), p, args);
        va_end(args);
        _pfw->WriteTextAsIs(_s);
    }


    void WriteValue(const char* sect, char* p)
    {
      for(int32_t i=0; i<_tabs; i++)
            _pfw->WriteTextAsIs("   ");

        _pfw->WriteTextAsIs(MKSTR("%s\t",sect));
        _pfw->WriteTextAsIs(p);
        _pfw->WriteTextAsIs("\r\n");
    }

    void WriteValue(const char* sect)
    {
      for(int32_t i=0; i<_tabs; i++)
            _pfw->WriteTextAsIs("   ");

        _pfw->WriteTextAsIs(MKSTR("%s\r\n",sect));
    }


    void WriteValue(const char* sect, v2d_t& f)
    {
        for(int32_t i=0; i<_tabs; i++)
            _pfw->WriteTextAsIs("   ");

        _pfw->WriteTextAsIs(MKSTR("%s\t",sect));
        _pfw->WriteTextAsIs(MKSTR("%f %f", f.u,f.v));
        _pfw->WriteTextAsIs("\r\n");
    }

    void WriteValue(const char* sect, u_rgba_t& f)
    {
        for(int32_t i=0; i<_tabs; i++)
            _pfw->WriteTextAsIs("   ");

        _pfw->WriteTextAsIs(MKSTR("%s\t",sect));
        _pfw->WriteTextAsIs(MKSTR("%d ", f.r));
        _pfw->WriteTextAsIs(MKSTR("%d ", f.g));
        _pfw->WriteTextAsIs(MKSTR("%d ", f.b));
        _pfw->WriteTextAsIs(MKSTR("%d",  f.a));
        _pfw->WriteTextAsIs("\r\n");

    }
    void WriteValue(const char* sect, v3d_t& f)
    {
        for(int32_t i=0; i<_tabs; i++)
            _pfw->WriteTextAsIs("   ");

        _pfw->WriteTextAsIs(MKSTR("%s\t",sect));
        _pfw->WriteTextAsIs(MKSTR("%f ", f.x));
        _pfw->WriteTextAsIs(MKSTR("%f ", f.y));
        _pfw->WriteTextAsIs(MKSTR("%f",  f.z));
        _pfw->WriteTextAsIs("\r\n");
    }


    void WriteValue(const char* sect, u_int32_t f)
    {
        WriteValue(sect, (int32_t) f);
    }

    void WriteValue(const char* sect, u_int64_t f)
    {
        WriteValue(sect, (u_int64_t) f);
    }

    void WriteValue(const char* sect, int32_t f)
    {
        for(int32_t i=0; i<_tabs; i++)
            _pfw->WriteTextAsIs("   ");
        _pfw->WriteTextAsIs(MKSTR("%s\t",sect));
        _pfw->WriteTextAsIs(MKSTR("%d", f));
        _pfw->WriteTextAsIs("\r\n");
    }


    void WriteValue(const char* sect, float f)
    {
        for(int32_t i=0; i<_tabs; i++)
            _pfw->WriteTextAsIs("   ");

        _pfw->WriteTextAsIs(MKSTR("%s\t",sect));
        _pfw->WriteTextAsIs(MKSTR("%f", (float)f));
        _pfw->WriteTextAsIs("\r\n");
    }


    void Open(char* p)
    {
        int32_t i;
        for( i=0; i<_tabs; i++)
           _pfw->WriteTextAsIs("   ");
        _pfw->WriteTextAsIs(p);
        _pfw->WriteTextAsIs("\r\n");
        for(i=0; i<_tabs; i++)
           _pfw->WriteTextAsIs("   ");
        _pfw->WriteTextAsIs("{\r\n");
        ++_tabs;
    }
    void Close()
    {
        --_tabs;
        _pfw->WriteTextAsIs("\r\n");
        for(int32_t i=0; i<_tabs; i++)
            _pfw->WriteTextAsIs("   ");
        _pfw->WriteTextAsIs("}\r\n");
    }
private:
    char       _s[512];
    FileWrap*   _pfw;
    int32_t         _tabs;
};


struct XmlFace{
    XmlFace():vxi(4),tc1i(4),tc2i(4),tc3i(4){}
    vvector<int32_t>     vxi;
    vvector<int32_t>     tc1i;
    vvector<int32_t>     tc2i;
    vvector<int32_t>     tc3i;
    int32_t              clri[3]; //asde
    Poly*            pPoly;
};


typedef struct _GUID {
  u_int32_t Data1;
  u_int16_t  Data2;
  u_int16_t  Data3;
  u_int8_t  Data4[8];
} GUID;

//---------------------------------------------------------------------------------------
void Document::SaveInXML(const char* pSzName, u_bool32 bBrshes)
{
    map<int32_t,int32_t>    texids;
    int32_t             idx = 0;
    FileWrap        fw;
    int32_t             fileVer   = DOC_VERSION5;
    int32_t             cntTex=0;
    GUID            g = {0};


    CWaitCursor cwc;
    theApp->ReDir();	
    BARDLG()->OnApply() ;
    fw.Open(pSzName,"wb");

    VRMLWritter  w(&fw);

    w.Open((char*)MKSTR("getic #%d",DOC_VERSION10));
    w.Open("level");
        if(_scene._si.sceneSpash>=0)
            w.WriteValue(MKSTR("splash '%s'",_scene._si.sceneSpash));
        w.WriteValue("author", _scene._si.author);
        w.WriteValue("comments",_scene._si.comments);
        w.WriteValue("leveltype",_scene._si.levelType);
        w.WriteValue("actors",_scene._si.maxActors);
        w.WriteValue("fargs",_scene._si.maxFrags);
        w.WriteValue("time",_scene._si.maxTime);
    w.Close();
    w.Open("scene");
        w.WriteValue("coordsystem","left");

        if(GMeters	== 1)
            w.WriteValue("measureunit", "centimeter radians");
        else if(GMeters	== 100)
            w.WriteValue("measureunit", "meter radians");
        else
            w.WriteValue("measureunit", "feet radians");
        w.WriteValue("globallight",_scene._si.defLumin);
        w.WriteValue("gravitation",-(int32_t)_scene._si.gravAcc);
        if(_scene._si.fogNear ||_scene._si.fogFar)
        {
            w.Open("fog");
                w.WriteValue("near",_scene._si.fogNear);
                w.WriteValue("far",_scene._si.fogFar);
                w.WriteValue("color",_scene._si.fogColor);
                w.WriteValue("density",_scene._si.fogDens);
                w.WriteValue("falloff",_scene._si.fogFallOff);
            w.Close();
        }
        w.Open("camera");
            w.WriteValue("position", _cam._pos);
            w.WriteValue("orientation", _cam._euler);
            w.WriteValue("focalDistance", 8.0f);
            w.WriteValue("heightAngle", _scene._si.camFov);
        w.Close();

        w.Open("textures");
            texids[0] = 0;         // map index with texid
            TexSys::iterator bt = theApp->_TexSys.begin();
            TexSys::iterator et = theApp->_TexSys.end();
            for(;bt != et; bt++)
            {
                Texture& tex      = (*bt).second;
                texids[tex.hTex.hTex] = cntTex++;
                if(! strncmp(tex.filename, "@L", 2))
                {
                    continue;
                }
                w.Write("filename '%s' %d %d %d %x", tex.filename, tex.cx, tex.cy, tex.bpp, tex.hTex.glTarget);
            }
        w.Close();

        vvector<XmlFace>    faces(4096);
        vvector<u_rgba_t>        colors(4096);
        vvector<v3d_t>         vxes(16384);
        vvector<v2d_t>         tcs(8912);
        u_int32_t                k;

        PBrushes* brshses = _scene.GetPrimitives();
        FOREACH(PBrushes, (*brshses), ppb)
        {
            Brush* pB = (*ppb)->_pResult;
            FOREACH(Polys, pB->_polys, pp)
            {
                XmlFace xf;
                xf.pPoly = (Poly*)&(*pp);

                k = colors.push_unique(pp->_colorD);
                xf.clri[0] =  k;
                k = colors.push_unique(pp->_colorS);
                xf.clri[1] =  k;
                k = colors.push_unique(pp->_colorE);
                xf.clri[2] =  k;
                FOREACH(vvector<Vtx2>, pp->_vtci, pv)
                {
                    vxes.push_unique((pv)->_xyz,&k);
                    xf.vxi  << k;
                    tcs.push_unique((pv)->_uv[0],&k);
                    xf.tc1i <<  k;
                    tcs.push_unique((pv)->_uv[1],&k);
                    xf.tc2i << k;
                    tcs.push_unique((pv)->_uv[2],&k);
                    xf.tc3i <<  k;
                }
                faces << xf;
            }
        }

    w.Open("points3Df");
    FOREACH(vvector<v3d_t>, vxes, pv3)
    {
        w.WriteValue("", *pv3); // index
    }
    w.Close();

    w.Open("points2Df");
        FOREACH(vvector<v2d_t>, tcs, puv)
        {
            w.WriteValue("", *puv); // index
        }
    w.Close();

    w.Open("colrs4ub");
        FOREACH(vvector<u_rgba_t>, colors, pcrl)
        {
            w.WriteValue("", *pcrl); // index
        }
    w.Close();

    w.Open("faces");
    int32_t index=0;
    FOREACH(vvector<XmlFace>, faces, pxmlf)
    {
        w.Open((char*)MKSTR("%d", index++));

            if(pxmlf->pPoly->_polyflags || pxmlf->pPoly->_polyflags2)
                w.Write("flags %x %x %x ", pxmlf->pPoly->_polyprops, pxmlf->pPoly->_polyflags, pxmlf->pPoly->_polyflags2);
            w.Write("textureindex %d", texids[pxmlf->pPoly->GetHtex(0)]);
            if(pxmlf->pPoly->Apply(0))
                w.Write("texapplymodes %x", pxmlf->pPoly->Apply(0));
            w.Write("colorindex %d %d %d",pxmlf->clri[0], pxmlf->clri[1], pxmlf->clri[2]);

            if(pxmlf->pPoly->_shiness)
                w.Write("shiness %d",pxmlf->pPoly->_shiness);

            if(pxmlf->pPoly->_bump!=1)
                w.Write("physics %f %f",pxmlf->pPoly->_bump, pxmlf->pPoly->_friction);
            if(pxmlf->pPoly->_use2splitidx!=-1)
                w.WriteValue("splitter",pxmlf->pPoly->_use2splitidx);


            w.Open("vertexes");
                vvector<int32_t>::iterator ivx   = pxmlf->vxi.begin();
                vvector<int32_t>::iterator itc1i = pxmlf->tc1i.begin();
//                vvector<int32_t>::iterator itc2i = pxmlf->tc2i.begin();
//                vvector<int32_t>::iterator itc3i = pxmlf->tc3i.begin();
                w.Tab();
                for(;ivx != pxmlf->vxi.end();ivx++, itc1i++)//, itc3i++, itc2i++)
                {
                    w.WriteValNoCR("[%d %d] ",*ivx, *itc1i);//, *itc2i, *itc3i);
                }
            w.Close();

        w.Close();
    }
    w.Close();

    w.Open("brushes");
    vvector<XmlFace>::iterator xmlfi = faces.begin();
    int32_t fIdx=0;
    FOREACH(PBrushes, (*brshses), ppb)
    {

        Brush* pB = (*ppb)->_pResult;

        pB->_name[15] = 0;
        w.Open(pB->_name);

        w.Write("flags %x %x",pB->_brushflags, pB->_brushprops);
        if(pB->_group)
            w.Write("group %d",pB->_group);
        int32_t idxMotion = _scene.GetIndex(pB->_pMotion);
        if(idxMotion!=-1)
            w.WriteValue("motionsindex",idxMotion);
            w.Open("faceindexes");
                idx = 0;
                w.Tab();
                FOREACH(Polys, pB->_polys, pp)
                {
                    w.WriteValNoCR("%d ",fIdx++);
                }
            w.Close();
        w.Close();
    }
    w.Close();

    w.Open("items");
    {
        FOREACH(vvector<SceItem*>, (*GetItems()), pItemPtr)
        {
            SceItem* pItem = *pItemPtr;
            pItem->_name[24] = 0;

            w.Open(pItem->_name);
            {

                w.WriteValue("type",pItem->_item);
                w.WriteValue("group",pItem->_group);
                w.WriteValue("category",(char*)pItem->_catname);
                w.Write("flags %x %x",pItem->_props, pItem->_flags);
                w.WriteValue("color4ub",pItem->_colorD);
                w.WriteValue("position",pItem->_t);
                w.WriteValue("scalation",pItem->_s);
                w.WriteValue("orientation",pItem->_euler);
                w.WriteValue("texture", (char*)MKSTR("%d %d",texids[pItem->GetHtex(0)],pItem->Apply(0)));
                switch(pItem->_item)
                {
                    default:
                        break;
                    case ITM_LIGTBULB:

                         w.WriteValue("specularangle",((CLightBulb*)pItem)->_specAngle);
                         w.WriteValue("intensity",((CLightBulb*)pItem)->_lmIntensity);
                         w.WriteValue("maximradius",((CLightBulb*)pItem)->_radius);
                         w.WriteValue("minimhallo",((CLightBulb*)pItem)->_halloRadMin);
                         w.WriteValue("maximhallo",((CLightBulb*)pItem)->_halloRadMax);
                        break;
                    case ITM_TRIGER:
                        {
                            u_int32_t   ptr = -1;
                            v3d_t ex = ((TriggerItem*)pItem)->_drawBrush._box.GetExtends();
                            w.WriteValue("effectpoint",((TriggerItem*)pItem)->_efectPoint);
                            w.WriteValue("extends",ex);
                        }
                        break;
                }
                w.Close();
            }
        }
        w.Close();
    }

    w.Open("motions");
    {
        FOREACH(vvector<CMotion*>,  (*_scene.GetMotions()), ppMotion)
        {
            CMotion* pM = *ppMotion;

            w.WriteValue("closed",  pM->_bLoop);
            w.WriteValue("center",pM->_center);
            w.WriteValue("brushindex", pM->_pBrush->_unicID);
            w.Open("path");
            {
                for(int32_t i=0;i<pM->GetNodesCount();i++)
                {
                    MState* pS = pM->GetState(i);

                    w.WriteValue("position", pS->_pos);
                    w.WriteValue("orientation", pS->_euler);
                }
                w.Close();
            }
        }
        w.Close();
    }

    w.Open("scripts");
    FOREACH(vvector<ScriptItem*>,  (*_scene.GetScripts()), ppScript)
    {
        ScriptItem*	pSi = *ppScript;
        w.Open("script");
        {
            fw.Write((void*)pSi->_script.c_str(),pSi->_script.length());
            w.Close();
        }
    }
    w.Close();

}

//---------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
void Document::ReadFromFile(const char* szFilename)
{
    Invalidate();
    //theApp->PumpMessage(WM_PAINT,10);

    char        sigFile[5] = {0};
    FileWrap    fw;

    if(!fw.Open(szFilename,"rb"))
    {
        TAfxMessageBox(MKSTR("Cannot Open The File: '%s'. Error: %d ", szFilename, GetLastError()),MB_OK|MB_ICONHAND);
        return;
    }

    fw.Read(sigFile,4);
    if(strcmp(sigFile,"GGRF") && strcmp(sigFile,"GPGF"))
    {
        fw.Close();
        TAfxMessageBox("Current File Is Not a Getic Geometry File",MB_OK|MB_ICONHAND);
        return;
    }
    fw.Read(i_geomfilever);
    fw.Close();

    if(i_geomfilever == DOC_VERSION3 || i_geomfilever == DOC_VERSION2)
    {
        TAfxMessageBox("Very old files are not longer suported...",MB_OK|MB_ICONHAND);
        return ;
    }
    else if(i_geomfilever >= DOC_VERSION4)
    {
        START_WAIT("Please Wait...");

        CDocSaver cfv(szFilename, false);

        try{
            GCur g(VIEW3D(0,0));

            cfv.SerializeDoc(0);
            Recut();
            DeselectAll();
            //(VIEW3D(0,0))->SetFocus();
            END_WAIT("");
            //AfxGetMainWnd()->SendMessage(WM_COMMAND, PB_REFR);
            Pd->Invalidate();
            return;
        }catch(const char* ex)
        {
            END_WAIT("");
            TAfxMessageBox(ex,MB_OK|MB_ICONHAND);
        }
        catch(...)
        {
            END_WAIT("");
            TAfxMessageBox("Unknkown Exception. File is malformated",MB_OK|MB_ICONHAND);
        }
    }
    else
    {
        TAfxMessageBox("Error Reading File. Version Not Longer Supported",MB_OK|MB_ICONHAND);
    }
    InitDocVars();
    //(VIEW3D(0,0))->SetFocus();
    //AfxGetMainWnd()->SendMessage(WM_COMMAND, PB_REFR);
    Pd->Invalidate();
}

void Document::ReadFromBSP(const char* szFilename)
{

}

