#ifndef LEFTDOCK_H
#define LEFTDOCK_H

#include <os.h>
#include <QDockWidget>

class QMainWindow;
class SceItem;
class Brush;
class CMotion;
class Poly;
class LeftDock : public QDockWidget
{
public:
    static LeftDock* GBarDlg;
    LeftDock(QMainWindow*);

    void    ActivateTexturesDlg(u_bool32 bShow);
    void    ShowItemDlg(SceItem* pItem);
    char*   GetCurSelTexture();
    void    ShowSelItmDlg(int32_t sel);
    void    OnBrushesSelected();
    void    OnItemsSelected();
    void    OnFacesSelected();
    u_int32_t   GettexGenFlag();
    void    ResetBrushes();
    void	BlockRetreive(u_bool32 blk){_blkRetreive = blk;}
    u_bool32	IsRetreiveBlocked(){return (_blkRetreive || _initDlg);}
    void    RefreshItemsList();
    void    ShowItemProps(u_int32_t itmtype);
    void	AddItem(SceItem* pItem);
    void    DelItem(SceItem* pItem);
    void    OnItemPropsChanged(SceItem* pItem);
    void    ScanCreationFlags();
    void    OnItemSelected(SceItem* pItem, int32_t sel=1);
    void    UpdateItmProps(SceItem* pItem);
    void    UpdateMotionProps(Brush*, CMotion* pM);
    void	ForceApply(){Dirty(); OnApply();};
    void    MakeVisible(QWidget* pDlg);
    void    DeselectItems(u_bool32 notyFrame);
    void    HideAll();
    void    Rescan();
    void    Refresh();
    void    ResetDlgsContent();
    void    Clean(){

            _dirty=0;
            /*
            if(IsWindow(m_hWnd))
            {
                GetDlgItem(IDC_BUTTON3)->EnableWindow(0);
                GetDlgItem(ST_MSG)->SetWindowText("Ready");
            }
            */

    };
    void    Dirty(){
        if(IsRetreiveBlocked())
            return;
        if(_dirty==0)
        {
            _dirty = 1;
            /*
            if(IsWindow(m_hWnd))
            {
                GetDlgItem(IDC_BUTTON3)->EnableWindow(1);
                GetDlgItem(ST_MSG)->SetWindowText("Press Enter To Commit");
            }
            */

        }
    }
    void OnApply();
    void ShowBrushesProperties(Brush** ppBrushes, int32_t count);
    void ShowFacesProperties(Poly** ppPolys, int32_t count);
    // Implementation
protected:
    void    DisableDlg(QWidget* pd);
    void    EnableDlg(QWidget* pd);

    void    CommonInitializer();

    void OnSize(u_int32_t nType, int32_t cx, int32_t cy);

    u_bool32            _dirty;
public:

    int32_t             _tabSelIdx;
    u_bool32			_blkRetreive;
    QBrush          _dirtyBrush;
    u_bool32            _initDlg;
    u_bool32            _blockDocEvents;

};



#define BARDLG()    LeftDock::GBarDlg


#endif // LEFTDOCK_H
