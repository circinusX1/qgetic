// LightBulb.h: interface for the CSoundItem class.
//
#ifndef __SOUND_ITEM_H__
#define __SOUND_ITEM_H__

#include "sceitem.h"

class CSoundItem  : public SceItem
{
public:
    CSoundItem();
    CSoundItem(const CSoundItem& r);
    CSoundItem& operator=(const CSoundItem& r);
    virtual ~CSoundItem();
    Htex& GetTexture();
    OVERWRITE void Serialize(FileWrap* pfv,const vvector<Texture>& texNames, map<int32_t,int32_t>& texassoc);
    virtual void GlDraw(View3D* ax){
        SceItem::GlDraw(ax);
    }
    void    Play();
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
        Q_UNUSED(t);
        Q_UNUSED(sc);
        f32_t length = s.len();
        if(s.x<0||s.y<0||s.z<0)
            length=-length;
        _radius += length;
        if(_radius < 2)
            _radius=2;
    }
    virtual int32_t  Size(){return sizeof(*this);}

    f32_t    _specAngle;
    f32_t    _volume;
    f32_t    _radius;
    string  _soundfile;
};

#endif // !defined(AFX_LIGHTBULB_H__AB671A26_16BA_4D98_96A6_2A5E1631C930__INCLUDED_)
