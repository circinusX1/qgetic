#ifndef __TEXREF2_H__
#define __TEXREF2_H__

#include "texsys.h"
#ifdef WINDOWS
#   pragma warning (disable: 4786)
#endif
//---------------------------------------------------------------------------------------
#define TEX_CAPACITY    32768
#define WM_REMOVETEX    (WM_USER+1007)

//---------------------------------------------------------------------------------------
class TexRef  : public TexSys
{
public:
    TexRef():TexSys(TexRef::GlGenTex, TexRef::RemoveTex){
        _texmap = new u_int32_t[TEX_CAPACITY+1];
        ::memset(_texmap,0,sizeof(u_int32_t)*(TEX_CAPACITY));
    };
    ~TexRef(){
        TexSys::Clean();
        delete[]_texmap;
    };
public:
    static void  Clear(u_bool32 b=0);
    static void	 _RemoveTex(Htex& it);
    static Htex  _GlGenTex(int32_t x, int32_t y, int32_t bpp, u_int8_t* pBuff, u_int32_t flags);
    static Htex	 GlGenTex(int32_t x, int32_t y, int32_t bpp, u_int8_t* pBuff, u_int32_t flags);
    static void	 RemoveTex(Htex* it, int32_t);
    static Htex  LoadRCTexture(const char* id,int32_t,int32_t,u_int8_t* pwantbuff=0);
    static u_int8_t* LoadRCTextureBuffer(const char* id,int32_t,int32_t);
    static Htex  LoadFile(char* pszFileName, u_int32_t flags);
    static Htex  LoadThisFile(const char* pszFileName, u_int32_t flags);
    static void  GlLmapMode(u_bool32 start);
    static void  GlDecalMode(u_bool32 start);
    static void  GlHalloMode(u_bool32 start);
    static void  GlDetailMode(u_bool32 start);
    static Htex	_defTex;
    static u_bool32 _MakeCurrent;

};


//---------------------------------------------------------------------------------------
// implements fast per/object reference count to minimize hits on TexSys
class SmartTex
{
public:
    static   int32_t _TexCopy;
    explicit SmartTex();
             SmartTex (const SmartTex& r);
    virtual  ~SmartTex();
    SmartTex& operator=(const SmartTex& r);
    SmartTex& operator=(const Htex& ht);
    operator    u_int32_t(){return _hTex;}
    u_bool32 operator==(const SmartTex& r){return (r._hTex.hTex == _hTex.hTex);};
    void        operator--();
    const       char* GetTexName();
    void        AddRef();
    SmartTex&   Assign(const char* p, u_int32_t flags);
    void        Reset();
    u_bool32        Valid(){return _hTex.hTex > 0;};
    operator    Htex&(){return _hTex;};
private:
    SmartTex& operator = (const char* p){return Assign(p, 0);};
    void Clear();
private:
    Htex     _hTex;
    u_int32_t*    _pRefs;
};


//---------------------------------------------------------------------------------------
// texture indexex
enum {TX_0 = 0, TX_LM, TX_1, TX_2};         //tex index
//---------------------------------------------------------------------------------------
// apply flags, gen flags, texture stage flags
struct Texs
{
    Texs(){memset(_texApply,0,sizeof(_texApply));}
    virtual ~Texs(){
    }
    Texs(const Texs& r){
        _texts[0] = r._texts[0];
        _texts[1] = r._texts[1];
        _texts[2] = r._texts[2];
        _texts[3] = r._texts[3];
        _texts[4] = r._texts[4];
        ::memcpy(_texApply, r._texApply, sizeof(_texApply));
    }

    Texs& operator=(const Texs& r){
        if(&r != this)
        {
            _texts[0] = r._texts[0];
            _texts[1] = r._texts[1];
            _texts[2] = r._texts[2];
            _texts[3] = r._texts[3];
            _texts[4] = r._texts[4];
            ::memcpy(_texApply, r._texApply, sizeof(_texApply));
        }
        return *this;
    }
    SmartTex& operator[](u_int32_t i){return _texts[i];}
    void     GetTextureInts(u_int16_t *texts, u_int32_t* applyes){
        for(int32_t i=0;i<4;i++)*(texts+i) = (u_int16_t)(u_int32_t)_texts[i];
        memcpy(applyes, _texApply, sizeof(u_int32_t)*4);
    }
    SmartTex    _texts[5];
    u_int32_t       _texApply[5];		// target, genST, envMode
};

//---------------------------------------------------------------------------------------
void    MakeCurrent(HDC hdc, HGLRC hrc);
void    MakeCurrent(char);
class NO_VT NoTexCopy{
public:
    NoTexCopy(){
        SmartTex::_TexCopy=0;
    }
    ~NoTexCopy(){
        SmartTex::_TexCopy=1;
    }
};


#endif // !__TEXREF_H__
