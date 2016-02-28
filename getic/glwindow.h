#ifndef GLWINDOW_H
#define GLWINDOW_H

#include <QtOpenGL/QGLWidget>

class glwindow : public QGLWidget
{
public:
    glwindow(QWidget* parent=0,
             const QGLWidget* shareWidget = 0, Qt::WindowFlags f=0);

    void    SelectTextCtx(const QRect& wrect);
    void    DeSelectTextCtx();
};

struct GCur
{
    glwindow* _pv;
    GCur(glwindow* pv):_pv(pv){
        if(_pv)
            _pv->makeCurrent();
    }

    ~GCur(){
        if(_pv)
            _pv->doneCurrent();
    }
};


#endif // GLWINDOW_H
