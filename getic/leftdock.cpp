
#include "leftdock.h"
#include "document.h"
#include "sceitem.h"

LeftDock* LeftDock::GBarDlg;

LeftDock::LeftDock(QMainWindow* p):QDockWidget()
{
    Q_UNUSED(p);
    GBarDlg =this;

}


//---------------------------------------------------------------------------------------
void    LeftDock::ShowItemProps(u_int32_t itmtype)
{
    Q_UNUSED(itmtype);
}

//---------------------------------------------------------------------------------------
void    LeftDock::ShowSelItmDlg(int32_t sel)
{
    Q_UNUSED(sel);
}


//---------------------------------------------------------------------------------------
void    LeftDock::OnBrushesSelected()
{
    if(Pd->_pSelBrushes.size())
    {

    }
}


void    LeftDock::OnItemsSelected()
{
    OnItemSelected(0,0);
}

//---------------------------------------------------------------------------------------
void    LeftDock::OnFacesSelected()
{

}

//---------------------------------------------------------------------------------------
void LeftDock::OnApply()
{

}

//---------------------------------------------------------------------------------------
// restore original state of brush
void LeftDock::ResetBrushes()
{
    int32_t nCount;
    Brush** pBrsh = Pd->GetSelBrushes(nCount);
    for(int32_t i=0; i< nCount; i++)
    {
        (*pBrsh)->Reinit();
        pBrsh++;
    }
    Pd->Recut();
}

//---------------------------------------------------------------------------------------
void LeftDock::ShowBrushesProperties(Brush** ppBrushes, int32_t nCount)
{
    Q_UNUSED(ppBrushes);
    Q_UNUSED(nCount);
}

//---------------------------------------------------------------------------------------
void LeftDock::ShowFacesProperties(Poly** ppPolys, int32_t nCount)
{
    Q_UNUSED(ppPolys);
    Q_UNUSED(nCount);
}

u_int32_t   LeftDock::GettexGenFlag()
{
    return 0;
}

//---------------------------------------------------------------------------------------
char*   LeftDock::GetCurSelTexture()
{
    return 0;
}


void    LeftDock::AddItem(SceItem* pItem)
{
    Q_UNUSED(pItem);
}

//---------------------------------------------------------------------------------------
void    LeftDock::DelItem(SceItem* pItem)
{
    Q_UNUSED(pItem);
}

//---------------------------------------------------------------------------------------
void    LeftDock::OnItemPropsChanged(SceItem* pItem)
{
    Q_UNUSED(pItem);
}

//---------------------------------------------------------------------------------------
void    LeftDock::ShowItemDlg(SceItem* pItem)
{
    Q_UNUSED(pItem);
}

void    LeftDock::DeselectItems(u_bool32 notyFrame)
{

    Q_UNUSED(notyFrame);

}

//---------------------------------------------------------------------------------------
void    LeftDock::OnItemSelected(SceItem* pItem, int32_t sel)
{
    Q_UNUSED(pItem);
    Q_UNUSED(sel);
}

//---------------------------------------------------------------------------------------
void    LeftDock::UpdateItmProps(SceItem* pItem)
{
    Q_UNUSED(pItem);
}


void    LeftDock::RefreshItemsList()
{

    Dirty();

}



void    LeftDock::HideAll()
{
}


//-----------------------------------------------------------------------------
void    LeftDock::UpdateMotionProps(Brush* pBrush, CMotion* pM)
{
    Q_UNUSED(pBrush);
    Q_UNUSED(pM);
}

//-----------------------------------------------------------------------------
void    LeftDock::Rescan()
{
    OnBrushesSelected();
    OnFacesSelected();
    OnItemsSelected();
}

//-----------------------------------------------------------------------------
void LeftDock::Refresh()
{
    OnBrushesSelected();
    OnFacesSelected();
    OnItemsSelected();
    RefreshItemsList();
}

//-----------------------------------------------------------------------------
// clen the content of bardlg here //CMenu
void LeftDock::ResetDlgsContent()
{
}



