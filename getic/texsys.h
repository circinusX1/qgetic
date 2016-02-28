// TexHandler.h: interface for the TexHandler class.
// This code has been released for tutorial purposes. Do not use any of this code in
// any other sofware products.
//
// Started: Dec 2001
//---------------------------------------------------------------------------------------
#ifndef __ZTEXTURE_H__
#define __ZTEXTURE_H__

#include <map>
#include <jpeglib.h> //sudo apt-get install libjpeg-dev
#include <_irender.h>
#include "baselibhdrs.h"
#include "basecont.h"




#ifndef __HTEX
    typedef u_int32_t HTEX;
    #define __HTEX
#endif //


//--------------[READS JPEG, TGA IMAGES FILE]--------------------------------------------
class NO_VT TexHandler
{
public:
    TexHandler();
    virtual ~TexHandler();
    u_bool32    LoadThisFile(const char *sFileName, u_int32_t flags);
    u_bool32    LoadFile(const char *sFileName, u_int32_t flags);
    int32_t     SaveTgaFile(const char *sFileName);
    u_bool32    MakeBW();
    u_bool32    MakeDot3(f32_t);
    u_int8_t*   Buffer(){
        if(p_pBuff)
            return p_pBuff[0];
        return 0;
    }
    u_int8_t**   Buffepr(){
        return p_pBuff;
    }
    void Reset()
    {
        Deallocate();
        n_x      = 0;
        n_y      = 0;
        n_bpp    = 0;
        n_size   = 0;
        b_Filter = false;
        b_MipMap = true;
        b_Ilum   = false;
    }
    void MaskBW();
    static void Resize(int32_t nBytes, u_int8_t* pBufIn, int32_t wIn,
                  int32_t hIn, u_int8_t* pBufOut, int32_t wOut, int32_t hOut) ;
    static u_int8_t* AlphaIt(u_int8_t* pTemp, int32_t x, int32_t y, int32_t& bpp);
    static void  MaskBW(u_int8_t* pb, int32_t sz, int32_t bpp);
    static void  SwapRB(u_int8_t* pRGB, int32_t bpp, int32_t cx, int32_t cy);
    u_int32_t GetImgSize(){return n_size;}
    void  AlphaIt(u_int8_t alpha);
    void  Invert();
    void  SaveBMP(char *filename);
    void  SwapRB();

protected:
    u_bool32   _GetPixel(u_int8_t* pOld,  int32_t x, int32_t y);
    int32_t    LoadFile2(const char *sFileName, u_int32_t flags=0);
    u_bool32   LoadJpegFile(const char *sFileName, u_int32_t flags=0);
    u_bool32   LoadTgaFile(const char *sFileName, u_int32_t flags=0);
    u_bool32   LoadBmpFile(const char *sFileName, u_int32_t flags=0);

    u_bool32   CreateBuffer(int32_t width,int32_t height, int32_t deep);
    u_bool32   CreateMap(f32_t mapScale);
    void   Deallocate();
    u_int8_t** Allocate(int32_t lines, int32_t block);
    u_bool32 LoadBMP1bit(FILE *f, BITMAPFILEHEADER &fileheader, BITMAPINFOHEADER &infoheader);
    u_bool32 LoadBMP8bit(FILE *f, BITMAPFILEHEADER &fileheader, BITMAPINFOHEADER &infoheader);
    u_bool32 LoadBMP4bit(FILE *f, BITMAPFILEHEADER &fileheader, BITMAPINFOHEADER &infoheader);

public:
    int32_t     n_x;
    int32_t     n_y;
    int32_t     n_bpp;
    int32_t     n_size;
    u_int8_t**  p_pBuff;
    u_bool32    b_MipMap;
    u_bool32    b_Filter;
    u_bool32    b_Ilum;
};

#pragma pack(push, 8)


//-------------[TEXTURE MAPP BETWEEN FILE AND ID]----------------------------------------
struct Fl_TextureOld
{
    Fl_TextureOld():cx(0),cy(0),bpp(0),pBuff(0),refcount(0){filename[0]=0;}
    Htex            hTex;
    int32_t         cx;
    int32_t         cy;
    int32_t         bpp;
    int32_t         pBuff;
    int32_t         refcount;
    char            filename[32];
};

/*
Texture():cx(0),cy(0),bpp(0),pBuff(0),refcount(0){}
Htex        hTex;
int         cx;
int         cy;
int         bpp;
BYTE*       pBuff;
int         refcount;
char       filename[32];
*/
struct Fl_Texture
{
    Fl_Texture(){

    }
    Fl_Texture(const Fl_TextureOld& o):
        cx(o.cx),
        cy(o.cy),
        bpp(o.bpp),
        pBuff((int32_t*)o.pBuff),
        refcount(o.refcount)
    {
        ::strcpy(filename,o.filename);
    }
    Htex            hTex;
    int32_t         cx;
    int32_t         cy;
    int32_t         bpp;
    int32_t*        pBuff;
    int32_t         refcount;
    char            filename[32];
};

struct Texture
{
    Texture():cx(0),cy(0),bpp(0),pBuff(0),refcount(0){}

    Texture(Fl_Texture& r):hTex(r.hTex),cx(r.cx),
                                      cy(r.cy),
                                      bpp(r.bpp),
                                      pBuff((u_int8_t*)r.pBuff),
                                      refcount(r.refcount)
    {
        ::strcpy(filename, r.filename);
    }

    Htex            hTex;
    int32_t         cx;
    int32_t         cy;
    int32_t         bpp;
    u_int8_t*       pBuff;
    int32_t         refcount;
    char            filename[32];
};
#pragma pack(pop)

//-------------[]-----------------------------------------------------------------------
typedef Htex  (*STCB)(int32_t dx, int32_t dy, int32_t bpp, u_int8_t* pb, u_int32_t mips);
typedef void  (*RTCB)(Htex* tex, int32_t ncount);
//-------------------------[manages textures in system]---------------------------------
class TexSys : public map<tstring, Texture>
{
public:
    TexSys();
    TexSys(STCB scb, RTCB rtc);
    ~TexSys(){
        assert(size()==0);
    }
    Htex& GenTexture(const char* ptName, int32_t x, int32_t y, int32_t bpp, u_int8_t* buff, u_int32_t flags);
    Htex& AddTextureFile(const char* ptFile, u_int32_t flags = TEX_NORMAL);
    Htex& AddTextureFileGetImage(const char* filename, u_int8_t** pb, int32_t* dx, int32_t* dy, int32_t* bpp, u_int32_t flags= TEX_NORMAL);
    Htex& Assign(const char* psz, u_int32_t flags);
    const char* GetTexName(Htex& id);
    void RemoveTexture(Htex& iTex,u_bool32 b=true);
    void RemoveTextures(Htex* iextures, int32_t count);
    void RemoveTextureFile(const char* psz, u_int32_t flags=TEX_NORMAL);
    void Clean();
    Texture* GetTempTexture(const char* ptName, u_int32_t flags=TEX_NORMAL);
    Texture* GetTempTexture(Htex& tex);

public:
    STCB            _stx;
    RTCB            _rtx;
    static u_bool32 _GNoRefs;
    u_bool32        _cleaned;
    u_int32_t*      _texmap;
};



#endif // !defined(AFX_ZTEXTURE_H__078F6B43_840A_4286_8A95_CB2DB6C66FE5__INCLUDED_)
