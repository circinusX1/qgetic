// --------------------------------------------------------------------------------------
// Copyright (c) 2000-2005 Zalsoft Inc
// Copyright: zalsoft Inc
// --------------------------------------------------------------------------------------

#include "baselib.h"
#include "basecont.h"


u_rgba_t ZBLACK(0, 0, 0);
u_rgba_t ZLIGHTGRAY(192,192,192);
u_rgba_t ZGREY(128,128,128);
u_rgba_t ZMEDIUMGRAY(128,128,128);
u_rgba_t ZRED(255,0,0);
u_rgba_t ZGREEN(0,255,0);
u_rgba_t ZYELLOW(255,255,0);
u_rgba_t ZBLUE(0,0,255);
u_rgba_t ZWHITE(255,255,255);
u_rgba_t ZCYAN(0,255,255);
u_rgba_t ZREDLIGHT(255,119,119);
u_rgba_t ZBLUELIGHT(119,119,255);
u_rgba_t ZGREENLIGHT(119,255,119);


v3d_t VX(1.0,0.0,0.0);
v3d_t VZ(0.0,0.0,1.0);
v3d_t VY(0.0,1.0,0.0);
v3d_t V0(0.0,0.0,0.0);
v3d_t V1(1.0,1.0,1.0);

// bounding box corner normals. used to clip box against it's movement
v3d_t   Box::_norms[8]=
{	v3d_t(-Cosr(PIPE4),-Cosr(PIPE4),-Cosr(PIPE4)),
    v3d_t( Cosr(PIPE4), Cosr(PIPE4), Cosr(PIPE4)),
    v3d_t( Cosr(PIPE4),-Cosr(PIPE4),-Cosr(PIPE4)),
    v3d_t(-Cosr(PIPE4), Cosr(PIPE4), Cosr(PIPE4)),
    v3d_t( Cosr(PIPE4), Cosr(PIPE4),-Cosr(PIPE4)),
    v3d_t(-Cosr(PIPE4),-Cosr(PIPE4), Cosr(PIPE4)),
    v3d_t(-Cosr(PIPE4), Cosr(PIPE4),-Cosr(PIPE4)),
    v3d_t( Cosr(PIPE4),-Cosr(PIPE4), Cosr(PIPE4))
};


u_bool32   Sphere::FindRayIP(const v3d_t& start, const v3d_t& end, v3d_t& ip)
{
	return 0;   
}

