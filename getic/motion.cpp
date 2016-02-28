// Motion.cpp: implementation of the CMotion class.
//

#include "stdafx.h"
#include "motion.h"
#include "document.h"
#include "brush.h"



u_bool32    CMotion::Create()
{
//    int32_t             _iNodes;
//    u_bool32            _bLoop;
//    vvector<MState> _states;

    v3d_t  pos = _pBrush->_box.GetCenter();

    for(int32_t i=0; i<_iNodes; i++)
    {
        MState  ms;
        ms._trTimes[0]=0;
        ms._trTimes[1]=1000;
        ms._pMotion = this;
        ms._pos     = pos;
        ms._euler   = V0;
        _states << ms;

        pos += VY * 64.0f;
    }
    return 1;
}


//MERGE//>
void   CMotion::GlDraw()
{
    _bLoop ? glBegin(GL_LINE_LOOP) : glBegin(GL_LINE_STRIP) ;
    int32_t i;
    // draw  path
    for( i=0; i<_iNodes; i++)
    {
        glVertex3fv((f32_t*)_states[i]._pos);
    }
    glEnd();

    // draw points
    glPointSize(4.0f);
    glBegin(GL_POINTS);
    for(i=0; i<_iNodes; i++)
    {
        if(_selNode == i)
        {
            glColor4ubv(CLR_SELECT);
        }

        glVertex3fv((f32_t*)_states[i]._pos);

        if(_selNode == i)
        {
            glColor4ubv(CLR_MOTION);
        }

    }
    glEnd();
    glPointSize(1.0f);

    // draw ortho of each point
    for(i=0; i<_iNodes; i++)
    {
        if(_selNode == i)
            Pd->DrawOrtho(_states[i]._pos, VZ, 16.0, .5);
        {
            v3d_t euler;
            Pd->DrawOrtho(_states[i]._pos, _states[i]._euler, 32.0, _selNode == i ? 1.0 : .7);
        }
    }
    // draw orthos
}
//MERGE//<


void CMotion::RefreshPosition()
{
    v3d_t depl = _pBrush->_box.GetCenter() - _center;
    _center = _pBrush->_box.GetCenter();
    for(int32_t i=0; i<_iNodes; i++)
    {
        _states[i]._pos += depl;
    }
}
