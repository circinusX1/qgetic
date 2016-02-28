
#include <QMouseEvent>
#include <QCursor>
#include "view2d.h"
#include "document.h"
#include "qgttypes.h"
#include "geticapp.h"


View2D::View2D(char p, QGLWidget* sharewith):glwindow(0, sharewith),_vt(p),_ncursor(-1)
{
    if(_vt=='x')Ptx=this;
    else if(_vt=='y')Pty=this;
    else if(_vt=='z')Ptz=this;

    Reinit();
    setMouseTracking(true);
}

void View2D::initializeGL()
{
   GCur g(this);
    Pd->InitGL(2/*2D*/,0,0);
    /*
    qglClearColor(QColor(0,55,0));

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_MULTISAMPLE);
*/

    _rt=rect();

}

void View2D::resizeGL(int32_t width, int32_t height)
{
    GCur g(this);
    _rt=rect();
}

void View2D::paintGL()
{

    GCur g(this);
    Ta.Disable();
    if(_rt.right() && _rt.bottom())
    {
        Project2D(_rt.right(), _rt.bottom());
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
            GLPaint(_rt);
        glEnable(GL_DEPTH_TEST);
        PrintCoords(_rt, _vport, V0, _vt, _active,cs_ct.toUtf8(),&_textmouse);

//        if(Pd->_glAddSwapHintRectWIN)
//            (Pd->_glAddSwapHintRectWIN)(0,0,_rt.right,_rt.bottom);
//        SwapBuffers(hdc);
    }
}


void View2D::Project2D(int32_t iWidth, int32_t iHeight)
{
    //GCur g(this);
    if(iHeight == 0)
    {
        iHeight = 1;
    }

    glViewport(0, 0, iWidth, iHeight);
    if(Pd->b_whitebk !=0 )
    {
        if(Pd->b_whitebk==1)
            _active ? glClearColor(1.f, 1.f, 1.f, 0.0f) : glClearColor(.9f, .9f, .9f, 0.0f) ;
        else
            glClearColor(1.f, 1.f, 1.f, 0.0f);

    }
    else
    {
        if(Pd->_bShowCsg)
            glClearColor(0.11f, 0.11f, 0.11f, 0.0f);
        else
        {
            _active ? glClearColor(.0f, .0f, .0f, 0.0f) : glClearColor(.0800f, .0800f, .0800f, 0.0f) ;
        }
    }
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    _vport.left   = -(((f32_t)iWidth * 0.5)   / _zoom)   + _shiftx;
    _vport.right  =  (((f32_t)iWidth * 0.5)   / _zoom)   + _shiftx;
    _vport.top    = -(((f32_t)iHeight * 0.5)  / _zoom)   + _shifty;
    _vport.bottom  = (((f32_t)iHeight * 0.5)  / _zoom)   + _shifty;
    glOrtho(_vport.left, _vport.right, _vport.top , _vport.bottom, -131072, 131072);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void View2D::GLPaint(const QRect& rt)
{
    glPushMatrix();
    {
        Ta.Disable();
        if(Pd->b_showgrid)
            DrawGrid(rt);
        Pd->Draw2DScene(this);
    }
    glPopMatrix();

}


void View2D::DrawGrid(const QRect& rt)
{
    switch(_vt)
    {
        case 'y':
                DrawTopGrid(rt);
                break;
        case 'x':
                DrawSideGrid(rt);
                break;
        case 'z':
                DrawFrontGrid(rt);
                break;
    }
}



//---------------------------------------------------------------------------------------
void View2D::DrawFrontGrid(const QRect& rt)
{
    int32_t hssz = Pd->_gridSize / 2;

    f32_t Left    = Wnd2World(rt.left(),  'h');
    f32_t Right   = Wnd2World(rt.right(), 'h');
    f32_t Top     = Wnd2World(rt.top(),   'v');
    f32_t Bottom  = Wnd2World(rt.bottom(),'v');
    int32_t  wp;

    if (Left < -hssz)		Left   = -hssz;
    if (Right > hssz)		Right  = hssz;
    if (Top > hssz)		    Top    = hssz;
    if (Bottom < -hssz)		Bottom = -hssz;

    glPushMatrix();
    if(_zoom>.04)       // shows the fine geid if zoom is less then .4
    {
        _active ? glColor4ubv(CLR_FINEGRIDF) : glColor4ubv(CLR_FINEGRIDD);
        glBegin(GL_LINES);
        for(int32_t decVal = 0; decVal <= Pd->_gridSize; decVal += Pd->_gridStep)
        {
            if (decVal % (Pd->_gridStep * 10) && decVal != hssz)
            {
                wp = hssz - decVal;
                if (wp > (int32_t)Left && wp < (int32_t)Right)
                {
                    glVertex3f(wp, Top, 0.0);
                    glVertex3f(wp, Bottom, 0.0);
                }

                if (wp > (int32_t)Bottom && wp < (int32_t)Top)
                {
                    glVertex3f(Left, wp, 0.0);
                    glVertex3f(Right, wp, 0.0);
                }
            }
        }
        glEnd();
    }

    // shows different grid color on current active view

    _active ?   glColor4ubv(CLR_GRIDF):glColor4ubv(CLR_GRIDD);

    glBegin(GL_LINES);
    for(int32_t decVal = 0; decVal <= Pd->_gridSize; decVal += (Pd->_gridStep * 10))
    {
        wp = hssz - decVal;
        if (decVal != hssz)
        {
            if (wp >= (int32_t)Left && wp <= (int32_t)Right)
            {
                glVertex3f(wp, Top, 0.0);
                glVertex3f(wp, Bottom, 0.0);
            }

            if (wp >= (int32_t)Bottom && wp <= (int32_t)Top)
            {
                glVertex3f(Left, wp, 0.0);
                glVertex3f(Right, wp, 0.0);
            }
        }
    }
    glEnd();

    glBegin(GL_LINES);
        // Horizontal lines
        glColor4ubv(CLR_AXES_X);
        glVertex3f(Left, 0.0, 0.0);
        glVertex3f(Right, 0.0, 0.0);
        // Vertical lines
        glColor4ubv(CLR_AXES_Y);
        glVertex3f(0.0, Top, 0.0);
        glVertex3f(0.0, Bottom, 0.0);

    glEnd();
    glPopMatrix();
}

//---------------------------------------------------------------------------------------
void View2D::DrawTopGrid(const QRect& rt)
{
    int32_t hgsz = Pd->_gridSize / 2;

    GLfloat Left    = Wnd2World(rt.left(), 'h');
    GLfloat Right   = Wnd2World(rt.right(), 'h');
    GLfloat Top     = Wnd2World(rt.top(), 'v');
    GLfloat Bottom  = Wnd2World(rt.bottom(), 'v');
    int32_t     wp;

    if (Left < -hgsz)		Left = -hgsz;
    if (Right > hgsz)		Right = hgsz;
    if (Top < -hgsz)		Top = -hgsz;
    if (Bottom > hgsz)		Bottom = hgsz;

    glPushMatrix();
    glRotatef(90, 1, 0, 0);

    if(_zoom>.04)
    {
        _active ? glColor4ubv(CLR_FINEGRIDF) : glColor4ubv(CLR_FINEGRIDD);
        glBegin(GL_LINES);
        for(int32_t vDec = 0; vDec <= Pd->_gridSize; vDec += Pd->_gridStep)
        {
            if (vDec % (Pd->_gridStep * 10) && vDec != hgsz)
            {
                wp = hgsz - vDec;
                if (wp > (int32_t)Left && wp < (int32_t)Right)
                {
                    glVertex3f(wp, 0.0, Top);
                    glVertex3f(wp, 0.0, Bottom);
                }
                if (wp < (int32_t)Bottom && wp > (int32_t)Top)
                {
                    glVertex3f(Left, 0.0, wp);
                    glVertex3f(Right, 0.0, wp);
                }
            }
        }
        glEnd();
    }

    _active ?   glColor4ubv(CLR_GRIDF):glColor4ubv(CLR_GRIDD);
    glBegin(GL_LINES);
    for(int32_t vDec = 0; vDec <= Pd->_gridSize; vDec += (Pd->_gridStep * 10))
    {
        if (vDec != hgsz)
        {
            wp = hgsz - vDec;
            if (wp >= (int32_t)Left && wp <= (int32_t)Right)
            {
                glVertex3f(wp, 0.0, Top);
                glVertex3f(wp, 0.0, Bottom);
            }
            if (wp <= (int32_t)Bottom && wp >= (int32_t)Top)
            {
                glVertex3f(Left, 0.0, wp);
                glVertex3f(Right, 0.0, wp);
            }
        }
    }
    glEnd();

    glBegin(GL_LINES);

        glColor4ubv(CLR_AXES_X);
        glVertex3f((int32_t)Left, 0, 0);
        glVertex3f((int32_t)Right, 0, 0);

        glColor4ubv(CLR_AXES_Z);
        glVertex3f(0.0, 0.0, Top);
        glVertex3f(0.0, 0.0, Bottom);


    glEnd();

    glPopMatrix();
}

//---------------------------------------------------------------------------------------
void View2D::DrawSideGrid(const QRect& rt)
{
    int32_t hdsz = Pd->_gridSize / 2;

    GLfloat Left = Wnd2World(rt.left(), 'h');
    GLfloat Right = Wnd2World(rt.right(), 'h');
    GLfloat Top = Wnd2World(rt.top(), 'v');
    GLfloat Bottom = Wnd2World(rt.bottom(), 'v');
    int32_t wp;

    if (Left < -hdsz)
        Left = -hdsz;
    if (Right > hdsz)
        Right = hdsz;
    if (Top > hdsz)
        Top = hdsz;
    if (Bottom < -hdsz)
        Bottom = -hdsz;


    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    if(_zoom>.04)
    {
        _active ? glColor4ubv(CLR_FINEGRIDF) : glColor4ubv(CLR_FINEGRIDD);

        glBegin(GL_LINES);
        for(int32_t fd= 0; fd<= Pd->_gridSize; fd+= Pd->_gridStep)
        {
            if (fd% (Pd->_gridStep * 10) && fd!= hdsz)
            {
                wp = hdsz - fd;
                if (wp > (int32_t)Left && wp < (int32_t)Right)
                {
                    glVertex3f(0.0, Top, wp);
                    glVertex3f(0.0, Bottom, wp);
                }
                if (wp > (int32_t)Bottom && wp < (int32_t)Top)
                {
                    glVertex3f(0.0, wp, Left);
                    glVertex3f(0.0, wp, Right);
                }
            }
        }
        glEnd();
    }

    _active ?   glColor4ubv(CLR_GRIDF):glColor4ubv(CLR_GRIDD);

    glBegin(GL_LINES);
    for( int32_t fd= 0; fd<= Pd->_gridSize; fd+= (Pd->_gridStep * 10))
    {
        if (fd!= hdsz)
        {
            wp = hdsz - fd;
            if (wp >= (int32_t)Left && wp <= (int32_t)Right)
            {
                // Vertical lines
                glVertex3f(0.0, Top, wp);
                glVertex3f(0.0, Bottom, wp);
            }
            if (wp >= (int32_t)Bottom && wp <= (int32_t)Top)
            {
                // Horizontal lines
                glVertex3f(0.0, wp, Left);
                glVertex3f(0.0, wp, Right);
            }
        }
    }
    glEnd();


    glBegin(GL_LINES);
        // Horizontal lines
        glColor4ubv(CLR_AXES_Z);
        glVertex3f(0.0, 0.0, Left);
        glVertex3f(0.0, 0.0, Right);

    // Vertical lines
        glColor4ubv(CLR_AXES_Y);
        glVertex3f(0.0, Top, 0.0);
        glVertex3f(0.0, Bottom, 0.0);

    glEnd();

    glPopMatrix();

}



int32_t View2D::World2Wnd(f32_t coord, char dir)
{
    int32_t wpos;

    if (dir == 'h')
    {
        wpos = (int32_t)(_zoom * (coord - _shiftx));
    }
    else
        if (_vt != 'y')
            wpos = (int32_t)(_zoom * (coord - _shifty));
        else
            wpos = (int32_t)(_zoom * (coord + _shifty));

    if (dir == 'h')
        wpos += (int32_t)(0.5 * _rt.right());
    else
    {
        if (_vt != 'y')
            wpos *= -1;
        wpos += (int32_t)(0.5 * _rt.bottom());
    }
    return wpos;
}



u_bool32 View2D::IsBoxInViewPort(const Box& box)
{
    return true;
    switch(_vt)
    {
        case 'x':
            if(box._max.z < _vport.left)
                return false;
            if(box._min.z > _vport.right)
                return false;
            if(box._min.y > _vport.bottom)
                return false;
            if(box._max.y < _vport.top)
                return false;
            break;
        case 'y':
            if(box._max.x < _vport.left)
                return false;
            if(box._min.x > _vport.right)
                return false;
            if(box._min.z > _vport.bottom)
                return false;
            if(box._max.z < _vport.top)
                return false;
            break;
        case 'z':
            if(box._max.x < _vport.left)
                return false;
            if(box._min.x > _vport.right)
                return false;
            if(box._min.y > -_vport.top)
                return false;
            if(box._max.y < -_vport.bottom)
                return false;
            break;
    }
    return true;
}

u_bool32 View2D::IsVxInViewPort(const v3d_t& vx)
{
    return true; //TODO
    switch(_vt)
    {
        case 'x':
            if(vx.z < _vport.left)
                return false;
            if(vx.y < _vport.top)
                return false;
            break;
        case 'y':
            if(vx.x < _vport.left)
                return false;
            if(vx.z < _vport.top)
                return false;
            break;
        case 'z':
            if(vx.x < _vport.left)
                return false;
            if(vx.y < -_vport.bottom)
                return false;
            break;
    }
    return true;
}



void View2D::Reinit()
{
    _zoom   = 0.4f;                 //initial zoom
    _shifty = 0.0f;
    _shiftx = 0.0f;
    _selBox = FALSE;
    _active = FALSE;
    _canMove= 1;
    _scrPoint.setX(0);
    _scrPoint.setY(0);
    _selCenter  = V0;
    _ptDown     = V0;
}


void View2D::focusInEvent(QFocusEvent *p)
{
    _active=true;
    updateGL();
}
void View2D::focusOutEvent(QFocusEvent *p)
{
    _active=false;
    updateGL();
}
void View2D::closeEvent(QCloseEvent *p)
{
}


void View2D::PrintCoords(const QRect& wrect,
                           const FRECT& vrect,
                           const v3d_t& pct,
                           char vt,
                           u_bool32 active,
                           const char* cs,
                           QPoint* mt)
{
    if(Pd->_clearing || !Pd->b_showgrid)
        return;

    SelectTextCtx(wrect);

    switch(vt)
    {
        case 'x': //SIDE
            PrintOut(12,4,"+z");
            PrintOut(4,12,"+y");
            //PrintOut(2,wrect.bottom()-12,"SIDE", active ?  ZWHITE : ZRED);
            PrintOut(2,wrect.bottom()-12,MKSTR("SIDE %.2fx%.2f %s",
                                       (vrect.right-vrect.left)/GMeters,(vrect.bottom-vrect.top)/GMeters,
                                       GMeters==1 ? "cm" : ((GMeters==100 ? "m":"ft"))),
                                       active ? ZWHITE : ZRED);
            //V
            PrintOut(wrect.right()/2+2, wrect.bottom()-12,  MKSTR("+Y %4.2f",vrect.bottom/GMeters));
            PrintOut(wrect.right()/2+2, 12,               MKSTR("-Y %4.2f",vrect.top/GMeters));

            //H
            PrintOut(0,             wrect.bottom()/2+4,MKSTR("-Z %4.2f",vrect.left/GMeters));
            PrintOut(wrect.right()-52,wrect.bottom()/2+4,MKSTR("+Z %4.2f",vrect.right/GMeters));


            if(cs && *cs)
            {
                PrintOut(mt->x(),mt->y(), cs, ZGREEN);
            }

            break;
        case 'y': //TOP
            PrintOut(12,4,"+x");
            PrintOut(0,12,"-z");
            //PrintOut(2,wrect.bottom()-12,"TOP",active ? ZWHITE : ZGREEN);
            PrintOut(2,wrect.bottom()-12,MKSTR("TOP %.2fx%.2f %s",
                                       (vrect.right-vrect.left)/GMeters,(vrect.bottom-vrect.top)/GMeters,
                                       GMeters==1 ? "cm" : ((GMeters==100 ? "m":"ft"))),
                                       active ? ZWHITE : ZGREEN);
            //V
            PrintOut(wrect.right()/2+2,   wrect.bottom()-12,MKSTR("+Z %4.2f", vrect.bottom/GMeters));
            PrintOut(wrect.right()/2+2,   12,               MKSTR("-Z %4.2f",-vrect.top/GMeters));

            //H
            PrintOut(0,             wrect.bottom()/2+4,MKSTR("-X %4.2f",vrect.left/GMeters));
            PrintOut(wrect.right()-52,wrect.bottom()/2+4,MKSTR("+X %4.2f",vrect.right/GMeters));

            if(cs && *cs)
            {
                PrintOut(mt->x(),mt->y(), cs, ZGREEN);
            }

            break;
        case 'z'://FRONT
            PrintOut(12,2,"+x");
            PrintOut(4,12,"+y");
            //PrintOut(2,wrect.bottom()-12,"FRONT",active ? ZWHITE :ZBLUE);
            PrintOut(2,wrect.bottom()-12,MKSTR("FRONT %.2fx%.2f %s",
                                       (vrect.right-vrect.left)/GMeters,(vrect.bottom-vrect.top)/GMeters,
                                       GMeters==1 ? "cm" : ((GMeters==100 ? "m":"ft"))),
                                       active ? ZWHITE :ZBLUE);
            //V
            PrintOut(wrect.right()/2,wrect.bottom()-12,MKSTR("-Y %.2f",vrect.bottom/GMeters));

            PrintOut(wrect.right()/2,             12,MKSTR("+Y %.2f",vrect.top/GMeters));

            //H
            PrintOut(0,             wrect.bottom()/2+4,MKSTR("-X %.2f",vrect.left/GMeters));
            PrintOut(wrect.right()-52,wrect.bottom()/2+4,MKSTR("+X %.2f",vrect.right/GMeters));

            if(cs && *cs)
            {
                PrintOut(mt->x(),mt->y(), cs, ZGREEN);
            }

            break;
    }

    if(pct.len2())
        PrintOut(wrect.right()/2+70, wrect.top(), MKSTR("X: %003.2f Y: %003.2f S: %003.2f",
                                         pct.x,pct.y,pct.z),ZWHITE);

    DeSelectTextCtx();
}


void View2D::PrintOut(int32_t x, int32_t y, const QString& text,const u_rgba_t& clr)
{
/*
    glColor3ub(clr.r,clr.g,clr.b);
    glRasterPos2i( x, y );
    glPushAttrib(GL_LIST_BIT);
    glListBase( 2000-32 );
    glCallLists( strlen( text ), GL_UNSIGNED_BYTE, text );
    glPopAttrib();
*/

    glPushMatrix();
    glColor3ub(clr.r,clr.g,clr.b);
    renderText(x,y,text);
    glPopMatrix();
}

void View2D::_releasemouse()
{
    if(_mm._ldown+_mm._rdown+_mm._mdown==0)
    {
        //setMouseTracking(false);
        releaseMouse();
    }
}

void View2D::lButtonUp(QMouseEvent *p)
{
    Q_UNUSED(p);
    _mm._ldown=0;
    _releasemouse();

    _scaleDirDown = 0;
    _canMoveDown  = 0;
    _mm._ldown    = 0;
    _depl         = 0;
    /*
    if(_accmode == BR_VIEW)
    {
      if(_scrPoint.x!=0)
          SetCursorPos(_scrPoint.x, _scrPoint.y);
    }
      */
    QPoint point = p->pos();
    if(DOC()->_selBox._brushflags & BRSH_SELVIS)
    {
        _MannageCursor(point,0);
        DOC()->SelectByBox();
        DOC()->Invalidate();
        return;
    }

    if(_dirty)
    {
        DOC()->OnFinishMoveRotScale(_dirty, &_dirtyBrushes);
    }

    if(DOC()->_selmode != SEL_NONE && DOC()->_blocksel==FALSE)
    {
      QPoint pdiff = point - _mm._ptd;
      if(abs(pdiff.x()) < 2 && abs(pdiff.y()) < 2)
      {
          if(AKEY(Qt::Key_Shift) )
          {
              DoSelection(point);
          }
      }
    }

    _MannageCursor(point,0);
    _dirty = FALSE;
    _dirtyBrushes.clear();
    DOC()->Invalidate();
}

void View2D::DoSelection(const QPoint&  point)
{
    f32_t mxw = Wnd2World(point.x(), 'h');
    f32_t myw = Wnd2World(point.y(), 'v');

    _EL_SEL sel = DOC()->_selmode;

    if(SEL_NONE!=sel)
    {
        DOC()->DoSelection2D(mxw,myw,this, sel);
    }
    DOC()->Invalidate();
}


void View2D::rButtonUp(QMouseEvent *p)
{
    Q_UNUSED(p);
    _mm._rdown=0;
    _releasemouse();


}

void View2D::mButtonUp(QMouseEvent *p)
{
    Q_UNUSED(p);
    _mm._mdown=0;
    _releasemouse();
}

void View2D::mousePressEvent(QMouseEvent *p)
{
    Qt::MouseButton b =	p->button();
    if(_mm._ldown==0) _mm._ldown = (b == Qt::LeftButton);
    if(_mm._rdown==0) _mm._rdown = (b == Qt::RightButton);
    if(_mm._mdown==0) _mm._mdown = (b == Qt::MidButton);
    //_rotating  = _mm._ldown;
    _mm._pt = _mm._ptd = p->pos();
    _mm._ptd   = _mm._ptd;

    _scrPoint = p->globalPos();

    //setMouseTracking(true);

    _scaleDirDown = _scaleDir;
     _canMoveDown  = _canMove;
     CheckCoordRestrains();

     if(Pd->OnStartMoveRotScale())
     {
         //-GetSelectionItems(_mm._ptd);
         _arccircle[0] = V0;
         _arccircle[1] = V0;
         _arccircle[2] = V0;
     }
     if(0 == _canMoveDown || _accmode == BR_VIEW)
     {
        //-HideTheCursor();
     }


    _dirty = FALSE;
    //grabMouse();
    //HideTheCursor();
    Pd->Invalidate(true);

}

void View2D::CheckCoordRestrains()
{
    _depl = 0;
    // allow the moving scaling onl;y on the coord that has been pressed
    switch(_vt)
    {
        case 'x':       // can block Z or Y (SIDE)
            if(theApp->isKeyDown('X')) _depl = 2;
            if(theApp->isKeyDown('Y')) _depl = 3;
            break;
        case 'y':       // can block Z or X (TOP)
            if(theApp->isKeyDown('X'))_depl=3;
            if(theApp->isKeyDown('Y'))_depl=1;
            break;
        case 'z':       // can block X or Y (FRONT)
            if(theApp->isKeyDown('X'))_depl=2;
            if(theApp->isKeyDown('Y'))_depl=1;
            break;
    }
}


void View2D::mouseReleaseEvent(QMouseEvent *p)
{
    Qt::MouseButton b =	p->button();
    if(b==Qt::LeftButton)
            emit lButtonUp(p);
    if(b==Qt::RightButton)
            emit rButtonUp(p);
    if(b==Qt::MidButton)
            emit mButtonUp(p);


}

void View2D::mouseDoubleClickEvent(QMouseEvent *p)
{
}

void View2D::mouseMoveEvent(QMouseEvent *p)
{
    QPoint point = p->pos();
    QRect rt= this->rect();

    if(point.x()>=rt.right()){point.setX(rt.right());return;}
    if(point.x()<=0){point.setX(0);;return;}
    if(point.y()>=rt.bottom()){point.setY(rt.bottom());;return;}
    if(point.y()<=0){point.setY(0);;return;}


    f32_t fx = Wnd2World(point.x(), 'h');
    f32_t fy = Wnd2World(point.y(), 'v');



    MouseWorldMove(fx, fy, point, p->globalPos());
    _wmouse.setX(point.x()+16);
    _wmouse.setY(point.y()+16);

    _onSetCursor(point);
    SBT(0, MKSTR("%f,%f", fx,fy));
}

void View2D::keyPressEvent(QKeyEvent *p)
{
}

void View2D::keyReleaseEvent(QKeyEvent *p)
{
}

void View2D::MouseWorldMove(f32_t mxw, f32_t  myw, const QPoint& point, const QPoint& spoint)
{
    if(_accmode == BR_MOVE)
    {
        switch(_vt)
        {
            case 'x':
                SBT(3,MKSTR("M: z:%04.2f y:%04.2f z:%1.4f", mxw/GMeters, myw/GMeters, _zoom));
                break;
            case 'y':
                SBT(3,MKSTR("M: x:%04.2f z:%04.2f z:%1.4f", mxw/GMeters, myw/GMeters, _zoom));
               break;
            case 'z':
                SBT(3,MKSTR("M: x:%04.2f y:%04.2f z:%1.4f", mxw/GMeters, myw/GMeters, _zoom));
                break;
        }
    }

    int32_t   selMode = Pd->_selmode;
    _accmode = Pd->_brmode;
    if(_mm._ldown)
    {
        QPoint pt = _mm._pt - point;
        /*
        if(DOC()->_bSnapGrid)
        {
            if(abs(pt.x) >= 16 || abs(pt.y)>=SnapToXY)
                _mm._pt   = point;
            else
                return;
        }
        else
        {
        */
            _mm._pt   = point;
        //}

        if(theApp->isKeyDown(Qt::ShiftModifier))
        {
            ScrollView(pt);
        }
        else
        {
            CheckCoordRestrains();
            switch(_accmode)//BR_VIEW
            {
                case BR_VIEW:
                    ScrollView(pt);
                    break;
                case BR_MOVE:
                    if(Pd->_selBox._brushflags & BRSH_SELVIS)
                    {
                        MoveBrush(pt);
                    }
                    else
                    {
                        if(selMode == SEL_BRUSH)
                        {
                            MoveBrush(pt);
                            if(Pd->_allselected)
                                MoveItem(pt);
                        }
                        else if(SEL_ITEMS == selMode )
                        {
                            MoveItem(pt);
                        }
                        else if(SEL_VERTEX == selMode)
                        {
                            MoveVertex(pt);
                        }
                        else if(SEL_FACE == selMode)
                        {
                            MovePoly(pt);
                        }
                    }
                    break;
                case BR_SCALE:
                    if(selMode == SEL_BRUSH ||(Pd->_selBox._brushflags & BRSH_SELVIS))
                        ScaleBrush(pt, mxw, myw);
                    else if(SEL_ITEMS == selMode )
                        ScaleItem(pt);
                    else if(SEL_FACE == selMode)
                        ScalePoly(pt);
                    break;
                case BR_ROTATE:
                    if(selMode == SEL_BRUSH)
                        RotateBrush(pt);
                    else if(SEL_ITEMS == selMode )
                        RotateItem(pt);
                    else if(SEL_FACE == selMode)
                        RotatePoly(pt);
                    break;
                case BR_RISE:
                    RaiseBrush(pt);
                    break;
            }
        }
        if(GUpdateImm)
            Pd->Invalidate3D(1) ;
    }
    else if (_mm._rdown)
    {
        QPoint pt = _mm._pt - point;
        _mm._pt   = point;

        if(_zoom<.01)
            _zoom += (f32_t)pt.y()/8256.0;
        else if(_zoom<.1)
            _zoom += (f32_t)pt.y()/1256.0;
        else
            _zoom += (f32_t)pt.y()/256.0;

        CLAMPVAL(_zoom,.002,8.f);

        if(Pd->_xyz_lock)
        {
            Pd->SetZoom(this);
        }
        Pd->Invalidate();
    }
    else
    {

        _scaleDir = 0;
       // setMouseTracking(false);
        releaseMouse();

    }
    if(_mm._ldown || _mm._rdown)
    {
        switch(_vt)
        {
            case 'x':
                SBT(1, MKSTR("2D: x=%04.2f z=%04.2f * %1.4f", _shiftx/GMeters, _shifty/GMeters, _zoom));

                break;
            case 'y':
                SBT(1, MKSTR("2D: z=%04.2f y=%04.2f * %1.4f", _shiftx/GMeters, _shifty/GMeters, _zoom));
               break;
            case 'z':
                SBT(1, MKSTR("2D: x=%04.2f y=%04.2f * %1.4f", _shiftx/GMeters, _shifty/GMeters, _zoom));
                break;
        }

        if(0 == _canMoveDown || _accmode == BR_VIEW)
        {
            Rollover(spoint);
        }
    }
    QRect lr = _rt;
    lr.setSize(QSize(lr.width()-4,lr.height()-4));
}



void    View2D::ScrollView(const QPoint& pt)
{
    _shiftx += (f32_t)pt.x() / _zoom;
    _shifty -= (f32_t)pt.y() / _zoom;

    if(DOC()->_xyz_lock)
    {
        DOC()->SetShift(this);
    }
    else
    {
        switch(_vt)
        {
            case 'x':
                SBT(3, MKSTR("2D: x=%04.2f z=%04.2f * %2.4f", _shiftx/GMeters, _shifty/GMeters, _zoom));
                break;
            case 'y':
                SBT(3, MKSTR("2D: z=%04.2f y=%04.2f * %2.4f", _shiftx/GMeters, _shifty/GMeters, _zoom));
               break;
            case 'z':
                SBT(3 , MKSTR("2D: x=%04.2f y=%04.2f * %2.4f", _shiftx/GMeters, _shifty/GMeters, _zoom));
                break;
        }
    }
    DOC()->Invalidate();
}


void    View2D::MoveBrush(const QPoint& pt)
{
    if(0 == _canMoveDown)
    {
        ScrollView(pt);
        return;
    }
    v3d_t  t = _GetTranslation(pt);
    //DOC()->RoundToNear(t);

    if(DOC()->IsSelBoxOn())
    {
        DOC()->_selBox.Move(t);
        DOC()->_selBox.Recalc();
        DrawView();
    }
    else
    {
        int         count;
        Brush** b = DOC()->GetSelBrushes(count);

        if(_depl==1) t.x=0;
        if(_depl==2) t.y=0;
        if(_depl==3) t.z=0;

        if(1== count && (*b)->HasMotion() && !(*b)->IsDisabled())
        {
            count = _MoveBrushMotion((*b)->_pMotion, t);
        }
        v3d_t  c;
        DOC()->OnStartMoveRotScale(M_MOVED, b, count);
        for(int i=0; i < count; i++, b++)
        {
            if((*b)->IsDisabled())
                continue;

            (*b)->Move(t);
            (*b)->Recalc();
            c =(*b)->_box.GetCenter();
            SetCursorText(_wmouse, MKSTR("x:%4.2f y:%4.2f z:%4.2f", c.x/GMeters, c.y/GMeters, c.z/GMeters));
        }
        _dirty = M_MOVED;
    }
    DrawView();
}

void View2D::DrawView()
{
    DOC()->Invalidate(true);
    DOC()->Invalidate3D();
}


void    View2D::MoveItem(const QPoint& pt)
{
    if(0 == _canMoveDown)
    {
        ScrollView(pt);
        return;
    }
    v3d_t  t = _GetTranslation(pt);
    //DOC()->RoundToNear(t);

    if(DOC()->IsSelBoxOn())
    {
        DOC()->_selBox.Move(t);
        DOC()->_selBox.Recalc();
    }
    else
    {
        int         count;
        SceItem**   it = DOC()->GetSelItems(count);

        if(_depl==1) t.x=0;
        if(_depl==2) t.y=0;
        if(_depl==3) t.z=0;

        if(count)
        {
            DOC()->OnStartMoveRotScale(M_MOVED, it, count);
            for(int i=0;i<count;i++,it++)
            {
                (*it)->Move(t);
            }
        }
    }
    DrawView();
}

void    View2D::ScaleItem(const QPoint& pt)
{
    v3d_t s,t;
    if(0 == _scaleDirDown)
    {
        ScrollView(pt);
        return;
    }
    if(_scaleDirDown == SIZE_MOVE)
    {
        MoveItem(pt);
        return;
    }
    _GetScaleValues(pt, s,t);
    //DOC()->RoundToNear(s);
    //DOC()->RoundToNear(t);

    if(DOC()->IsSelBoxOn())
    {
        DOC()->_selBox.Scale(s,t, _zoom);
        DOC()->_selBox.Recalc();
        v3d_t ex = DOC()->_selBox._box.GetExtends();
        SBT(3,MKSTR("w:%4.2f h:%4.2f d:%4.2f", ex.x/GMeters, ex.y/GMeters, ex.z/GMeters));
    }
    else
    {
        if(s.x==0)t.x=0;
        if(s.y==0)t.y=0;
        if(s.z==0)t.z=0;

        int count;
        SceItem** it = DOC()->GetSelItems(count);
        if(count)
        {
            DOC()->OnStartMoveRotScale(M_MOVED, it, count);

            f32_t radius = 0;
            for(int i=0;i<count;i++,it++)
            {
                (*it)->Scale(s,t,_zoom);
            }
            DrawView();
            _dirty = ITM_SCALED;
        }
    }
}

void    View2D::ScaleBrush(const QPoint& pt, f32_t mxw, f32_t myz)
{
    v3d_t s,t;
    if(0 == _scaleDirDown)
    {
        ScrollView(pt);
        return;
    }
    if(_scaleDirDown == SIZE_MOVE)
    {
        MoveBrush(pt);
        return;
    }
    _GetScaleValues(pt, s,t);
    //DOC()->RoundToNear(s);
    //DOC()->RoundToNear(t);

    if(DOC()->IsSelBoxOn())
    {
        DOC()->_selBox.Scale(s,t, _zoom);
        DOC()->_selBox.Recalc();
        DrawView();

        v3d_t ex = DOC()->_selBox._box.GetExtends();
        SBT(3,MKSTR("w:%4.2f h:%4.2f d:%4.2f", ex.x/GMeters, ex.y/GMeters, ex.z/GMeters));
        switch(_vt)
        {
            case 'x':
                SetCursorText(_wmouse, MKSTR("h:%4.2f d:%4.2f", ex.y/GMeters, ex.z/GMeters));
                break;
            case 'y':
                SetCursorText(_wmouse, MKSTR("w:%4.2f d:%4.2f", ex.x/GMeters, ex.z/GMeters));
                break;
            case 'z':
                SetCursorText(_wmouse, MKSTR("w:%4.2f h:%4.2f", ex.x/GMeters, ex.y/GMeters));
                break;
        }
        DOC()->Invalidate();
    }
    else
    {
        Box box;
        int count;
        Brush** b = DOC()->GetSelBrushes(count);
        if(count)
        {
            DOC()->OnStartMoveRotScale(M_SCALED, b, count);
            for(int i=0;i<count;i++,b++)
            {
                if((*b)->IsDisabled())
                    continue;
                if(s.x==0)t.x=0;
                if(s.y==0)t.y=0;
                if(s.z==0)t.z=0;

                (*b)->Scale(s,t,_zoom);
                (*b)->Recalc();
                box.Union((*b)->_box); // for sbar text
            }
            DrawView();
            v3d_t ex = box.GetExtends();
            SBT(3,MKSTR("w:%4.2f h:%4.2f d:%4.2f", ex.x/GMeters, ex.y/GMeters, ex.z/GMeters));
            switch(_vt)
            {
                case 'x':
                    SetCursorText(_wmouse, MKSTR("h:%4.2f d:%4.2f", ex.y/GMeters, ex.z/GMeters));
                    break;
                case 'y':
                    SetCursorText(_wmouse, MKSTR("w:%4.2f d:%4.2f", ex.x/GMeters, ex.z/GMeters));
                    break;
                case 'z':
                    SetCursorText(_wmouse, MKSTR("w:%4.2f h:%4.2f", ex.x/GMeters, ex.y/GMeters));
                    break;
            }
            _dirty = M_SCALED;
            if(GUpdateImm)
                DOC()->Invalidate3D(1) ;
        }
    }
}

void    View2D::ScalePoly(const QPoint& pt)
{
    v3d_t s,t;
    if(0 == _scaleDirDown)
    {
        ScrollView(pt);
        return;
    }
    if(_scaleDirDown == SIZE_MOVE)
    {
        MoveBrush(pt);
        return;
    }
    _GetScaleValues(pt, s,t);
    //DOC()->RoundToNear(s);
    //DOC()->RoundToNear(t);

    vvector<v3d_t*>       toMove(32);
    int     count    = 0;
    Poly**  pppoly   = DOC()->GetSelPolys(count);
    if(count !=1 )
    {
        SBT(0,"Cannot resize more than one polygon");
        Beep(600,20);
        return;
    }

    Poly* pPoly  = *pppoly;
    if(!(pPoly->_polyflags & POLY_EXTRUDED))
    {
        Beep(600,20);
        SBT(0,"Cannot scale non-extruded polygon");
        return;
    }

    Brush* pBrush = pPoly->_pBrush;
    v3d_t     center;

    DOC()->OnStartMoveRotScale(M_SCALED, &pBrush, 1);

    FOREACH(vvector<Vtx2>, pPoly->_vtci, pvtcis)
    {
        toMove << &pvtcis->_xyz;
        center += pvtcis->_xyz;
        // search this vx in the brush and add it to the toMove;
        FOREACH(vvector<Poly>, pBrush->_polys, ppoly)
        {
            if(&(*ppoly)== pPoly)
                continue;
            FOREACH(vvector<Vtx2>, ppoly->_vtci, potherv)
            {
                if(potherv->_xyz == pvtcis->_xyz)
                    toMove << &potherv->_xyz;
            }
        }
    }
    center /= (f32_t)pPoly->_vtci.size();
    Box b;
    Box b2;
    FOREACH(vvector<v3d_t*>, toMove, ppv3d_t){
        b.AddPoint(**ppv3d_t);
        b2.AddPoint(**ppv3d_t);
    }

    b2._max.x += s.x;
    b2._max.y += s.y;
    b2._max.z += s.z;

    v3d_t den = b._max-b._min;
    if(den.x==0)den.x=1;
    if(den.y==0)den.y=1;
    if(den.z==0)den.z=1; // fix .#IND
    v3d_t rap = (b2._max-b2._min)/den;
    FOREACH(vvector<v3d_t*>, toMove, ppv3d_t){
        v3d_t& vx  = **ppv3d_t;
        vx      = b._min + (vx-b._min) * rap;
        vx      += t;
    }
    this->Add4Update(pBrush);
    DrawView();
}

void    View2D::RotateItem(const QPoint& pt)
{
    if(0 == _canMoveDown)
    {
        ScrollView(pt);
        return;
    }
    int count;
    SceItem** it = DOC()->GetSelItems(count);
    if(count)
    {
        GetSelCenter(it, count);
        v3d_t      r = GetMouseRotAngle(pt);
        _cccGrd += r;
        SBT(0, MKSTR("%2.2f° %2.2f° %2.2f°",R2G(_cccGrd.x), R2G(_cccGrd.y), R2G(_cccGrd.z)));
        DOC()->OnStartMoveRotScale(ITM_ROTATED, it, count);

        int aroundOrigin = AKEY(Qt::Key_Menu);
        for(int i=0;i<count;i++,it++)
        {
            (*it)->Rotate(r, aroundOrigin);
        }
        DrawView();
        _dirty = ITM_ROTATED;
    }
}

void    View2D::RotateBrush(const QPoint& pt)
{
    if(0 == _canMoveDown)
    {
        ScrollView(pt);
        return;
    }
    int count;
    Brush** b = DOC()->GetSelBrushes(count);
    int bao = AKEY(Qt::Key_Menu);
    //MERGE>


    if(count)
    {
        v3d_t   r  = GetMouseRotAngle(pt);
        _cccGrd += r;
        ROLL2PI(_cccGrd.x);
        ROLL2PI(_cccGrd.y);
        ROLL2PI(_cccGrd.z);
        SBT(0, MKSTR("%2.2f° %2.2f° %2.2f°",R2G(_cccGrd.x), R2G(_cccGrd.y), R2G(_cccGrd.z)));
        DOC()->OnStartMoveRotScale(ITM_ROTATED, b, count);
        for(int i=0;i<count;i++,b++)
        {
            if((*b)->IsDisabled())
                continue;
            (*b)->Rotate(r, bao);
            (*b)->Recalc();
        }
        DrawView();
        _dirty = M_ROTATED;
    }
}

void    View2D::RotatePoly(const QPoint& pt)
{
    if(0 == _canMoveDown)
    {
        ScrollView(pt);
        return;
    }
    v3d_t   r        = GetMouseRotAngle(pt);
    int  count    = 0;
    if(AKEY('T') || AKEY('t'))
    {
        Poly**  pppoly   = DOC()->GetSelPolys(count);
        for(int i=0;i<count;i++,pppoly++)
        {
            (*pppoly)->RotateTCRad(R2G((r.x+r.y+r.z)), 0);
        }
    }
    else
    {
        vvector<v3d_t*>       toMove(32);

        Poly**  pppoly   = DOC()->GetSelPolys(count);
        if(count !=1)
        {
            Beep(600,20);
            SBT(0,"Cannot rotate more than one polygon");
            return;
        }

        Poly* pPoly   = *pppoly;
        Brush* pBrush = pPoly->_pBrush;
        Box   b;
/*
        if(!(pBrush->_brushflags & BRSH_TRIANGLES))
        {
            Beep(600,20);
            SBT(0,"Cannot rotate while brush is not triangulated");
            return;
        }
*/

        DOC()->OnStartMoveRotScale(ITM_ROTATED, &pBrush, 1);

        FOREACH(vvector<Vtx2>, pPoly->_vtci, pvtcis)
        {
            toMove << &pvtcis->_xyz;
            b.AddPoint(pvtcis->_xyz);
            // search this vx in the brush and add it to the toMove;
            FOREACH(vvector<Poly>, pBrush->_polys, ppoly)
            {
                if(&(*ppoly) == pPoly)
                    continue;
                FOREACH(vvector<Vtx2>, ppoly->_vtci, potherv)
                {
                    if(potherv->_xyz == pvtcis->_xyz)
                        toMove << &potherv->_xyz;
                }
            }
        }

        _cccGrd += r;
        ROLL2PI(_cccGrd.x);
        ROLL2PI(_cccGrd.y);
        ROLL2PI(_cccGrd.z);
        SBT(0, MKSTR("%2.2f° %2.2f° %2.2f°",R2G(_cccGrd.x), R2G(_cccGrd.y), R2G(_cccGrd.z)));

        m_16t mr;
        if(DOC()->_selEdges.size()) //rot around the edge
        {
            PolyEdge& pe = DOC()->_selEdges[0];
            f32_t rotangle = tmax(r.x,r.y);
                 rotangle = tmax(rotangle,r.z);
            v3d_t   edge = (pe._edge[1]->_xyz - pe._edge[0]->_xyz);
            // rotate around edge
            mr = MRotAround(edge, rotangle);
        }
        else
        {
            mr = MRadRotate(r.x,r.y,r.z);
            FOREACH(vvector<v3d_t*>, toMove, ppv3d_t)
            {
                v3d_t& rv = *(*ppv3d_t);
                rv-=b.GetCenter();
                mr.v3rotate(**ppv3d_t);
                rv+=b.GetCenter();
            }
        }
        DrawView();
        this->Add4Update(pBrush);
    }
}

void    View2D::MoveVertex(const QPoint& pt)
{
    int i;
    if(0 == _canMoveDown)
    {
        ScrollView(pt);
        return;
    }
    v3d_t  t = _GetTranslation(pt);
    //DOC()->RoundToNear(t);

    int    count  = 0;
    Vtx2** ppvx   = DOC()->GetSelVxes(count);
    Brush* pBrush = (*ppvx)->_pPoly->_pBrush;
    if(!pBrush)
        return;

    for( i = 0; i < count; i++, ppvx++)
    {
        if((*ppvx)->_pPoly->_pBrush != pBrush)
        {
            Beep(600,20);
            SBT(0,"Vertexes should be on the same brush");
            return;
        }
    }
    if(!(pBrush->_brushflags & BRSH_TRIANGLES))
    {
        Beep(600,20);
        SBT(0,"Cannot move while brush is not triangulated");
        return;
    }

    DOC()->OnStartMoveRotScale(ITM_ROTATED, &pBrush, 1);

    while(--i>=0)
    {
        --ppvx;
        (*ppvx)->_xyz += t;
    }
    DrawView();
    Add4Update(pBrush);
}



void    View2D::MovePoly(const QPoint& pt)
{
    if(0 == _canMoveDown)
    {
        ScrollView(pt);
        return;
    }
    v3d_t  t = _GetTranslation(pt);
    //DOC()->RoundToNear(t);

    vvector<v3d_t*>       toMove(32);
    int     count    = 0;
    Poly**  pppoly   = DOC()->GetSelPolys(count);
    if(count !=1 )
    {
        SBT(0,"Cannot move more than one polygon");
        Beep(600,20);
        return;
    }
    Poly* pPoly  = *pppoly;

    Brush* pBrush = pPoly->_pBrush;
    if(pBrush==0)
    {
        SBT(0,"Error selection parent brush");
        Beep(600,20);
        return;
    }
    DOC()->OnStartMoveRotScale(M_MOVED, &pBrush, count);

    FOREACH(vvector<Vtx2>, pPoly->_vtci, pvtcis)
    {
        toMove << &pvtcis->_xyz;

        // search this vx in the brush and add it to the toMove;
        FOREACH(vvector<Poly>, pBrush->_polys, ppoly)
        {
            if(&(*ppoly) == pPoly)
                continue;
            FOREACH(vvector<Vtx2>, ppoly->_vtci, potherv)
            {
                if(::IsZero(potherv->_xyz-pvtcis->_xyz,EPSMED))
                {
                    toMove << &potherv->_xyz;
                }
            }
        }
    }

    if(_depl==1) t.x=0;
    if(_depl==2) t.y=0;
    if(_depl==3) t.z=0;

    if(AKEY('N')||AKEY('n'))
    {
        FOREACH(vvector<v3d_t*>, toMove, ppv3d_t)
        {
            (*ppv3d_t)->x += (pPoly->_n.x * (t.x+t.y+t.z));
            (*ppv3d_t)->y += (pPoly->_n.y * (t.x+t.y+t.z));
            (*ppv3d_t)->z += (pPoly->_n.z * (t.x+t.y+t.z));
        }
    }
    else
    {
        FOREACH(vvector<v3d_t*>, toMove, ppv3d_t)
        {
            (*ppv3d_t)->x+=t.x;
            (*ppv3d_t)->y+=t.y;
            (*ppv3d_t)->z+=t.z;
        }
    }
    this->Add4Update(pBrush);
    DrawView();
}

void    View2D::RaiseBrush(const QPoint& pt)
{
    if(0 == _canMoveDown)
    {
        ScrollView(pt);
    }
    else
    {
        v3d_t     t;
        switch(_vt)
        {
            case 'x':   // side view
                switch(_scaleDir)
                {
                    case SIZE_N:
                        break;
                    case SIZE_S:
                        break;
                    case SIZE_E:
                        break;
                    case SIZE_W:
                        break;
                }
                break;
            case 'y':   //top view
                switch(_scaleDir)
                {
                    case SIZE_N:
                        break;
                    case SIZE_S:
                        break;
                    case SIZE_E:
                        break;
                    case SIZE_W:
                        break;
                }
               break;
            case 'z': // front view
                switch(_scaleDir)
                {
                    case SIZE_N:
                        break;
                    case SIZE_S:
                        break;
                    case SIZE_E:
                        break;
                    case SIZE_W:
                        break;
                }
                break;
        }


        int     count;
        Brush** b = DOC()->GetSelBrushes(count);
        if(count)
        {
            DOC()->OnStartMoveRotScale(ITM_ROTATED, b, count);
            for(int i=0;i<count;i++,b++)
            {
                (*b)->Raise(t, _zoom);
            }
            DrawView();
            _dirty = M_SHIFTED;
        }
    }
}


void    View2D::Rollover(const QPoint pt)
{
    QWidget* pw = QApplication::widgetAt(pt);

    if(pw && !pw->hasMouseTracking() )
    {
        if(_scrPoint.x()!=0)
            QCursor::setPos(pt);
        _mm._pt  = _mm._ptd;
    }
}

void    View2D::SetCursorText(const QPoint& pt, const char* str)
{
    _textmouse = pt;
    if(str && *str)
        cs_ct = str;
    else
        cs_ct ="";
}


int    View2D::_MoveBrushMotion( CMotion* pM, v3d_t & t)
{
    int nNode;
    if((nNode = pM->GetSelNode()) > 0) // mode sel node
    {
        //extend all nodes in this direction
        MState*    pS = pM->GetSelState();
        v3d_t         nP = pS->_pos + t;

        if(AKEY(Qt::Key_Control))
        {
            // eqidist all points from start to here
            MState*    pSHead = pM->GetState(0);
            v3d_t      calc;

            for(int i=1 ; i < nNode; i++)
            {
                double  stp = (double)i/(double)nNode;
                calc = pSHead->_pos + ((nP-pSHead->_pos)*stp);
                pM->GetState(i)->_pos = calc;
            }
        }
        pS->_pos = nP;
        return 0;
    }
    return 1;
}

void     View2D::_GetScaleValues( const QPoint& ptin, v3d_t& s, v3d_t& t)
{
    QPoint pt=ptin;

    //prelilinari elimination by cursor shape
    if(_scaleDirDown == SIZE_N || _scaleDirDown == SIZE_S)
        pt.setX(0);
    if(_scaleDirDown == SIZE_E || _scaleDirDown == SIZE_W)
        pt.setY(0);



    if(AKEY('J') || AKEY('j'))
    {
        if(pt.y())
            pt.setX(pt.y());
        else
            pt.setY(pt.x());
    }

    switch(_vt)
    {
        case 'y':	//X
            s.x = -(f32_t)pt.x() / _zoom;
            s.z = -(f32_t)pt.y() / _zoom;

            switch(_scaleDirDown)
            {
                case SIZE_NE:
                case SIZE_N:
                    s.z=-s.z;
                    t.z=-s.z;
                    break;
                case SIZE_SW:
                case SIZE_W:
                    s.x=-s.x;
                    t.x=-s.x;
                    break;
                case SIZE_NW:
                    s.z=-s.z;
                    t.z=-s.z;
                    s.x=-s.x;
                    t.x=-s.x;
                    break;
            }
            break;

    case 'x':	//-z Y
            s.z = -(f32_t)pt.x() / _zoom;
            s.y = (f32_t)pt.y()  / _zoom;

            switch(_scaleDirDown)
            {
                case SIZE_NE:
                case SIZE_N:
                    s.y=-s.y;
                    t.y=-s.y;
                    break;
                case SIZE_SW:
                case SIZE_W:
                    s.z=-s.z;
                    t.z=-s.z;
                    break;
                case SIZE_NW:
                    s.y=-s.y;
                    t.y=-s.y;
                    s.z=-s.z;
                    t.z=-s.z;
                    break;
            }
            break;

        case 'z':
            s.x = -(f32_t)pt.x() / _zoom;
            s.y = (f32_t)pt.y()  / _zoom;

            switch(_scaleDirDown)
            {
                case SIZE_SW:
                    s.x=-s.x;
                    t.x=-s.x;
                    s.y=-s.y;
                    t.y=-s.y;
                    break;
                case SIZE_NW:
                case SIZE_W:
                    s.x=-s.x;
                    t.x=-s.x;
                    break;
                case SIZE_SE:
                case SIZE_S:
                    s.y=-s.y;
                    t.y=-s.y;
                    break;
            }
            break;
    }
}

void     View2D::_DrawLines(int hgsz,
                                GLfloat Left,
                                GLfloat Right,
                                GLfloat Top,
                                GLfloat Bottom ,
                                int     wp)
{
    if(_zoom>.04)
    {
        _active ? glColor4ubv(CLR_FINEGRIDF) : glColor4ubv(CLR_FINEGRIDD);
        glBegin(GL_LINES);
        for(int vDec = 0; vDec <= DOC()->_gridSize; vDec += DOC()->_gridStep)
        {
            if (vDec % (DOC()->_gridStep * 10) && vDec != hgsz)
            {
                wp = hgsz - vDec;
                if (wp > (int)Left && wp < (int)Right)
                {
                    glVertex3f(wp, 0.0, Top);
                    glVertex3f(wp, 0.0, Bottom);
                }
                if (wp < (int)Bottom && wp > (int)Top)
                {
                    glVertex3f(Left, 0.0, wp);
                    glVertex3f(Right, 0.0, wp);
                }
            }
        }
        glEnd();
    }

    _active ?   glColor4ubv(CLR_GRIDF):glColor4ubv(CLR_GRIDD);
    glBegin(GL_LINES);
    for(int vDec = 0; vDec <= DOC()->_gridSize; vDec += (DOC()->_gridStep * 10))
    {
        if (vDec != hgsz)
        {
            wp = hgsz - vDec;
            if (wp >= (int)Left && wp <= (int)Right)
            {
                glVertex3f(wp, 0.0, Top);
                glVertex3f(wp, 0.0, Bottom);
            }
            if (wp <= (int)Bottom && wp >= (int)Top)
            {
                glVertex3f(Left, 0.0, wp);
                glVertex3f(Right, 0.0, wp);
            }
        }
    }
    glEnd();

    glBegin(GL_LINES);

        glColor4ubv(CLR_AXES_X);
        glVertex3f((int)Left, 0, 0);
        glVertex3f((int)Right, 0, 0);

        glColor4ubv(CLR_AXES_Z);
        glVertex3f(0.0, 0.0, Top);
        glVertex3f(0.0, 0.0, Bottom);


    glEnd();
}

void    View2D::Add4Update(Brush* pb)
{
    _dirty = M_MOVED;
    pb->Dirty(1);
    _dirtyBrushes << pb;
}


void View2D::GetSelCenter(Poly** pPoly, int count)
{
    _selCenter.reset();
    f32_t fnc = count;
    while(--count>=0)
        _selCenter+=(pPoly[count]->_box.GetCenter());
    _selCenter/=fnc;
}

void View2D::GetSelCenter(SceItem** it, int count)
{
    _selCenter.reset();
    f32_t fnc = count;
    while(--count>=0)
        _selCenter+=it[count]->_t;
    _selCenter/=fnc;

}

void View2D::GetSelCenter(Brush** it, int count)
{
    _selCenter.reset();
    f32_t fnc = count;
    while(--count>=0)
        _selCenter+=(it[count]->_box.GetCenter());
    _selCenter/=fnc;
}


v3d_t View2D::_GetTranslation(const QPoint& pt)
{
    v3d_t t;
    switch(_vt)
    {
        default:
        case 'x':
            t.z = -(f32_t)pt.x() / _zoom;
            t.y = (f32_t)pt.y() / _zoom;
            break;
        case 'y':
            t.x = -(f32_t)pt.x() / _zoom;
            t.z = -(f32_t)pt.y() / _zoom;
           break;
        case 'z':
            t.x = -(f32_t)pt.x() / _zoom;
            t.y = (f32_t)pt.y() / _zoom;
            break;
    }
    return t;
}


v3d_t   View2D::GetMouseRotAngle(const QPoint& pt)
{
 /*
    V3 r;
    switch(_vt)
    {
        case 'x':
              r.x = pt.x/128.f;
              r.x = pt.y/128.f;
              break;

        case 'y':
              r.y = pt.x/128.f;
              r.y = pt.y/128.f;
              break;
        case 'z':
              r.z = pt.x/128.f;
              r.z = pt.y/128.f;
              break;
    }

    if(AKEY(VK_SPACE))
    {
        if(r.x)
            r.x/=32.f;
        if(r.y)
            r.y/=32.f;
        if(r.z)
            r.z/=32.f;
    }
    return r;

*/
    v3d_t   mult;
    v3d_t   vCenter = _selCenter;
    v3d_t   v3down;
    v3d_t   v3cur;
    v3d_t   vPdt;


    f32_t dsx = Wnd2World(_mm._ptd.x(), 'h');
    f32_t dsy = Wnd2World(_mm._ptd.y(), 'v');

    f32_t sx = Wnd2World(_mm._pt.x(), 'h');
    f32_t sy = Wnd2World(_mm._pt.y(), 'v');
    f32_t fx = Wnd2World(_mm._pt.x() + pt.x(), 'h');
    f32_t fy = Wnd2World(_mm._pt.y() + pt.y(), 'v');

    switch(_vt)
    {
        case 'x':

            vPdt.x = vCenter.x;
            vPdt.z = dsx;
            vPdt.y = dsy;

            v3down.x = vCenter.x;
            v3down.z = sx;
            v3down.y = sy;

            v3cur.x = vCenter.x;
            v3cur.z = fx;
            v3cur.y = fy;
            mult = VX;
            _grades = (_cccGrd.x);
            break;

        case 'y':
            vPdt.y = vCenter.y;
            vPdt.x = dsx;
            vPdt.z = dsy;


            v3down.y = vCenter.y;
            v3down.x = sx;
            v3down.z = sy;

            v3cur.y = vCenter.y;
            v3cur.x = fx;
            v3cur.z = fy;
            mult = VY;
            _grades = (_cccGrd.y);
           break;

        case 'z':
            vPdt.z = vCenter.z;
            vPdt.x = dsx;
            vPdt.y = dsy;

            v3down.z = vCenter.z;
            v3down.x = sx;
            v3down.y = sy;

            v3cur.z = vCenter.z;
            v3cur.x = fx;
            v3cur.y = fy;
            mult = VZ;
            _grades = (_cccGrd.z);
            break;
    }


    v3d_t di = (v3down - vCenter).norm();
    v3d_t dc = (v3cur  - vCenter).norm();

    _arccircle[0] = vCenter;
    _arccircle[1] = vCenter + (vPdt-vCenter).norm() * 128.0/_zoom;
    _arccircle[2] = vCenter + dc * 128.0/_zoom;



    f32_t cosang = Vdp(di, dc);
    mult *= acos(cosang);

    // see the sign
    v3d_t perp = Vcp(di, dc).norm();
    f32_t r = perp.x ? perp.x : (perp.y?perp.y:perp.z);
    if(r)
    {
        int br = r > 0 ? -1 : 1;

        switch(_vt)
        {
            case 'x':
                mult*=br;
                break;
            case 'y':
                mult*=br;
                break;
            case 'z':
                mult*=br;
                break;
        }
        return mult;
    }
    return V0;
}




f32_t    View2D::Wnd2World(f32_t coord, char dir)
{
    int      itemp = 0;
    f32_t     fWp = 0.0;

    if(dir=='h')
    {
        itemp = (int)(coord + (-0.5 * _rt.right()));
        fWp = itemp / _zoom + _shiftx;
    }
    else
    {
        if(_vt != 'y')
        {
            itemp = (int)(-1 * (coord + (-0.5 * _rt.bottom())));
            fWp = itemp / _zoom + _shifty;
        }
        else
        {
            itemp = (int)(coord + (-0.5 * _rt.bottom()));
            fWp = itemp / _zoom - _shifty;

        }
    }
    return fWp;
}






void View2D::_onSetCursor(const QPoint& clientxy)
{
    switch(_accmode)
    {
        case BR_VIEW:
            _setTheCursor(CUR_SCROLL);
            break;
        case BR_MOVE:
            {
                _canMove = 0;
                if(!_MannageCursor(clientxy,0)){
                    _setTheCursor(CUR_MOVE);
                    break;
                }
            }
            _setTheCursor(CUR_SCROLL);
            break;
        case BR_SCALE:
            {
                _scaleDir = 0;
                if(_MannageCursor(clientxy,1)){
                    break;
                }
            }
            _setTheCursor(CUR_SCROLL);
            break;
        case BR_ROTATE:
            {
                //if(_vt=='z')return 1;
                _canMove = 0;
                if(!_MannageCursor(clientxy,0))
                {
                    _setTheCursor(CUR_ROTATE);
                    break;
                }
            }
            _setTheCursor(CUR_SCROLL);
            break;
        case BR_RISE:
            {
                //_canMove = 1;
                if(_MannageCursor(clientxy,2))
                {
                    break;
                }
            }
            _setTheCursor(CUR_SCROLL);
        default:
            break;

    }
}




u_bool32    View2D::_MannageCursor(const QPoint& point, u_bool32 set)
{

    int  count = 0;

    if(_accmode == BR_VIEW )
        return 0;

    Brush** ppb = DOC()->GetSelBrushes(count);

    if(0 == count &&
       DOC()->_pSelItems.size()== 0 &&
       DOC()->_pSelVertexes.size()==0 &&
       DOC()->_pSelPolys.size()==0)     // AUG 04
    {
        if(!DOC()->IsSelBoxOn())
            return 0;
    }

    Box acumBox;
    int  Mx,My,mx,my;


   _ResolveCurInBox(acumBox,Mx,My,mx,my);

    int hs = abs((Mx-mx)/3 + 1);
    int vs = abs((My-my)/3 + 1);
    SHCUR sScaleCurs[3][3] =
    {
        {{CUR_NWSE, SIZE_N|SIZE_W},  {CUR_NS,SIZE_N}                ,  {CUR_NESW, SIZE_N|SIZE_E}},
        {{CUR_EW,   SIZE_W},         {CUR_SCALE, SIZE_MOVE/*SIZE_0*/}, {CUR_EW,   SIZE_E}},
        {{CUR_NESW, SIZE_S|SIZE_W},  {CUR_NS,SIZE_S}                 , {CUR_NWSE, SIZE_S|SIZE_E}},
    };

    SHCUR sRiseCurs[3][3] =
    {
        {{CUR_MOVE, SIZE_MOVE|SIZE_W},{CUR_EW,SIZE_N}       , {CUR_MOVE, SIZE_MOVE}},
        {{CUR_NS,   SIZE_W},          {CUR_SCALE, SIZE_MOVE}, {CUR_NS,   SIZE_E}},
        {{CUR_MOVE, SIZE_MOVE},       {CUR_EW,SIZE_S}       , {CUR_MOVE, SIZE_MOVE}},
    };



    if(_vt == 'x')
    {
        uint32_t t;
        t = sScaleCurs[0][0].scDir;
        sScaleCurs[0][0].scDir=sScaleCurs[2][0].scDir;

        sScaleCurs[2][0].scDir=t;
        t = sScaleCurs[0][1].scDir;

        sScaleCurs[0][1].scDir=sScaleCurs[2][1].scDir;
        sScaleCurs[2][1].scDir=t;
        t = sScaleCurs[0][2].scDir;

        sScaleCurs[0][2].scDir=sScaleCurs[2][2].scDir;
        sScaleCurs[2][2].scDir=t;
    }

    if(!set)
    {
        QRect   rne(mx, my, Mx, My);
        if(rne.contains(point))
        {
            _canMove=1;
            return 1;
        }
    }
    else
    {
        int i=0,j=0;
        for(int xPos = mx; xPos < Mx; xPos+=hs)
        {
            j=0;
            for(int yPos = my; yPos < My; yPos+=vs)
            {
                QRect   rne;
                switch(_vt)
                {
                case 'x':
                        rne.setLeft(xPos);
                        rne.setTop(yPos);
                        rne.setRight(xPos+hs);
                        rne.setBottom(yPos+vs);

                        break;
                case 'y':
                        rne.setLeft   (xPos);
                        rne.setTop    (yPos);
                        rne.setRight  (xPos+hs);
                        rne.setBottom (yPos+vs);
                        break;
                case 'z':
                        rne.setLeft  (xPos);
                        rne.setTop   (yPos);
                        rne.setRight (xPos+hs);
                        rne.setBottom(yPos+vs);
                        break;
                default: break;
                }

/*
                CDC* pdc=GetDC();
                rne.InflateRect(-1,-1);
                pdc->Rectangle(rne);
                ReleaseDC(pdc);
*/
                if(rne.contains(point))
                {
                    _scaleDir = sScaleCurs[j][i].scDir;

                    if(2 == set) //rise flag is 2
                    {
                        _setTheCursor(sRiseCurs[j][i].hc);
                    }
                    else
                    {
                        _setTheCursor(sScaleCurs[j][i].hc);
                    }
                    return 1;
                }
                j++;
            }
            i++;
        }
    }
    return 0;

}



void View2D::_ResolveCurInBox(Box& acumBox,int& Mx,int& My,int& mx,int& my)
{
    int                 count   = 0;
    int                 count1  = 0;
    int                 count2  = 0;
    int                 count3  = 0;    // AUG 04
    Brush**             ppb     = DOC()->GetSelBrushes(count);
    SceItem**			ppi		= DOC()->GetSelItems(count1);
    Vtx2**              ppvx    = DOC()->GetSelVxes(count2);
    Poly**              ppply   = DOC()->GetSelPolys(count3);


    if(DOC()->IsSelBoxOn())
    {
        acumBox = DOC()->_selBox._box;
    }
    else  if(count)
    {

        if(count == 1 && ((*ppb))->HasMotion())
        {
            CMotion* pM = (*ppb)->_pMotion;
            if(pM->GetSelNode() > 0)
            {
                MState*    pS = pM->GetSelState();

                acumBox.AddPoint(pS->_pos);
                acumBox.Expand(32);

                //pS->_posWorld2Wnd

                count = 0;  // no brush bbox for cursors
            }
        }

        for(int i=0;i<count;i++,ppb++)
        {
            if((*ppb)->IsDisabled())
                continue;

            acumBox.Union((*ppb)->_box);
        }
    }
    else if(count1)
    {
        for(int i=0;i<count1;i++,ppi++)
        {
            Box box =  (*ppi)->GetWBox();
            acumBox.Union(box);
        }
    }else if(count2)
    {
        for(int i=0;i<count2;i++,ppvx++)
        {
            // 16 box aroud the vx
            v3d_t& vx   =  (*ppvx)->_xyz;
            Box box  =  Box(vx+v3d_t(32,32,32),vx+v3d_t(-32,-32,-32));
            acumBox.Union(box);
        }
    }else if(count3)
    {
        for(int i=0;i<count3;i++,ppply++)
        {
            // face box
            acumBox.Union((*ppply)->_box);
        }
    }

    acumBox.Expand0(32, 30);
    switch(_vt)
    {
        case 'x':	//-z Y
            Mx = World2Wnd(acumBox._max.z,'h');
            mx = World2Wnd(acumBox._min.z,'h');

            my = World2Wnd(acumBox._max.y,'v');
            My = World2Wnd(acumBox._min.y,'v');
            break;

        case 'y':	//X
            Mx = World2Wnd(acumBox._max.x,'h');
            mx = World2Wnd(acumBox._min.x,'h');

            My = World2Wnd(acumBox._max.z,'v');
            my = World2Wnd(acumBox._min.z,'v');
           break;

        case 'z':
            Mx = World2Wnd(acumBox._max.x,'h');
            mx = World2Wnd(acumBox._min.x,'h');

            my = World2Wnd(acumBox._max.y,'v');
            My = World2Wnd(acumBox._min.y,'v');
            break;
    }

    int dx =  (Mx-mx)/6;
    int dy =  (My-my)/6;

    Mx += dx;
    mx -= dx;
    My += dy;
    my -= dy;

}

void   View2D::_setTheCursor(int32_t c)
{
//    if(c != _ncursor)
//    {
        setCursor(theApp->_cursors[c]);
//        _ncursor=c;
//    }
}
