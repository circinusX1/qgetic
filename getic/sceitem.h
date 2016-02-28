
#ifndef __SCEITEM_H__
#define __SCEITEM_H__

#include "stdafx.h"     // for GBackface
#include <baseutils.h>
#include <bspfilestr.h>
#include "texref.h"
#include "texadapter.h"
#include "baseentity.h"

#define OVERWRITE

//-----------------------------------------------------------------------------
// base item class for editor
class CSoundItem;
class View3D;
class View2D;
struct Texture;

struct ItemSig
{
    char  text[8];
};

extern ItemSig IS[];

class SceItem : public TexCarrier, public BaseEntity, public CanProps
{
public:
    SceItem();
    SceItem(const SceItem& r);
    virtual ~SceItem();
    SceItem& operator=(const SceItem& r);
    static  SceItem* CreateSci(int32_t type);
    void    DeSelect(){_selected=false;}
    void    Select(){
        _selected=true;
    }
    u_bool32    IsSelected(){
        return _selected;
    }
    virtual void Serialize(FileWrap* pfv, const vvector<Texture>& texNames, map<int32_t,int32_t>& texassoc);

    virtual void InitCommit(){};
    virtual void Commit(){};
    virtual void GlDraw(View3D* pV);
    virtual void GlDrawSel(View3D* pV);
    u_bool32	IsDirty(){return _flags & BRSH_DIRTY;};
    void	Dirty(u_bool32 b){
        if(b)
            _flags|=BRSH_DIRTY;
        else
            _flags&=~BRSH_DIRTY;
    };
    virtual void    SetPos(v3d_t& pos){
        _t=pos;
    }
    virtual Htex& GetTexture();
    virtual void    GlDraw2D(View2D* ax);
    virtual void    Move(v3d_t& t){
        _tl = _t;
        _t += t;
    }
    virtual void    Scale(v3d_t& s, v3d_t t, f32_t sc=1.0){
        Q_UNUSED(t);
        Q_UNUSED(sc);
        _s += s;
        _sl = _s;
        Q_UNUSED(t);
    }
    virtual void    Rotate(v3d_t& r, u_bool32 bao=false);
    virtual void UndoLastTr(){
    }
    virtual int32_t     Size(){return sizeof(*this);}
    virtual Box    GetBox(){
        Box box(v3d_t(-32,-32,-32),v3d_t(32,32,32));
        return box;
    }
    virtual Box    GetWBox(){
            Box box(v3d_t(-32,-32,-32),v3d_t(32,32,32));
            box._min += _t;
            box._max += _t;
           // box._min *= _s;
           // box._max *= _s;
            return box;
    };

public:
    EITEM           _item;
    char            _catname[16];
    int32_t             _id;
    u_int32_t           _props;
    u_int32_t           _flags;
    u_bool32			_selected;
    u_rgba_t             _colorD;
    u_int32_t           _zonearea;
    v3d_t              _t;
    v3d_t              _s;
    v3d_t              _euler;
    m_16t              _m;

    v3d_t              _tl;
    v3d_t              _sl;
    v3d_t              _dirl;	// last direction

    int32_t            _group;
    int32_t			_leafIdx;

    static  v_vunc_t     _Vtci[4];
    static  u_bool32    _BInitalized;
    static  int32_t    _GItemID;
};



#endif //
