
#include "stdafx.h"
#include "geticmainwnd.h"
#include "geticapp.h"
#include "document.h"


int32_t main(int32_t argc, char *argv[])
{
    ThrID = GetCurrentThreadId();
    GeticApp a(argc, argv);

    Document  doc;
    GeticMainWnd w;

    QCoreApplication::setApplicationName("QGetic 1.0.0");

    w.show();
    a.PumpMessage(0,0);
    return a.exec();
}




u_int32_t       UTimer;
Htex		    _selTex;
Htex		    _dumptex;
Htex			_lbulbtex;
Htex			_itmqtex;
Htex			_sndtex;
Htex			_startPosTex;
Htex			_tbtex;
Htex            _wLmap;
Htex            _ambLmap;
//HWND            _Hmain;// = m_hWNd;

//---------------------------------------------------------------------------------------
/*
CMainFrame* _Hmain->PFrame;
int32_t		    _Hmain->_font1;
HFONT	    _Hmain->_hf;
View3D*     Ptz;
View2D* Ptx;
View2D* Pty;
View3D*  PT3;
*/
