// TriggerItem.h: interface for the TriggerItem class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __TRIGGERITEM_H__
#define __TRIGGERITEM_H__

#include "sceitem.h"
#include "brush.h"

class TriggerItem   : public SceItem
{
public:
    TriggerItem();
    virtual ~TriggerItem();
    TriggerItem(const TriggerItem& r);
    TriggerItem& operator=(const TriggerItem& r);

    OVERWRITE void    Serialize(FileWrap* pfv, const vvector<Texture>& texNames, map<int32_t,int32_t>& texassoc);
    OVERWRITE void    GlDraw2D(View2D* ax);
    OVERWRITE void    GlDraw(View3D* pV);
    OVERWRITE void    GlDrawSel(View3D* pV);
    OVERWRITE int32_t     Size(){return sizeof(*this);}
    OVERWRITE void    Move(v3d_t& t);
    OVERWRITE void    Scale(v3d_t& s, v3d_t t, f32_t sc=1.0);
    OVERWRITE Box    GetBox(){
        if(_bSelEfect){
            Box befBox(v3d_t(-32.0,-32.0,-32.0),v3d_t(32.0,32.0,32.0));
            befBox._min+=_efectPoint;
            befBox._max+=_efectPoint;
            return befBox;
        }
        return _drawBrush._box;
    }
    OVERWRITE Box    GetWBox(){
        assert(0);
        //_bSelEfect  = theApp->get_modifiers(Qt::Key_Control);
        return GetBox();
    }
    void    SelectEffect(u_bool32 bs){_bSelEfect = bs;}
    u_bool32    IsEfectSelected(){
        assert(0);
        //_bSelEfect  = theApp->get_modifiers(Qt::Key_Control);
        return _bSelEfect ;
    }
    void    SetEfectBrush(Brush* p,u_bool32 bCenter=false){
        //detach old brush
        if(_pBrush)
        {
            _pBrush->_brushflags &= ~BRSH_HASTRIG;
            _pBrush = 0;
        }
        if(p)
        {
            _pBrush = p;
            _pBrush->_brushflags |= BRSH_HASTRIG;
            if(bCenter)
            {
                _efectPoint   = _pBrush->_box.GetCenter();
            }
        }
        else
        {
            if(bCenter)
                _efectPoint   = _t + VY*64.0;
        }
    }

    virtual void Commit(){
        if(_pBrush)
        {
            _efectPoint   = _pBrush->_box.GetCenter();
        }
    };
    virtual void InitCommit(){
        Commit();
    };
    virtual void    SetPos(v3d_t& pos){
        _drawBrush.SetPos(pos);
        _drawBrush.Recalc();
        _t = _drawBrush._box.GetCenter(); // update the t
       _efectPoint = _t + VY * 128.0;
    }

    Brush*  GetEfectBrush(){return _pBrush;}
    v3d_t&     EfectPoint(){return _efectPoint;}

private:
    void    FindTargetBrush();

public:
// trigger data
    Brush*          _pBrush;		// target brush
    v3d_t              _efectPoint;    // efect point world position

//saving in BSP
    int32_t             _brIndex;

//edittor spec drawinf moving data
    Brush           _drawBrush;     // just to draw the trigger bounding box
    u_bool32            _bSelEfect;     // efect point is selected
};

#endif // !__TRIGGERITEM_H__
