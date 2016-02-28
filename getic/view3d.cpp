
#include <QWSServer>
#include <QMouseEvent>
#include <QCursor>
#include "geticapp.h"
#include "view3d.h"
#include "document.h"


View3D::View3D():glwindow(0)
{
    PT3=this;
    Reinit();
}

View3D::~View3D()
{
    PT3=0;
}

void View3D::initializeGL()
{
    GCur g(this);
    Pd->InitGL(3/*3D*/,(void*)0,(void*)0);
    /*
    qglClearColor(QColor(0,55,0));

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_MULTISAMPLE);
*/
}

void View3D::resizeGL(int32_t cx, int32_t cy)
 {
    GCur g(this);
    Project3D(cx, cy);
    _rt = this->rect();
    if(_rt.bottom()==0)
        _rt.setBottom(1);

 }

void View3D::paintGL()
{
    GCur g(this);
    DrawInHrc(true);//GLPaint();

}


void View3D::DrawInHrc(u_bool32 update)
{
    Q_UNUSED(update);
    Ta.Enable();
    //glDisable(GL_COLOR_MATERIAL);

    _rt = rect();
    if(_rt.right() && _rt.bottom())
    {
        Project3D(_rt.right(), _rt.bottom());
        glClear (GL_COLOR_BUFFER_BIT|
                 GL_DEPTH_BUFFER_BIT|
                 GL_STENCIL_BUFFER_BIT);
        GLPaint();
        //SwapBuffers(hdc);
    }
    //Ta.Disable();
}

//---------------------------------------------------------------------------------------
void View3D::TextOut(const v3d_t& v, const char* text,const u_rgba_t& clr)
{
    GCur g(this);
    glEnable(GL_BLEND);
    glColor3ub(clr.r,clr.g,clr.b);

    glPushMatrix();
    {
        glRasterPos3f(v.x,v.y,v.z);
        glPushAttrib(GL_LIST_BIT);
        glListBase( _font1-32 );
        glCallLists( strlen( text ), GL_UNSIGNED_BYTE, text );
    }
    glPopMatrix();
    glDisable(GL_BLEND);
}


//---------------------------------------------------------------------------------------
void  View3D::Project3D(int32_t iWidth, int32_t iHeight)
{
    //GCur g(this);

    if(iHeight == 0)
        iHeight = 1;

    if(Pd->b_whitebk)
    {
        glClearColor(.8f, .8f, .8f, 0.0f);
    }
    else
    {
        if(Pd->_bShowCsg)
            glClearColor(0.11f, 0.11f, 0.11f, 0.0f);
        else
            glClearColor(.02f, .02f, .04f, 0.0f);
    }
    f32_t farCam = _ffar;
/*
    if(SCENE()._si.fogFar)
        farCam = SCENE()._si.fogFar*_ffar;
*/

    glViewport(0, 0, iWidth, iHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(_fov,(GLfloat)iWidth/(GLfloat)iHeight, _fnear, farCam);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

//	MakeCurrent(_hdc, 0);
}

//---------------------------------------------------------------------------------------
void View3D::GLPaint()
{

    glPushMatrix();
        Camera& cam = Pd->_cam;
        const m_16t&		mt = cam.GetViewMtx(0);
        cam.UpdateFrustrumPlanes();
        glColor3f(1.0, 1.0f, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
#ifdef _BDL_PREC
        glLoadMatrixd((f32_t*)&mt);
#else
        glLoadMatrixf((f32_t*)&mt);
#endif
        Pd->Draw3DScene(this);

    glPopMatrix();
}



void View3D::Reinit()
{
    _rotating           = false;
    _fov                = 64.0f;
    _fnear              = 4.0f;
    _ffar               = SCENE()._si.camFar;
    _capture            = 0;
    _scrPoint.setX(0);
    _scrPoint.setY(0);
    _downKey            = 0;
    _blockKeyCall       = 0;
    _blockMouseCall     = 0;
}

void View3D::mousePressEvent(QMouseEvent *p)
{
    Qt::MouseButton b =	p->button();

    if(_mm._ldown==0)_mm._ldown = (b == Qt::LeftButton);
    if(_mm._rdown==0)_mm._rdown = (b == Qt::RightButton);
    if(_mm._mdown==0)_mm._mdown = (b == Qt::MidButton);

    _rotating  = !_mm._ldown;

    char s[128];
    sprintf(s,"%d%d%d\n",_mm._ldown,_mm._mdown,_mm._rdown);
    //qDebug(s);


    _mm._pt = _mm._ptd = p->pos();
    _scrPoint = p->globalPos();

    setMouseTracking(true);
    //grabMouse();
    HideTheCursor();
    v3d_t dummy;
    Pd->Make3dCursorPos(this, _mm._pt, _rt, dummy);
}

void View3D::mouseReleaseEvent(QMouseEvent *p)
{
    Qt::MouseButton b =	p->button();
    if(b==Qt::LeftButton)
            emit lButtonUp(p);
    if(b==Qt::RightButton)
            emit rButtonUp(p);
    if(b==Qt::MidButton)
            emit mButtonUp(p);
}

void View3D::lButtonUp(QMouseEvent *p)
{
    _mm._ldown = 0;
    if(_mm._rdown==0)
    {
        releaseMouse();
        setMouseTracking(false);
        if(_scrPoint.x()!=0)
            QCursor::setPos(_scrPoint);
        ShowTheCursor();
    }

    if(!_rotating )
    {
        _EL_SEL sel = Pd->_selmode;
        if(SEL_NONE!=sel && Pd->_blocksel==FALSE)
        {
            Pd->DoSelection3D(p->pos(), this, sel);
        }
    }
    Pd->Invalidate();
}




void View3D::rButtonUp(QMouseEvent *p)
{
    if(_mm._ldown==0)
    {
        this->releaseMouse();
        setMouseTracking(false);

        if(_scrPoint.x()!=0)
            QCursor::setPos(_scrPoint);
        ShowTheCursor();
    }
    _mm._rdown=false;
    Pd->Invalidate();

}

void View3D::mButtonUp(QMouseEvent *p)
{
}

void View3D::mouseMoveEvent(QMouseEvent *p)
{
    v3d_t      scrPt;
    QPoint  pt = _mm._pt - p->pos();

     if(_mm._rdown || _mm._ldown)
     {
         _mm._pt    = p->pos();
         //SetCursorPos(_mm._pt.x, _mm._pt.y);

         if(theApp->isKeyDown('L'))
         {
             update();
             //CView::OnMouseMove(nFlags, point);
             f32_t cf = _fov + pt.y();

             if(cf < 35) cf=35;
             if(cf>140)  cf=140;

             _fov = cf ;
             Pd->_cam.SetFov(_fov);

             SBT(3,MKSTR("FOV :%03.0f", _fov));
             return;
         }

     }

     Pd->Make3dCursorPos(this, p->pos(), _rt, scrPt);


     if(_mm._ldown)
     {

         if(abs(pt.x()) > 0||
            abs(pt.y()) > 0)
         {
             if(_mm._rdown)
             {
                 if(theApp->isKeyDown(Qt::Key_Space))
                 {
                     Pd->_cam.MoveUp(pt.y()*64.f);
                 }
                 else
                 {
                     Pd->_cam.MoveUp(pt.y()*2.f);
                 }

                 if(theApp->isKeyDown(Qt::Key_Space))
                 {
                     Pd->_cam.MoveSide(-pt.x()*64.f);
                 }
                 else
                 {
                     Pd->_cam.MoveSide(-pt.x()*2.f);
                 }
             }
             else
             {
                 if(theApp->isKeyDown(Qt::Key_Shift))
                 {
                     v3d_t		cr ;
                     int32_t		nC = 0;
                     Brush** ppb = Pd->GetSelBrushes(nC);
                     if(nC){
                         cr = (*ppb)->_box.GetCenter();
                     }
                     f32_t dst = -vdist(Pd->_cam._pos , cr);
                     Pd->_cam._pos = cr;
                     Pd->_cam.Rotate(-pt.x()/128.f,-pt.y()/128.f,0);
                     Pd->_cam._pos = cr + (Pd->_cam._fwd)*dst;


                 }
                 else
                 {
                     Pd->_cam.Rotate(-pt.x()/128.f,-pt.y()/128.f,0);
                 }

             }
             _rotating = TRUE;
         }

         SBT(2,MKSTR("CAM: %04.2f %04.2f z:%04.2f",	Pd->_cam._pos.x/GMeters,
                     Pd->_cam._pos.y/GMeters,
                     Pd->_cam._pos.z/GMeters));
         if(Pd->_compviewmode != C_NA)
         {
             Pd->Invalidate(); //allow to see the full BSP
         }
         else
             update();

     }
     else if(_mm._rdown )
     {

         if(theApp->isKeyDown(Qt::Key_Space) )
             Pd->_cam.MoveFwd(pt.y()*64.f);
         else
             Pd->_cam.MoveFwd(pt.y()*4.f);

         SBT(2,MKSTR("CAM: %04.2f %04.2f z:%04.2f",Pd->_cam._pos.x/GMeters,
                     Pd->_cam._pos.y/GMeters,
                     Pd->_cam._pos.z/GMeters));
         _rotating = TRUE;
         if(Pd->_compviewmode != C_NA)
         {
             Pd->Invalidate();
         }
         else
             update();

     }
/*
     if(_mm._rdown || _mm._ldown)
         Rollover(point);
*/

}


void View3D::mouseDoubleClickEvent(QMouseEvent *p)
{
}

void View3D::keyPressEvent(QKeyEvent *p)
{
}
void View3D::keyReleaseEvent(QKeyEvent *p)
{
}
void View3D::focusInEvent(QFocusEvent *p)
{
    _active=true;
    updateGL();
}
void View3D::focusOutEvent(QFocusEvent *p)
{
    _active=false;
    updateGL();
}
void View3D::closeEvent(QCloseEvent *p)
{
}



void View3D::PrintOut(const v3d_t& v, char vt, const char* text, const u_rgba_t& clr)
{
    /*
    //renderText
    glEnable(GL_BLEND);
    glColor3ub(clr.r,clr.g,clr.b);
    glRasterPos3f(v.x,v.y,v.z);
    glPushAttrib(GL_LIST_BIT);
        glListBase( 2000-32 );
        glCallLists( strlen( text ), GL_UNSIGNED_BYTE, text );
    glPopAttrib();
    glDisable(GL_BLEND);
    */

    glPushMatrix();
    glColor3ub(clr.r,clr.g,clr.b);
    renderText(v.x,v.y,v.z, text);
    glPopMatrix();
}


void View3D::HideTheCursor()
{
#ifdef Q_WS_QWS
    QWSServer::setCursorVisible( false );
#endif

}

void View3D::ShowTheCursor()
{
#ifdef Q_WS_QWS
    QWSServer::setCursorVisible( true );
#endif

}


