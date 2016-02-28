// GeoUtils.cpp: implementation of the GeoUtils class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <baselib.h>
#include "geoutils.h"

v3d_t* __GetCone(const v3d_t& center, f32_t h, f32_t r, int32_t& nverts )
{
    static int32_t  snverts = 0;
    static v3d_t   cirpct[32];

    if(snverts)
    {
        nverts = snverts;
        return cirpct;
    }

    f32_t step = 0;
    cirpct[0] = V0;
    int32_t i;
    for( i = 0; i<12; i++)
    {
        cirpct[1+i].z = 0;
        cirpct[1+i].x = r * sinf(step);
        cirpct[1+i].y = r * cosf(step);
        step+=PIPE6;
    }
    cirpct[1+i] = cirpct[1];
    cirpct[0].z += h;
    nverts = 14;
    snverts = nverts;
    return cirpct;
}
