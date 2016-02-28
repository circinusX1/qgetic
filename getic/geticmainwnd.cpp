
#include <iostream>
#include <QBoxLayout>
#include <QToolBox>
#include <QLabel>
#include <QSplitter>
#include "geticapp.h"
#include "document.h"
#include "ui_geticmainwnd.h"
#include "geticmainwnd.h"
#include "view2d.h"
#include "view3d.h"
//
//extern GeticApp* theApp;

GeticMainWnd*  _Hmain;
View2D* Ptz=0;
View2D* Ptx=0;
View2D* Pty=0;
View3D*  PT3=0;

GeticMainWnd::GeticMainWnd(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GeticMainWnd)
{
    _Hmain = this;
    ui->setupUi(this);
    _doToolBox();
    _doViews();
    //char d[255];
    //getcwd(d,255);
    //Pd->ReadFromFile("one.grf");

}

GeticMainWnd::~GeticMainWnd()
{
    delete ui;
}


void GeticMainWnd::_doToolBox()
{

    LeftDock* pd = (LeftDock*)ui->bardlg_2;

    QToolBox* toolBox = new QToolBox;
    toolBox->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
    toolBox->setMinimumWidth(pd->sizeHint().width());

    QLabel *itemWidget1 = new QLabel;

    QLabel *itemWidget2 = new QLabel;
    toolBox->addItem(itemWidget2, "WID2");
    toolBox->addItem(itemWidget1, "WID1");

    pd->setWidget(toolBox);


}

void GeticMainWnd::_doViews()
{
    _scene = new QGraphicsScene;

    _h1Splitter = new QSplitter;
    _h2Splitter = new QSplitter;

    QSplitter *vSplitter = new QSplitter;
    vSplitter->setOrientation(Qt::Vertical);
    vSplitter->addWidget(_h1Splitter);
    vSplitter->addWidget(_h2Splitter);

    View3D* viewxyz = new View3D();
    _h1Splitter->addWidget(viewxyz);

    View2D *view1 = new View2D('x', viewxyz);
    _h1Splitter->addWidget(view1);

    View2D *view2 = new View2D('y',viewxyz);
    _h2Splitter->addWidget(view2);

    View2D *view3 = new View2D('z',viewxyz);
    _h2Splitter->addWidget(view3);

    this->setCentralWidget(vSplitter);
    //QHBoxLayout *layout = new QHBoxLayout;
    //ui->formLayout->addWidget(vSplitter);

    //setLayout(layout);
    view1->_vt = 'x';
    view1->_viewrot=90;

    view2->_vt = 'y';
    view2->_viewrot=90;

    view3->_vt = 'z';
    view3->_viewrot=0;

    if(_dumptex==0)
    {
        uint32_t imageData[]={0xFFFFFF,0xFF00FF,0xFF00FF, 0x00FFFF,
                              0xFFFFFF,0xFF00FF,0xFF00FF, 0x00FFFF,
                              0xFFFFFF,0xFF00FF,0xFF00FF, 0x00FFFF};

        _dumptex=theApp->_TexSys.GenTexture("__d",4,4,3,(uint8_t*)imageData,0);
    }

}



void GeticMainWnd::onUpdate(int32_t m, void* w, void* l)
{
    (void)m;
    (void)w;
    (void)l;
}



void  SBT(int32_t nPane, const char* pTxt)
{
    Q_UNUSED(nPane);
    Q_UNUSED(pTxt);

    static QString ss= "";
    if(ss != pTxt)
    {
        ss=pTxt;
        _Hmain->statusBar()->showMessage(QString(pTxt));
    }
    /*
    static u_int32_t lastTime = ge_gettick();
    if(ThrID == GetCurrentThreadId() || pTxt[0]=='+' )
    {
        if(ThrID != GetCurrentThreadId())
        {
            strcpy(Sbt, pTxt);
            ::PostMessage(_Hmain,WM_SBT_MSG,nPane,0);
        }
        else
        {
             strcpy(Sbt, pTxt);
             AfxGetMainWnd()->SendMessage(WM_SBT_MSG,nPane,0);
             //((CMainFrame*)AfxGetMainWnd())->m_toltip.SetWindowText(pTxt);
        }
    }
    */

}


void GeticMainWnd::on_action_New_tCtrl_N_triggered()
{
    DOC()->OnNewDocument();
}

void GeticMainWnd::on_action_Open_Ctrl_0_triggered()
{
    DOC()->OnFileOpen();
}

void GeticMainWnd::on_action_Save_Ctrl_S_triggered()
{
    DOC()->OnFileSave();
}

void GeticMainWnd::on_actionSave_As_Ctrl_A_triggered()
{
    DOC()->OnFileSaveAs();
}

void GeticMainWnd::on_action_Export_Ctrl_E_triggered()
{

}

//---------------------------------------------------------------------------
void GeticMainWnd::on_actionMove_Ctrl_M_triggered()
{

}

void GeticMainWnd::on_actionScale_Ctrl_L_triggered()
{

}

void GeticMainWnd::on_actionRotate_Ctrl_R_triggered()
{

}


void GeticMainWnd::on_actionBRUSH_triggered()
{

}

void GeticMainWnd::on_actionCone_Ctrl_N_triggered()
{

}

void GeticMainWnd::on_actionCylinder_Ctrl_Y_triggered()
{

}

void GeticMainWnd::on_actionBox_Ctrl_O_triggered()
{
    DOC()->OnMkBox();
}


//----------------------------------------------------------------------
void GeticMainWnd::on_actionVertex_Ctrl_T_triggered()
{

}


void GeticMainWnd::on_actionBrush_Ctrl_B_triggered()
{
    DOC()->OnButselbrush();
}

void GeticMainWnd::on_actionSlide_Ctrl_S_triggered()
{

}
