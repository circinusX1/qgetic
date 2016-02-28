// SkyDom.h: interface for the SkyDom class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_SKYDOM_H__2EBBE3CC_8BC6_4764_8FEC_C6E48723B361__INCLUDED_)
#define AFX_SKYDOM_H__2EBBE3CC_8BC6_4764_8FEC_C6E48723B361__INCLUDED_

#include    "brush.h"

class SkyDom
{
public:
    enum { SKY_CUBE, SKY_CONE, SKY_CYL, SKY_SPHERE};
    enum{P_SKY,P_CLOUD, P_LAND, P_LAVAL, P_EAST, P_WEST, P_SOUTH, P_NORTH, P_SIDE};
    SkyDom();
    virtual ~SkyDom();
    void    Clear(){_brush.Clear();}
    void    Make(int32_t shape);
    Brush&  GetBrush(){return _brush;}

private:

    u_int32_t       _shape;          // see enum section
    u_int32_t       _flags;
    v3d_t          _rotSpeed;
    v2d_t          _cloudSpeed;
    v2d_t          _lavaSpeed;
    v3d_t          _boxExt;
    v3d_t          _boxInt;
    Brush       _brush;
};

#endif // !defined(AFX_SKYDOM_H__2EBBE3CC_8BC6_4764_8FEC_C6E48723B361__INCLUDED_)
