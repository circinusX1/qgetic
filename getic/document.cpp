#include "stdafx.h"
#include "winwrap.h"
#include <baseutils.h>
#include <sys/stat.h>
#include "texadapter.h"
#include "document.h"
#include "view3d.h"
#include "view3d.h"
#include <algorithm>
#include "lightbulb.h"
#include "motion.h"
#include "triggeritem.h"
#include "geticapp.h"
#include "progressdlg.h"
#include "cwaitcursor.h"
#include "bigterrain.h"
#include "geticapp.h"
#include <QFileDialog>
//


Document*  Pd;

Document::Document()
{
    Pd=this;
    _frmTimer     = 0;
    _complBreaking = false;
    _blocksel      = false;
    _lastFocusView = 0;
    _clearing = false;
    _hrc = 0;
    _curgroup      = -1; //all
    _maxUsedGrp    = 0;
    _curCreatGroup = 0;
    _cmdMsg		   = 0;
    GUtex      = 0;
    _userBool  = false;
    _allselected = 0;
    _tildselect  = 0;
    //
    _undodirt       = 0;
    _docDirty       = 0;
    _alignment      = false;
    _brmodePrev     = BR_VIEW;
    _brmode         = BR_VIEW;
    _leafNo         = -1;
    _compiling      = false;
    _closing        = false;
    _selBoxByTouch  = false;
    _cachedSett._skyDom = 0;
    _cachedSett._isSolid    =1;
    _cachedSett._radstrips  =-1;
    _cachedSett._reversed   =0;
    _cachedSett._detail  =0;
    _cachedSett._thikness   =0;
    _cachedSett._vstrips    =-1;
    _cachedSett._tm         = TM_LIN;
    _pActiveView        = 0;
    _exViewMode	        =  0;
    _vmodeback          = false;
    _selmode            = SEL_NONE;
    _compviewmode       = C_NA;
    _lastItmSel         = 0;
    _viewChanged        = 0;
    _selShowSelBox      = 0;
    _viewCount          = 0;
    b_showgrid          = 1;
    b_whitebk           = 0;
    _use2splitidx       = -1;
    _maxUsedGrp       = 0;
    Brush::GID        = 0;
    _use2splitidx       = -1;
    SceItem::_GItemID = 0;

    _camSpot            = 0;
    InitDocVars();

}

Document::~Document()
{
    Pd=0;
} //OCR_SIZEALL



void Document::InitDocVars()
{
#ifdef _FREE
    GLight = 0;
#endif //
    DeselectAll();
    _scene.Clear();

    _undo.Clear();
    _undodirt       = 0;
    _docDirty       = 0;

    _use2splitidx = -1;
    _closing      = false;
    _gridSize     = 40000;
    _gridStep     = 20;
    _xyz_lock     = 1;
    _bDrawCamRay  = true;
    _brmode       = BR_VIEW;
    _brmodePrev     = BR_VIEW;
    _compviewmode = C_NA;
    _bSnapGrid = false;
    _selmode   = SEL_NONE;

    _vmodefire = false;
    _vmodeback = false;
    _bShowCsg  = false;
    _maxUsedGrp   = 0;
    Brush::GID        = 0;
    SceItem::_GItemID = 0;
    _use2splitidx       = -1;
    _cam.SetFov(2*PIPE6);
    _lvupdate = ge_gettick();
    _compiler._finalBSpFileName="";

    _pClipboard.clear();
    _pClipboardItm.clear();
    _undo.Clear();

    _cam.SetFar(24800.0);//250m
    _cam.SetAngles(-PIPE4,-PIPE6,0);
    _cam.Euler2Pos();
    _cam._pos = v3d_t(1600.0f,1000.0f,1600.0f);

    _viewChanged = 6;
    theApp->CleanTexMan();

    if(VIEW2D('x'))
        VIEW2D('x')->Reinit();
    if(VIEW2D('y'))
        VIEW2D('y')->Reinit();
    if(VIEW2D('z'))
        VIEW2D('z')->Reinit();

    _selBox.Clear();
    _selBox.MakeCube(v3d_t(1000, 1000, 1000));
    _selBox._brushflags = BRSH_SELBOX;
    _selBox.Recalc();
    //TexHandler::SetSearchPath(0);

    return ;
}



/////////////////////////////////////////////////////////////////////////////
// Document serialization

void Document::Serialize(CArchive& ar)
{

}

/////////////////////////////////////////////////////////////////////////////
// Document diagnostics

#ifdef _DEBUG
void Document::AssertValid() const
{
    CDocument::AssertValid();
}

void Document::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Document commands

void Document::SetShift(View2D* pSrc)
{
    View2D* pTX = VIEW2D('x');
    View2D* pTY = VIEW2D('y');
    View2D* pTZ = VIEW2D('z');

    switch(pSrc->_vt)
    {
        case 'x':                 // xYZ        // SIDE
            pTY->_shifty = -pSrc->_shiftx;
            pTZ->_shifty = pSrc->_shifty;
            _shift.y = -pSrc->_shifty;
            _shift.z = pSrc->_shiftx;
            break;
        case 'y':               // XyZ          // TOP
            pTX->_shiftx = -pSrc->_shifty;  // side
            pTZ->_shiftx = pSrc->_shiftx;              // front
            _shift.z = pSrc->_shifty;
            _shift.x = pSrc->_shiftx;
            break;
        case 'z':               // XYz          // FRONT
            pTX->_shifty = pSrc->_shifty;
            pTY->_shiftx = pSrc->_shiftx;               // top
            _shift.x = pSrc->_shiftx;
            _shift.y = pSrc->_shifty;
            break;
    }

}


void Document::SetZoom(View2D* pSrc)
{
    View2D* pTX = VIEW2D('x');
    View2D* pTY = VIEW2D('y');
    View2D* pTZ = VIEW2D('z');
    pTY->_zoom = pSrc->_zoom;
    pTX->_zoom = pSrc->_zoom;
    pTZ->_zoom = pSrc->_zoom;
    _zoom = pTZ->_zoom;
}


void Document::Invalidate(u_bool32 update)
{
    if(ThrID != GetCurrentThreadId())
        return;

    View2D* pTX   = VIEW2D('x');
    View2D* pTY   = VIEW2D('y');
    View2D* pTZ   = VIEW2D('z');
    View3D* pT3D   = VIEW3D(0,0);
    if(pTX && pT3D)
    {
        pT3D->updateGL();
        pTX->updateGL();
        pTZ->updateGL();
        pTY->updateGL();
    }
}

void    Document::Invalidate3D(u_bool32 fromMM)
{
    if(ThrID != GetCurrentThreadId())
        return;

    if(fromMM)
    {
        if(ge_gettick() - _lvupdate < 32)
        {
            _lvupdate = ge_gettick();
            return;
        }
    }
    View3D* pTXYZ = VIEW3D(0,0);
    pTXYZ->updateGL();

}



void Document::OnBurotate()
{
    if(_brmode == BR_ROTATE)
    {
        _brmode = BR_VIEW;
        return;
    }
    _brmode = BR_ROTATE;
}

void Document::OnBuriselow()
{
    if(_brmode!=BR_RISE)
        _brmode = BR_RISE;
    else
        _brmode = BR_VIEW;

}




//SEL_NONE, SEL_BRUSH, SEL_FACE, SEL_VERTEX
void Document::OnButselbrush()
{
    DeselectAll();
    _brmode       = BR_VIEW;

    _compviewmode = C_NA;
    if(_selmode == SEL_BRUSH)
    {
        _selmode = SEL_NONE;
        VIEW2D('z')->_accmode = BR_VIEW;
        VIEW2D('x')->_accmode = BR_VIEW;
        VIEW2D('y')->_accmode = BR_VIEW;
        Invalidate();
        return;
    }
    _selmode = SEL_BRUSH;
    Invalidate();
}


void Document::OnBumove()
{
    if(_brmode == BR_MOVE)
    {
        _brmode = BR_VIEW;
        return;
    }
    _brmode = BR_MOVE;
}


void Document::OnBUScale()
{
    if(_brmode == BR_SCALE)
    {
        _brmode = BR_VIEW;
        return;
    }
    _brmode = BR_SCALE;
}





#ifdef DOCCMD_MFC
//----------------------------------------------------------------------------------------
void Document::OnFileOpen()
{
    char szFilters[] = _T("Geometry raw (*.grf)|*.grf|"
                          "Geometry selection (*.grs)|*.grs|"
                          "Geometry prefab (*.gpm)|*.gpm|"
                          "Getic BSP(*.gbt)|*.gbt||");



    LPTSTR          lpszPathName = NULL;
    _closing        = false;

    CFileDialog dlg(true, _T("grf"), 0, OFN_HIDEREADONLY, szFilters);
    DeselectAll();

    //
    // Prompt the user for a filename
    //
    dlg.m_ofn.lpstrTitle = _T("Select  File");
    dlg.m_ofn.lpstrInitialDir = theApp->DocsDir();

    if (theApp->DoFileDialog(dlg) != IDOK)
    {
        theApp->ReDir();
        return;
    }

    theApp->ReDir();
    PathHandler ph(dlg.m_ofn.lpstrFile);
    theApp->DocsDir(ph.Path());

    DeleteContents();
    InitDocVars();

    ////TAfxMessageBox(MKSTR("%s%s_res",ph.Path(),ph.JustFile()));

    TexHandler::SetSearchPath(MKSTR("%s%s_res",ph.Path(),ph.JustFile()));

    char szupper[_MAX_PATH];
    ::_tcscpy(szupper, dlg.m_ofn.lpstrFile);
    CharUpper(szupper);
    if(_tcsstr(szupper,".GBT"))
    {
        ReadFromBSP(dlg.m_ofn.lpstrFile);
    }
    else
    {
        CopyResources(dlg.m_ofn.lpstrFile);
        ReadFromFile(dlg.m_ofn.lpstrFile);
    }

    SetModifiedFlag(true);
    SetTitle(dlg.m_ofn.lpstrFile);
    TestBackupFile();

    Invalidate(1);
    _docDirty=0;

}

//----------------------------------------------------------------------------------------
void Document::OnFileMerge()
{

    char szFilters[] = _T("Geometry raw (*.grf)|*.grf|"
                          "Geometry selection (*.grs)|*.grs|"
                          "Geometry prefab (*.gpm)|*.gpm||");

    LPTSTR          lpszPathName = NULL;
    _closing        = false;

    CFileDialog dlg(true, _T("grf"), 0, OFN_HIDEREADONLY, szFilters);
    DeselectAll();
    // Prompt the user for a filename
    dlg.m_ofn.lpstrTitle = _T("Select  File");
    {
        dlg.m_ofn.lpstrInitialDir = theApp->DocsDir();

        if (theApp->DoFileDialog(dlg) != IDOK)
        {
            theApp->ReDir();
            return;
        }
        theApp->ReDir();
        PathHandler ph(dlg.m_ofn.lpstrFile);
        theApp->DocsDir(ph.Path());
    }
    ++Pd->_maxUsedGrp;
     ReadFromFile(dlg.m_ofn.lpstrFile);
    SetModifiedFlag(true);
    TAfxMessageBox(MKSTR("Merged Scene Is On Group: '%d' And Up", _maxUsedGrp), MB_OK|MB_ICONINFORMATION);

}



u_bool32 Document::OnOpenDocument(const char* lpszPathName)
{
    Poly::__Max=v3d_t(0,0,0);
    if(_scene.GetPrimitives()->size() && _docDirty)
    {
        u_int32_t retval = TAfxMessageBox("Do you want to Save the current Getic scene file ? ", MB_YESNOCANCEL|MB_ICONQUESTION);
        if(retval==QMessageBox::Cancel)
            return 0;
        if(retval==IDYES)
        {
            OnFileSave();
        }
    }

    _closing = false;
    InitDocVars();
    return true;
}


void Document::OnTbLockxyz()
{
    _xyz_lock = !_xyz_lock;


    View2D* pTX = VIEW2D('x');
    View2D* pTY = VIEW2D('y');
    View2D* pTZ = VIEW2D('z');

    pTX->Reinit();
    pTY->Reinit();
    pTZ->Reinit();
/*
    SetShift(pSrc);
    SetZoom(pSrc);
*/
    Invalidate();
}

void Document::OnUpdateTbLockxyz(CCmdUI* pCmdUI)
{
    if(_compiling)
    {
        pCmdUI->Enable(false);
        return;
    }

    pCmdUI->SetCheck(_xyz_lock);
}



void Document::OnUpdateBuriselow(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(0);
}


void Document::OnBurotate()
{
    if(_brmode == BR_ROTATE)
    {
        _brmode = BR_VIEW;
        return;
    }
    _brmode = BR_ROTATE;
}

void Document::OnUpdateBurotate(CCmdUI* pCmdUI)
{
    if(_bShowCsg || _compiling ||IsSelBoxOn() || Pd->_compviewmode != C_NA)
    {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->Enable((_pSelBrushes.size() || _pSelItems.size()) || _pSelPolys.size()==1);
    pCmdUI->SetCheck(_brmode == BR_ROTATE);
}


void Document::OnBumove()
{
    if(_brmode == BR_MOVE)
    {
        _brmode = BR_VIEW;
        return;
    }
    _brmode = BR_MOVE;
}

void Document::OnUpdateBumove(CCmdUI* pCmdUI)
{
    if(_bShowCsg || _compiling || Pd->_compviewmode != C_NA)
    {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->Enable(_pSelBrushes.size() || _pSelItems.size() || _pSelVertexes.size()||
                   _pSelPolys.size()==1 || _selBox._brushflags & BRSH_SELVIS);
    pCmdUI->SetCheck(_brmode == BR_MOVE);

    SBAR().SetSelection(_pSelItems.size() || _pSelPolys.size() || _pSelBrushes.size() || _pSelVertexes.size());

}


void Document::OnUpdateBUScale(CCmdUI* pCmdUI)
{
    if(_bShowCsg || _compiling || Pd->_compviewmode != C_NA)
    {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->Enable((_pSelBrushes.size()  ||
                    _pSelItems.size()) || _pSelPolys.size()==1||
                    _selBox._brushflags & BRSH_SELVIS);
    pCmdUI->SetCheck(_brmode == BR_SCALE);
}



void Document::OnBushcam()
{
    _bDrawCamRay = !_bDrawCamRay;

}

void Document::OnUpdateBushcam(CCmdUI* pCmdUI)
{
    if(_compiling)
    {
        pCmdUI->Enable(false);
        return;
    }

    pCmdUI->SetCheck(_bDrawCamRay);
}

void Document::OnBusnapgrid()
{
    _bSnapGrid=!_bSnapGrid;

}

void Document::OnUpdateBusnapgrid(CCmdUI* pCmdUI)
{
    if(_compiling)
    {
        pCmdUI->Enable(false);
        return;
    }

    pCmdUI->SetCheck(_bSnapGrid);
}


void DocumentBritem()
{
    DeselectAll();

    _brmode       = BR_VIEW;
    _compviewmode = C_NA;

    if(_selmode == SEL_ITEMS)
    {
        _selmode = SEL_NONE;

        VIEW2D('z')->_accmode = BR_VIEW;
        VIEW2D('x')->_accmode = BR_VIEW;
        VIEW2D('y')->_accmode = BR_VIEW;

        return;
    }
    _selmode = SEL_ITEMS;
    Invalidate();
}



void Document::OnSeledge()
{
    DeselectAll();
    _brmode       = BR_VIEW;
    _compviewmode = C_NA;
    if(_selmode == SEL_EDGE)
    {
        _selmode = SEL_NONE;
        VIEW2D('z')->_accmode = BR_VIEW;
        VIEW2D('x')->_accmode = BR_VIEW;
        VIEW2D('y')->_accmode = BR_VIEW;

        Invalidate();
        return;
    }
    _selmode = SEL_EDGE;
    Invalidate();
}

void Document::OnUpdateSeledge(CCmdUI* pCmdUI)
{
    if(_bShowCsg || _compiling || Pd->_compviewmode != C_NA || (_scene.GetPrimitives()->size()==0))
    {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->SetCheck(_selmode == SEL_EDGE);

}


void Document::OnSelvertex()
{
    DeselectAll();
    _brmode       = BR_VIEW;
    _compviewmode = C_NA;
    if(_selmode == SEL_VERTEX)
    {
        _selmode = SEL_NONE;
        VIEW2D('z')->_accmode = BR_VIEW;
        VIEW2D('x')->_accmode = BR_VIEW;
        VIEW2D('y')->_accmode = BR_VIEW;

        Invalidate();
        return;
    }
    _selmode = SEL_VERTEX;
    Invalidate();
}


void Document::OnBufacesel()
{
    DeselectAll();
    _brmode       = BR_VIEW;
    _compviewmode = C_NA;
    if(_selmode == SEL_FACE)
    {
        _selmode = SEL_NONE;
        VIEW2D('z')->_accmode = BR_VIEW;
        VIEW2D('x')->_accmode = BR_VIEW;
        VIEW2D('y')->_accmode = BR_VIEW;

        Invalidate();
        return;
    }
    _selmode = SEL_FACE;
    Invalidate();
}

void Document::OnUpdateButselbrush(CCmdUI* pCmdUI)
{
    if(_bShowCsg || _compiling || Pd->_compviewmode != C_NA || (_scene.GetPrimitives()->size()==0))
    {
        pCmdUI->Enable(false);
        return;
    }

    pCmdUI->Enable(!(_selBox._brushflags & BRSH_SELVIS));

    pCmdUI->SetCheck(_selmode == SEL_BRUSH);
}

void Document::OnUpdateSelitem(CCmdUI* pCmdUI)
{
    if(_bShowCsg || _compiling || _compviewmode != C_NA ||(_scene.GetItems()->size()==0))
    {
        pCmdUI->Enable(0);
        return;
    }
    pCmdUI->Enable(1);
    pCmdUI->SetCheck(_selmode == SEL_ITEMS);
}


void Document::OnUpdateSelvertex(CCmdUI* pCmdUI)
{
    if(_bShowCsg || _compiling || Pd->_compviewmode != C_NA || (_scene.GetPrimitives()->size()==0))
    {
        //_compviewmode==C_FULL
        pCmdUI->Enable(0);
        return;
    }
    pCmdUI->Enable(1);
    pCmdUI->SetCheck(_selmode == SEL_VERTEX);
}


void Document::OnUpdateBufacesel(CCmdUI* pCmdUI)
{
    if(_bShowCsg || _compiling || Pd->_compviewmode != C_NA || (_scene.GetPrimitives()->size()==0))
    {
        pCmdUI->Enable(false);
        return;
    }

    pCmdUI->Enable(!(_selBox._brushflags & BRSH_SELVIS));
    pCmdUI->SetCheck(_selmode == SEL_FACE);
}





void Document::OnFileSave()
{
#ifdef _TRIAL_
    return;
#else //
    if(GetTitle() == "NoName")
    {
        OnFileSaveAs();
    }
    else
    {
        _lasterror.erase();

        try{
            DeselectAll();
            SaveInFile(GetTitle());
            _docDirty = 0;
        }
        catch(int32_t& i)
        {
            i;
            TAfxMessageBox(_lasterror.c_str(), MB_OK|MB_ICONWARNING);
            _lasterror.erase();
        }
        catch(...)
        {
            TAfxMessageBox("Unknown Exception while Saving", MB_OK|MB_ICONHAND);
        }
    }
#endif
}


void Document::OnFileSaveAs()
{
#ifdef _TRIAL_
    return;
#else

    theApp->ReDir();
    LPTSTR lpszPathName = NULL;
    char szFilters[] = _T("Geometry raw (*.grf)|*.grf|"
                          "Geometry selection (*.grs)|*.grs|"
                          "Geometry prefab (*.gpm)|*.gpm||");

    // Prompt the user for a filename
    CFileDialog dlg(false, _T("grf"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilters);
    dlg.m_ofn.lpstrTitle = _T("Select Geometry File");

    dlg.m_ofn.lpstrInitialDir = theApp->DocsDir();
    // Show the dialog

    if (theApp->DoFileDialog(dlg) != IDOK) return;
    PathHandler ph(dlg.m_ofn.lpstrFile);
    theApp->DocsDir(ph.Path());


    try{
        SaveInFile(dlg.m_ofn.lpstrFile);
        SetTitle(dlg.m_ofn.lpstrFile);
        _docDirty=0;
    }
    catch(int32_t& i)
    {
        i;
        TAfxMessageBox(_lasterror.c_str(), MB_OK|MB_ICONWARNING);
        _lasterror.erase();
    }
    catch(...)
    {
        TAfxMessageBox("Unknown Exception while Saving", MB_OK|MB_ICONHAND);
    }
    theApp->ReDir();
#endif //
}

//---------------------------------------------------------------------------------------
void Document::OnEditPaste()
{
    SelModeSave sms(_brmode,_selmode);

    DeselectAll();
    if(_pClipboard.size())
    {
        SBT(0, "Pasted OK.");

        FOREACH(PBrushes, _pClipboard, ppb)
        {
            Brush* pB       = *ppb;

            if(pB->_brushflags & BRSH_WIREBOX ||
                pB->_brushflags & BRSH_REF)
                continue;

            Brush* pbNew    = new Brush(*pB);
            pbNew->_pMotion = 0;
            pbNew->_brushflags |=  BRSH_NEW;
            pbNew->_brushflags &= ~BRSH_HASTRIG; //has no trigger copy
            pbNew->Recalc();
            SelectBrush(pbNew);
        }
        Invalidate();
        return;
    }
    if(_pClipboardItm.size())
    {
        FOREACH(vvector<SceItem*>, _pClipboardItm, pps)
        {
            switch((*pps)->_item)
            {
                case ITM_LIGTBULB:
                {
                    CLightBulb* plb  = new CLightBulb(*((CLightBulb*)(*pps)));
                    plb->_flags      |= BRSH_NEW;
                    plb->_group      = _curCreatGroup;
                    SelectItem(plb);
                }
                break;
            }
        }
        Invalidate();
        return;
    }

    Beep(400,100);
    SBT(0, "Clipboard Is Empty!");
}

void Document::OnBushowback()
{
     _viewChanged=3;

    _vmodeback=!_vmodeback;

    Invalidate();
}

//---------------------------------------------------------------------------------------
void Document::OnUpdateBushowback(CCmdUI* pCmdUI)
{
    if(_compiling)
    {
        pCmdUI->Enable(false);
        return;
    }

    pCmdUI->SetCheck(_vmodeback);
}

//---------------------------------------------------------------------------------------
void Document::OnBuwireframe()
{
    _vmodefire =! _vmodefire;
    _viewChanged = 3;

    Invalidate();
}

//---------------------------------------------------------------------------------------
void Document::OnUpdateBuwireframe(CCmdUI* pCmdUI)
{
    if(_compiling)
    {
        pCmdUI->Enable(false);
        return;
    }

    pCmdUI->SetCheck(_vmodefire);
}


//---------------------------------------------------------------------------------------
void Document::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
    if(_compiling)
    {
        pCmdUI->Enable(false);
        return;
    }

    pCmdUI->Enable(HasSomeSelected() );
}

//---------------------------------------------------------------------------------------
void Document::OnUpdateCutBrush(CCmdUI* pCmdUI)
{
    if(_compiling)
    {
        pCmdUI->Enable(false);
        return;
    }
}

//---------------------------------------------------------------------------------------
void Document::OnUpdateCut(CCmdUI* pCmdUI)
{
    if(_compiling)
    {
        pCmdUI->Enable(false);
        return;
    }

    pCmdUI->Enable(HasSomeSelected());
}

//---------------------------------------------------------------------------------------
void Document::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
    if(_compiling)
    {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->Enable(_pClipboard.size()>0 || _pClipboardItm.size()>0);
}

//---------------------------------------------------------------------------------------
void Document::OnUpdateDelete(CCmdUI* pCmdUI)
{
    if(_compiling)
    {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->Enable(HasSomeSelected());
}



//---------------------------------------------------------------------------------------
void Document::OnEscape()
{
    _allselected=0;
    Escape();
}

//---------------------------------------------------------------------------------------
void Document::OnEditCopy()
{
    SelModeSave sms(_brmode,_selmode);

    DeleteUncomittedNewBrushes();
    DeleteUncomittedNewItems();

    if(_pSelBrushes.size())
    {
AGAIN1:
        FOREACH(PBrushes,  _pSelBrushes, ppb)
        {
            if((*ppb)->IsSelection())
            {
                _pSelBrushes.erase(ppb);
                goto AGAIN1;
            }
        }

        _pClipboard = _pSelBrushes;
        DeselectAll();
        SBT(0, MKSTR("%d Brushes Copyed", _pSelBrushes.size()));
    }
    else if(_pSelItems.size())
    {
        _pClipboardItm = _pSelItems;

        DeselectAll();
        SBT(0, MKSTR("%d Items Copyed", _pSelItems.size()));
    }
}

//---------------------------------------------------------------------------------------
void Document::OnCut()
{
    OnEditCopy() ;
    OnDelete();
}


//---------------------------------------------------------------------------------------
void Document::OnDelete()
{
    if(!_userBool)
    {
        DeleteUncomittedNewBrushes();
        DeleteUncomittedNewItems();
    }
    u_bool32 b = false;
    _pClipboard.clear();
    _pClipboardItm.clear();

    if(_selmode == SEL_BRUSH )
    {
AGAIN1:
        FOREACH(PBrushes,  _pSelBrushes, ppb)
        {
            b |= 1;
            DeleteBrush(*ppb);
            _pSelBrushes.erase(ppb);
            goto AGAIN1;
        }
        if(b)
        {
            Recut();
            DeselectAll();
        }
        Invalidate();
        return ;
    }

    if(_selmode == SEL_ITEMS)
    {
AGAIN2:
        FOREACH(vvector<SceItem*>, _pSelItems, ppItem)
        {
            b |= true;
            SceItem*  pItem  = *ppItem;
            _pSelItems.erase(ppItem);
            DelItem(pItem);
            goto AGAIN2;
        }
        DeselectItems();                        // triger bar update
        if(b)
        {
            DeselectAll();
        }
    }

    if(_selmode == SEL_FACE)
    {
        set<Brush*> brushes;                    // touched brushes
AGAIN:
        FOREACH(PPolys,  _pSelPolys, ppp)       // selected polys
        {
            Poly& p2f =*(*ppp);
            brushes.insert(p2f._pBrush);        // store the touched brush

            _pSelPolys.erase(ppp);
            p2f._pBrush->ErasePoly(p2f);

            goto AGAIN;
        }
        DeselectPolys();                            // deselect them (clean the list)
        FOREACH(set<Brush*>, brushes, pb)
        {
            (*pb)->Recalc();
            SelectBrush(*pb);
            b = true;
        }
    }

    if(b)
    {
        Recut();
        DeselectAll();
    }
    Invalidate();
}




//---------------------------------------------------------------------------------------
void Document::OnUpdateEnter(CCmdUI* pCmdUI)
{
    if(_compiling)
    {
        pCmdUI->Enable(false);
        return;
    }
}

//---------------------------------------------------------------------------------------
void Document::OnBushowcsg()
{
    _selmode = SEL_NONE;
        VIEW2D('z')->_accmode = BR_VIEW;
        VIEW2D('x')->_accmode = BR_VIEW;
        VIEW2D('y')->_accmode = BR_VIEW;

    _compviewmode = C_NA;

    _brmode       = BR_VIEW;
    _bShowCsg=!_bShowCsg;
    _viewChanged=3;
    DeselectAll();
    SetFaceMode(1);
    Invalidate();
}

//---------------------------------------------------------------------------------------
void Document::OnUpdateBushowcsg(CCmdUI* pCmdUI)
{
    if(_compiling)
    {
        pCmdUI->Enable(false);
        return;
    }

    if(_scene.GetBrushes()->size()==0)
    {
        pCmdUI->SetCheck(0);
        pCmdUI->Enable(false);
    }
    else
        pCmdUI->SetCheck(_bShowCsg);
}

//---------------------------------------------------------------------------------------
void Document::OnUpdateMirror(CCmdUI* pCmdUI)
{
    if(_compiling)
    {
        pCmdUI->Enable(false);
        return;
    }

    pCmdUI->Enable(_pSelBrushes.size())	;
}

//---------------------------------------------------------------------------------------
void Document::OnEnter()
{
  //  SaveTempFile();

    u_bool32    recut = true;
    if(0==_pSelBrushes.size() && 0 == _pSelItems.size())
        return;
    BARDLG()->OnApply();

    PBrushes  pSelBrushes = _pSelBrushes;

    if(pSelBrushes.size())
    {
        SBT(0, MKSTR("%d Brushes Commited", pSelBrushes.size()) );

        FOREACH(PBrushes,  pSelBrushes, ppb)
        {
            if((*ppb)->_brushflags & BRSH_NEW)           //new brushes just move them in scene
            {
                // keep last bouding box ex for next created brush
                (*ppb)->_brushflags &= ~BRSH_NEW;
                (*ppb)->Dirty(0);
                (*ppb)->Recalc();

                if((*ppb)->IsSplitter())
                {
                    SplitBySplitterBrush(*ppb);
                    delete (*ppb);
                    recut = true;
                    break; //only one can be
                }
                /*
                else if((*ppb)->IsZone())
                {
                    // add to zones
                }
                else
                {
                */
                    /**
                    if(GDetEnabled && (*ppb)->IsSolid())
                    {
                        EvaluateDetailbrsh((*ppb));
                    }
                    */
                    _scene.AddPrimitive((*ppb));
                    CameraPointTo((*ppb)->_box.GetCenter());
                //}
            }
            else if((*ppb)->IsDirty())				// moved scaled brushes, just commit them
            {
                (*ppb)->Recalc();
                /*
                if(GDetEnabled && (*ppb)->IsSolid())
                {
                    EvaluateDetailbrsh((*ppb));
                }
                */
            }
        }
        if(recut )
            Recut();
        BARDLG()->OnBrushesSelected();
    }

    if(_pSelItems.size())
    {
        FOREACH(vvector<SceItem*>, _pSelItems, ppItem)
        {
            if((*ppItem)->_flags & BRSH_NEW)
            {
                (*ppItem)->_flags &= ~BRSH_NEW;

                _scene.AddItem(*ppItem);
                BARDLG()->AddItem(*ppItem);
                BARDLG()->Clean();
                BARDLG()->OnItemSelected(*ppItem);
                CameraPointTo((*ppItem)->_t);
            }
        }
        //BARDLG()->OnItemsSelected();

        SBT(0, MKSTR("%d Items Commited", _pSelItems.size()) );
    }
    Invalidate();
}


//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
void Document::OnEditUndo()
{
    _docDirty = 1;
    OnUndo();
}

//---------------------------------------------------------------------------------------
void Document::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
    if(_compiling)
    {
        pCmdUI->Enable(false);
        return;
    }

    pCmdUI->Enable(1);
}


//---------------------------------------------------------------------------------------
void Document::OnTabPrevsel()
{
    if(_selmode == SEL_NONE)
        return;
    SelectTabBrush(-1, 0);				//walk backward
}

//---------------------------------------------------------------------------------------
void Document::OnTabNextsel()
{
    if(_compviewmode  != C_NA)//
    {
        _viewChanged=3;
        if(_compviewmode & C_LBYL)
        {
            _leafNo++;
        }
        _exViewMode++;
        Invalidate();
        //(VIEW3D(0,0))->InvalidateRect(0);
        Invalidate();
        return;
    }
    if(_selmode == SEL_NONE)
        return;
    SelectTabBrush(1, 0);				//walk forwards
}

//---------------------------------------------------------------------------------------
void Document::OnTabSelnext()
{
    if(_selmode == SEL_NONE)
        return;
    SelectTabBrush(1, 1);				//walk forwards
}

//---------------------------------------------------------------------------------------
void Document::OnTabSelprev()
{
    if(_compviewmode & C_LBYL)
    {
        _viewChanged = 3;
        _leafNo--;
        return;
    }
    if(_selmode == SEL_NONE)
        return;
    SelectTabBrush(-1, 1);				//walk forwards
}


//---------------------------------------------------------------------------------------
void Document::OnCompbsp()
{
    SaveTempFile();

    BARDLG()->Dirty();
    BARDLG()->OnApply();

    vvector<Poly>    allpolys;

    FOREACH(PBrushes,  _pSelBrushes, psb)
    {
        Brush* pB = *psb;
        FOREACH(vvector<Poly>,pB->_polys,pp)
            allpolys.push_back(*pp);
    }

    if(allpolys.size())
    {
        Brush* pB = _pSelBrushes[0];
        MiniBsp mini;
        if(!mini.Compile(allpolys, pB->_brushflags))
        {
            TAfxMessageBox("Error compiling BSP for the selected Brush", MB_OK|MB_ICONWARNING);
        }
    }
}

void Document::OnZones()
{
    DeselectAll();
    _brmode    = BR_VIEW;
    if(_compviewmode == C_ZONES)
    {
        _compviewmode = C_NA;
        return;
    }
    _compviewmode = C_ZONES;
    Invalidate();

}

void Document::OnUpdateZones(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(_compiler.Done()!=0);
    pCmdUI->SetCheck(_compviewmode == C_ZONES);
}



void Document::OnViewGraph()
{
    SetFaceMode(1);

     DeselectAll();
    _brmode    = BR_VIEW;
    if(_compviewmode == C_GRAPH)
    {
        _compviewmode = C_NA;
        if(::IsWindow(FRM()->_dlgleafs.m_hWnd))
        {
            FRM()->DestroyLeafExplorer();
        }
        _cam._pos =_camsave[0];
        _cam._euler =_camsave[1];
        _cam.Euler2Pos();

        Invalidate();
        return;
    }
     _camsave[0] = _cam._pos;
    _camsave[1] = _cam._euler;
    _cam.SetAngles(0,0,0);
    _cam.Euler2Pos();
    _compviewmode = C_GRAPH;
    Invalidate();
}

void Document::OnUpdateViewGraph(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(_compiler.Done()!=0);
    pCmdUI->SetCheck(_compviewmode == C_GRAPH);
}


void Document::OnViewLeafByLeaf()
{
    SetFaceMode(1);
    _selmode = SEL_NONE;
    VIEW2D('z')->_accmode = BR_VIEW;
    VIEW2D('x')->_accmode = BR_VIEW;
    VIEW2D('y')->_accmode = BR_VIEW;

    DeselectAll();

    _compviewmode &= ~(C_FULL|C_LMAP|C_GRAPH|C_LEAFS|C_ZONES);

    if(_compviewmode & C_LBYL)
    {
        _compviewmode &= ~C_LBYL;

        if(::IsWindow(FRM()->_dlgleafs.m_hWnd))
        {
            FRM()->DestroyLeafExplorer();
        }
    }
    else
    {
        _compviewmode |= C_LBYL;
    }

    _viewChanged = 3;
    _leafNo      = 0;
    Invalidate();
}

void Document::OnUpdateViewLeafByLeaf(CCmdUI* pCmdUI)
{
    if(_compiling)
    {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->Enable(_compiler.Done()!=0);
    pCmdUI->SetCheck(!!(_compviewmode & C_LBYL));
}

void Document::OnViewLightmaps()
{

    SetFaceMode(1);

    _selmode = SEL_NONE;
        VIEW2D('z')->_accmode = BR_VIEW;
        VIEW2D('x')->_accmode = BR_VIEW;
        VIEW2D('y')->_accmode = BR_VIEW;

    DeselectAll();

    _compviewmode &= ~(C_FULL|C_LBYL|C_GRAPH|C_LEAFS|C_ZONES);

    if(_compviewmode & C_LMAP)
    {
        _compviewmode &= ~C_LMAP;
    }
    else
    {
        _compviewmode |= C_LMAP;

        if(::IsWindow(FRM()->_dlgleafs.m_hWnd))
        {
            FRM()->DestroyLeafExplorer();
        }
    }

    _viewChanged = 3;
    _leafNo       = 0;

    Invalidate();

}

void Document::OnUpdateViewLightmaps(CCmdUI* pCmdUI)
{
    if(_compiling)
    {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->Enable(_compiler.Done()!=0);
    pCmdUI->SetCheck(!!(_compviewmode & C_LMAP));

}

void Document::OnViewFullBSP()
{
    SetFaceMode(1);
    _selmode = SEL_NONE;
        VIEW2D('z')->_accmode = BR_VIEW;
        VIEW2D('x')->_accmode = BR_VIEW;
        VIEW2D('y')->_accmode = BR_VIEW;

    DeselectAll();

    _compviewmode &= ~(C_LEAFS|C_LBYL|C_LMAP|C_GRAPH|C_ZONES);

    if(_compviewmode & C_FULL)
    {
        _compviewmode &= ~C_FULL;
    }
    else
    {
        _compviewmode |= C_FULL;

        if(::IsWindow(FRM()->_dlgleafs.m_hWnd))
        {
            FRM()->DestroyLeafExplorer();
        }
    }
    _viewChanged = 3;
    Invalidate();
}

void Document::OnUpdateOnViewFullBSP(CCmdUI* pCmdUI)
{
    if(_compiling)
    {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->Enable(_compiler.Done()!=0);
    pCmdUI->SetCheck(!!(_compviewmode & C_FULL));
}

//---------------------------------------------------------------------------------------
void Document::OnViewPortals()
{
    SetFaceMode(1);

    _selmode = SEL_NONE;
        VIEW2D('z')->_accmode = BR_VIEW;
        VIEW2D('x')->_accmode = BR_VIEW;
        VIEW2D('y')->_accmode = BR_VIEW;

    DeselectAll();

    _compviewmode &= ~(C_GRAPH);
    if(_compviewmode & C_PORT)
    {
        _compviewmode &= ~C_PORT;
    }
    else
    {
        _compviewmode |= C_PORT;

        if(::IsWindow(FRM()->_dlgleafs.m_hWnd))
        {
            FRM()->DestroyLeafExplorer();
        }
    }
    _viewChanged = 3;
    Invalidate();
}

//---------------------------------------------------------------------------------------
void Document::OnUpdateOnViewPortals(CCmdUI* pCmdUI)
{
    if(_compiling)
    {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->Enable(_compiler.Done()!=0);
    pCmdUI->SetCheck(!!(_compviewmode & C_PORT));
}

void Document::OnViewWhite()
{
    SetFaceMode(1);

    _selmode = SEL_NONE;
        VIEW2D('z')->_accmode = BR_VIEW;
        VIEW2D('x')->_accmode = BR_VIEW;
        VIEW2D('y')->_accmode = BR_VIEW;

    DeselectAll();

    if(_compviewmode & C_WHITE)
    {
        _compviewmode &= ~C_WHITE;
    }
    else
    {
        _compviewmode |= C_WHITE;

        if(::IsWindow(FRM()->_dlgleafs.m_hWnd))
        {
            FRM()->DestroyLeafExplorer();
        }
    }
    _viewChanged = 3;
    Invalidate();

}

void Document::OnViewByFar()
{
    SetFaceMode(1);
    _selmode = SEL_NONE;
        VIEW2D('z')->_accmode = BR_VIEW;
        VIEW2D('x')->_accmode = BR_VIEW;
        VIEW2D('y')->_accmode = BR_VIEW;

    DeselectAll();

    if(_compviewmode & C_BYFAR)
    {
        _compviewmode &= ~C_BYFAR;
    }
    else
    {
        _compviewmode |= C_BYFAR;

        if(::IsWindow(FRM()->_dlgleafs.m_hWnd))
        {
            FRM()->DestroyLeafExplorer();
        }
    }
    _viewChanged = 3;
    Invalidate();

}

void Document::OnViewByFarUpdate(CCmdUI* pCmdUI)
{
    if(_compiling)
    {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->Enable(_compiler.Done()!=0 && _compviewmode!=0);
    pCmdUI->SetCheck(!!(_compviewmode & C_BYFAR));
}



void Document::OnViewWhiteUpdate(CCmdUI* pCmdUI)
{
    if(_compiling)
    {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->Enable(_compiler.Done()!=0 && _compviewmode!=0);
    pCmdUI->SetCheck(!!(_compviewmode & C_WHITE));
}


//---------------------------------------------------------------------------------------
void Document::OnViewColorLeafs()
{
    SetFaceMode(1);

    _selmode = SEL_NONE;
        VIEW2D('z')->_accmode = BR_VIEW;
        VIEW2D('x')->_accmode = BR_VIEW;
        VIEW2D('y')->_accmode = BR_VIEW;

    DeselectAll();

    _compviewmode &= ~(C_GRAPH|C_LBYL|C_FULL|C_LMAP|C_ZONES);
    if(_compviewmode & C_LEAFS)
    {
        _compviewmode &= ~C_LEAFS;
    }
    else
    {
        _compviewmode |= C_LEAFS;

        if(::IsWindow(FRM()->_dlgleafs.m_hWnd))
        {
            FRM()->DestroyLeafExplorer();
        }
    }
    _viewChanged = 3;
    Invalidate();
}



//---------------------------------------------------------------------------------------
void Document::OnUpdateOnViewColorLeafs(CCmdUI* pCmdUI)
{
    if(_compiling)
    {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->Enable(_compiler.Done()!=0);
    pCmdUI->SetCheck(!!(_compviewmode & C_LEAFS));
}


void Document::OnSelnondetbrushes()
{
    if(_selmode == SEL_BRUSH )
    {
        OnEscape();
        PBrushes* rbs = _scene.GetPrimitives();
        FOREACH(PBrushes, (*rbs), ppB)
        {
            if(_curgroup>=0)
            {
                if(_curgroup != (*ppB)->_group)
                    continue;
            }
            if((*ppB)->_brushflags & BRSH_DETAIL)
                continue;

            SelectBrush(*ppB);
        }
        BARDLG()->OnBrushesSelected();
    }

}

void Document::OnDetbrushes()
{
    if(_selmode == SEL_BRUSH )
    {
        OnEscape();
        PBrushes* rbs = _scene.GetPrimitives();
        FOREACH(PBrushes, (*rbs), ppB)
        {
            if(_curgroup>=0)
            {
                if(_curgroup != (*ppB)->_group)
                    continue;
            }
            if(!((*ppB)->_brushflags & BRSH_DETAIL))
                continue;

            SelectBrush(*ppB);
        }
        BARDLG()->OnBrushesSelected();
    }

}

//---------------------------------------------------------------------------------------
void Document::OnSelectall()
{
    SelModeSave sms(_brmode,_selmode);

    u_bool32 selAll = false;

    if(_selmode == SEL_NONE)
    {
        return;
    }

    if(_selmode == SEL_BRUSH )
    {
        OnEscape();
        _allselected = 1;
        PBrushes* rbs = _scene.GetPrimitives();
        FOREACH(PBrushes, (*rbs), ppB)
        {
            if(_curgroup>=0)
            {
                if(_curgroup != (*ppB)->_group)
                    continue;
            }

            SelectBrush(*ppB);
        }
        BARDLG()->OnBrushesSelected();

        if(theApp->get_modifiers('g') || theApp->get_modifiers('G'))
        {
            vvector<SceItem*>*  pTemsPtr = GetItems();
            FOREACH(vvector<SceItem*>, (*pTemsPtr), ppItem)
            {
                SceItem*  pItem  = *ppItem;

                if(_curgroup>=0)
                {
                    if(_curgroup != pItem->_group)
                        continue;
                }

                this->SelectItem(pItem);
            }
        }
    }
    else if(_selmode == SEL_FACE)
    {

        // select all faces
        if(_pSelPolys.size())
        {
            Poly*	pP = _pSelPolys[0];
            Brush*	pB = pP->_pBrush;
            OnEscape();

            // find the selected poly in the brush
            vvector<Poly>::iterator pPly = pB->_polys.begin();
            vvector<Poly>::iterator be = pB->_polys.end();
            for(; pPly != be; pPly++)
            {
                Poly& rP = *pPly;
                if(pP == &rP)
                    continue;
                SelectFace(&rP, pB);
            }
        }
        BARDLG()->OnFacesSelected();
    }

    else if (_selmode == SEL_ITEMS )
    {
        OnEscape();
        vvector<SceItem*>*  pTemsPtr = GetItems();
        FOREACH(vvector<SceItem*>, (*pTemsPtr), ppItem)
        {
            SceItem*  pItem  = *ppItem;

            if(_curgroup>=0)
            {
                if(_curgroup != pItem->_group)
                    continue;
            }
            this->SelectItem(pItem);
            BARDLG()->OnItemsSelected();
        }

    }
    Invalidate();
}

//---------------------------------------------------------------------------------------
//
//  selects all faces facing from center to this direction
//
void Document::OnSelsameasselface()
{
    if(_pSelPolys.size())
    {
        Poly*	pP     = _pSelPolys.back();
        Brush*	pB     = pP->_pBrush;
        v3d_t      bc2pc  = (pP->_box.GetCenter() - pB->_box.GetCenter()).norm();
        f32_t    vndir  = Vdp(pP->_n, bc2pc);

        // find the selected poly in the brush
        vvector<Poly>::iterator pPly = pB->_polys.begin();
        vvector<Poly>::iterator be = pB->_polys.end();
        for(; pPly != be; pPly++)
        {
            Poly& rP = *pPly;
            if(pP == &rP)
                continue;

            v3d_t      bc2pc  = (pPly->_box.GetCenter() - pB->_box.GetCenter()).norm();
            f32_t    vndir2 = Vdp(pPly->_n, bc2pc);

            if(vndir2 * vndir > 0)
            {
                SelectFace(&rP, pB);
            }
        }
    }
    Invalidate();
}


void Document::OnUpdateTriangulate(CCmdUI* pCmdUI)
{
    if(_bShowCsg || _compiling ||IsSelBoxOn() || Pd->_compviewmode != C_NA)
    {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->Enable(_pSelBrushes.size()!=0);
}


void Document::OnTriangulate()
{
    FOREACH(PBrushes,  _pSelBrushes, ppBrsh)
    {
        if(!(*ppBrsh)->Triangulate())
        {
            SBT(0, "Faces: Too many or too small.");
        }
    }
    Recut();
    Invalidate();
}


void Document::OnTriFan()
{
    FOREACH(PBrushes,  _pSelBrushes, ppBrsh)
    {
        (*ppBrsh)->TriFanIt();
    }
    Recut();
    Invalidate();
}

void Document::OnUpdateTriFan(CCmdUI* pCmdUI)
{
    if(_bShowCsg || _compiling ||IsSelBoxOn() || Pd->_compviewmode != C_NA)
    {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->Enable(_pSelBrushes.size()!=0);
}

void Document::OnInsvx()
{

}

void Document::OnUpdateInsvx(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(0);
}





void Document::OnSamenormface()	//Ctl+ H
{
    if(_pSelPolys.size())
    {
        Poly*	pP     = _pSelPolys.back();
        Brush*	pB     = pP->_pBrush;
        v3d_t	   norm    = pP->_n;

        if(theApp->get_modifiers(VK_SHIFT))
        {
            FOREACH(PBrushes, (*_scene.GetPrimitives()), ppbb)
            {
                Brush* pB =*ppbb;

                vvector<Poly>::iterator pPly = pB->_polys.begin();
                vvector<Poly>::iterator be = pB->_polys.end();
                for(; pPly != be; pPly++)
                {
                    Poly& rP = *pPly;
                    if(pP == &rP)
                        continue;
                    if(norm == rP._n)
                    {
                        SelectFace(&rP, pB);
                    }
                }
            }
        }
        else
        {
            // find the selected poly in the brush
            vvector<Poly>::iterator pPly = pB->_polys.begin();
            vvector<Poly>::iterator be = pB->_polys.end();
            for(; pPly != be; pPly++)
            {
                Poly& rP = *pPly;
                if(pP == &rP)
                    continue;
                if(norm == rP._n)
                {
                    SelectFace(&rP, pB);
                }
            }
        }
    }
    Invalidate();
}



void Document::OnUpdateXoy(CCmdUI* pCmdUI)
{
    if(_bShowCsg || _compiling || _compviewmode  != C_NA)
    {
        pCmdUI->Enable(_pSelBrushes.size());
        return;
    }
}


void Document::OnUpdateXoz(CCmdUI* pCmdUI)
{
    if(_bShowCsg || _compiling || _compviewmode  != C_NA)
    {
        pCmdUI->Enable(_pSelBrushes.size());
        return;
    }

}


void Document::OnUpdateZoy(CCmdUI* pCmdUI)
{
    if(_bShowCsg || _compiling || _compviewmode  != C_NA)
    {
        pCmdUI->Enable(_pSelBrushes.size());
        return;
    }
}

void Document::OnUpdateExtrude(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(_pSelPolys.size()>=1);
}



// extrude on the direction of face normals
void Document::OnExtrude()
{
    SaveTempFile();

    if(_pSelPolys.size()==0)
        return;

    Poly    poly;
    Poly    np;
    vvector<Poly> nps(32);
    Brush*  pBrsh;
    pBrsh = _pSelPolys[0]->_pBrush;

    // see if all polygons belong to same brush
    FOREACH(PPolys,  _pSelPolys, a)
    {
        if((*a)->_pBrush != pBrsh)
        {
            TAfxMessageBox("Cannot Extrude different brushes polygons", MB_OK|MB_ICONHAND);
            return;
        }
    }

    // store original brush for undo

    // copy the selected face
    FOREACH(PPolys,  _pSelPolys, a)
    {
        poly = *(*a);

        // extrude vertexes, ir 'R' backward
        FOREACH(vvector<Vtx2>, poly._vtci, pV){
            if(theApp->get_modifiers('R'))
                pV->_xyz += poly._n * (-64);
            else
                pV->_xyz += poly._n * 64;
        }
        poly.Recalc();

        // make sideways polygons
        int32_t  vxes = poly._vtci.size();
        for(int32_t i=0; i < vxes; i++)
        {
            np.Clear();
            np.GetTexs() = poly.GetTexs();
            int32_t vx0 = i % vxes;
            int32_t vx1 = (i+1) % vxes;

            np << (*a)->_vtci[vx0];
            np << (*a)->_vtci[vx1];

            np << poly._vtci[vx1];
            np << poly._vtci[vx0];

            if(vxes==4)
            {
                np.ResetTexCoords(-1);
            }
            np.Recalc();
            nps.push_back(np);
        }
        // flag the poly extruded
        poly._polyflags |= POLY_EXTRUDED;
        nps.push_back(poly);
        poly.Clear();
    }

    // mark extruded polygons deleted
    FOREACH(PPolys,  _pSelPolys, a)
    {
        (*a)->_polyflags |= POLY_DELETED;
    }
    DeselectAll();


    // add new sideways polygons
    FOREACH(vvector<Poly>, nps, pPoly)
    {
        pPoly->_polyflags |= POLY_EXTRUDED;
        pBrsh->AddPoly(*pPoly);
    }
    nps.clear();

    // delete marked faces
AGAIN:
    FOREACH(Polys, pBrsh->_polys, pp)
    {
        if(pp->_polyflags & POLY_DELETED)
        {
            pBrsh->ErasePoly(*pp);
            goto AGAIN;
        }
    }
    pBrsh->Recalc();
    SelectBrush(pBrsh);
    UpdateBrushLinks(pBrsh);
    Recut();
    DeselectBrushes();
    Invalidate(1);
}

void Document::OnUpdateHallounhallo(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(_pSelBrushes.size()!=0);
}

void Document::OnHallounhallo()
{
    FOREACH(PBrushes,  _pSelBrushes, ppb)
    {
        (*ppb)->HalloUnHallo();
    }
    Invalidate();
}

void Document::OnMerge()
{
    SaveTempFile();

    if(_pSelBrushes.size()>=2)
    {
        Brush* pba = _pSelBrushes[0];
        DeSelectBrush(pba);

        for(u_int32_t i=0; i<_pSelBrushes.size();i++)
        {
            Brush& bb = *_pSelBrushes[i];
            pba->AppendPrim(bb);
            pba->Recalc();
        }
        OnDelete() ;
        SelectBrush(pba);
        OnEnter();
    }

}

void Document::OnUpdateMerge(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(_pSelBrushes.size()>=2);
}




void Document::OnPermcut()
{
    SaveTempFile();

    set<Brush*>         brshSolids;
    vvector<Brush*>     selCopy = _pSelBrushes;
    vvector<Brush*>     newBrushes;
    int32_t                 collsz,collsz2;
    Brush*              pResultSceneBrsh  = 0;

    DeselectAll();

    _TRY
    {
        u_int32_t flags = 0;

        // collect solids tounching each other
        FOREACH(PBrushes,  selCopy, ppb)
        {
            Brush* pBrush = (*ppb)->_pResult;

            if(pBrush->_brushflags & BRSH_FLAGGED)
                continue;

            if(!pBrush->IsSolid())
                continue;
            if( pBrush->_brushflags & BRSH_DETAIL)
                continue;

            flags    |= pBrush->_brushflags;
            pBrush->_brushflags |= BRSH_FLAGGED;
            brshSolids.insert(pBrush);

            collsz = brshSolids.size();
            FOREACH(PBrushes,  selCopy, ppb1)
            {
                if(ppb1 == ppb)                     continue;
                if((*ppb1)->_brushflags & BRSH_FLAGGED)  continue;
                if( pBrush->_brushflags & BRSH_DETAIL)
                    continue;


                if((*ppb1)->_box.IsTouchesBox(pBrush->_box))
                {
                    flags           |= pBrush->_brushflags;
                    (*ppb1)->_brushflags |= BRSH_FLAGGED;
                    brshSolids.insert(*ppb1);
                }
            }
            collsz2 = brshSolids.size();
            while(collsz2!= collsz) // as much there are adds
            {
                // search again in added list and see any more touching polygons
                collsz = brshSolids.size();
                set<Brush*>::iterator si = brshSolids.begin();
                set<Brush*>::iterator se = brshSolids.end();
                for(;si!=se;si++)
                {
                    FOREACH(PBrushes,  selCopy, ppb1)
                    {
                        if(*ppb1 == *si)                    continue;
                        if((*ppb1)->_brushflags & BRSH_FLAGGED)  continue;

                        if((*ppb1)->_box.IsTouchesBox((*si)->_box))
                        {
                            flags           |= pBrush->_brushflags;
                            (*ppb1)->_brushflags |= BRSH_FLAGGED;
                            brshSolids.insert(*ppb1);
                        }
                    }
                }
                collsz2 = brshSolids.size();
            }

            // now for each set build a csged brush
            if(brshSolids.size())
            {
                pResultSceneBrsh = new Brush();
                pResultSceneBrsh->_brushflags = flags|BRSH_SOLID|BRSH_NEW;
                vvector<Brush*> loco(brshSolids.size());

                set<Brush*>::iterator si = brshSolids.begin();
                set<Brush*>::iterator se = brshSolids.end();
                for(;si!=se;si++)
                {
                    loco << *si;
                }

                if(!Compiler::Union(loco, *pResultSceneBrsh,1))
                {
                    delete pResultSceneBrsh;
                    pResultSceneBrsh = 0;
                }
                else
                {
                    pResultSceneBrsh->_brushflags &= ~BRSH_FLAGGED;
                    pResultSceneBrsh->_brushflags |= BRSH_NEW;

                    newBrushes << pResultSceneBrsh;
                    pResultSceneBrsh = 0;
                }
                //SelectBrush(pResultSceneBrsh);
            }
            brshSolids.clear();
            flags = 0;
        }


        _userBool    = true;
        _pSelBrushes = selCopy;

        OnDelete();         // delete old selected brushes
        _userBool = false;

        DeselectAll();
        _pSelBrushes = newBrushes;
        OnEnter();
        Invalidate();

        VIEW3D(0,0)->SetFocus();

        FOREACH(PBrushes,  newBrushes, ppb)
        {
            (*ppb)->_brushflags &= ~BRSH_FLAGGED;
        }

    }
    _CATCHX()
    {
        TAfxMessageBox("Cannot Commit Changes",MB_ICONWARNING,MB_OK);
    }
    if(pResultSceneBrsh)
        delete pResultSceneBrsh;
    Invalidate();
}


void Document::OnUpdatePermcut(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(_pSelBrushes.size()>=2);
}

void Document::OnUndo()
{
    SaveTempFile();
    OnEscape();
    /*
    _undo.Undo(this,1);
    Recut();
    DeselectAll();
    Invalidate();
    */

}

void Document::OnUpdateUndo(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(0);//_undo.Size());
}

void Document::OnUpdateTervol(CCmdUI* pCmdUI)
{
    if(_pSelBrushes.size()==1 && _pSelBrushes[0]->_brushflags & BRSH_CSGTERRAIN)
        pCmdUI->Enable(1);
    else
        pCmdUI->Enable(0);

}

void Document::OnTervol()
{

    Brush*  pSel = _pSelBrushes[0];
    Box     box  =  pSel->_box;
    v3d_t&     pos  =  pSel->_box.GetCenter();

    // build a box
    Brush*   pSquare = new Brush();
    box._max.y *= 2;
    box._min.y *= 2;

    pSquare->MakeCube(box.GetExtends());
    pSquare->Move(pos);

    SelectBrush(pSquare);
    _scene.AddPrimitive(pSquare);
    pSquare->_brushflags&=~BRSH_NEW;

    //OnPermcut();

    OnMerge();
    Invalidate();
    // mergeit with the terrain
    // flag it non terrain

}

void Document::OnManTex()
{
    DlgTexMody d; d.DoModal();
}

void Document::OnUpdateManTex(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(0);
}

void Document::OnToXml()
{
    SaveTempFile();

    theApp->ReDir();

    LPTSTR lpszPathName = NULL;
    char szFilters[] = _T("VMRL 1.0 (*.txt)|*.txt|All Files (*.*)|*.*||");

    // Prompt the user for a filename
    CFileDialog dlg(false, _T("txt"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilters);
    dlg.m_ofn.lpstrTitle = _T("Select TEXT File");

    dlg.m_ofn.lpstrInitialDir = theApp->DocsDir();
    // Show the dialog
    if (theApp->DoFileDialog(dlg) != IDOK) return;
    PathHandler ph(dlg.m_ofn.lpstrFile);
    theApp->DocsDir(ph.Path());

    SaveInXML(dlg.m_ofn.lpstrFile);

    theApp->ReDir();
}

void Document::OnUpdateToXml(CCmdUI* pCmdUI)
{

}



void Document::OnLoadlastbackip()
{
    DeleteContents();
    InitDocVars();
    ReadFromFile(_tempfileName);
    SetModifiedFlag(true);

}

void Document::OnUpdateLoadlastbackip(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(_tempfileName.GetLength());
}


void Document::OnSnapvxes()
{
    FOREACH(PBrushes,  _pSelBrushes, ppb)
    {
        Brush* pb = *ppb;
        if(pb->_brushflags & BRSH_DETAIL)
            continue;
        Polys& polys = pb->_polys;
        FOREACH(Polys, polys, pB)
        {
            Poly& poly = *pB;
            FOREACH(vvector<Vtx2>, poly._vtci, pV)
            {
                RoundToNear(pV->_xyz, 16);
            }
        }
    }
    Recut();
    Invalidate();
}

void Document::OnUpdateSnapvxes(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(_pSelBrushes.size());
}


void Document::OnShowgrid()
{
    b_showgrid=!b_showgrid;
    Invalidate();
}

void Document::OnUpdateShowgrid(CCmdUI* pCmdUI)
{
    // TODO: Add your command update UI handler code here

}

//---------------------------------------------------------------------------------------
void Document::OnFinalRelease()
{
    CDocument::OnFinalRelease();
}



//---------------------------------------------------------------------------------------
void    Document::OnStartMoveRotScale(int32_t operation, SceItem** ppItems, int32_t count)
{
    if(0==count)
        return;
    if(_undodirt==0)
    {
        /*
        _undodirt=1;
        while(--count>=0)
        {
           _undo.Add(*ppItems, operation);
            ppItems++;
        }
        */

    }
}

//---------------------------------------------------------------------------------------
void    Document::OnStartMoveRotScale(int32_t operation, Brush** ppBrushes, int32_t count)
{
    if(0==count)
        return;

    if(_undodirt==0)
    {
        /*
        _undodirt=1;
        while(--count>=0)
        {
           _undo.Add(*ppBrushes, operation);
            ppBrushes++;
        }
        */

    }
}

//---------------------------------------------------------------------------------------
// reset them to initla valies to see last translation that happend
// I can undo last roattion or last translation because scale is not
// acually a scalation
u_bool32    Document::OnStartMoveRotScale()
{
    u_bool32 rv = 0;
    _docDirty = 1;
    if(_selmode == SEL_BRUSH ||
       _selmode == SEL_FACE ||
       _selmode == SEL_VERTEX ||
       _selmode == SEL_EDGE)
    {
        FOREACH(PBrushes,  _pSelBrushes, ppb)
        {
            (*ppb)->Dirty(0);
        }
        rv=1;
    }
    _undodirt=0;
    return rv;
}




u_bool32 Document::OnCmdMsg(u_int32_t nID, int32_t nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    _cmdMsg=true;
    u_bool32 b = CDocument::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
    //SbarShowCurMode();
    return b;
}


/---------------------------------------------------------------------------------------
// selecta ll brushes from the group as current selected brush
void Document::OnSelectgroup()
{

}

//---------------------------------------------------------------------------------------
// selects all brushes having the same index as the curent selected
void Document::OnSelcurid()
{

}

//---------------------------------------------------------------------------------------
void Document::OnUpdateSelcurid(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(0);
}

//---------------------------------------------------------------------------------------
void Document::OnUpdateSelectgroup(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(0);
}

//---------------------------------------------------------------------------------------
void Document::OnUpdateSelsameasselface(CCmdUI* pCmdUI)
{
    if(_bShowCsg || _compiling || _compviewmode != C_NA)
    {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->Enable((_pSelPolys.size()) && _selmode==SEL_FACE);
}

//---------------------------------------------------------------------------------------
void Document::OnUpdateSelectall(CCmdUI* pCmdUI)
{
    if(_bShowCsg || _compiling || _compviewmode != C_NA)
    {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->Enable(_scene.GetPrimitives()->size()!=0 || _scene.GetItems()->size()!=0);
}

void Document::OnBureverse()
{
    int32_t nCount;
    Brush** b = GetSelBrushes(nCount);
    if(nCount)
    {
        for(int32_t i=0;i<nCount;i++,b++)
        {
            (*b)->Reverse();
            UpdateBrushLinks(*b);
        }

        Recut();
    }
    else
    if(_pSelPolys.size())
    {
        set<Brush*> pbrushes;
        FOREACH(PPolys,  _pSelPolys, a)
        {
            Poly   *p = (*a);
            pbrushes.insert(p->_pBrush);
            p->Reverse();
        }
        FOREACH(set<Brush*>,pbrushes,ppb)
        {
            SelectBrush(*ppb);
            UpdateBrushLinks(*ppb);
        }
        Recut();
        DeselectBrushes();
    }
    Invalidate();
}


//---------------------------------------------------------------------------------------
void Document::OnUpdateBureverse(CCmdUI* pCmdUI)
{
    if(_bShowCsg || _compiling || _compviewmode != C_NA)
    {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->Enable(_pSelBrushes.size()||_pSelPolys.size());

}

//---------------------------------------------------------------------------------------
void Document::OnUpdateBucsg(CCmdUI* pCmdUI)
{
    if(_bShowCsg || _compiling|| _compviewmode != C_NA)
    {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->Enable(_scene.GetPrimitives()->size());
}

//---------------------------------------------------------------------------------------
void Document::OnCtx2dRotatecw30()
{
    RotateByAngleCw(PIPE6);
}

//---------------------------------------------------------------------------------------
void Document::OnCtx2dRotatecw60()
{
    RotateByAngleCw(PIPE6*2);
}

//---------------------------------------------------------------------------------------
void Document::OnCtx2dRotatecw90()
{
    RotateByAngleCw(PIPE2);
}

//---------------------------------------------------------------------------------------
void Document::OnCtx2dRotatecw45()
{
    RotateByAngleCw(PIPE4);
}

void Document::OnWpncfg()
{

}




void Document::OnImport()
{
    /**
    char szFilters[] = _T("Geometry raw (*.grf)|*.grf|All Files (*.*)|*.*||");

    CFileDialog dlg(_bFileOpen, "WMP", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilters);
    if (dlg.DoModal() != IDOK) return;

    FileWrap    fw;

    if(!fw.Open(dlg.m_ofn.lpstrFile,"rb"))
        return;

    int32_t  mBlkCnt = 1;
    int32_t  mObjCnt = 1;
    char line[256];
    char expectedBlock[64];
    char expectedObject[64];

    _tstprintf(expectedBlock, "block %d {", mBlkCnt);
    _tstprintf(expectedObject, "block %d {", mObjCnt);

    vvector<v3d_t>  block;
    vvector <vvector<v3d_t> > blocks;


    while(! feof(fw._pf))
    {

        if(pf.ReadLine(line, 256))
        {
            if(strstr(line, expectedBlock))
            {
                mBlkCnt++;
                _tstprintf(expectedBlock, "block %d {", mBlkCnt);

                ReadBlock(fw);

            }
            else
            if(strstr(line, expectedObject))
            {
                mObjCnt++;
                _tstprintf(expectedObject, "block %d {", mObjCnt);
            }



        }
        if(feof(fw._pf))
            break;
    }




    fw.Close();
    */
}


void Document::OnTexrefdlg()
{
    CDlgTexRefs().DoModal();

}


void Document::OnSametexface() // Ctl+G
{
    if(_pSelPolys.size())
    {

        Poly*	pP       = _pSelPolys.back();
        Brush*	pB       = pP->_pBrush;
        u_int32_t    sametex  = (int32_t)pP->GetHtex(GUtex);

        if(theApp->get_modifiers(Qt::Key_Shift))//accumulates selections
        {
            FOREACH(PBrushes, (*_scene.GetPrimitives()), ppbb)
            {
                Brush* pB =*ppbb;

                vvector<Poly>::iterator pPly = pB->_polys.begin();
                vvector<Poly>::iterator be = pB->_polys.end();
                for(; pPly != be; pPly++)
                {
                    Poly& rP = *pPly;
                    if(pP == &rP)
                        continue;
                    if(sametex == rP.GetHtex(GUtex))
                    {
                        SelectFace(&rP, pB);
                    }
                }
            }
        }
        else
        {
             // find the selected poly in the brush
            vvector<Poly>::iterator pPly = pB->_polys.begin();
            vvector<Poly>::iterator be = pB->_polys.end();
            for(; pPly != be; pPly++)
            {
                Poly& rP = *pPly;
                if(pP == &rP)
                    continue;
                if(sametex == rP.GetHtex(GUtex))
                {
                    SelectFace(&rP, pB);
                }
            }
        }
    }
    Invalidate();
}


//CTRL+D
void Document::OnExtractfaces()
{
    if(_pSelPolys.size()==0)
        return;

    set<Brush*>	brushes;

    Brush*    frontB = new Brush();

    frontB->_brushflags = BRSH_SOLID|BRSH_NEW|BRSH_DETAIL;

    FOREACH(PPolys,  _pSelPolys, a)
    {
        Poly   p = *(*a);
        p._pBrush = frontB;
        p.DeSelect();
        frontB->_polys << p;

        (*a)->_polyflags |= POLY_DELETED;
        brushes.insert((*a)->_pBrush);
    }
    _pSelPolys.clear();

    frontB->Recalc();
    _scene.AddPrimitive(frontB);

    for(set<Brush*>::iterator sb = brushes.begin(); sb!=brushes.end(); sb++)
    {
        (*sb)->DeletePolys();
        (*sb)->Recalc();
    }

    Invalidate();
    OnEnter();
}

void Document::OnUpdateExtractfaces(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(_pSelPolys.size()>4)	;
}



void Document::OnCreatesb()
{
    SelModeSave sms(_brmode,_selmode);

    QPoint ptDummy;

    DeselectAll();
    CreateSelBox(ptDummy);
    if(theApp->get_modifiers(Qt::Key_Shift))
        _selBoxByTouch=true;
    else
        _selBoxByTouch=false;

    SelectByBox();
    Invalidate();
    //_brmode     = BR_SCALE;
    //_selmode	= SEL_BRUSH;

}

void Document::OnUpdateCreatesb(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(_selmode != SEL_NONE);
}


void Document::OnTrpolys()
{
    BShowTrPolys	=! BShowTrPolys;
    Invalidate();
}

void Document::OnRepoint() //join vertexes in same pos
{
    f32_t mindist = 65535.0;
    f32_t dist    = _selBox._box.GetMinExtend();
    Brush* refBrush = 0;

    // find first the closest distance
    FOREACH(vvector<Vtx2*>,_pSelVertexes,ppv1)
    {
        (*ppv1)->_extra1 = 0; // unaligned;
    }

    // get all selected vertexes
    FOREACH(vvector<Vtx2*>,_pSelVertexes,ppv1)
    {
        if((*ppv1)->_extra1)
            continue;
        FOREACH(vvector<Vtx2*>,_pSelVertexes,ppv2)
        {
            if(ppv1 == ppv2)
                continue;
            if((*ppv2)->_extra1)
                continue;
            if((*ppv1)->_pPoly && (*ppv2)->_pPoly)
            {
                if((*ppv1)->_pPoly->_pBrush != (*ppv2)->_pPoly->_pBrush)
                {
                    if(vdist((*ppv1)->_xyz, (*ppv2)->_xyz) < dist)
                    {
                        if(0 == refBrush)
                            refBrush = (*ppv1)->_pPoly->_pBrush;

                        if((*ppv1)->_pPoly->_pBrush == refBrush)
                        {
                            (*ppv2)->_xyz = (*ppv1)->_xyz;
                            (*ppv1)->_extra1 = 1;
                        }
                        else
                        {
                            (*ppv1)->_xyz = (*ppv2)->_xyz;
                            (*ppv2)->_extra1 = 1;
                        }
                    }
                }
            }
        }
    }

    FOREACH(vvector<Vtx2*>,_pSelVertexes,ppv1)
    {
        (*ppv1)->_extra1 = 0; // unaligned;
    }

    Invalidate();
}

void Document::OnUpdateRepoint(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(SEL_VERTEX==_selmode && IsSelBoxOn() );
}

void Document::OnRepointbrsh()
{
    SelModeSave sms(_brmode,_selmode);

    vvector<v3d_t> svxes;
    Brush* refBrush1 =0;
    Brush* refBrush2 =0;
    FOREACH(vvector<Vtx2*>, _pSelVertexes, ppvx)
    {
        if(svxes.push_unique((*ppvx)->_xyz))
        {
            if(refBrush1==0)
                refBrush1=(*ppvx)->_pPoly->_pBrush;
            else  if(refBrush2==0)
                refBrush2=(*ppvx)->_pPoly->_pBrush;
        }
    }

    if(refBrush1 && refBrush2 && refBrush1!=refBrush2)
    {
        v3d_t diff = svxes[1]-svxes[0];
        refBrush1->Dirty(1);
        refBrush1->Move(diff);
        refBrush1->Recalc();
        this->OnFinishMoveRotScale(M_MOVED);
        Invalidate();
    }
}

void Document::OnUpdateRepointbrsh(CCmdUI* pCmdUI)
{
    if(SEL_VERTEX==_selmode)
    {
        vvector<v3d_t> svxes;
        FOREACH(vvector<Vtx2*>, _pSelVertexes, ppvx)
        {
            svxes.push_unique((*ppvx)->_xyz);
        }
        pCmdUI->Enable(svxes.size()==2);
    }
    else
        pCmdUI->Enable(0);

}


void Document::OnUpdateFileSave(CCmdUI* pCmdUI)
{
}

void Document::OnUpdateFileSaveAs(CCmdUI* pCmdUI)
{
}


void Document::OnCamLspot()
{
    _camSpot=!_camSpot;
    Invalidate();
}



void Document::OnFileSaveselAs()
{
    theApp->ReDir();
    LPTSTR lpszPathName = NULL;
    char szFilters[] = _T("Geometry raw (*.grs)|*.grs|All Files (*.*)|*.*||");

    // Prompt the user for a filename
    CFileDialog dlg(false, _T("grf"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilters);
    dlg.m_ofn.lpstrTitle = _T("Select Geometry File");

    dlg.m_ofn.lpstrInitialDir = theApp->DocsDir();
    // Show the dialog

    if (theApp->DoFileDialog(dlg) != IDOK) return;
    PathHandler ph(dlg.m_ofn.lpstrFile);
    theApp->DocsDir(ph.Path());



    try{
        SaveInFile(dlg.m_ofn.lpstrFile, 1);
        _docDirty = 0;
    }
    catch(int32_t& i)
    {
        i;
        TAfxMessageBox(_lasterror.c_str(), MB_OK|MB_ICONWARNING);
        _lasterror.erase();
    }
    catch(...)
    {
        TAfxMessageBox("Unknown Exception while Saving", MB_OK|MB_ICONHAND);
    }
    theApp->ReDir();

}

void Document::OnUpdateFileSaveselAs(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(_pSelBrushes.size());
}

void Document::OnInsface()
{
    if(_pSelBrushes.size()==1)
    {
        Brush* pb = _pSelBrushes.front();

        Poly p;
        v3d_t   v[4];
        v3d_t   &dims   = pb->_box.GetExtends();
        v3d_t   &center = pb->_box.GetCenter();

        v[0]  = center + v3d_t(-(dims.x/2), (dims.y/2), -(dims.z/2));
        v[1]  = center + v3d_t(-(dims.x/2), (dims.y/2), (dims.z/2));
        v[2]  = center + v3d_t((dims.x/2), (dims.y/2), (dims.z/2));
        v[3]  = center + v3d_t((dims.x/2), (dims.y/2), -(dims.z/2));

        p.Create(4, v, pb);
        p.ResetTexCoords(-1);
        p.InitCalcTC(1);
        p._polyflags = POLY_EXTRUDED|POLY_INSERTED;
        p._polyprops = FACE_NOCOLLIDE|FACE_SHOWBACK;

        p._colorD   = ZWHITE;
        pb->AddPoly(p);

        DeselectAll();
        SelectFace(&pb->_polys.back(), pb);
        SetSelMode(SEL_FACE);
        Invalidate();
    }
}

void Document::OnUpdateInsface(CCmdUI* pCmdUI)
{
    if(_bShowCsg || _compiling ||IsSelBoxOn() || Pd->_compviewmode != C_NA)
    {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->Enable(_pSelBrushes.size()==1);
}

void Document::OnCutwithbrush()
{
    if(_pSelPolys.size()<2)
        return;

    Poly    poly   = *(_pSelPolys.front());
    Brush*  pBrush = poly._pBrush;
    Poly    a;
    Poly    b;

    FOREACH(PPolys,  _pSelPolys, ppP)
    {
        if(ppP == _pSelPolys.begin())
            continue;

        a.CopyProps(poly);
        poly.Split((Plane&)*(*ppP),a,b); //keep front fragment
        if(a._vtci.size()>=3)
        {
            poly = a;
            b.Clear();
            a.Clear();
        }
        else
            break;
    }
     pBrush->ErasePoly(*_pSelPolys.front());
    _pSelPolys.clear();
    pBrush->AddPoly(poly);

    Invalidate();
}

void Document::OnUpdateCutwithbrush(CCmdUI* pCmdUI)
{
    if(_bShowCsg || _compiling ||IsSelBoxOn() || Pd->_compviewmode != C_NA)
    {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->Enable(_pSelPolys.size()>1);

}


void Document::OnEvaldets()
{
    SaveTempFile();
    if(_devdet.m_hWnd == 0)
    {
        Pd->DeselectAll();

        Pd->BlockSelections(1);
        Pd->_selmode = SEL_FACE;
        _devdet.Create(DLG_AUTODETAILS, AfxGetMainWnd());
    }
    _devdet.ShowWindow(SW_SHOW);

}

void Document::OnUpdateEvaldets(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(!_devdet._onScreen);
}

void Document::OnHome()
{
    _cam.SetFov(2*PIPE6);
    _lvupdate = ge_gettick();
    _cam.SetFar(24800.0);//250m
    _cam.SetAngles(-PIPE4,-PIPE6,0);
    _cam.Euler2Pos();
    _cam._pos = v3d_t(1000.0f,1000.0f,1600.0f);
    SBT(2,MKSTR("3D: x:%05.2f y:%05.2f z:%5.2f",_cam._pos.x,_cam._pos.y,_cam._pos.z));
    Invalidate();
}

void     Document::SaveTempFile()
{
#ifdef _FREE
    return;
#endif //
    if(!GAutoSave) return;

    char                loco[_MAX_PATH];
    char                iittaa[32];
    int32_t                 lastID = 0;
    string              filenames[32];
    int32_t                 stored = 0;

    ::strcpy(loco, GetTitle());
    PathHandler ph(loco);

    do{
        theApp->ReDir();
        mkdir("temp");
        CDirChange	    cd("temp/");

        //search backups
        HANDLE			    hfFind;
        WIN32_FIND_DATA	    fDATA;
        int32_t                 count = 0;
        cd.UseObj();

        for(int32_t i=0; i<16; i++)
            filenames[i]="";

        if((hfFind=FindFirstFile(_T("*.grf"), &fDATA)) != INVALID_HANDLE_VALUE)
        {
            if(fDATA.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
            {
                do{
                    char* tild = strchr(fDATA.cFileName,'~');
                    if(tild && strstr(fDATA.cFileName, ph.JustFile()))
                    {
                        ++tild;
                        ++stored;
                        lastID =  tmax(lastID, ::atoi(tild));
                        lastID = lastID%16;
                        filenames[lastID] = fDATA.cFileName;
                    }
                }while(FindNextFile(hfFind, &fDATA)!=0);
            }

        }
        FindClose(hfFind);
    }while(0);

    // if count roll up and rename files
    if(stored >=15 )
    {
        for(int32_t i=0; i<16; i++)
        {
            string& fname = filenames[i];
            if(fname.length()==0) continue;
            int32_t npos = fname.find('~')+1;
            int32_t id   = ::atoi(fname.substr(npos,2).c_str());
            if(0 == id)
            {
                _unlink(fname.c_str());
                --lastID;
            }
            else
            {
                string newname = fname;

                ::sprintf(iittaa, "%02d", id-1);
                newname.replace(npos, 2, iittaa);

                ::CopyFile(fname.c_str(), newname.c_str(),0);
                ::_unlink(fname.c_str());
            }
        }
        // delete the 0 and rname them up
    }

    ++lastID;
    lastID = lastID%16;
    _tempfileName =  "temp/";
    _tempfileName += "~";
    ::sprintf(iittaa, "%02d", lastID);
    _tempfileName += iittaa;
    _tempfileName += ph.File();

    _TRY
    {
        SaveInFile(_tempfileName);
        TestBackupFile();
        _docDirty = 0;
    }
    _CATCHX()
    {
        TAfxMessageBox("Unknown Exception while Saving", MB_OK|MB_ICONHAND);
        GAutoSave=0;
    }
}

void Document::OnUpdateExpp(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(!_scene.GetPrimitives()->empty() && _docDirty==0 && GetTitle()!="NoName");

}

u_bool32 Document::OnSaveDocument(const char* lpszPathName)
{
    return true;
}
void Document::OnCloseDocument()
{
    _clearing = 1;
    _viewChanged = 3;
    _closing = true;
    DeselectAll();
    EndEditor();
    _clearing = 0;
    CDocument::OnCloseDocument();
}

void Document::OnAddModel()
{
    if(_scene.GetBrushes()->size())
    {
        SavePrefFile();
    }
}

void Document::OnShowselbbox()
{
    SelModeSave sms(_brmode,_selmode);

    _selShowSelBox	= !_selShowSelBox;
    Invalidate();
    if(_selShowSelBox)
    {
        OnFinishMoveRotScale(M_MOVED);
    }
}



void Document::OnWbck()
{
    static RGBA	    clr_NORMAL	    [3] = { {150,150,150,255} , {32 ,32 ,32 ,255} , {11 ,11 ,11 ,255}  };
    static RGBA	    clr_DARK	    [3] = { {128,128,128,255} , {196,196,196,255} , {196,196,196,255}  };
    static RGBA	    clr_CUT		    [3] = { {180,0  ,0  ,255} , {180,0  ,0  ,255} , {180,0  ,0  ,255}  };
    static RGBA	    clr_WHITE	    [3] = { {255,255,255,255} , {0  ,0  ,0  ,255} , {0  ,0  ,0  ,255}  };
    static RGBA	    clr_SELECT	    [3] = { {193,235,137,32 } , {93 ,135,37 ,32 } , {93 ,135,37 ,32 }  };
    static RGBA	    clr_SELECTDIMED	[3] = { {163,205,107,128} , {93 ,235,37 ,32 } , {93 ,235,37 ,32 }  };
    static RGBA	    clr_SELBOX	    [3] = { {0  ,64 ,200,192} , {0  ,64 ,200,192} , {0  ,64 ,200,192}  };
    static RGBA	    clr_SELBOX_TCH  [3] = { {0  ,96 ,255,255} , {0  ,96 ,255,255} , {0  ,96 ,255,255}  };
    static RGBA	    clr_CAMSTART    [3] = { {255,0  ,0  ,255} , {54 ,0  ,0  ,255} , {54 ,0  ,0  ,255}  };
    static RGBA	    clr_CAMEND      [3] = { {64 ,0  ,0  ,255} , {255,0  ,0  ,255} , {255,0  ,0  ,255}  };
    static RGBA	    clr_CAMFRUST    [3] = { {255,0  ,0  ,255} , {128,0  ,0  ,255} , {128,0  ,0  ,255}  };
    static RGBA	    clr_SCRSURFACE  [3] = { {0  ,0  ,255,75 } , {0  ,0  ,128,75 } , {0  ,0  ,128,75 }  };
    static RGBA	    clr_CUT3DBRUSH  [3] = { {255,0  ,0  ,64 } , {192,0  ,0  ,64 } , {192,0  ,0  ,64 }  };
    static RGBA	    clr_PRJDBRUSH   [3] = { {0  ,0  ,255,64 } , {0  ,0  ,192,64 } , {0  ,0  ,192,64 }  };
    static RGBA	    clr_CUT3DBRUSHREV[3] ={ {0  ,255,0  ,54 } , {0  ,192,0  ,54 } , {0  ,192,0  ,54 }  };
    static RGBA	    clr_PORTALS     [3] = { {0  ,0  ,128,200} , {0  ,0  ,92 ,200} , {0  ,0  ,92 ,200}  };
    static RGBA	    clr_PORTALS1    [3] = { {0  ,128,0  ,200} , {0  ,92 ,0  ,200} , {0  ,92 ,0  ,200}  };
    static RGBA	    clr_PORTALSLINE [3] = { {255,255,192,255} , {0  ,0  ,192,255} , {0  ,0  ,192,255}  };
    static RGBA	    clr_BLOOR       [3] = { {255,255,255,128} , {0  ,0  ,0  ,128} , {0  ,0  ,0  ,128}  };
    static RGBA	    clr_DETAIL      [3] = { {128,128,0  ,255} , {128,128,0  ,255} , {128,128,0  ,255}  };
    static RGBA	    clr_TERRAIN     [3] = { {64 ,52 ,40 ,255} , {64 ,52 ,40 ,255} , {64 ,52 ,40 ,255}  };
    static RGBA	    clr_DETAILTRIG  [3] = { {192,255,0  ,255} , {192,64 ,0  ,255} , {192,64 ,0  ,255}  };
    static RGBA	    clr_SKYDOM      [3] = { {0  ,200,200,255} , {0  ,200,200,255} , {0  ,200,200,255}  };
    static RGBA	    clr_LIGHTBULB   [3] = { {64 ,0  ,0  ,255} , {192 ,0  ,0 ,255} , {192 ,0  ,0 ,255}  };
    static RGBA	    clr_MOTION      [3] = { {46 ,150,220,255} , {46 ,150,220,255} , {46 ,150,220,255}  };
    static RGBA	    clr_TRIGGER     [3] = { {250,84 ,84 ,255} , {250,84 ,84 ,255} , {250,84 ,84 ,255}  };
    static RGBA	    clr_FACEDETAIL  [3] = { {192,192,100,255} , {192,192,100,255} , {192,192,100,255}  };
    static RGBA     clr_NEW_BRITEM  [3] = { {255,147,149,255} , {192,147,149,255} , {192,147,149,255}  };
    static RGBA     clr_ZONE        [3] = { {160,60 ,160,255} , {160,60 ,160,255} , {160,60 ,160,255}  };
    static RGBA     clr_AXES_X		[3] = { {151,64 ,64 ,255} , {111,64 ,64 ,255} , {255,0  ,0 ,255}  };
    static RGBA     clr_AXES_Y		[3] = { {64 ,151,64 ,255} , {64 ,111,64 ,255} , {0  ,255,0 ,255}  };
    static RGBA     clr_AXES_Z		[3] = { {64 ,64 ,151,255} , {64 ,64 ,111,255} , {0  ,0  ,255,255}  };
    static RGBA     clr_GRIDF		[3] = { {48 ,48 ,48 ,255} , {200,200,200,255} , {190,190,190,255}  };
    static RGBA     clr_GRIDD		[3] = { {48 ,48 ,48 ,255} , {200,200,200,255} , {190,190,190,255}  };
    static RGBA     clr_FINEGRIDF	[3] = { {52 ,52 ,52 ,255} , {220,220,220,255} , {200,200,200,255}  };
    static RGBA     clr_FINEGRIDD	[3] = { {38 ,38 ,38 ,255} , {210,210,210,255} , {190,190,190,255}  };


    b_whitebk++;
    if(b_whitebk==3)
        b_whitebk=0;


    ClrCpy( CLR_NORMAL          , clr_NORMAL       [b_whitebk]);
    ClrCpy( CLR_DARK            , clr_DARK         [b_whitebk]);
    ClrCpy( CLR_CUT             , clr_CUT          [b_whitebk]);
    ClrCpy( CLR_WHITE           , clr_WHITE        [b_whitebk]);
    ClrCpy( CLR_SELECT          , clr_SELECT       [b_whitebk]);
    ClrCpy( CLR_SELECTDIMED     , clr_SELECTDIMED  [b_whitebk]);
    ClrCpy( CLR_SELBOX          , clr_SELBOX       [b_whitebk]);
    ClrCpy( CLR_SELBOX_TCH      , clr_SELBOX_TCH   [b_whitebk]);
    ClrCpy( CLR_CAMSTART        , clr_CAMSTART     [b_whitebk]);
    ClrCpy( CLR_CAMEND          , clr_CAMEND       [b_whitebk]);
    ClrCpy( CLR_CAMFRUST        , clr_CAMFRUST     [b_whitebk]);
    ClrCpy( CLR_SCRSURFACE      , clr_SCRSURFACE   [b_whitebk]);
    ClrCpy( CLR_CUT3DBRUSH      , clr_CUT3DBRUSH   [b_whitebk]);
    ClrCpy( CLR_PRJDBRUSH       , clr_PRJDBRUSH    [b_whitebk]);
    ClrCpy( CLR_BLOOR           , clr_BLOOR        [b_whitebk]);
    ClrCpy( CLR_CUT3DBRUSHREV   , clr_CUT3DBRUSHREV[b_whitebk]);
    ClrCpy( CLR_PORTALS         , clr_PORTALS      [b_whitebk]);
    ClrCpy( CLR_PORTALS1        , clr_PORTALS1     [b_whitebk]);
    ClrCpy( CLR_PORTALSLINE     , clr_PORTALSLINE  [b_whitebk]);
    ClrCpy( CLR_DETAIL          , clr_DETAIL       [b_whitebk]);
    ClrCpy( CLR_TERRAIN         , clr_TERRAIN      [b_whitebk]);
    ClrCpy( CLR_DETAILTRIG      , clr_DETAILTRIG   [b_whitebk]);
    ClrCpy( CLR_LIGHTBULB       , clr_LIGHTBULB    [b_whitebk]);
    ClrCpy( CLR_SKYDOM          , clr_SKYDOM       [b_whitebk]);
    ClrCpy( CLR_MOTION          , clr_MOTION       [b_whitebk]);
    ClrCpy( CLR_TRIGGER         , clr_TRIGGER      [b_whitebk]);
    ClrCpy( CLR_FACEDETAIL      , clr_FACEDETAIL   [b_whitebk]);
    ClrCpy( CLR_FINEGRIDF	    , clr_FINEGRIDF	  [b_whitebk]);
    ClrCpy( CLR_GRIDF		    , clr_GRIDF		  [b_whitebk]);
    ClrCpy( CLR_GRIDD		    , clr_GRIDD		  [b_whitebk]);
    ClrCpy( CLR_FINEGRIDD	    , clr_FINEGRIDD	  [b_whitebk]);
    ClrCpy( CLR_AXES_X		    , clr_AXES_X		  [b_whitebk]);
    ClrCpy( CLR_AXES_Y		    , clr_AXES_Y		  [b_whitebk]);
    ClrCpy( CLR_AXES_Z		    , clr_AXES_Z		  [b_whitebk]);


    Invalidate();
}

extern u_int32_t UTimer;
void Document::OnPaintimer()
{
    if(UTimer)
    {
        FRM()->KillTimer(UTimer);
        UTimer = 0;
    }
    else
    {
        UTimer = FRM()->SetTimer(332,100,0);
    }
}



void Document::OnUpdateCombo(CCmdUI* pCmdUI)
{
    if(_bShowCsg || _compiling || Pd->_compviewmode != C_NA)
    {
        FRM()->m_combobox.EnableWindow(false);
        return;
    }
    FRM()->m_combobox.EnableWindow(1);
}

void Document::OnUpdateCombots(CCmdUI* pCmdUI)
{

    if(_bShowCsg || _compiling || Pd->_compviewmode != C_NA)
    {
        FRM()->m_comboboxts.EnableWindow(false);
        return;
    }
    FRM()->m_comboboxts.EnableWindow(1);

}

void Document::OnImpp()
{
    theApp->ReDir();
    LPTSTR lpszPathName = NULL;
    char szFilters[] = _T("Geometry raw (*.grf)|*.grf|");

    // Prompt the user for a filename
    CFileDialog dlg(true, _T("grf"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilters);
    dlg.m_ofn.lpstrTitle = _T("Select Geometry File");

    dlg.m_ofn.lpstrInitialDir = theApp->DocsDir();
    if (theApp->DoFileDialog(dlg) != IDOK) return;

    PathHandler phSrc(dlg.m_ofn.lpstrFile);

    theApp->DocsDir(phSrc.Path());

    QString     dstDir = theApp->HomeDir();
    dstDir += "\\maps\\";
    mkdir(dstDir);

    QString     dstDirRes = theApp->HomeDir();
    dstDirRes += "\\res\\";
    mkdir(dstDir);

    ::CopyFile(dlg.m_ofn.lpstrFile, MKSTR("%s%s",dstDir, phSrc.File()),0);

    ::G_CopyFolder(dstDirRes, MKSTR("%s%s\\",phSrc.Path(), phSrc.JustFile()), "*.*");


}

void Document::OnUpdateImpp(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(1);
}

void Document::OnExpp()
{
    const char* pfld = BrowseForFolder("Select Folder",  theApp->DocsDir());
    if(0 == pfld)
        return;
    QString folderDst  = pfld;

    QString curFile    = GetTitle();
    if(curFile=="NoName")
    {
        TAfxMessageBox("Cannot save an unamend file", MB_OK|MB_ICONWARNING);
        return;
    }
    theApp->DocsDir(folderDst);
    //
    // make dest res dir
    //
    PathHandler ph(curFile);

    ::mkdir(folderDst);
    QString folderDstRes = folderDst + ph.JustFile();
    folderDstRes+="_res\\";
    ::mkdir(folderDstRes);

    ::CopyFile(curFile, MKSTR("%s%s",folderDst,ph.File()),0);

    //
    // copy all textures used in this map
    //
    QString resDir = theApp->HomeDir();
    resDir += "\\res\\";
    TexSys::iterator bt = theApp->_TexSys.begin();
    TexSys::iterator et = theApp->_TexSys.end();
    for(;bt != et; bt++)
    {
        Texture& tex      = (*bt).second;
        if(!strncmp(tex.filename, "@L",2))
            continue;
        CopyFile(MKSTR("%s%s",resDir, tex.filename), MKSTR("%s%s",folderDstRes,tex.filename), 0);
    }

    //
    // copy sounds
    //
    vvector<CSoundItem*>&  sounds = *_scene.GetSounds();
    FOREACH(vvector<CSoundItem*>, sounds, ppsound)
    {
        CSoundItem* psi = *ppsound;
        CopyFile(MKSTR("%s%s",resDir, psi->_soundfile), MKSTR("%s%s",folderDstRes,psi->_soundfile), 0);
    }
    //
    // copy scripts
    //
    vvector<ScriptItem*>&  scripts = *_scene.GetScripts();
    FOREACH(vvector<ScriptItem*>, scripts, ppscript)
    {
        ScriptItem* psi = *ppscript;
        CopyFile(MKSTR("%s%s",resDir, psi->_name), MKSTR("%s%s",folderDstRes,psi->_name), 0);
    }


}


void Document::OnLeafs()
{
    if(!::IsWindow(FRM()->_dlgleafs.m_hWnd))
        FRM()->CreateLeafExplorer();
    else
        FRM()->DestroyLeafExplorer();
}

void Document::OnUpdateLeafs(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(_compviewmode & (C_LBYL|C_GRAPH));
    pCmdUI->SetCheck(::IsWindow(FRM()->_dlgleafs.m_hWnd));
}



void Document::OnUpdateColor(CCmdUI* pCmdUI)
{
    if(_bShowCsg || _compiling || Pd->_compviewmode != C_NA)
    {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->Enable(_pSelBrushes.size() || _pSelItems.size() || _pSelVertexes.size()|| _pSelPolys.size()==1);
    SBAR().SetSelection(_pSelItems.size() || _pSelPolys.size() || _pSelBrushes.size() || _pSelVertexes.size());

}

void Document::OnColor()
{

    CColorDialog    d(RGB(200,200,200), CC_FULLOPEN|CC_RGBINIT, AfxGetMainWnd());
    if(d.DoModal()==IDOK)
    {
        CWaitCursor cwc;
        COLORREF crf = d.GetColor();

        crf &= 0x00FEFEFE;

        if(_pSelBrushes.size())
        {
            _SeColor(crf, _pSelBrushes);
        }
        else if(_pSelItems.size())
        {
            _SeColor(crf, _pSelItems);
        }
        else if(_pSelVertexes.size())
        {
            _SeColor(crf, _pSelVertexes);
        }
        else if(_pSelPolys.size())
        {
            _SeColor(crf, _pSelPolys);
        }
        Invalidate();
    }
}


// make the cuts permanent
/*
void Document::OnPermcut()
{
    PBrushes    brshSolids(32);
    PBrushes    brshCuts(32);

    try{
        Brush* pResultSceneBrsh  = 0;

        u_int32_t flags = 0;

        // collect solids tounching each other
        FOREACH(PBrushes,  _pSelBrushes, ppb)
        {
            Brush* pBrush = (*ppb)->_pResult;
            if(pBrush->IsSolid())
            {
                brshSolids.push_back(pBrush);
                flags |= pBrush->_brushflags;
            }
        }

        // union the solids
        if(brshSolids.size() != 0)
        {
            pResultSceneBrsh = new Brush();
            pResultSceneBrsh->_flags = flags|BRSH_SOLID|BRSH_NEW;

            if(!Compiler::Union(brshSolids, *pResultSceneBrsh,1))
            {
                delete pResultSceneBrsh;
                pResultSceneBrsh = 0;
            }
        }

        _userBool = true;
        OnDelete();
        _userBool = false;

        SelectBrush(pResultSceneBrsh);
        Invalidate();
        VIEW3D(0,0)->SetFocus();
        OnEnter();

    }catch(...)
    {
        TAfxMessageBox("Cannot Commit Changes",MB_ICONWARNING,MB_OK);
    }
}
*/


//---------------------------------------------------------------------------------------
void Document::OnCompileroptions()
{

    char   docName[_MAX_PATH];
    ::_tcscpy(docName, Pd->GetTitle());
    PathHandler ph(docName);

    _compiler._finalBSpFileName = theApp->BspsDir();
    _compiler._finalBSpFileName += ph.JustFile();
    _compiler._finalBSpFileName += ".gbt";

    CDlgCompile d;

    _compiler.FromRegistry(1);

    d._ckSave       = _compiler._bSaveFinalBSP;
    d._szipit       = _compiler._szipit;
    d._fileName     = _compiler._finalBSpFileName;
    d._bSaveImages  = _compiler._bSaveImages ;
    d._lmapIntens  = _compiler._lmapIntens ;
    d._maxFacesLeaf = _compiler._maxFacesLeaf ;
    d._bSavePortals = _compiler._bSavePortals ;
    d._bPolySortCrit = _compiler._bPolySortCrit;
    d._bMergeNarrowLeafs = _compiler._bMergeNarrowLeafs;
    d._bSaveSkyDom  = _compiler._bSaveSkyDom ;
    d._bLightMaps   = _compiler._bLightMaps;
    d._lmpowof2     = _compiler._lmpowof2;
    d._lmConst     = _compiler._lmConst;
    d._bConvexEarly = _compiler._bConvexEarly;
    d._b3SampleLM   = _compiler._b3SampleLM;

    d._bSaveBigTerrainBSP = _compiler._bSaveBigTerrainBSP;
    d._bSaveBigTerrain    = _compiler._bSaveBigTerrain;


//    d._bAvoidNonOrt =   _bAvoidNonOrt;
    //d._bUseUserPriority =_bUseUserPriority;


    d._bSnapMainBsp     = _compiler._bSnapMainBsp;
    d._bPvsMethod     = _compiler._bPvsMethod;
    d._bGeticBSP    = _compiler._bGeticBSP;
    d._bZones     = _compiler._bZones;
    d._bCompressPvs= _compiler._bCompressPvs;
    d._bAABBTreeCsgTer= _compiler._bAABBTreeCsgTer;


    d._bRemopl      = _compiler._removeOnePolyLeaf;
    d._portalCutFlag = _compiler._portalCutFlag;
    d._balance      = _compiler._balance;
    d._bExportFile   = _compiler._bExportFile;
    d._lmapMaxSize       = _compiler._lmapMaxSize;
    d._bLmPatch          = _compiler._bLmPatch;
    d._lMapLumelSz       = _compiler._lMapLumelSz;


    if(d.DoModal() == IDOK)
    {
        BARDLG()->OnApply();

        OnEscape();
        _compiler._bSaveFinalBSP    = d._ckSave;
        _compiler._finalBSpFileName = d._fileName;
        _compiler._szipit       = d._szipit;
        _compiler._bSaveImages  = d._bSaveImages;
        _compiler._lmapIntens    = d._lmapIntens;
        _compiler._maxFacesLeaf = d._maxFacesLeaf;
        _compiler._bSavePortals = d._bSavePortals;
        _compiler._bPolySortCrit = d._bPolySortCrit;
        _compiler._bMergeNarrowLeafs =d._bMergeNarrowLeafs;
        _compiler._bSaveSkyDom  = d._bSaveSkyDom;

        _compiler._bSaveBigTerrainBSP = d._bSaveBigTerrainBSP;
        _compiler._bSaveBigTerrain    = d._bSaveBigTerrain;

//        _compiler._bAvoidNonOrt =   d._bAvoidNonOrt;
//        _compiler._bUseUserPriority = d._bUseUserPriority;

        _compiler._bConvexEarly = 0;//d._bConvexEarly;
        _compiler._bSnapMainBsp = d._bSnapMainBsp;
        _compiler._bPvsMethod     = d._bPvsMethod;
        _compiler._bGeticBSP    = d._bGeticBSP;
        _compiler._bZones        = d._bZones;

        _compiler._bCompressPvs = d._bCompressPvs;
        _compiler._bAABBTreeCsgTer = d._bAABBTreeCsgTer;

        _compiler._removeOnePolyLeaf = d._bRemopl;
        _compiler._portalCutFlag = d._portalCutFlag;
        _compiler._balance           = d._balance;
        _compiler._bExportFile   = d._bExportFile;

        _compiler._lmpowof2     = d._lmpowof2;
        _compiler._lmConst     = d._lmConst;
         _compiler._lmapMaxSize = d._lmapMaxSize;
         _compiler._bLmPatch    = d._bLmPatch;
         _compiler._lMapLumelSz = d._lMapLumelSz;
         _compiler._b3SampleLM  = d._b3SampleLM;


        _compiler.FromRegistry(0);
    }

}

//---------------------------------------------------------------------------------------
void Document::OnBucompile()
{
    if(_compiling == 0)
    {
        _compiling = true;
        SaveTempFile();
        OnFileSave() ;
        // up all buttons view modes selections  etc
        BARDLG()->OnApply();

        _compviewmode = C_NA;
        _brmode       = BR_VIEW;
        _bSnapGrid    = false;
        _selmode      = SEL_NONE;
        _vmodefire    = false; //mco-mco change back
        _vmodeback    = false;
        _bShowCsg     = false;
        _compviewmode  = C_NA;
        _debugPolys.clear();

        LOG(I_DONE,"+");
        DisableAll(true);
        FRM()->ShowProgressDlg(true);
        FRM()->Invalidate();
        FRM()->UpdateWindow();
        _compiler.FromRegistry(1);
        _compiler.Compile(_scene);
    }
    else
    {
        CWaitCursor cwc; // if not var is excluded in release(1);
        _complBreaking = true;
        _compiler.Break();
        START_WAIT("Please Wait. Breaking Compilation...");
    }
}

//---------------------------------------------------------------------------------------
int32_t Document::OnCompileDone(WPARAM wErr, LPARAM pStr)
{
    PROG(-1);
    if(_complBreaking)
    {
        END_WAIT("");
    }
    if(0 ==_compiling)
    {
        SBT(0, "Ready...");
        return 0;
    }
    _compiling     = 0;
    _complBreaking = 0;

    _compiler.Stop();
    Sleep(32);
    OnEscape();

    Document::DisableAll(false);
    u_bool32 bchb = _compiler.HasBreaked();//
    _compiler.Break(0);

    if(bchb)
    {
        CWaitCursor cwc; // if not var is excluded in release(0);
    }

    if(wErr !=0 )
    {
        if(bchb)
        {
            LOG(0,"! Incomplette compilation. User break. ");
        }
        else
        {
            LOG(0,"!! Error compiling level ");
        }
        _compiler.Clear();
        return 0;
    }
    _compiler.CommitChanges();

    LOG(0,"BSP Details:--------------------------------------------------------");
    int32_t totPolys = 0;
    FOREACH(vvector<CBspTree*>, _compiler._bTrees, pBtree)
    {
        LOG(0,"BSP: '%s', Nodes :  %d, Polygons: %d, Leafs: %d", (*pBtree)->_name,
                                                                (*pBtree)->_nodesPtr.size(),
                                                                (*pBtree)->_polys.size(),
                                                                (*pBtree)->_leafs.size());

        totPolys+=(*pBtree)->_polys.size();
    }

    LOG(0,"Compilation Details:------------------------------------------------");
    LOG(0,"Portals: %d, Leafs: %d, PVS: %d b, BSP's: %d",_compiler._portPrc._portals.size(),
                                                            _compiler._pvsPrc._nleafs,
                                                            _compiler._pvsPrc._cPvss,
                                                            _compiler._bTrees.size());

    LOG(0,"Scene Details:------------------------------------------------------");
    LOG(0,"Number of Polygons: %d",totPolys);
    LOG(0,"Number of Textures: %d",theApp->_TexSys.size());

    SaveFinalBSP();

    if(_compiler._errDetected)
    {
        LOGERR(E_BSP,V0,"There were some warnings detected. Scroll up to ckeck ! ");
    }
    _compiler._errDetected=0;

    if(pStr)
    {
        Brush* pBrush = (Brush* )pStr;
        Pd->SelectBrush(pBrush);
        Pd->MoveCamera(pBrush->_box.GetCenter());
        Pd->Invalidate();
    }


    return 0;
}

//---------------------------------------------------------------------------------------
void Document::OnUpdateBucompile(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(_scene.GetPrimitives()->size()!=0);
    pCmdUI->SetCheck(_compiling);
}

//---------------------------------------------------------------------------------------
void Document::OnUpdateCompileroptions(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(!_compiling);
}


//---------------------------------------------------------------------------------------
void Document::OnBspcursel()
{
    int32_t nCount;
    Brush** b = GetSelBrushes(nCount);
    for(int32_t i=0;i<nCount;i++,b++)
    {
        MiniBsp test;
        u_bool32    result;

        test._treeprops = (*b)->_brushprops;
        result = test.Compile((*b)->_polys, test._treeprops);
        if(result)
            SBT(0,MKSTR("P: %d / %d D: %d N: %d",(*b)->_polys.size(), test._polys.size(), test._reachedDepth, test._nodesPtr.size() ));
        else
            TAfxMessageBox("Failed to build BSP");
    }
}


void Document::OnReclmaps()
{
    CWaitCursor cwc ;

    _compviewmode = C_NA;
    _brmode       = BR_VIEW;
    _bSnapGrid    = false;
    _selmode      = SEL_NONE;
    _vmodefire    = false;
    _vmodeback    = false;
    _bShowCsg     = false;
    _compviewmode  = C_NA;


    BARDLG()->OnApply();


    _compiler.DoLightMaps();
    SaveFinalBSP();


}

void Document::OnUpdateReclmaps(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(_compiler.Done()!=0);

}


//---------------------------------------------------------------------------------------
// makes 2 boxes one insode of other by confing settings
void Document::OnMkShellBox(v3d_t& vdim)
{
}

void Document::OnBucone()
{
    //OnFileSave();
    OnEscape() ;    //cancel any action because we've begin a new one
    if(_cachedSett._radstrips<0)
        _cachedSett._radstrips=16;
    if(_cachedSett._vstrips<0)
        _cachedSett._vstrips=1;

    _cachedSett._boxdim = GetViewsDims();

    dlgBrush._brshSett = _cachedSett;
    dlgBrush._hideZone=true;
    if(QMessageBox::Cancel == dlgBrush.DoModal())
        return;
    _cachedSett = dlgBrush._brshSett;
    _selmode = SEL_BRUSH;

    if(_cachedSett._radstrips<3 ||_cachedSett._radstrips > 32 )
    {
        TAfxMessageBox("Cone Radial Strips Bands are between 3 and 32",MB_OK|MB_ICONINFORMATION);
        return;
    }


    f32_t    rad        = tmin(_cachedSett._boxdim.x,_cachedSett._boxdim.z) / 2;
    f32_t    height     = _cachedSett._boxdim.y;

    Brush* pBrush = new Brush();

    pBrush->MakeCone(rad, height, _cachedSett._radstrips, _cachedSett._vstrips, _cachedSett._tm);
    if(_cachedSett._thikness != 0)
    {
        pBrush->StartSecGrp(_cachedSett._thikness);

        Brush tb;
        tb.MakeCone(rad-_cachedSett._thikness,
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
    pBrush->_cf._t    = E_CONE;

}

void Document::OnBusphere()
{
   OnEscape() ;    //cancel any action because we've begin a new one
    if(_cachedSett._radstrips<=0)
        _cachedSett._radstrips=16;
    if(_cachedSett._vstrips<=0)
        _cachedSett._vstrips=16;

   _cachedSett._boxdim = GetViewsDims();
    dlgBrush._brshSett = _cachedSett;
    dlgBrush._hideZone=true;
    if(QMessageBox::Cancel == dlgBrush.DoModal())
        return;
    _cachedSett = dlgBrush._brshSett;
    _selmode = SEL_BRUSH;

#ifndef DEBUG
    if(_cachedSett._radstrips < 4 || _cachedSett._vstrips < 4 ||
        _cachedSett._radstrips>18 || _cachedSett._vstrips>18){
        TAfxMessageBox("Sphere Vertical/Horizontal Strips  are between 4 and 18", MB_OK|MB_ICONINFORMATION);
        return;
    }
#endif//

    Brush* pBrush = new Brush();
    f32_t    rad   = tmin(_cachedSett._boxdim.x,_cachedSett._boxdim.z) / 2;

    pBrush->MakeSphere(rad, _cachedSett._radstrips,_cachedSett._vstrips, _cachedSett._tm);


    if(_cachedSett._thikness != 0)
    {
        pBrush->StartSecGrp(_cachedSett._thikness);

        Brush tb;
        tb.MakeSphere(rad-_cachedSett._thikness,  _cachedSett._vstrips,_cachedSett._radstrips, _cachedSett._tm);


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
    pBrush->_cf._t    = E_SPHERE;

}


void Document::OnBustairs()
{
   OnEscape() ;    //cancel any action because we've begin a new one
    if(_cachedSett._radstrips<0)
        _cachedSett._radstrips=8;
    if(_cachedSett._vstrips<0)
        _cachedSett._vstrips=1;

    _cachedSett._boxdim = GetViewsDims();

    dlgBrush._brshSett = _cachedSett;
    dlgBrush._hideZone=true;
    if(QMessageBox::Cancel == dlgBrush.DoModal())
        return;
    _cachedSett = dlgBrush._brshSett;
    _selmode = SEL_BRUSH;

    if(_cachedSett._vstrips < 2 || _cachedSett._vstrips>32)
    {
        TAfxMessageBox("Stair Case Vertical Strips  are between 2 and 32",MB_OK|MB_ICONINFORMATION);
        return;
    }


    Brush* pBrush = new Brush();


    pBrush->MakeStairs(_cachedSett._boxdim, _cachedSett._radstrips, _cachedSett._vstrips, _cachedSett._thikness, _cachedSett._tm);
    ApplyFlags(pBrush);
    pBrush->_brushflags |= BRSH_DETAIL;

    pBrush->_cf._tm      = _cachedSett._tm;
    pBrush->_cf._brushflags   = pBrush->_brushflags;
    pBrush->_cf._hstrips = _cachedSett._radstrips;
    pBrush->_cf._vstrips = _cachedSett._vstrips;
    pBrush->_cf._thick   = _cachedSett._thikness;
    pBrush->_cf._t       = E_STAIRS;
}




void Document::OnBusheet()
{
   OnEscape() ;    //cancel any action because we've begin a new one
    if(_cachedSett._radstrips<0)
        _cachedSett._radstrips=1;
    if(_cachedSett._vstrips<0)
        _cachedSett._vstrips=1;

    _cachedSett._boxdim = GetViewsDims();
    dlgBrush._brshSett = _cachedSett;
    dlgBrush._hideZone=true;
    if(QMessageBox::Cancel == dlgBrush.DoModal())

        return;
    _cachedSett = dlgBrush._brshSett;
    _selmode = SEL_BRUSH;

    if(_cachedSett._radstrips<1 || _cachedSett._vstrips < 1 ||
        _cachedSett._radstrips>32 ){
        TAfxMessageBox("Sheet Vertical/Horizontal Strips are between 1 and 32",MB_OK|MB_ICONINFORMATION);
        return;
    }

    Brush* pBrush = new Brush();

    pBrush->MakeSheet(_cachedSett._boxdim, _cachedSett._vstrips, _cachedSett._radstrips);


    pBrush->_brushflags |= BRSH_DETAIL;

    pBrush->Rotate(v3d_t(PIPE2,0,PI));//IsZero

    ApplyFlags(pBrush);

    pBrush->_cf._tm      = _cachedSett._tm;
    pBrush->_cf._brushflags   = pBrush->_brushflags;
    pBrush->_cf._hstrips = _cachedSett._radstrips;
    pBrush->_cf._vstrips = _cachedSett._vstrips;
    pBrush->_cf._thick   = _cachedSett._thikness;
    pBrush->_cf._t    = E_STAIRS;
    _pLastMakedBrush = pBrush;
}


void Document::OnInspref()
{
    OnEscape();
    DlgNewPref  dn;

    if(dn.DoModal() == QMessageBox::Cancel)
        return;

    Brush* pBrush = dn.GetBrush();
    _cachedSett._splitter = 0;
    ApplyFlags(pBrush);

    pBrush->Reverse();


    pBrush->_cf._tm      = _cachedSett._tm;
    pBrush->_cf._brushflags   = pBrush->_brushflags;
    pBrush->_cf._hstrips = 0;
    pBrush->_cf._vstrips = 0;
    pBrush->_cf._thick   = _cachedSett._thikness;
    pBrush->_cf._t    = E_PREFAB;

    Invalidate();

    _brmode= BR_MOVE;
    _docDirty = 1;

}


void Document::OnUpdateInsertMesh(CCmdUI* pCmdUI)
{
    if(_bShowCsg || _compiling || _compviewmode  != C_NA)
    {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->Enable(true);
}

void Document::OnUpdateAddModel(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(_bShowCsg);
}


void Document::OnTerian()
{
    OnEscape();
    // special dialog box for terian
    CDlgNewTerrain  ter;
    if(IDOK!=ter.DoModal())
    {
        return;
    }
    ASSERT(ter._pWorkingBrush);

    FOREACH(vvector<Poly>,ter._pWorkingBrush->_polys,pPoly)
    {
        Poly& poly = *pPoly;

        if(poly.GetHtex(GUtex)<= 0)
        {
            poly.SetTex(BARDLG()->GetCurSelTexture(), GUtex, BARDLG()->GettexGenFlag());
        }

        FOREACH(vvector<Vtx2>,poly._vtci,pvtci)
            pvtci->_rgb = ZWHITE;////_scene._si.defLumin;

    }

    ter._pWorkingBrush->_group = _curCreatGroup;
    ter._pWorkingBrush->_brushflags |= BRSH_NEW;
    _maxUsedGrp  = tmax(_maxUsedGrp, _curCreatGroup);
    //ter._pWorkingBrush->Rotate(v3d_t(0,0,PI));

    SelectBrush(ter._pWorkingBrush);

    View3D*  pT3 = VIEW3D(0,0);
    pT3->SetFocus();

    _brmode= BR_MOVE;
    Invalidate(1);

    _docDirty = 1;

}

void Document::OnUpdateTerian(CCmdUI* pCmdUI)
{
    if(_bShowCsg || _compiling || _compviewmode  != C_NA)
    {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->Enable(true);

}

void Document::OnTerianbig()
{
    OnEscape();
    CDlgBigTerrain  d;
    CBigTerrain&    terr = SCENE().Terrain();
    d._terrain = terr;
    if(d.DoModal()==IDOK)
    {

        terr = d._terrain;

        if(terr.p_dummyBrush)
        {
            SCENE().AddPrimitive(terr.p_dummyBrush);
            Invalidate();
            SelectBrush(terr.p_dummyBrush);
            _brmode     = BR_SCALE;
            _selmode	= SEL_BRUSH;
        }

    }
}

void Document::OnUpdateTerianbig(CCmdUI* pCmdUI)
{
    if(_bShowCsg || _compiling || _compviewmode  != C_NA)
    {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->Enable(true);
}

void Document::On2dshape()
{
}

void Document::OnUpdate2dshape(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(0);
}

void Document::OnUpdateParskin(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(1);
}

void Document::OnParskin()
{
    if(IDOK == DlgEditor2D().DoModal())
    {
        VIEW3D(0,0)->SetFocus();
        Invalidate();
    }
}


void Document::OnInsgameitem()
{
    SaveTempFile();

    OnEscape();
    DlgClassItem	dni ;
    if(dni.DoModal() == IDOK)
    {
        ////BARDLG()->ShowItemDlg(dni._pCurItem);
        VIEW3D(0,0)->SetFocus();
        Invalidate();
    }

}

void Document::OnUpdateInsgameitem(CCmdUI* pCmdUI)
{
    if(_bShowCsg || _compiling || _compviewmode  != C_NA)
    {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->Enable(true);


}

// spline mode
void Document::OnSpline()
{

}

void Document::OnUpdateSpline(CCmdUI* pCmdUI)
{

}

void Document::OnClosedspline()
{

}

void Document::OnUpdateClosedspline(CCmdUI* pCmdUI)
{

}

void Document::OnProjBrush()
{
    OnEscape();

    View2D* pTX = VIEW2D('z');
    View2D* pTY = VIEW2D('y');
    View2D* pTZ = VIEW2D('x');
    View3D*  pT3 = VIEW3D(0,0);

    Brush*    pBrush   = new Brush();

    f32_t xd = (pTX->_vport.right - pTX->_vport.left) / 2.0;

    pBrush->MakeCube(v3d_t(xd,xd,xd), TM_LIN);
    pBrush->Move(v3d_t(pTY->_shiftx, pTZ->_shifty, -pTY->_shifty));
    pBrush->Recalc();
    pBrush->_group = _curCreatGroup;
    pBrush->_brushflags = (BRSH_NEW|BRSH_PROJECTOR);

    FOREACH(vvector<Poly>,pBrush->_polys,pPoly)
    {
        Poly& poly = *pPoly;

        if(poly.GetHtex(GUtex) <= 0)
        {
            poly.SetTex(BARDLG()->GetCurSelTexture(),GUtex, BARDLG()->GettexGenFlag()) ;
        }
        FOREACH(vvector<Vtx2>,poly._vtci,pvtci)
            pvtci->_rgb = ZWHITE;//_scene._si.defLumin;

    }

    pBrush->Reverse();
    pBrush->ResetTexCoords();	//FOR LIGHT MAP
    _pSelBrushes.push_back(pBrush); //place in sel brushes with the new flag set
    Invalidate();
    pT3->SetFocus();

    _selmode		= SEL_BRUSH;
    _bShowCsg		= 0;
    _compviewmode   = C_NA;
    _brmode         = BR_SCALE;

    _maxUsedGrp     = tmax(_maxUsedGrp, _curCreatGroup);

    pBrush->_cf._tm      = TM_LIN;
    pBrush->_cf._brushflags   = BRSH_PROJECTOR;
    pBrush->_cf._hstrips = 0;
    pBrush->_cf._vstrips = 0;
    pBrush->_cf._thick   = 0;
    pBrush->_cf._t       = E_CUBE;


}

void Document::OnUpdateProjBrush(CCmdUI* pCmdUI)
{
     if(_bShowCsg || _compiling || _compviewmode  != C_NA)
    {
        pCmdUI->Enable(false);
        return;
    }
    pCmdUI->Enable(true);
}

void Document::OnInsitem()
{
    OnEscape();
    DlgNewItem	dni ;
    if(dni.DoModal() == IDOK)
    {
        BARDLG()->ShowItemDlg(dni._pCurItem);
        Invalidate();

        BARDLG()->Dirty();
        VIEW3D(0,0)->SetFocus();
        _selmode = SEL_ITEMS;
        _brmode = BR_SCALE;
        _docDirty = 1;

    }
}


#endif //MFC


void    Document::DeselectAll(u_bool32 killDets)
{
//    if(killDets)
//        _devdet.DestroyWindow();

    DeselectPolys();
    DeselectBrushes();
    DeselectItems();
    DeselectVertexes();
    _selEdges.clear();

    if(!_closing)
    {
        if(_selmode == SEL_BRUSH || _selmode == SEL_FACE)
        {
            BARDLG()->OnBrushesSelected();
            BARDLG()->OnFacesSelected();
        }
        else if(_selmode == SEL_ITEMS)
        {
            BARDLG()->OnItemSelected(0,0);
        }
        BARDLG()->Rescan();
        if(!_clearing)
            Invalidate();
    }

}




void    Document::DoSelection2D(f32_t x, f32_t y, View2D* pV, _EL_SEL sel)
{
    v3d_t	 start = _cam._pos;
    v3d_t	 end   = _cam._pos;


    switch(pV->_vt)
    {
        case 'x':
            start.y = y;
            start.z = x;
            if (_cam._fwd.x>0)
            {
                end.x   += 32768.f;
            }
            else
            {
                end.x -= 32768.f;
            }
            break;
        case 'y':
            start.x = x;
            start.z = y;

            if (_cam._fwd.y>0)
            {
                end.y += 32768.f;
            }
            else
            {
                end.y -= 32768.f;
            }
            break;
        case 'z':
            start.x = x;
            start.y = y;
            if (_cam._fwd.z>0)
            {
                end.z += 32768.f;
            }
            else
            {
                end.z -= 32768.f;
            }
            break;
    }
    RayIntersectBrush(start,end,sel);
}

void    Document::DoSelection3D(const QPoint& point, View3D* pV, _EL_SEL sel)
{

    QRect& rt = pV->_rt;
    if(rt.bottom()==0)rt.setBottom(1);
    v3d_t  scrPt;
    Make3dCursorPos(pV, point, rt, scrPt);


    v3d_t  start = _cam._pos;                 // build the seg
    v3d_t  vdir  = (scrPt - start).norm();
    v3d_t  end   = start + vdir * 32768.f;     // 64 meters int32_t

    if(_compviewmode==C_FULL)
    {
        /*
        if(SEL_VERTEX == _selmode || SEL_FACE == _selmode)
        {
            _compiler.RayIntersect(start, vdir, end, _selmode);
        }
        */

    }
    else
    {
        if( SEL_BRUSH  == _selmode||
            SEL_FACE   == _selmode ||
            SEL_VERTEX == _selmode||
            SEL_EDGE   == _selmode)
            RayIntersectBrush(start, end, sel);
        else if(_selmode == SEL_ITEMS)
            RayIntersectItems(start, end);
    }
}

void Document::RayIntersectItems(v3d_t& s, v3d_t& vend)
{
    int32_t       itmSel = -1;
    m_16t        tm;
    v3d_t        ipct;
    f32_t      bestDist = 999999999.0;
    SceItem*  pItemFnd = 0;
    v3d_t        vdir =   vend-s;
    v3d_t        ovj2Cam;

    vdir.norm();

    _cam.Euler2Pos();
    vvector<SceItem*>*  pTemsPtr = GetItems();

    FOREACH(vvector<SceItem*>, (*pTemsPtr), ppItem)
    {
        SceItem*  pItem  = *ppItem;

        // reject any out of +-45 degrees
        ovj2Cam = pItem->_t - s;
        ovj2Cam.norm();
        if(Vdp(ovj2Cam, _cam._fwd) < .707)
            continue;

        // buld a plane there facig toward us
        Plane pthere(-_cam._fwd, pItem->_t);

        if(pthere.RayIntersect(s, vdir, ipct))
        {

            f32_t vd = vdist(ipct, pItem->_t) ;
            if(vd < pItem->GetWBox().GetExtends().len())
            {
                f32_t dtoip = vdist(ipct, s);
                if(dtoip < bestDist)
                {
                    bestDist = dtoip;
                    pItemFnd = pItem;
                    _ip[0]   = ipct;
                    _ip[1]   = pthere._n;
                }
            }
        }
    }

    if(pItemFnd)
    {
        /*
        if(theApp->get_modifiers(Qt::Key_Control) ) // select cumulative multiple selection
        {
            if(pItemFnd->IsSelected())
            {
                DeselectItem(pItemFnd);
            }
            else
            {
                SelectItem(pItemFnd);
            }

        }
        else                                     // exclusive selection
        */
        //qDebug("TODO");
        {
            DeselectItems();
            SelectItem(pItemFnd);
        }
        Invalidate();
/*
        // update the Bar Dlg with the first selected item
        vvector<SceItem*>*  pTemsPtr = GetItems();
        FOREACH(vvector<SceItem*>, (*pTemsPtr), ppItem)
        {
            if((*ppItem)->IsSelected())
            {
                BARDLG()->OnItemSelected((*ppItem));
                break;
            }
        }
*/

    }
}

void    Document::DeselectItem(SceItem* pItemFnd, u_bool32 bNoty)
{
    pItemFnd->DeSelect();
    if(_pSelItems.erase_if(pItemFnd))
    {
        if(bNoty)
        {
            BARDLG()->OnItemSelected(pItemFnd,0);
        }
    }

    if(_pSelItems.size() == 0)
    {
        _brmode  = BR_VIEW;
    }
}

u_bool32    Document::SelectItem(SceItem* pItemFnd, u_bool32 notyBar)
{
    if(_pSelItems.push_unique(pItemFnd))
    {
        pItemFnd->Select();
        if(_bSnapGrid)
        {
            int32_t x = pItemFnd->_t.x; x/=SnapToXY; x*=SnapToXY;
            int32_t y = pItemFnd->_t.x; y/=SnapToXY; y*=SnapToXY;
            int32_t z = pItemFnd->_t.y; z/=SnapToXY; z*=SnapToXY;
            pItemFnd->_t.x = x;
            pItemFnd->_t.y = y;
            pItemFnd->_t.z = z;
        }

        if(notyBar)
        {

            BARDLG()->OnItemSelected(pItemFnd);

        }
        return 1;
    }
    return 0;
}

void Document::DeselectItems(u_bool32 notyBar)
{
    FOREACH(vvector<SceItem*>, _pSelItems, ppItem)
    {
        (*ppItem)->DeSelect();
        if((*ppItem)->_flags & BRSH_NEW)
        {
            delete (*ppItem);
        }
    }
    if(notyBar)
        BARDLG()->DeselectItems(0);
    _pSelItems.clear();
}

void Document::RayIntersectBrush(v3d_t& s, v3d_t& end, _EL_SEL sel)
{
    int32_t brSel,faceSel,vxSel;

    // see if ve've intersect some first seen brush or first seen face
    if(BrushIntersect(s, end, brSel, faceSel, vxSel))
    {
        Poly* ps;
        switch(sel)
        {
            default:
            case SEL_NONE:
                break;
            case SEL_BRUSH:
                SelectBrush(brSel, faceSel);
                BARDLG()->OnBrushesSelected();
                break;
            case SEL_FACE:
                {
                    Brush* b = GetForm(brSel);
                    if(b && b->IsSpecial())
                        break;
                }
                ps = SelectFace(brSel, faceSel);
                if(ps && theApp->isKeyDown(Qt::Key_Menu))
                {
                    AddVertex(ps, _ip[0]);
                    DeselectAll();
                }
                else
                {
                    BARDLG()->OnFacesSelected();
                }
                break;
            case SEL_VERTEX:
                SelectVertex(brSel, faceSel, vxSel);
                break;
            case SEL_EDGE:
                SelectEdge(brSel, faceSel, vxSel);
                break;

        }
    }
}



void    Document::Make3dCursorPos(View3D* pV, const QPoint& point, QRect rt, v3d_t& out)
{
    if(rt.bottom()==0) rt.setBottom(1);

    double rr = (double)rt.right() /2.0;
    double rb = (double)rt.bottom()/2.0;

    //transpofrm the cursorpos by screen center
    double    ptxpC  = ((double)point.x() - rr) / rr;
    double    ptypC  = (rb - (double)point.y()) / rb;   //inverty

    // make the fov
    double aspect     = rr/rb;

    double halfScrV   = tan(G2R((double)pV->_fov/2.0)) * (double)pV->_fnear;
    double halfScrH   = halfScrV * aspect;

    _cam.Euler2Pos();

    // calc the on screeen pos
    double scrX = ptxpC * halfScrH;
    double scrY = ptypC * halfScrV;

//    qDebug("%2.2f %2.2f \r\n", scrX, scrY);

    _frustDots[0] = _cam._pos;
    _frustDots[1] = _cam._pos + _cam._fwd*pV->_fnear + (_cam._up * halfScrV)  +   _cam._right *  (-halfScrH);
    _frustDots[2] = _cam._pos + _cam._fwd*pV->_fnear + (_cam._up * halfScrV)  +   _cam._right *  (halfScrH);
    _frustDots[3] = _cam._pos + _cam._fwd*pV->_fnear + (_cam._up * (-halfScrV))  + _cam._right *  (halfScrH);
    _frustDots[4] = _cam._pos + _cam._fwd*pV->_fnear + (_cam._up * (-halfScrV))  + _cam._right *  (-halfScrH);

    v3d_t  pointer =  _cam._pos + (_cam._fwd * pV->_fnear) + (_cam._up * scrY) + (_cam._right * scrX);
    _camray[2] = pointer;
    _camray[0] =_cam._pos;

    if(theApp->isKeyDown(Qt::LeftButton)) //getAsyncKeyState
    {
        _camray[1] = (pointer-_cam._pos);
        _camray[1].norm();
        _camray[1] = _cam._pos + _camray[1] * 6400.f;
    }


    out = pointer;



}

u_bool32    Document::BrushIntersect(v3d_t& vstart, v3d_t& vend,
                                  int32_t& bsIdx,
                                  int32_t& psIdx,
                                  int32_t& vxIdx)
{
    v3d_t      a,b,d,cp;
    f32_t    dist  = (999999999.f);
    int32_t     bIdx = 0;

    bsIdx   = -1;
    psIdx   = -1;
    vxIdx   = -1;
    a		= vstart;
    b		= vend;
    cp		= vstart;//_cam._pos;
    d		= (b-a).norm();
    Box	box;
    u_bool32	bstartInSolid = false;
    v3d_t      ip;

    _cam.Euler2Pos();
    Plane	camPlane(_cam._fwd, _cam._pos+_cam._right);

    PBrushes* rbs = this->GetForms();

    // if cut select the far face
    FOREACH(PBrushes, (*rbs), ppB)
    {
        Brush& rb = (**ppB);

        if(_curgroup>=0)
        {
            if(_curgroup != rb._group)
                continue;
        }

        if(_cam.IsCulled(&rb._box._min,&rb._box._max))
           continue;

        box		  = rb._box;
        box.Expand(8.f);


        if(!rb.IsSolid() && box.ContainPoint(a))
            bstartInSolid=true;
        else
            bstartInSolid=false;

        Polys& polys = rb._polys;


        FOREACH(Polys, polys, pB)
        {
            Poly& pl = *pB;

            if(_cam.IsCulled(&pl._box._min,&pl._box._max))
                continue;

            // shooting face
            // normally we shoot from front face DP>0
            f32_t shf  = Vdp(d, pl._n);

            if(bstartInSolid)		//we invert the shf. we shoot from behind faces
                shf=-shf;

            /*
            // select back faces from behind too
            if(shf > 0 && !(pl._props & FACE_SHOWBACK))
                continue;
            */

            if(camPlane.GetSide(pl._box.GetCenter())<0)
                continue;

            if(pl.RayIntersect(a, d, ip) )
            {
                if(camPlane.GetSide(ip)<0)
                    continue;

                if(pl.ContainPoint(ip) && box.ContainPoint(ip))
                {
                    f32_t d2   = vdist2(ip, cp);
                    if(d2 < dist )
                    {
                        bsIdx    = ppB - rbs->begin();
                        psIdx    = pB  - rb._polys.begin();
                        dist     = d2;
                        _ip[0]   = ip;
                        _ip[1]   = pl._n;
                    }
                }
            }
        }
    }

    //
    // selects the closest vertx on selected face
    //
    if(psIdx>=0 &&  (_selmode == SEL_VERTEX ||
                     _selmode == SEL_EDGE))
    {
        f32_t   dst;
        f32_t   closest = INFINIT;
        Brush* pSelB   = (*rbs)[bsIdx];
        Poly&  poly    = pSelB->_polys[psIdx];

        FOREACH(vvector<Vtx2>, poly._vtci, pV)
        {
            dst = vdist2(pV->_xyz, _ip[0]);
            if(dst < closest)
            {
                closest = dst;
                vxIdx   = pV - poly._vtci.begin();
            }
        }
    }
    return (bsIdx >= 0);
}



//allways select the primitives
void Document::SelectBrush(int32_t brSel, int32_t faceSel)
{
    if(theApp->isKeyDown(Qt::Key_Control) ) // select cumulative multiple selection
    {
        Brush* b = GetForm(brSel);
        if(!b->IsSelected())
            SelectBrush(b);
        else
            DeSelectBrush(b);
    }
    else                                     // exclusive selection
    {
        DeselectBrushes();
        SelectBrush(GetForm(brSel));
    }
}


void Document::SelectAllBrushVertexes(Brush& b, Vtx2& x)
{
    FOREACH(vvector<Poly>, b._polys,pp)
    {
        FOREACH(vvector<Vtx2>, pp->_vtci, pv)
        {
            if(IsZero(x._xyz-pv->_xyz,EPSMED))
            {
                SelectVertex(&(*pv), &(*pp));
            }
        }
    }
}

void Document::DeselectVertex(Vtx2* pv)
{
    if(_pSelVertexes.erase_if(pv))
    {
        if(pv->_pPoly)
            DeSelectFace(pv->_pPoly);
        pv->Select(0);
    }

}

void Document::SelectVertex(Vtx2* pv, Poly* pPoly)
{
    _pSelVertexes.push_unique(pv);
    SelectFace(pPoly);
    pv->Select(pPoly);
}

void Document::SelectVertex(int32_t brSel, int32_t nPoly, int32_t vxSel)
{
    try{
        if(!theApp->isKeyDown(Qt::Key_Control) )   // acumulate selections
        {
            DeselectVertexes();
        }
        Brush* b = GetForm(brSel);
        if(0==b)
            return;

        // sel vertex blocked on already rotated brush
        if((b->_r.isnot0()))
        {
            Beep(600,20);
            SBT(0,"Cannot select a vertex on rotated brush");
            return;
        }

        if((u_int32_t)nPoly >= b->_polys.size())       return;
                Poly& p = b->_polys[nPoly];
        if((u_int32_t)vxSel >= p._vtci.size())         return;
            Vtx2& x = p._vtci[vxSel];

        SelectAllBrushVertexes(*b, x);
    }
    catch(...){

    }
}



void Document::SelectEdge(int32_t brSel, int32_t nPoly, int32_t vxSel)
{
    _selEdges.clear();

    PolyEdge edge;
    //find the closest line to ip and select that edge (2 vertexex)
    //_ip[0]
    Brush* b      = GetForm(brSel);
    edge._poly    = &b->_polys[nPoly];
    edge._edge[0] = &edge._poly->_vtci[vxSel];
    int32_t vxSel2    = ((vxSel+1) % edge._poly->_vtci.size());
    edge._edge[1] = &edge._poly->_vtci[vxSel2];
    _selEdges.push_back(edge);

    SelectFace(brSel, nPoly);

}


// allways select the primitives
Poly* Document::SelectFace(int32_t brSel, int32_t faceSel)
{
    Poly* poly = 0;
    if(theApp->isKeyDown(Qt::Key_Control) ) // acumulate selections
    {
        Brush* b = GetForm(brSel);
        Poly&  p = b->_polys[faceSel];
        if(! p.IsSelected())
        {
            SelectFace(&p);
            poly = &p;
        }
        else
            DeSelectFace(&p);
    }
    else                                    // exclusive selection
    {
        Brush* b = GetForm(brSel);
        DeselectPolys();
        if(faceSel >= 0 && faceSel < (int32_t)b->_polys.size())
        {
            Brush* b = GetForm(brSel);
            if(b)
            {
                Poly&  p = b->_polys[faceSel];
                SelectFace(&p, b);
                poly = &p;
            }
        }
    }
    return poly;
}

void    Document::DeselectBrushes()
{
    FOREACH(PBrushes,  _pSelBrushes,a)
    {
        (*a)->DeSelect();
        (*a)->Dirty(0);
        if((*a)->_brushflags & BRSH_NEW)
            delete *a;
    }
   _pSelBrushes.clear();
}//Vtx2

void    Document::DeselectPolys(u_bool32 desel)
{
    if(desel)
    {
        FOREACH(PPolys,  _pSelPolys, a)
            (*a)->DeSelect();
    }
   _pSelPolys.clear();
}

void    Document::DeselectVertexes()
{
    DeselectPolys();
    FOREACH(vvector<Vtx2*>,_pSelVertexes,ppv)
        (*ppv)->Select(0);
    _pSelVertexes.clear();
}

//---------------------------------------------------------------------------------------
void Document::DeleteContents()
{
    _clearing = 1;
    if(!_closing)
    {

        DeselectAll();
        _selBox.Clear();
        _compiler.Clear();
        _scene.Clear();
        _pActiveView   = 0;
        _exViewMode	   = 0;
        _vmodefire	   = false;
        _vmodeback     = false;
        _bShowCsg      = false;
        _compviewmode  = C_NA;

        _maxUsedGrp       = 0;
        Brush::GID        = 0;
        _use2splitidx       = -1;
        SceItem::_GItemID = 0;
        _undo.Clear();

        EndEditor();

        if(Pd->_viewCount)
            Ta.Disable();
        TexRef::Clear();
        if(_closing  == false)
        {
            BARDLG()->Rescan();
            BARDLG()->ResetDlgsContent();
        }
    }


    _clearing = 0;
}

//---------------------------------------------------------------------------------------
void	Document::Recut(vvector<Brush*>* extraBrushes)
{
    int32_t         nCount = 0;
    Brush** ppb = GetSelBrushes(nCount);
    if(nCount)
    {
        UpdateCutsCollisionList(ppb, nCount);
    }

    if(extraBrushes )
    {
        nCount = extraBrushes->size();
        if(nCount)
        {
            ppb    = &((*extraBrushes)[0]);
            UpdateCutsCollisionList(ppb, nCount);
        }
    }

    Pd->_polycount = _scene.GetPolyCnt();
    SBT(0,MKSTR("Scene has %d Polygons", Pd->_polycount));

#ifdef _DEBUG
/*
    FOREACH(PBrushes, (*_scene.GetPrimitives()), ppbb)
    {
        Brush* pB =*ppbb;

        if(pB->_flags & BRSH_SOLID)
        {

            qDebug("Brush %x (%d) has %d cuts \r\n", pB, pB->_unicID, pB->_pCutBrshes.size());
            FOREACH(PBrushes, pB->_pCutBrshes, ppc)
            {
                qDebug("Cut %x(%d), ", (*ppc), (*ppc)->_unicID);
            }
            qDebug("\r\n");

            qDebug("Brush %x (%d) has %d brushes", pB, pB->_unicID, pB->_pSldBrushes.size());
            FOREACH(PBrushes, pB->_pSldBrushes, ppc)
            {
                qDebug("Brush %x(%d), ", (*ppc), (*ppc)->_unicID);
            }

        }
        else
        {
            qDebug("Cut %x (%d) has %d brushes", pB, pB->_unicID, pB->_pSldBrushes.size());

            FOREACH(PBrushes, pB->_pSldBrushes, ppc)
            {
                qDebug("Brush %x(%d), ", (*ppc), (*ppc)->_unicID);
            }
            qDebug("\r\n");

            qDebug("Cut %x (%d) has %d cuts \r\n", pB, pB->_unicID, pB->_pCutBrshes.size());
            FOREACH(PBrushes, pB->_pCutBrshes, ppc)
            {
                qDebug("Cut %x(%d), ", (*ppc), (*ppc)->_unicID);
            }
            qDebug("\r\n");
        }
    }
    */
#endif //

}

//---------------------------------------------------------------------------------------
void    Document::HideSelBox()
{
    _selBox._brushflags &= ~BRSH_SELVIS;
    Invalidate();
    SBT(0,"Edit Mode");
}
//returns the minimum projection in 2d projection views
v3d_t   Document::GetViewXYZWidths()
{
    v3d_t dims;
    View2D* pTX = VIEW2D('z');
    View2D* pTY = VIEW2D('y');
    View2D* pTZ = VIEW2D('z');

    dims.x  =   fabs(pTZ->_vport.right - pTZ->_vport.left);
    f32_t x2 =   fabs(pTY->_vport.right - pTY->_vport.left);
    dims.x = tmin(dims.x,x2);

    dims.y  =   fabs(pTZ->_vport.top   - pTZ->_vport.bottom);
    f32_t y2 =   fabs(pTX->_vport.top   -  pTX->_vport.bottom);
    dims.y = tmin(dims.y,y2);

    dims.z =   fabs(pTY->_vport.top   - pTZ->_vport.bottom);
    f32_t z2 =  fabs(pTX->_vport.right - pTX->_vport.left);
    dims.x = tmin(dims.x,z2);

    return dims;
}

//---------------------------------------------------------------------------------------
void    Document::CreateSelBox(QPoint& pt)
{
    DeselectAll();

    _pClipboard.clear();
    _pClipboardItm.clear();


    View2D* pTX = VIEW2D('z');
    View2D* pTY = VIEW2D('y');
    View2D* pTZ = VIEW2D('z');

    v3d_t center = _selBox._box.GetCenter();
    v3d_t bbxyz  = _selBox._box.GetExtends();
    v3d_t dims   = GetViewXYZWidths();

    _selBox.Scale(dims / bbxyz, V0, 1);
    _selBox.Move(v3d_t(pTY->_shiftx  - center.x,
                    pTZ->_shifty  - center.y,
                    -pTY->_shifty - center.z));

    _selBox.Recalc();
    _selBox._brushflags |= BRSH_SELVIS;



    Invalidate();
    SBT(0,"Selection Mode with Box");
}

//---------------------------------------------------------------------------------------
void Document::Escape(u_bool32 resetSels)
{
    if(_selmode == SEL_NONE)
    {
        _brmode = BR_VIEW;
    }

    VIEW2D('z')->_accmode = BR_VIEW;
    VIEW2D('x')->_accmode = BR_VIEW;
    VIEW2D('y')->_accmode = BR_VIEW;
    _pLastMakedBrush = 0;
    if(IsSelBoxOn())
    {
        _selBoxByTouch = false;
        HideSelBox();
        return;
    }

    DeleteUncomittedNewBrushes();
    DeleteUncomittedNewItems();
    DeselectAll();

    _pClipboard.clear();
    _pClipboardItm.clear();
}

u_bool32 Document::DeleteBrush(Brush* pBrush, u_bool32 undo)
{
    if(0 == pBrush)
        return 0;

    if(pBrush->IsSelection() || pBrush->IsProjector() || pBrush->IsSplitter() ||
        pBrush->_brushflags & BRSH_BIGTERRAIN)
        undo=0;

    ////if(undo)
    ////    _undo.Add(pBrush,M_DELETED);

    PBrushes* pBSs = GetForms();
    FOREACH(PBrushes, (*pBSs), ppB)	// find the brush in the collection
    {
        if((*ppB)!=pBrush)			continue;

        DeleteBrushItemLinks(*ppB);
        if((*ppB)->IsSolid() )
        {
            // dereference the cut brushes
            FOREACH(PBrushes, pBrush->_pCutBrshes, ppCB)
            {
                // _pSelBrushes.push_back(*ppCB);
                (*ppCB)->RemoveBrush(pBrush);
                (*ppCB)->UpdateSolidsTouches();
            }
            pBSs->erase(ppB);
            break;
        }
        if(pBrush->IsProjector())
        {
            this->CleanProjections(*pBrush, GKeepProjections);
            pBSs->erase(ppB);
            break;
        }
        else
        {
            FOREACH(PBrushes, (*ppB)->_pSldBrushes, ppSB)
            {
                ////_pSelBrushes.push_back(*ppSB);
                (*ppSB)->RemoveCutter(pBrush);
                (*ppSB)->UpdateCutsTouches();
                (*ppSB)->ApplyCuts();
            }
            pBSs->erase(ppB);
            break;
        }
    }
    //if(!undo)
    //{
    delete pBrush;
    return 1;
    //}
    ////return 0;
}


//---------------------------------------------------------------------------------------
void Document::DoBoxSelSelection()
{
    HideSelBox();
    DeselectAll();
    // select anu brush cinside this box or touching this box
    _selBox.Recalc();
    PBrushes* pBSs = GetPrimitives();

    if(pBSs->size())
    {
        FOREACH(PBrushes, (*pBSs), ppB)
        {
            if(_curgroup >= 0 && (*ppB)->_group != _curgroup)
                continue;

            if(_selBox._box.ContainPoint((*ppB)->_box._min) &&
               _selBox._box.ContainPoint((*ppB)->_box._max))
            {
                SelectBrush(*ppB);
            }
        }
        Invalidate();
    }

}

//---------------------------------------------------------------------------------------
u_bool32    Document::SelectBrush(Brush* pB)
{
    if(_pSelBrushes.push_unique(pB))
    {
        pB->Select();

        if(_bSnapGrid)
        {
            int32_t x = pB->_t.x; x/=SnapToXY; x*=SnapToXY;
            int32_t y = pB->_t.x; y/=SnapToXY; y*=SnapToXY;
            int32_t z = pB->_t.y; z/=SnapToXY; z*=SnapToXY;

            pB->_t.x = x;
            pB->_t.y = y;
            pB->_t.z = z;
        }


        if(pB->_pMotion)
        {
            pB->_pMotion->SelectNode(0);
        }
        return 1;
    }
    return 0;
}


//---------------------------------------------------------------------------------------
void    Document::DeSelectBrush(Brush* pB)
{
    if(_pSelBrushes.erase_if(pB))
    {
        if(pB->_pMotion)
            pB->_pMotion->_selNode=0;
        pB->DeSelect();
        pB->Dirty(0);
    }
}

//---------------------------------------------------------------------------------------
u_bool32    Document::SelectFace(Poly* pP, Brush* pbr)
{
    if(!pP)
        return 0;
    if(!pbr)
        pbr = pP->_pBrush;

    if(pbr->IsSelection() )
        return 0;

    if(_pSelPolys.push_unique(pP))
    {
        pP->Select();
        //??
        return 1;
    }
    return 0;
}

//---------------------------------------------------------------------------------------
void    Document::DeSelectFace(Poly* pB)
{
    if(_pSelPolys.erase_if(pB))
    {
        pB->DeSelect();
    }
}


//---------------------------------------------------------------------------------------
void	 Document::SelectTabBrush(int32_t direction, u_bool32 cumulate)
{
    HWND hwnd = QApplication::focusWidget();

    if(_selmode == SEL_BRUSH)
    {
        if(_pSelBrushes.size())
        {

            Brush* pB;

            if(direction>0)                 // step forward
                pB = _pSelBrushes.back();   // get tail
            else
                pB = _pSelBrushes[0];       // get head

            // search the brush pos in the scene brushes
            PBrushes::iterator fb = find(_scene.GetPrimitives()->begin(),
                                         _scene.GetPrimitives()->end(),pB);

            // if found
            if(fb != _scene.GetPrimitives()->end())
            {
                if(direction>0)
                {
                    ++fb;
                    if(fb == _scene.GetPrimitives()->end())
                    {
                        fb=_scene.GetPrimitives()->begin();
                    }
                }
                else
                {
                    if(fb == _scene.GetPrimitives()->begin())
                    {
                        fb = _scene.GetPrimitives()->end();
                    }
                    --fb;
                }
            }
            if(cumulate==0)
            {
                DeselectBrushes();
            }
            if(fb!=_scene.GetPrimitives()->end())
            {
                SelectBrush(*fb);
            }
            BARDLG()->OnBrushesSelected();
        }
    }
    else if(_selmode == SEL_FACE) // next face in this brush then roll
    {                             // again in this brush
        if(_pSelPolys.size())
        {

            Poly* pP;

            if(direction>0)
                pP = _pSelPolys.back();
            else
                pP = _pSelPolys[0];

            Brush* pB = pP->_pBrush;

            // find the selected poly in the brush
            vvector<Poly>::iterator pPly = pB->_polys.begin();
            vvector<Poly>::iterator be = pB->_polys.end();
            for(; pPly != be; )
            {
                Poly& rP = *pPly;
                if(&(*pPly) == pP)
                {
                    if(direction > 0)
                    {
                        ++pPly;
                        if(pPly == pB->_polys.end())
                        {
                            pPly = pB->_polys.begin();//roll aup
                        }
                    }
                    else
                    {
                        if(pPly==pB->_polys.begin())
                            pPly=pB->_polys.end();
                        --pPly;
                    }
                    break;
                }
                pPly++;
            }
            if(!cumulate)
                DeselectPolys();
            if(pPly != pB->_polys.end())
            {
                Poly& rP = *pPly;
                SelectFace(&rP, pB);
                BARDLG()->OnFacesSelected();
            }
        }
    }
    else if(_selmode == SEL_VERTEX)
    {
        if(_pSelVertexes.size() && _pSelVertexes.size() == _pSelPolys.size())
        {
            Vtx2 *pvx   = _pSelVertexes[0];
            Poly* pPoly = pvx->_pPoly;

            DeselectVertexes();

            assert(pPoly);
            if(pPoly)
            {
                vvector<Vtx2>::iterator pvtx;
                FOREACH(vvector<Vtx2>, pPoly->_vtci, pV)
                {
                    if(pV->_xyz == pvx->_xyz)
                    {
                        pvtx = pV;
                        break;
                    }
                }

                if(pvtx == pPoly->_vtci.end()-1)
                    pvtx=pPoly->_vtci.begin();
                else
                    pvtx++;
                SelectAllBrushVertexes(*pPoly->_pBrush, (*pvtx));
            }

        }
    }
    if(_selmode == SEL_EDGE)
    {
        if(_selEdges.size())
        {
            PolyEdge& curEdge = _selEdges[0];
            Poly*     pNextPoly = 0;

            // curEdge._poly->_pBrush
            int32_t i1 = curEdge._poly->GetVxIndex(*curEdge._edge[0]);
            int32_t i2 = curEdge._poly->GetVxIndex(*curEdge._edge[1]);

            if(direction>0) // forward
            {
                if(i2==0) //switch to next poly in brush
                {
                    DeSelectFace(curEdge._poly);
                    curEdge._poly = curEdge._poly->_pBrush->GetNextPoly(curEdge._poly);
                    SelectFace(curEdge._poly);

                    i1=0;
                    i2=1;
                }
                else
                {
                    ++i1;
                    if(++i2 >=  curEdge._poly->_vtci.size())
                        i2=0;
                }
            }
            else //backward
            {
                if(--i1==0)
                    i1=curEdge._poly->_vtci.size()-1;
                if(--i2==0)
                {
                    // switch to prev poly
                    DeSelectFace(curEdge._poly);
                    curEdge._poly = curEdge._poly->_pBrush->GetPrevPoly(curEdge._poly);
                    SelectFace(curEdge._poly);
                    i1 = curEdge._poly->_vtci.size()-2;
                    i2 = curEdge._poly->_vtci.size()-1;
                }
            }

            curEdge._edge[0] = &curEdge._poly->_vtci[i1];
            curEdge._edge[1] = &curEdge._poly->_vtci[i2];

        }
    }

    else if(_selmode == SEL_ITEMS)
    {
        if(_pSelItems.size())
        {
            vvector<SceItem*>*  pSi = _scene.GetItems();
            SceItem*            pB;

            // get first or last selected item
            if(direction>0)
                pB = _pSelItems.back();
            else
                pB = _pSelItems[0];

            // search the item pos in the scene brushes
            vvector<SceItem*>::iterator fb = find(pSi->begin(), pSi->end(), pB);

            if(fb != pSi->end())
            {
                if(direction>0)
                {
                    ++fb;
                    if(fb == pSi->end())
                        fb = pSi->begin();
                }
                else
                {
                    if(fb == pSi->begin())
                    {
                        fb = pSi->end();
                    }
                    --fb;
                }
            }
            if(cumulate==0)
            {
                DeselectItems();
            }
            if(fb != pSi->end())
            {
                SelectItem(*fb);
            }
        }
    }

    if(hwnd != QApplication::focusWidget())
    {
       qDebug("setfocus");
        //QApplication::setCurrentWidget()
    }

    Invalidate();
}


//---------------------------------------------------------------------------------------
// reevaluates if this brush should be marked detail or not
// baed on global settings, by size or ny number o polygons
void Document::EvaluateDetailbrsh(Brush* pb)
{
    if(pb->_brushprops & MODEL_IS_SKYDOM)
        return;

    v3d_t ex = pb->_box.GetExtends();
    f32_t    dimM = ex.getmax();
    f32_t    dimm = ex.getmin();

    if(dimm <= (f32_t)GMinDetDim && dimM < (f32_t)GMaxDetDim)
    {
        pb->_brushflags |= BRSH_DETAIL;
        return;
    }

    if(pb->_polys.size() >= (u_int32_t)GMinDetPolys)
    {
        pb->_brushflags |= BRSH_DETAIL;
        return;
    }
}


//---------------------------------------------------------------------------------------
void Document::DisableAll(u_bool32 bd)
{
    /*
    CMenu* cm  = FRM()->GetSystemMenu(false);
    if(_compiling==true)
        cm->EnableMenuItem(SC_CLOSE,MF_DISABLED|MF_GRAYED);
    else
        cm->EnableMenuItem(SC_CLOSE,MF_ENABLED);
        */

}


//---------------------------------------------------------------------------------------
void Document::RotateByAngleCw(f32_t rad)
{
    /*
    v3d_t  vrot;
    if(_pActiveView == VIEW2D('x'))
    {
    }
    else if(_pActiveView == VIEW2D('y'))
    {
    }
    else if(_pActiveView == VIEW2D('z'))
    {
    }
    */
}

u_bool32 Document::AddItem(SceItem* pItem)
{
    _docDirty = 1;
    pItem->_flags|= BRSH_NEW;
    pItem->_group = _curCreatGroup;
    DeselectItems();
    SelectItem(pItem);
    _selmode = SEL_ITEMS;
    return 1;
}

void Document::DelItem(SceItem* pItem)
{
    _docDirty = 1;
    //_undo.Add(pItem,M_DELETED);
    BARDLG()->DelItem(pItem);
    _scene.DelItem(pItem);
//    delete pItem;
    Invalidate();
}



void  Document::MoveCamera(const v3d_t& ptPos)
{
    v3d_t  ditTo = _cam._pos - ptPos;
    ditTo.norm();

    v3d_t np = ptPos + ditTo * 100;
    _cam._pos = np;
    Invalidate();

}


void  Document::DeleteBrushMotion(Brush* pB)
{
    _docDirty = 1;
    if(pB->_pMotion )
    {
        CMotion* pM = pB->_pMotion;
        _scene.DelMotion(pM);
        pB->_pMotion = 0;
    }
}

u_bool32  Document::AddBrushMotion(Brush* _pCurBrush, CMotion* pNewMotion)
{
    _docDirty = 1;
    //SaveTempFile();

    assert(_pCurBrush->_pMotion == 0);

    _pCurBrush->_pMotion = pNewMotion;
    _scene.AddMotion(pNewMotion);
    return 1;
}

void Document::UpdateDynamicObjPos(Brush* pB)
{

    pB->_pMotion->RefreshPosition();
    BARDLG()->UpdateMotionProps(pB, pB->_pMotion);
}



Brush* Document::GetLastSelBrush()
{
    int32_t       nCount;
    Brush**   ppB =  GetSelBrushes(nCount);
    if(nCount)
    {
        return *((ppB+nCount)-1);
    }
    return 0;
}

Brush* Document::GetFirstSelBrush()
{
    int32_t       nCount;
    Brush**   ppB =  GetSelBrushes(nCount);
    if(nCount)
    {
        return *(ppB);
    }
    return 0;

}
void Document::DeleteBrushItemLinks(Brush* pBrush)
{

    //
    // motion is linked to the motions and cannot exists without brush
    //
    DeleteBrushMotion(pBrush);
    if(pBrush->_brushflags & BRSH_HASTRIG)
    {
        Pd->DetachTrigger(pBrush);
    }
    if(pBrush->_brushflags & BRSH_BIGTERRAIN)
    {
        CBigTerrain* pBt = (CBigTerrain*)pBrush->_pUsrData;
        pBt->Clear(0);
    }
}

//
// Brush hsa been scaled or moved. Move all associated links to it to this brush
//
void Document::UpdateBrushLinks(Brush* pBrush)
{
    //
    // move the entire motion object
    //
    if(pBrush->_pMotion )//1-2-1
    {
        UpdateDynamicObjPos(pBrush);
    }

    //
    // if the brush has a triger find that trigger.
    // this is fast as int32_t there are not many triggers
    //
    if(pBrush->_brushflags & BRSH_HASTRIG)
    {
        vvector<SceItem*>*  pTemsPtr = GetItems();

        FOREACH(vvector<SceItem*>, (*pTemsPtr), ppItem)
        {
            SceItem*  pItem  = *ppItem;
            if(pItem->_item == ITM_TRIGER)
            {
                TriggerItem* pTi = (TriggerItem*)pItem;
                if(pTi->GetEfectBrush()==pBrush)
                {
                    v3d_t& efp = pTi->EfectPoint();
                    efp = pBrush->_box.GetCenter();
                }
            }
        }
    }

}

void  Document::CameraPointTo(v3d_t& v)
{
//	_cam.LookAt(v);




//	VIEW3D(0,0)->RedrawDC();
}

void Document::EndEditor()
{
}


u_bool32 Document::StartEditor()
{
    return 0;
}

void Document::DetachTrigger(Brush* pB)
{
    vvector<SceItem*>*  pTemsPtr = GetItems();
    FOREACH(vvector<SceItem*>, (*pTemsPtr), ppItem)
    {
        if((*ppItem)->_item == ITM_TRIGER)
        {
            TriggerItem* pTi = (TriggerItem*)(*ppItem);
            if(pTi->_pBrush == pB)
            {
                pTi->_pBrush=0;
                return;
            }
        }
    }
}


void Document::ModifyBrushes(vvector<Brush*>& pChanged)
{
    FOREACH(vvector<Vtx2*>, _pSelVertexes, ppVx)
    {
        Poly* pPoly = (*ppVx)->_pPoly;
        if(pPoly)
            pPoly->_polyflags |= POLY_DIRTY;
    }

    FOREACH(vvector<Vtx2*>, _pSelVertexes, ppVx)
    {
        Poly* pPoly = (*ppVx)->_pPoly;

        if(pPoly && (pPoly->_polyflags & POLY_DIRTY))
        {
            pPoly->_polyflags &= ~POLY_DIRTY;
            pChanged.push_unique(pPoly->_pBrush);
        }
    }
}

void Document::SplitBySplitterBrush(Brush* pb)
{
    //SaveTempFile();

    CMotion* pM = 0;


    PBrushes brushes;
    PBrushes touchBrushes;
    DeselectAll();

    FOREACH(PBrushes, (*_scene.GetPrimitives()), ppb)
    {
        if((*ppb)->IsCut())
            continue;

        if(!(*ppb)->_box.IsTouchesBox(pb->_box))
            continue;

        touchBrushes << (*ppb);
    }

    u_bool32 bisP  = theApp->isKeyDown('P');
    u_bool32 bisB  = theApp->isKeyDown('B');

    if(false == (bisP|bisB))
    {
        u_int32_t id = TAfxMessageBox("Do you want to split the Brushe(s) \n (Press No To Split the Polygons) \n\n All brushes motion will be deleted", MB_ICONQUESTION|MB_YESNOCANCEL);
        if(IDYES==id)
        {
            bisB=true;
            bisP=false;
            this->DeleteBrushMotion(pb);
        }
        else if(IDNO==id)
        {
            bisB=false;
            bisP=true;
            this->DeleteBrushMotion(pb);
        }
        else
            return;
    }

    FOREACH(PBrushes, touchBrushes, ppb)
    {
        Brush*    frontB = new Brush();
        Brush*    backB  = new Brush();

        frontB->_brushflags = BRSH_SOLID|BRSH_NEW;
        backB->_brushflags = BRSH_SOLID|BRSH_NEW;

        // cut ppb by pb but retain both fragments
        if((*ppb)->Split(*pb, *frontB, *backB))
        {
            // clear the original brush
            (*ppb)->Clear();

            if(bisP)       // split the polygons
            {
                // make one brush with the fragments
                append(frontB->_polys, backB->_polys);
                (*ppb)->Clear();
                (*ppb)->Equal(*frontB, true);
                (*ppb)->Recalc();

                (*ppb)->_brushflags &= ~BRSH_NEW;

                SelectBrush((*ppb));

                delete frontB;
                delete backB;
            }
            else if(bisB)
            {
                DeleteBrush((*ppb));
                // retain 2 separate brushes
                if(frontB->_polys.size())
                {
                    frontB->_brushflags &= ~BRSH_NEW;
                    _scene.AddPrimitive(frontB);
                }
                else
                {
                    delete frontB;
                }

                if(backB->_polys.size())
                {
                    backB->_brushflags &= ~BRSH_NEW;
                    _scene.AddPrimitive(backB);
                }
                else
                {
                    delete backB;
                }
                assert(0);
                //OnEnter();
                //OnEscape();
            }
        }else
        {
            delete frontB;
            delete backB;
        }
    }
}


void Document::AddVertex(Poly* p, const v3d_t& v)
{
    // add a vertex get result
    Brush* pBrush = p->_pBrush;
    vvector<Poly>   fragments;
    p->AddVertex(v, fragments);

    pBrush->Deleteface(p);
    pBrush->AddPolys(fragments);
    pBrush->Recalc();
    Recut();
}


void Document::CleanProjections(Brush& projBrush, u_bool32 onlyFlags)
{
    Box&        box = projBrush._box;

    if(projBrush.IsProjector())
    {
    AGAIN:
        FOREACH(PBrushes, projBrush._pSldBrushes, ppB)
        {
            if(!(*ppB)->_box.IsTouchesBox(box))
            {
                // restore its polys
                vvector<Poly>::iterator pPly = (*ppB)->_polys.begin();
                vvector<Poly>::iterator be = (*ppB)->_polys.end();
                for(; pPly != be; ++pPly)
                {
                    if(pPly->_polyflags & FACE_PROJTEX)
                    {
                        pPly->_polyflags      &= ~FACE_PROJTEX;

                        if(!onlyFlags)
                        {
                            pPly->SetHtex(pPly->_nonProjTex, GUtex);
                            FOREACH(vvector<Vtx2>, pPly->_vtci, pv)
                            {
                                pv->_uv[GUtex] = pv->_uv[SAFE_TIDX];
                            }
                        }
                        else
                        {
                            pPly->SetHtex(pPly->_nonProjTex, GUtex);
                        }
                    }
                }
                projBrush.RemoveBrush(*ppB);
                goto AGAIN;
            }
        }
    }
    else if(projBrush.IsSolid())
    {
    AGAIN2:
        FOREACH(PBrushes, projBrush._pProjBrushes, ppB)
        {
            if(!(*ppB)->_box.IsTouchesBox(box))
            {
                projBrush.RemoveProjection(*ppB);
                goto AGAIN2;
            }
        }

        vvector<Poly>::iterator pPly = projBrush._polys.begin();
        vvector<Poly>::iterator be = projBrush._polys.end();
        for(; pPly != be; ++pPly)
        {

            if(pPly->_polyflags & FACE_PROJTEX)
            {
                pPly->_polyflags      &= ~FACE_PROJTEX;
                if(!GKeepProjections)
                {
                    pPly->SetHtex(pPly->_nonProjTex,GUtex);
                    FOREACH(vvector<Vtx2>, pPly->_vtci, pv)
                    {
                        pv->_uv[GUtex] = pv->_uv[SAFE_TIDX];
                    }
                }
                else
                {
                    pPly->_nonProjTex = _dumptex;
                }
            }
        }
    }
}

// texture is changed on the projector. Affects all faces where
// us projecting to;
void Document::ProjectTexture(Brush& projBrush)
{

    Box&        box = projBrush._box;

    PPolys          pSelPolys;

    // CleanProjections(projBrush);
    // get out untouched brushes, and restore th polygons tex coord
    // and original texture
    FOREACH(PBrushes, projBrush._pSldBrushes, ppB)
    {
        vvector<Poly>::iterator pPly = (*ppB)->_polys.begin();
        vvector<Poly>::iterator be = (*ppB)->_polys.end();
        for(; pPly != be; ++pPly)
        {
            if(pPly->_polyprops & POLY_NOPROJ)
                continue;
            if(pPly->_polyflags & FACE_PROJTEX)
            {
                pPly->_polyflags &= ~FACE_PROJTEX;
                pPly->SetHtex(pPly->_nonProjTex, GUtex);
                FOREACH(vvector<Vtx2>, pPly->_vtci, pv)
                {
                    pv->_uv[GUtex] = pv->_uv[SAFE_TIDX];
                }
            }



            if(box.ContainPoint(pPly->_box._min) &&
               box.ContainPoint(pPly->_box._max))
            {
                pSelPolys << &(*pPly);
            }
        }

    }

    // all faces are selected
    // project them on poly 2
    if(projBrush._polys.size()<=2)
    {
        return;
    }
    Poly    proj = projBrush._polys[2];

    v3d_t      o = proj._vtci[2]._xyz;
    v3d_t      s = (proj._vtci[1]._xyz - o).norm();
    v3d_t      t = (proj._vtci[3]._xyz - o).norm();

    v2d_t&     tm  = proj._vtci[2]._uv[GUtex];

    m_16t  m = MLook(o,-proj._n, t, s);

    // project the projection poly on it's plane. (z should be 0)
    FOREACH(vvector<Vtx2>, proj._vtci, pv)
    {
        m.v3transform(pv->_xyz);
    }

    //      get tmax extends of s and t
    v3d_t      mv = proj._vtci[0]._xyz;
    v3d_t      tv;

    // transform all polys in this 'O'
    FOREACH(PPolys,  pSelPolys, p)
    {
        Poly* pp   = (*p);

//        if(Vdp(pp->_n, proj._n) > 0)
//            continue;  // skip back faces

        FOREACH(vvector<Vtx2>, pp->_vtci, pv)
        {
            tv = pv->_xyz;
            m.v3transform(tv);

            pv->_uv[SAFE_TIDX] = pv->_uv[GUtex];

            pv->_uv[GUtex].u = (tv.x / mv.x) * proj._texcoord.scalex;
            pv->_uv[GUtex].v = (tv.y / mv.y) * proj._texcoord.scaley;

            pv->_uv[GUtex].u += proj._texcoord.shiftx;
            pv->_uv[GUtex].v += proj._texcoord.shifty;

        }

        pp->_polyflags      |= FACE_PROJTEX;
        pp->_nonProjTex      = pp->GetHtex(GUtex);
        pp->SetHtex(proj.GetHtex(GUtex), GUtex);
    }
}

// here are cmming the nCount we've dragged rotate or scaled around
// see what cuts intersec what brushes
void Document::UpdateCutsCollisionList(Brush** pMovedBrushes, int32_t nCount)
{
    map<Brush*, int32_t>           pBrsh2Update;
    int32_t			nBrushes     = nCount;
    Brush**     pWlkBrush	 = pMovedBrushes;
    u_bool32		bDlgOn       = false;
    u_int32_t		timeStart    = ge_gettick();


    // collect all brushes tha have to be updated
    while(nBrushes--)            // moved brushes
    {
        Brush* pMovedBrush = *pWlkBrush++;

        if(pMovedBrush->IsSelection())
        {
            continue;
        }
        if(pMovedBrush->_brushflags & BRSH_DISABLED)
        {
            if(ThrID == GetCurrentThreadId())
                continue;
        }

        CleanProjections(*pMovedBrush, GKeepProjections);

        // IF CUT or (PROJECTOR)
        if(pMovedBrush->IsCut() || pMovedBrush->IsProjector())
        {
            vvector<Brush*>::iterator ppb = pMovedBrush->_pSldBrushes.begin();
            vvector<Brush*>::iterator ppE = pMovedBrush->_pSldBrushes.end();

            for(;ppb!=ppE;ppb++)
            {
                pBrsh2Update[*ppb] = 1;
            };
            pMovedBrush->UpdateSolidsTouches();
        }
        else // IF SOLID
        {
            pBrsh2Update[pMovedBrush] = 1;
            pMovedBrush->UpdateCutsTouches();
        }
    }

    nBrushes	  = nCount;
    pWlkBrush	  = pMovedBrushes;
    while(nBrushes--)
    {
        Brush* pMovedBrush = *pWlkBrush++;

        if(pMovedBrush->_brushflags & BRSH_DISABLED)
        {
            if(ThrID == GetCurrentThreadId())
            continue;
        }
        if(pMovedBrush->IsSelection())
            continue;
        forall2( *(GetPrimitives()), this,
                 &Document::UpdateTouches,
                 pMovedBrush, pBrsh2Update);
    }


    map<Brush*,int32_t>	toDel;	// to delete due the unsuccess BSP build

    // now cut the brushes with the cuts
    int32_t	nCountB = 0;
    map<Brush*,int32_t>::iterator mb =  pBrsh2Update.begin();
    map<Brush*,int32_t>::iterator me =  pBrsh2Update.end();
    for(;mb!=me;mb++)
    {
        //	KEEP_WAIT(MKSTR("Building minimal CSG %x",mb));

        if((*mb).first->_brushflags & BRSH_DISABLED)
        {
            if(ThrID == GetCurrentThreadId())
                continue;
        }

        //
        // Uncomment to forbid terrain cutting
        //
        //if(!((*mb).first->_flags & BRSH_CSGTERRAIN))
        // ONLY projector has brushes
        if((*mb).first->IsProjector())
        {
            ProjectTexture(*(*mb).first);
        }
        else
        {
            if(!(*mb).first->ApplyCuts())
            {
                // delete it's cuts
                FOREACH( PBrushes, (*mb).first->_pCutBrshes, ppBrush)
                    toDel[(*ppBrush)] = 1;	// keep unicity
            }
        }

        if(ThrID == GetCurrentThreadId() && timeStart != 0)
        {
            if(ge_gettick() - timeStart > 2000)
            {
                timeStart=0;
                bDlgOn = 1;
                START_WAIT("Please Wait");
            }
        }
        nCountB++;
    }

    //now delete
    if(toDel.size())
    {
        if(bDlgOn)
            END_WAIT("");

        if(TAfxMessageBox("The geometry is too compilated to be cut\n"
                         "Do you want to delete the cut brush ?\n"
                         "If you select 'No' Try to move out of contact the one of the brushes", MB_YESNO|MB_ICONHAND)==IDYES)
        {
            Pd->DeselectAll();
            mb =  toDel.begin();
            me =  toDel.end();
            for(;mb!=me;mb++)
            {
                Pd->SelectBrush((*mb).first);
            }
            assert(0);
            //AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_DELETE,0);
        }
    }

    if(bDlgOn)
        END_WAIT("");
}

// link brush with it's cutters
// br->cut is need when 3d painting to show the already cutt brush
u_bool32 Document::UpdateTouches(Brush* pPrim,
                              Brush* pMovedBrush,
                              map<Brush*, int32_t>& accList)
{
    if(pMovedBrush == pPrim)
        return true;

    if((pMovedBrush->IsCut() && pPrim->IsCut()) ||
       (pMovedBrush->IsSolid() && pPrim->IsSolid()) ||
       (pMovedBrush->IsSelection() || pPrim->IsSelection()) )
       return true;


    if(pPrim->_brushflags & BRSH_DISABLED ||
       pMovedBrush->_brushflags & BRSH_DISABLED)
    {
        if(ThrID == GetCurrentThreadId())
            return true;
    }

    // only in this group
    if(GDisableGrpCSG )
    {
        if(pMovedBrush->_group>0 && pPrim->_group>0)
        {
            if(pMovedBrush->_group != pPrim->_group)
                return true;
        }
    }
/**
    if(pMovedBrush->_flags & BRSH_CSGTERRAIN || pPrim->_flags & BRSH_CSGTERRAIN )
        return 1;
*/
    if(pPrim->_box.IsTouchesBox(pMovedBrush->_box))
    {
        if(pPrim->IsSolid())
        {
            if(pMovedBrush->IsCut())
            {
                if(pMovedBrush->_brushflags & BRSH_CUTALL)
                {
                    pPrim->AddCutter(pMovedBrush);
                    pMovedBrush->AddBrush(pPrim);	// keep a list for cutter whwn we move away from cutter
                    accList[pPrim]=1;
                }
                else
                if(pMovedBrush->_unicID > pPrim->_unicID)
                {
                    pPrim->AddCutter(pMovedBrush);
                    pMovedBrush->AddBrush(pPrim);	// keep a list for cutter whwn we move away from cutter
                    accList[pPrim]=1;
                }
            }
            else if(pMovedBrush->IsProjector() && (pMovedBrush->_unicID > pPrim->_unicID))
            {
                ///pPrim->AddBrush(pMovedBrush);
                pMovedBrush->AddBrush(pPrim);       // prim is projector->on a solid brush
                accList[pMovedBrush]=1;
            }
        }
        else if(pPrim->IsCut())
        {
            if(pMovedBrush->IsProjector())
            {
                // nothing
            }
            else
            {
                if(pPrim->_brushflags & BRSH_CUTALL)
                {
                    pMovedBrush->AddCutter(pPrim);
                    pPrim->AddBrush(pMovedBrush);   // keep a list for cutter whwn we move away from cutter
                    accList[pMovedBrush]=1;
                }
                else
                if(pPrim->_unicID > pMovedBrush->_unicID)
                {

                    pMovedBrush->AddCutter(pPrim);
                    pPrim->AddBrush(pMovedBrush);   // keep a list for cutter whwn we move away from cutter
                    accList[pMovedBrush]=1;
                }
            }
        }
        else if(pPrim->IsProjector() && (pPrim->_unicID > pMovedBrush->_unicID))
        {
            //pMovedBrush->AddProjection(pPrim);
            pPrim->AddBrush(pMovedBrush);       // keep a list touched onproj brushes
            accList[pPrim]=1;
        }
    }
    return true;
}

void  Document::SbarShowCurMode()
{
    /*
    int32_t   i;
    char selText[128] = "";
    u_bool32  found;

    switch(_brmode)
    {
    case BR_VIEW:
        _tcscpy(selText,"View Mode, ");
        break;
    case BR_MOVE:
        _tcscpy(selText,"Move Mode, ");
        break;
    case BR_SCALE:
        _tcscpy(selText,"Scale Mode, ");
        break;
    case BR_ROTATE:
        found=false;
        for(i=2; i<9; i++)
        {
            if(theApp->get_modifiers(i+'0'))
            {
                if(theApp->get_modifiers(VK_SHIFT))
                {
                    _tcscpy(selText,MKSTR("Rotates -PI/%d, ",i));
                }
                else
                {
                    _tcscpy(selText,MKSTR("Rotates +PI/%d, ",i));
                }
                found=true;
                break;
            }
        }
        if(found)
            break;
        _tcscpy(selText,"Rotate Mode, ");
        break;
    case BR_RISE:
        _tcscpy(selText,"Not Used, ");
        break;
    }


    switch(_selmode)
    {
    case SEL_NONE:
        _tcscat(selText,"");
        break;
    case SEL_BRUSH:
        _tcscat(selText,"Brush Selection");
        break;

    case SEL_FACE:
        _tcscat(selText,"Face Selection");
        break;

    case SEL_VERTEX:
        _tcscat(selText,"Vertex Selection");
        break;
    case SEL_ITEMS:
        _tcscat(selText,"Item Selection");
        break;
    case SEL_BR_ITEM:
        _tcscat(selText,"N/A");
        break;

    }

    SBT(1,selText);
*/
}





void Document::Repos20()
{
    _zoom=.04;
    _cam.SetAngles(-PIPE4,-PIPE6,0);
    _cam.Euler2Pos();
    _cam._pos = v3d_t(1000.0f,1000.0f,1600.0f);

    View2D* pTX = VIEW2D('x');
    pTX->_shifty = 0;
    pTX->_shiftx = 0;
    pTX->_zoom   = 0.4;


    View2D* pTY = VIEW2D('y');
    pTY->_shifty = 0;
    pTY->_shiftx = 0;
    pTY->_zoom   = 0.4;

    View2D* pTZ = VIEW2D('z');
    pTZ->_shifty = 0;
    pTZ->_shiftx = 0;
    pTZ->_zoom   = 0.4;
    _shift.reset();
    _zoom.setval(V1);
    Invalidate(1);
}


void Document::MarkDetPolys(int32_t percent)
{
    // mark by area sorting
    // when commit, build from all having a common ege a detail brush
    // delete polys form it own original brush
}


void     Document::TestBackupFile()
{
    char iittaa[32];
    char                loco[_MAX_PATH];
    int32_t                 lastID = 0;
    vvector<string>     filenames;

    ::strcpy(loco, GetTitle().toUtf8());
    PathHandler ph(loco);

    do{
        theApp->ReDir();
        CDirChange	    cd("temp/");
/*
        //search backups
        HANDLE			    hfFind;
        WIN32_FIND_DATA	    fDATA;
        cd.UseObj();

        if((hfFind=FindFirstFile(_T("*.grf"), &fDATA)) != INVALID_HANDLE_VALUE)
        {
            if(fDATA.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
            {
                do{
                    char* tild = strchr(fDATA.cFileName,'~');
                    if(tild && strstr(fDATA.cFileName, ph.JustFile()))
                    {
                        ++tild;
                        lastID =  tmax(lastID, ::atoi(tild));
                    }
                }  while(FindNextFile(hfFind, &fDATA)!=0);
            }
        }
        FindClose(hfFind);
        */
        //TODO LINUX
    }while(0);

 //temp
    _tempfileName =  "temp/";
    _tempfileName += "~";
    ::sprintf(iittaa, "%02d", lastID);
    _tempfileName += iittaa;
    _tempfileName += ph.File();

    FileWrap fw;
    if(fw.Open(_tempfileName.toUtf8(),"rb"))
    {
        fw.Close();
    }
    else
    {
        _tempfileName="";
    }

}

//round to near 8 cm
void     Document::RoundToNear(v3d_t &v, int32_t snp)
{
    if(_bSnapGrid)
    {
        int32_t x = v.x;
        int32_t y = v.y;
        int32_t z = v.z;

        x/=SnapToXY;
        y/=SnapToXY;
        z/=SnapToXY;

        x*=SnapToXY;
        y*=SnapToXY;
        z*=SnapToXY;

        v.x = x;
        v.y = y;
        v.z = z;
    }
}


static void $Complement(u_int8_t* pClr)
{
    pClr[0] = 255-pClr[0];
    pClr[1] = 255-pClr[1];
    pClr[2] = 255-pClr[2];
}



u_bool32 Document::HasExtension(const char* szex)
{
    return _extensions.find(szex)!=-1;
}

void Document::AddBrush(Brush* pB)
{
    _scene.AddPrimitive(pB);
    Brush::GID = tmax(Brush::GID, pB->_unicID);
    _docDirty = 1;
    }

static void ClrCpy(RGBA& d, RGBA& s)
{
    memcpy(&d, &s , sizeof(RGBA));
}

Htex    Document::RecreateAmbLmap()
{
    if(_ambLmap)
        TexRef::RemoveTex((Htex*)&_ambLmap ,1);

    u_int8_t* loco = new u_int8_t[16*16*3];
    int32_t k=0;
    for(int32_t i=0;i<16;i++)
    {
        for(int32_t j=0;j<16;j++)
        {
            loco[k++] = _scene._si.defLumin.r;
            loco[k++] = _scene._si.defLumin.g;
            loco[k++] = _scene._si.defLumin.b;
        }
    }
    Htex rv =  TexRef::GlGenTex( 16, 16, 3 , loco, 0);
    delete[] loco;
    return rv;
}


QString Document::ResFile(const char* filenme)
{
    QString retval;
    QString cst = GetTitle();
    if(cst == "NoName")
    {
        retval = theApp->HomeDir();
        retval += "\\res\\";
        retval += filenme;
        return retval;
    }
    PathHandler ph(cst.toUtf8());
    retval = ph.Path();
    retval += MKSTR("%s_res\\",ph.JustFile());
    mkdir(retval.toUtf8(), 0777);
    retval += filenme;
    return retval;
}


u_bool32    Document::DuplicateFile(const char* src, u_bool32 over)
{
    /*
    QString resDir = theApp->HomeDir();

    resDir += "\\res\\";
    PathHandler ph(src);
    resDir += ph.File();

    if(resDir!=src)
        ::CopyFile(src, resDir, over);
    QString& rsf = ResFile(ph.File());
    if(rsf != src)
        ::CopyFile(src, ResFile(ph.File()), over);
    return 1;
    */
    assert(0);
}

void    Document::CopyResources(const char* srcFile)
{
    CWaitCursor cwc;

    PathHandler ph(srcFile);
    QString resDir = theApp->HomeDir();
    resDir += "\\res\\";
    //::G_CopyFolder(resDir.toUtf8(), MKSTR("%s%s\\",ph.Path(),ph.JustFile()), "*.*");
}



void Document::_SeColor(COLORREF crf, PBrushes& pSelBrushes)
{
    FOREACH(PBrushes, _pSelBrushes, ppe)
    {
        FOREACH(vvector<Poly>, (*ppe)->_polys, pe)
        {
            FOREACH(vvector<Vtx2>, (pe)->_vtci, pv)
            {
                pv->_rgb.r = GetRValue(crf);
                pv->_rgb.g = GetGValue(crf);
                pv->_rgb.b = GetBValue(crf);
            }
        }
    }
}

void Document::_SeColor(COLORREF crf, vvector<SceItem*>& pSelItems)
{
    FOREACH(vvector<SceItem*>, pSelItems, ppe)
    {
        (*ppe)->_colorD.r = GetRValue(crf);
        (*ppe)->_colorD.g = GetGValue(crf);
        (*ppe)->_colorD.b = GetBValue(crf);
    }
}


void Document::_SeColor(COLORREF crf, vvector<Vtx2*>& pSelVertexes)
{
    FOREACH(vvector<Vtx2*>, pSelVertexes, ppe)
    {
        (*ppe)->_rgb.r = GetRValue(crf);
        (*ppe)->_rgb.g = GetGValue(crf);
        (*ppe)->_rgb.b = GetBValue(crf);
    }
}

void Document::_SeColor(COLORREF crf, PPolys& pSelPolys)
{
    FOREACH(PPolys, pSelPolys, pe)
    {
        FOREACH(vvector<Vtx2>, (*pe)->_vtci, pv)
        {
            pv->_rgb.r = GetRValue(crf);
            pv->_rgb.g = GetGValue(crf);
            pv->_rgb.b = GetBValue(crf);
        }
    }

}


//---------------------------------------------------------------------------------------
void Document::OnEscape()
{
    _allselected=0;
    Escape();
}


//---------------------------------------------------------------------------------------
void    Document::OnStartMoveRotScale(int32_t operation, SceItem** ppItems, int32_t count)
{
    if(0==count)
        return;
    if(_undodirt==0)
    {
        /*
        _undodirt=1;
        while(--count>=0)
        {
           _undo.Add(*ppItems, operation);
            ppItems++;
        }
        */

    }
}

//---------------------------------------------------------------------------------------
void    Document::OnStartMoveRotScale(int32_t operation, Brush** ppBrushes, int32_t count)
{
    if(0==count)
        return;

    if(_undodirt==0)
    {
        /*
        _undodirt=1;
        while(--count>=0)
        {
           _undo.Add(*ppBrushes, operation);
            ppBrushes++;
        }
        */

    }
}

//---------------------------------------------------------------------------------------
// reset them to initla valies to see last translation that happend
// I can undo last roattion or last translation because scale is not
// acually a scalation
u_bool32    Document::OnStartMoveRotScale()
{
    u_bool32 rv = 0;
    _docDirty = 1;
    if(_selmode == SEL_BRUSH ||
       _selmode == SEL_FACE ||
       _selmode == SEL_VERTEX ||
       _selmode == SEL_EDGE)
    {
        FOREACH(PBrushes,  _pSelBrushes, ppb)
        {
            (*ppb)->Dirty(0);
        }
        rv=1;
    }
    _undodirt=0;
    return rv;
}

u_bool32 Document::OnNewDocument()
{
    Poly::__Max=v3d_t(0,0,0);
    if(_scene.GetPrimitives()->size() && _docDirty)
    {
        u_int32_t retval = TAfxMessageBox("Do you want to Save the current Getic scene file ?", MB_YESNOCANCEL|MB_ICONQUESTION);
        if(retval==QMessageBox::Cancel)
            return 0;
        if(retval==IDYES)
        {
            OnFileSave();
        }
    }
    InitDocVars();
    SceItem::_GItemID = 0;
    Brush::GID        = 0;

    if(!_scene.SearchCats(theApp->XsdCatItmFile()))
    {
        theApp->XsdCatItmFile("");

    }
    return 1;
}

void Document::SaveInFile(const QString& pSzName, u_bool32 selection)
{
    CDocSaver cfv(pSzName.toUtf8(), TRUE);

    try
    {
        cfv.SerializeDoc(selection);
    }
    catch(const char* ex)
    {
        TAfxMessageBox(ex,MB_OK|MB_ICONHAND);
    }
    catch(...)
    {
        TAfxMessageBox("Unknkown Exception. Load/Save File Failure",MB_OK|MB_ICONHAND);
    }
    (VIEW3D(0,0))->setFocus(Qt::OtherFocusReason);
}

u_bool32 Document::OnOpenDocument()
{
    Poly::__Max=v3d_t(0,0,0);
    if(_scene.GetPrimitives()->size() && _docDirty)
    {
        u_int32_t retval = TAfxMessageBox("Do you want to Save the current Getic scene file ? ", MB_YESNOCANCEL|MB_ICONQUESTION);
        if(retval==QMessageBox::Cancel)
            return 0;
        if(retval==IDYES)
        {
            OnFileSave();
        }
    }

    _closing = false;
    InitDocVars();
    return true;
}


//----------------------------------------------------------------------------------------
void Document::OnFileOpen()
{
    QString szFilters = "Geometry raw (*.grf);;"
                          "Geometry selection (*.grs);;"
                          "Geometry prefab (*.gpm);;"
                          "Getic BSP(*.gbt)";


    OnOpenDocument();
    _closing        = FALSE;

    QFileDialog dlg(_Hmain, QString("Open File"), theApp->DocsDir(), szFilters);
    DeselectAll();

    if (theApp->DoFileDialog(dlg)==0)
    {
        REDIR();
        return;
    }

    REDIR();
    QString opended = dlg.selectedFiles().at(0);
    PathHandler ph((const char*)opended.toUtf8());
    theApp->DocsDir(ph.Path());

    DeleteContents();
    InitDocVars();

    ////AfxMessageBox(MKSTR("%s%s_res",ph.Path(),ph.JustFile()));
    const char* path = ph.Path();
    const char* file = ph.JustFile();

    char sp[256];
    sprintf(sp,"%s%s_res/",path,file);
    theApp->TexDir(sp);

    char szupper[_MAX_PATH];
    ::strcpy(szupper, opended.toUtf8());
    if(_tcsstr(szupper,".gbt"))
    {
        ReadFromBSP(opended.toUtf8());
    }
    else
    {
        CopyResources(opended.toUtf8());
        ReadFromFile(opended.toUtf8());
    }

    //SetModifiedFlag(TRUE);
    SetTitle(opended);
    TestBackupFile();
    Invalidate(1);
    _docDirty=0;

}


void Document::OnFileSave()
{
    if(GetTitle() == "NoName")
    {
        OnFileSaveAs();
    }
    else
    {
        _lasterror.erase();

        try{
            DeselectAll();
            SaveInFile(GetTitle());
            _docDirty = 0;
        }
        catch(int& i)
        {
            i;
            TAfxMessageBox(_lasterror.c_str(), MB_OK|MB_ICONWARNING);
            _lasterror.erase();
        }
        catch(...)
        {
            TAfxMessageBox("Unknown Exception while Saving", MB_OK|MB_ICONHAND);
        }
    }

}


void Document::OnFileSaveAs()
{

    REDIR();
    QString szFilters("Geometry raw (*.grf);;"
                        "Geometry selection (*.grs);;"
                        "Geometry prefab (*.gpm))");
    QString defaultFilter("Geometry raw (*.grf)");
    // Prompt the user for a filename

   // QFileDialog dlg(_Hmain, QString("SaveAs File"), theApp->DocsDir(), szFilters, defaultFilter);
    DeselectAll();

    QString opended = QFileDialog::getSaveFileName(_Hmain, QString("Save As"),theApp->DocsDir(),szFilters);
    if(!opended.contains(".grf") &&
            !opended.contains(".grs") &&
            !opended.contains(".gpm"))
    {
        if(defaultFilter.contains("grf"))
            opended.append(".grf");
        else if(defaultFilter.contains("grs"))
            opended.append(".grs");
        else if(defaultFilter.contains("gpm"))
            opended.append(".gpm");
        else
            opended.append(".grf");
    }
    PathHandler ph((const char*)opended.toUtf8());
    theApp->DocsDir(ph.Path());


    try{
        SaveInFile(opended);
        SetTitle(opended);
        _docDirty=0;
    }
    catch(int& i)
    {
        i;
        TAfxMessageBox(_lasterror.c_str(), MB_OK|MB_ICONWARNING);
        _lasterror.erase();
    }
    catch(...)
    {
        TAfxMessageBox("Unknown Exception while Saving", MB_OK|MB_ICONHAND);
    }
    REDIR();

}



void Document::OnMkBox()
{
    //OnFileSave();
    OnEscape() ;                    //clear any initiated action
    //TODO SaveTempFile();

    _cachedSett._boxdim = GetViewsDims();

    DialogNewBrush   dlgBrush;
    dlgBrush._brshSett=_cachedSett;
    dlgBrush._hideZone=false;
    int32_t rez = dlgBrush.exec();

    if(rez!=QDialog::Accepted)
        return;

    _cachedSett = dlgBrush._brshSett;
    _selmode = SEL_BRUSH;

    Brush*    pBrush   = new Brush();

    pBrush->MakeCube(_cachedSett._boxdim, _cachedSett._tm);

    if(_cachedSett._thikness != 0)
    {
        pBrush->StartSecGrp(_cachedSett._thikness);

        v3d_t intBox       = _cachedSett._boxdim - _cachedSett._thikness;
        Brush	 b2;
        b2.MakeCube(intBox, _cachedSett._tm);
        b2.Reverse();
        FOREACH(Polys,b2._polys,pp)
        {
            pp->_pBrush = pBrush;
            pBrush->_polys.push_back(*pp);
        }
    }
    ApplyFlags(pBrush);

    pBrush->_cf._tm      = _cachedSett._tm;
    pBrush->_cf._brushflags   = pBrush->_brushflags;
    pBrush->_cf._hstrips = 0;
    pBrush->_cf._vstrips = 0;
    pBrush->_cf._thick   = _cachedSett._thikness;
    pBrush->_cf._t    = E_CUBE;

}



//---------------------------------------------------------------------------------------
void Document::OnEnter()
{
  //  SaveTempFile();

    u_bool32    recut = true;
    if(0==_pSelBrushes.size() && 0 == _pSelItems.size())
        return;
    // TODO??? BARDLG()->OnApply();

    PBrushes  pSelBrushes = _pSelBrushes;

    if(pSelBrushes.size())
    {
        SBT(0, MKSTR("%d Brushes Commited", pSelBrushes.size()) );

        FOREACH(PBrushes,  pSelBrushes, ppb)
        {
            if((*ppb)->_brushflags & BRSH_NEW)           //new brushes just move them in scene
            {
                // keep last bouding box ex for next created brush
                (*ppb)->_brushflags &= ~BRSH_NEW;
                (*ppb)->Dirty(0);
                (*ppb)->Recalc();

                if((*ppb)->IsSplitter())
                {
                    SplitBySplitterBrush(*ppb);
                    delete (*ppb);
                    recut = true;
                    break; //only one can be
                }
                /*
                else if((*ppb)->IsZone())
                {
                    // add to zones
                }
                else
                {
                */
                    /**
                    if(GDetEnabled && (*ppb)->IsSolid())
                    {
                        EvaluateDetailbrsh((*ppb));
                    }
                    */
                    _scene.AddPrimitive((*ppb));
                    //TODO??? CameraPointTo((*ppb)->_box.GetCenter());
                //}
            }
            else if((*ppb)->IsDirty())				// moved scaled brushes, just commit them
            {
                (*ppb)->Recalc();
                /*
                if(GDetEnabled && (*ppb)->IsSolid())
                {
                    EvaluateDetailbrsh((*ppb));
                }
                */
            }
        }
        if(recut )
            Recut();
        /// TODO??? BARDLG()->OnBrushesSelected();
    }

    if(_pSelItems.size())
    {
        FOREACH(vvector<SceItem*>, _pSelItems, ppItem)
        {
            if((*ppItem)->_flags & BRSH_NEW)
            {
                (*ppItem)->_flags &= ~BRSH_NEW;

                _scene.AddItem(*ppItem);
                BARDLG()->AddItem(*ppItem);
                BARDLG()->Clean();
                BARDLG()->OnItemSelected(*ppItem);
                CameraPointTo((*ppItem)->_t);
            }
        }
        //BARDLG()->OnItemsSelected();

        SBT(0, MKSTR("%d Items Commited", _pSelItems.size()) );
    }
    Invalidate();
}

void    Document::OnFinishMoveRotScale(int32_t dirty, vvector<Brush*>* pBrushes)
{
    if(dirty == 0)
        return;

    u_int32_t           dw = time(0);
    vvector<Brush*>	extraToAdjust;
    int32_t             is = 0;
    int32_t             id = 0;

    _TRY{
        if(M_SCALED == dirty || M_MOVED == dirty || M_ROTATED == dirty)
        {

            if(_pSelBrushes.size())
            {
                BARDLG()->OnBrushesSelected();
                FOREACH(vvector<Brush*>, _pSelBrushes, ppB)
                {

                    if(_bSnapGrid)
                    {
                        RoundToNear((*ppB)->_t);
                    }

                    (*ppB)->Recalc();
                    if((*ppB)->_brushflags & BRSH_NEW)
                        continue;
                    is++;                           // det cuting
                    UpdateBrushLinks(*ppB);
                }
            }
            if(pBrushes)
            {
                append(extraToAdjust,(*pBrushes));
            }

            if(_pSelPolys.size()) // collect
            {
                set<Brush*>  brushes;
                ::GetBrushes(_pSelPolys, brushes);
                append(extraToAdjust, brushes);
            }

            if(_pSelVertexes.size())// collect
            {
                ModifyBrushes(extraToAdjust);
                is++;                           // det cuting
            }

            FOREACH(PBrushes, extraToAdjust, ppb)// update
            {
                (*ppb)->Dirty(1);
                (*ppb)->Recalc();
                UpdateBrushLinks(*ppb);
                is++;
            }

            if(is)
            {
                Recut(&extraToAdjust);
            }
        }
        else if(ITM_MOVED  == dirty ||
                ITM_SCALED == dirty ||
                ITM_ROTATED == dirty )
        {
            // update the bar dialog showing the first selected item
            int32_t count = 0;
            vvector<SceItem*>*  pTemsPtr = GetItems();
            FOREACH(vvector<SceItem*>, (*pTemsPtr), ppItem)
            {
                if((*ppItem)->IsSelected())
                {

                    if(_bSnapGrid)
                    {
                        RoundToNear((*ppItem)->_t);
                    }

                    (*ppItem)->Commit();
                    if(0==count)
                    {
                        BARDLG()->OnItemPropsChanged((*ppItem));
                        ++count;
                    }
                }
            }
        }

        if(GLight)
        {
            View3D* pT3D   = VIEW3D(0,0);
            pT3D->DrawInHrc(1);
        }
        Invalidate();
    }_CATCHX()
    {
        ;
    }

    if(time(0) - dw  > 2)
    {
        SBT(0," Quite many Items to process  !");
        Beep(600,32);
    }
}
/*
void Document::OnUpdateButselbrush(CCmdUI* pCmdUI)
{
    if(_bShowCsg || _compiling || Pd->_compviewmode != C_NA || (_scene.GetPrimitives()->size()==0))
    {
        pCmdUI->Enable(false);
        return;
    }

    pCmdUI->Enable(!(_selBox._brushflags & BRSH_SELVIS));
    pCmdUI->SetCheck(_selmode == SEL_BRUSH);
}
*/

