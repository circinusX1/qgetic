#ifndef GETICMAINWND_H
#define GETICMAINWND_H

#include <QMainWindow>
#include <QSplitter>
#include <QGraphicsScene>
#include "leftdock.h"
#include "cdlgcompprogress.h"

namespace Ui {
    class GeticMainWnd;
};

class GeticMainWnd : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit GeticMainWnd(QWidget *parent = 0);
    ~GeticMainWnd();

public slots:

    void onUpdate(int32_t m, void*, void*);
    //bool event(QEvent *event);

private:
    void _doToolBox();
    void _doViews();

    Ui::GeticMainWnd *ui;

    QSplitter *_h1Splitter;
    QSplitter *_h2Splitter;
    QGraphicsScene *_scene;

public:
    //CDlgCompProgress _dlgProgress;
    int32_t             _font1;

private slots:
    void on_action_New_tCtrl_N_triggered();
    void on_action_Open_Ctrl_0_triggered();
    void on_action_Save_Ctrl_S_triggered();
    void on_actionSave_As_Ctrl_A_triggered();
    void on_action_Export_Ctrl_E_triggered();
    void on_actionMove_Ctrl_M_triggered();
    void on_actionScale_Ctrl_L_triggered();
    void on_actionVertex_Ctrl_T_triggered();
    void on_actionRotate_Ctrl_R_triggered();
    void on_actionBRUSH_triggered();
    void on_actionCone_Ctrl_N_triggered();
    void on_actionCylinder_Ctrl_Y_triggered();
    void on_actionBox_Ctrl_O_triggered();
    void on_actionBrush_Ctrl_B_triggered();
    void on_actionSlide_Ctrl_S_triggered();
};

extern GeticMainWnd*  _Hmain;

void  SBT(int32_t nPane, const char* pTxt);



#define SBAR()  _Hmain->PFrame->m_wndStatusBar

#define CLRLOG() //_Hmain->_dlgProgress.Clear()
#define LOG      //_Hmain->_dlgProgress.Log
#define LOGERR   //_Hmain->_dlgProgress.LogPos
#define PROG     //_Hmain->_dlgProgress.Progress

class View2D;
class View3D;

extern      View2D* Ptz;
extern  	View2D* Ptx;
extern  	View2D* Pty;
extern      View3D*  PT3;

INLN View2D* VIEW2D(char xyz)
{
    if(xyz=='z')//0,1
        return Ptz;
    if(xyz=='x')//1,0
        return Ptx;
    return Pty;
};

inline View3D* VIEW3D(int32_t x, int32_t y) {
    Q_UNUSED(x);
    Q_UNUSED(y);
    return PT3;
}



#endif // GETICMAINWND_H
