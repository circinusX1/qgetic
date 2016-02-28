// LightBulb.h: interface for the CLightBulb class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __LIGHT_BULB_H__
#define __LIGHT_BULB_H__

#include "sceitem.h"


class CLightBulb  : public SceItem
{
public:
    CLightBulb();
    CLightBulb(const CLightBulb& r);
    CLightBulb& operator=(const CLightBulb& r);
    virtual ~CLightBulb();
    OVERWRITE void Serialize(FileWrap* pfv, const vvector<Texture>& texNames, map<int32_t,int32_t>& texassoc);
    virtual void GlDraw(View3D* ax){
        SceItem::GlDraw(ax);
    }
    virtual Htex& GetTexture();
    virtual void    GlDraw2D(View2D* ax);
    virtual Box    GetWBox(){
        Box box = GetBox();
        box._min+=_t;
        box._max+=_t;
        return box;
    }
    virtual Box    GetBox(){
        Box box(v3d_t(-_radius*.707,-_radius*.707,-_radius*.707),
                 v3d_t(_radius*.707,_radius*.707,_radius*.707));
        return box;
    }
    virtual void    Scale(v3d_t& s, v3d_t t, f32_t sc=1.0){
        f32_t length = s.len();
        if(s.x<0||s.y<0||s.z<0)
            length=-length;
        _radius += length;
        if(_radius < 2)
            _radius=2;
    }
    virtual int32_t  Size(){return sizeof(*this);}

    //      light map calc
    f32_t    _specAngle;
    f32_t    _lmIntensity;
    f32_t    _radius;

    // hallo image
    f32_t    _halloRadMin;
    f32_t    _halloRadMax;

    f32_t    _minDist;
    f32_t    _maxDist;
    u_rgba_t		_colorE;	// emissive
    u_rgba_t		_colorS;	// specular
    int32_t	    _atten[3];	// for the 3 colors
};

#endif // !defined(AFX_LIGHTBULB_H__AB671A26_16BA_4D98_96A6_2A5E1631C930__INCLUDED_)
