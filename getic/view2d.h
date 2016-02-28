#ifndef VIEW2D_H
#define VIEW2D_H

#include <QCursor>
#include <baselib.h>
#include <basecont.h>
#include "glwindow.h"
#include "mmove.h"
#include "qgttypes.h"

class Brush;
class CMotion;
class SceItem;
class Poly;




class View2D : public glwindow
{
public:

public:
    View2D(char p, QGLWidget* sharewith);

protected:
    void initializeGL();
    void resizeGL(int32_t width, int32_t height);
    void paintGL();
    void Project2D(int32_t iWidth, int32_t iHeight);
    void GLPaint(const QRect&);
    void    DrawGrid(const QRect& rt);
    void    CheckCoordRestrains();
    void    DrawTopGrid(const QRect& rt);
    void    DrawFrontGrid(const QRect& rt);
    void    DrawSideGrid(const QRect& rt);
    f32_t    Wnd2World(f32_t coord, char dir);
    int32_t     World2Wnd(f32_t coord, char dir);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void keyReleaseEvent(QKeyEvent *);
    virtual void focusInEvent(QFocusEvent *);
    virtual void focusOutEvent(QFocusEvent *);
    virtual void closeEvent(QCloseEvent *);


    void lButtonUp(QMouseEvent *p);
    void rButtonUp(QMouseEvent *p);
    void mButtonUp(QMouseEvent *p);
public:

    u_bool32 IsBoxInViewPort(const Box& box);
    u_bool32 IsVxInViewPort(const v3d_t& vx);
    void Reinit();

    void PrintOut(int32_t x, int32_t y, const QString& text,const u_rgba_t& clr=ZGREY);
    void    PrintCoords(const QRect& wrect, const FRECT& vrect, const v3d_t& pct,
                        char vt, u_bool32 active, const char* ct, QPoint* mc);
    void MouseWorldMove(f32_t mxw, f32_t  myw, const QPoint& point, const QPoint& spoint);
    void    DoSelection(const QPoint&  point);
    void  ScrollView(const QPoint& pt);
    void  MoveBrush(const QPoint& pt);
    v3d_t   GetMouseRotAngle(const QPoint& pt);
    void  DrawView();
    void    RotatePoly(const QPoint& pt);
    void    RotateBrush(const QPoint& pt);
    void    RotateItem(const QPoint& pt);
    void    ScaleBrush(const QPoint& pt, f32_t mxw, f32_t myz);
    void    ScaleItem(const QPoint& pt);
    void    MoveItem(const QPoint& pt);
    void    MoveVertex(const QPoint& pt);
    void    ScalePoly(const QPoint& pt);
    void    MovePoly(const QPoint& pt);
    void    RaiseBrush(const QPoint& pt);
    void    Rollover(const QPoint pt);
    v3d_t  _GetTranslation(const QPoint& pt);
    void    SetCursorText(const QPoint& pt, const char* str);
    void    GetSelCenter(Brush** it, int count);
    void    GetSelCenter(SceItem** it, int count);
    void    GetSelCenter(Poly** pPoly, int count);
    void    _releasemouse();
    void   _setTheCursor(int32_t c);
    void   _onSetCursor(const QPoint& clientxy);



protected:

    u_bool32 _MannageCursor(const QPoint& point, u_bool32 set);
    void     _ResolveCurInBox(Box& acumBox,int& Mx,int& My,int& mx,int& my);
    int      _MoveBrushMotion( CMotion* pM, v3d_t & t);
    void     _GetScaleValues( const QPoint& pt, v3d_t& s, v3d_t& t);
    void     Add4Update(Brush* pb);
    void     _DrawLines(int hgsz,
                                    GLfloat Left,
                                    GLfloat Right,
                                    GLfloat Top,
                                    GLfloat Bottom ,
                                    int     wp);

public:
    char    _vt;
    QRect   _rt;
    FRECT   _vport;
    f32_t   _zoom;


    f32_t    _shifty;
    f32_t    _shiftx;
    MMove   _mm;
    u_bool32    _active;
    FQPoint	_prepdn;
    int32_t	    _dirty;
    _BR_MODE  _accmode;
    u_bool32    _selBox;
    u_bool32    _dd;    //draw dirty. can be redrawned
    u_int32_t	_scaleDir;
    u_int32_t	_canMove;
    u_int32_t	_canMoveDown;
    u_int32_t	_scaleDirDown;
    int32_t		_depl;			//limits the move on one axis to allow alig on move scale rot
    QPoint   _scrPoint;
    v3d_t	    _cccGrd;
    f32_t    _grades;
    QRect    _prevrt;
    QPoint   _wmouse;
    QPoint   _textmouse;
    QString cs_ct;
    f32_t    _viewrot;

    v3d_t      _selCenter;
    v3d_t      _ptDown;
    v3d_t      _arccircle[3];

    vvector<Brush*> _dirtyBrushes;
    int32_t     _ncursor;


};

#endif // VIEW2D_H
