#ifndef __TEXADAPTER_H__
#define __TEXADAPTER_H__

#include <GL/gl.h>
#ifdef WINDOWS
#   include <extern/glext.h>
#   include <extern/Glu.h>
#   include <extern/Glaux.h>
#endif
#include "texref.h"

extern 	Htex            _dumptex;
extern  u_int32_t        GUtex;
#define FLAGS_DISABLED  (0x80000000)


//---------------------------------------------------------------------------------------
struct LmInfo
{
    LmInfo():_flags(0),_lmIndex(-1){}

    u_int32_t   _flags;
    int32_t     _lmIndex;
    SIZE        _size;
    v3d_t       _lmAxes[3];
};

//---------------------------------------------------------------------------------------
class TexCarrier
{
public:
    TexCarrier():_combine(0){}
    ~TexCarrier(){}
    void            SetCombine(u_int32_t d){_combine=d;};
    u_int32_t           Combine(){return _combine;};
    Texs&           GetTexs(){return _textures;}
    Htex&           GetHtex(int32_t index){return _textures._texts[index];}
    void            SetHtex(Htex& ht, int32_t index){_textures._texts[index]=ht;}
    void            SetApply(u_int32_t apply, int32_t index){_textures._texApply[index]=apply;}
    u_int32_t&      Apply(int32_t index){return _textures._texApply[index];}
    Htex&           SetTex(const char* name, int32_t stage=GUtex, u_int32_t flags=0){
        Htex& rv = _textures[stage].Assign(name,flags);
        if((int32_t)rv)
            _combine |= (1<<stage);
        else
            _combine &= ~(1<<stage);
        return rv;
    }
protected:
    u_int32_t           _combine;
    Texs            _textures;
};

//=================================================================================
class NO_VT Tadapt
{
public:
    Tadapt():_glTarget(0),_glGen(0),_enabled(0),old_enabled(1){}
    ~Tadapt(){
        Disable();
    }

    u_bool32 Binds(Htex* pTexts, u_int32_t comb);
    u_bool32 Bind(const TexCarrier& pTexts, int32_t stage=0);
    u_bool32 Bind(Htex& tex, u_int32_t flags=0);
    INLN u_bool32 Bind(TexCarrier* pTexts, int32_t stage){return Bind(*pTexts, stage);}
    INLN u_bool32 Bind(const Texs& txs, int32_t stage=0){return Bind((Htex&)txs._texts[stage], txs._texApply[stage]);};
    INLN void Disable();
    INLN void Enable();
    INLN void Reset();
    INLN void Restore();

private:

    u_int32_t   _glTarget;
    u_int32_t   _glGen;
    u_bool32    _enabled;
    u_bool32    old_enabled;
    HTEX    _hTex;
};

extern Tadapt      Ta;
extern u_int32_t Targets[];


INLN void Tadapt::Enable()
{
    if(_enabled==1) return;
    old_enabled = _enabled;
    _enabled = 1;
    _glTarget ? glEnable(_glTarget) : glEnable(GL_TEXTURE_2D);
}

INLN void Tadapt::Disable()
{
    if(0==_enabled)  return;
    old_enabled = _enabled;
    _enabled = 0;
    Reset();
}

INLN void Tadapt::Restore()
{
    _enabled = old_enabled;
}

INLN void Tadapt::Reset()
{
    _glTarget ? glDisable(_glTarget) : glDisable(GL_TEXTURE_2D);
    _glTarget = 0;
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    _hTex    = 0;
}


#endif //__TEXADAPTER_H__


