//----------------------------------------------------------------------------------------
// Unauthorized use or duplication is strictly prohibited.
// Copyright Zalsoft Inc 1999-2004.
//// Author: Marius C of Zalsoft Inc 2000-2004
//----------------------------------------------------------------------------------------
#include "stdafx.h"
#include "camera.h"
#include "poly.h"

//--| pos, f32_t yaw, f32_t pich, f32_t rol|-------------------------------------------------
Camera::Camera(v3d_t& pos, f32_t yaw, f32_t pich, f32_t rol)
{
    SetAngles(yaw, pich, rol);
    _pos          = pos;
    _prevfov      = 0;
    _prevrs       = 0;
    _fov		  = 64;
}

//--|Camera::Camera|----------------------------------------------------------------------
Camera::Camera()
{
    _prevrs = 0;
    _fov    = 64;
}

//--| m_16t Camera::GetViewMtx|--------------------------------------------------------------
const m_16t Camera::GetViewMtx(f32_t dTime)
{
    Q_UNUSED(dTime);
    return MLook(_pos, _fwd, _up, _right);
}

//--| void Camera::AddOccluder|--------------------------------------------------------------
void Camera::AddOccluder(const Poly& p)
{
    return;
    const v3d_t* cur;
    const v3d_t* prev = &p._vtci.back()._xyz;
    vvector<Vtx2>::const_iterator b = p._vtci.begin();
    vvector<Vtx2>::const_iterator  e = p._vtci.end();
    _frustrum.AddFrustrumPlan(p._vtci[2]._xyz,p._vtci[1]._xyz,p._vtci[0]._xyz);
    for(; b != e; b++)
    {
        cur = &b->_xyz;
        _frustrum.AddFrustrumPlan(_pos,*cur,*prev);
        prev = cur;
    }

}

