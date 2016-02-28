#ifndef VIEW3D_H
#define VIEW3D_H


#include <baselib.h>
#include "glwindow.h"
#include "mmove.h"
#include "qgttypes.h"


class View3D : public glwindow
        
{
public:
    View3D();
    virtual ~View3D();
    void TextOut(const v3d_t& v, const char* text, const u_rgba_t& clr);

    void PrintOut(const v3d_t& v, char vt, const char* text, const u_rgba_t& clr);
    void DrawInHrc(u_bool32 update);

protected:
    void initializeGL();
    void resizeGL(int32_t width, int32_t height);
    void paintGL();
    void  Project3D(int32_t iWidth, int32_t iHeight);
    void  GLPaint();
    void Reinit();

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
    void ShowTheCursor();
    void HideTheCursor();

public:
    FRECT   _vport;
    u_bool32    _active;
    MMove   _mm;
    u_bool32    _rotating;
    u_bool32    _dd;
    QRect    _rt;
    f32_t    _fov;
    f32_t    _fnear;
    f32_t    _ffar;
    int32_t     _font1;
    int32_t		_capture;
    QPoint   _scrPoint;
    QRect    _prevrt;
    int32_t     _downKey;
    u_bool32    _blockKeyCall;
    u_bool32    _blockMouseCall;
};

#endif // VIEW3D_H
