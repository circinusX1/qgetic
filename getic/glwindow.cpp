#include "glwindow.h"

glwindow::glwindow(QWidget* parent,
                   const QGLWidget* shareWidget , Qt::WindowFlags ):QGLWidget(parent, shareWidget)
{
}



void    glwindow::SelectTextCtx(const QRect& wrect)
{
    glPushMatrix();
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(wrect.left(), wrect.right(), wrect.top() , wrect.bottom(), -32768, 32768);
        glColor3ub( 255,255,0);
    glMatrixMode(GL_MODELVIEW);

}

void    glwindow::DeSelectTextCtx()
{
    glPopMatrix();
}
