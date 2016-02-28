// --------------------------------------------------------------------------------------
// Copyright (c) 2000-2005 Zalsoft Inc
// Copyright: zalsoft Inc
// --------------------------------------------------------------------------------------
#ifndef __BASELIB_H__
#define __BASELIB_H__


#define _USE_BASEG

#ifdef _CFG_WIN32_WIN
#   define OS_DEFINED
#   ifndef WINDOWS
#   define WINDOWS
#   endif //
#   include "WIN_WIN\\os.h"
#   pragma pack(push, 8)
#else
#   pragma pack(push, 8)
#endif

#ifdef _CFG_GNU_WIN
#define OS_DEFINED
#ifndef WINDOWSG
#define WINDOWSG
#endif //
#include "GNU_WIN\\os.h"
#endif

#ifdef _CFG_GNU_LINUX
#   define OS_DEFINED
#   include <GNU_LINUX/os.h>
#   include <stdlib.h>
#   include <string.h>
#endif


#ifndef OS_DEFINED
#pragma message ("error: _CFG_GNU_LINUX, _CFG_GNU_WIN or _CFG_WIN32_WIN Not defined")
#endif //

#ifdef WIN32
#   pragma warning (disable: 4267)
#   pragma warning (disable: 4018)
#   pragma warning (disable: 4786)
#   pragma warning (disable: 4006)
#   pragma warning (disable: 4244)
#   pragma warning (disable: 4996)
#endif


#ifdef _CFG_WIN32_WIN
#   include <crtdbg.h>
#   ifdef UNICODE
#       define  _T(x)   Lx
#   else
#       define  _T(x)   x
#   endif
#else
#   define  _T(x)   const_cast<const char*>(x)
#   define  _G(x)   x
#endif //

//---------------------------------------------------------------------------------------
#ifdef _BDL_PREC
typedef	double f32_t;
#else
typedef	float f32_t;
#endif //

//---------------------------------------------------------------------------------------
#ifdef _USRDLL
#define EX_IN __declspec(dllexport)
#else
#define EX_IN __declspec(dllimport)
#endif

//---------------------------------------------------------------------------------------
#define VIRT            // dummmspy macro for virtual function
#define ABSTRACT        // dummymsp macro for abstract classes
#define STATIC          // dummy macro for static funtions
#define THRFN           // dummy macro for function on TLS
#define _SCOPE

//---------------------------------------------------------------------------------------
#define	NO_FLAG(f_,v_)	(!(f_&v_))
#define	HAS_FLAG(f_,v_)	(f_&v_)
#define	REM_FLAG(f_,v_)	(f_&=~v_)
#define	ADD_FLAG(f_,v_)	(f_|=v_)
#define SET_FLAG(f_,v_)	(f_=v_)

//---------------------------------------------------------------------------------------
#define DELPA(p_)        {delete[] p_; p_=0;}
#define DELP(p_)         {delete p_; p_=0;}
#define THROW_IF(v_)    if(v_)throw v_

//---------------------------------------------------------------------------------------

typedef int32_t              ERR;
#define HANDELED          1
#define DEFAULT           0
#define SUCCESS           0
#define FAILURE           (-1)


//---------------------------------------------------------------------------------------
typedef ERR (*Proc)(u_int32_t, int32_t wP, int32_t lP);
typedef ERR (*Proc1)(u_int32_t, int32_t wP);
typedef ERR (*Proc2)(u_int32_t);
typedef ERR (*Proc3)(void*);

//---------------------------------------------------------------------------------------
#define EPSILONBIG				0.0625F
#define EPSILON					0.0009765625F 
#define EPSMED                  0.00390625
#define INFINIT					999999999.0
#define PI						3.14159265358900                // PI in radians
#define DOIPI					6.28318530717900	            // 2 pi
#define PIPE2					1.57079632679400                // halfpi
#define PIPE4					0.78539816339700                // quad of  pi
#define PIPE6					0.52359877559800                // sixth of pi
#define ONEDG                   0.0174532925199F                // one grade
#define G2R(a)					(a*0.0174532925F)
#define R2G(a)					(a*57.295779513F)
#define SWITCHI(_x,_y)			((_x)^=(_y)^=(_x)^=(_y))                        // swithes 2 integers
#define ROLLG(a)				{if(a<0) a=360; else if (a > 360) a = 0;}       // roll's the angle
#define ROLLR(a)				{if(a<0) a=DOIPI; else if (a > DOIPI) a = 0.00;}
#define ROLL180(a)				{if(a<-180.00) a=180.00; else if (a > 180.00) a = -180.00;}
#define BLOCKRANGE(a)			if(a < - PIPE2-EPSILON) a =- (PIPE2-EPSILON); else if (a > PIPE2-EPSILON) a = PIPE2-EPSILON;  // blocks the range of angle to straight up/down
#define CLAMPVAL(_v_,m_,M_)     {if(_v_< m_) _v_= m_; else if (_v_> M_)  _v_=M_;}
#define ROLLPI(a)				{if(a < -(PI) ) a = (PI); else if (a > (PI)) a = -(PI);}
#define ROLL2PI(a)				{if(a < 0) a = DOIPI; else if (a > DOIPI) a = 0;}
#define RRND()		            ((f32_t)rand()*(1.0f /(f32_t)RAND_MAX))  // Floating point random number generator ( 0 -> 1)
#define SGN(a)                  ((a)<0?-1:((a)?1:0))
#define PZERO(t_)                memset(t_,0,sizeof(*t_));
//---------------------------------------------------------------------------------------

template <class T>T tmin(T a, T b)  {return a < b ? a : b;}
template <class T>T tmax(T a, T b)  {return a >= b ? a : b;}

#define FRAND(a,b) (f32_t)((f32_t)(rand()%a)/b)
#define FRANDPREC(nLow, nHigh) ( ( (1.0f / ( (rand() % 100) + 1) ) * ((nHigh) - (nLow ) ) ) + (nLow) )
#define IRAND(low,high) (rand() % ( (high) - (low) + 1) + (low) )


INLN u_bool32 IsPowOf2(int32_t value){ // if error here you did not define one of _CFG_**** (see top)
    return (value&(value-1))==0;
}
INLN int32_t ToPow2(int32_t value, int32_t sv=0x1){
    while(value>>=1){
        sv<<=1;
    }
    return sv;
}

INLN f32_t Tanr(f32_t gr){
    return tanf(gr);
}

INLN f32_t Sinr(f32_t gr){
    return sinf(gr);
}

INLN f32_t Cosr(f32_t gr){
    return cosf(gr);
}

INLN f32_t Sina(f32_t gr){
    ROLLG(gr); return Sinr(gr);
}

INLN f32_t Cosa(f32_t gr){
    ROLLG(gr); return Cosr(gr);
}

INLN int32_t R2I (f32_t f){
#ifdef WINDOWS
    static int32_t i;
    ASM_ fld dword ptr [esp+4]
        ASM_ fistp i
        ASM_ mov eax, i
        ASM_ ret
#else
        return (int32_t)f;
#endif //
}
INLN int32_t Randval(int32_t iMin, int32_t iMax)
{
    return((rand() % (abs(iMax-iMin)+1))+iMin);
}

INLN f32_t  Randval(f32_t fMin, f32_t fMax)
{
    if (fMin == fMax) return(fMin);
    f32_t fRandom = (f32_t)rand() / (f32_t)RAND_MAX;
    return((fRandom * (f32_t)fabs(fMax-fMin))+fMin);
}

INLN f32_t Rabs (f32_t f){
    int32_t i = *(int32_t*) &f;
    i &= 0x7FFFFFFF;
    return *(f32_t*)&i;
}

INLN f32_t Frand(){

    static u_int32_t x = /* QfRandomInt() */ 1234567;
    static u_int32_t jflone = 0x40000000;
    static u_int32_t jflmask = 0x007fffff;

    x = x * 1664525 + 1013904223;
    u_int32_t t = jflone | (jflmask & x);
    return (*(f32_t*)&t) - 3.0f;
}

inline u_bool32  IsZero(f32_t a , f32_t b=EPSILONBIG){
    if(a==0)return true;
    return ( a > -b && a < b );
}

INLN f32_t Asinr (f32_t val) {
    if ( -1.0 < val ) {
        if ( val < 1.0 ) {
            return f32_t (asin(val));
        } else {
            return -PIPE2;
        }
    } else {
        return PIPE2;
    }
}

inline f32_t ATanr (f32_t val) {
    return f32_t(atan(val));
}

//---------------------------------------------------------------------------------------
struct V3I{int32_t x,y,z;};
typedef  f32_t    AV3[4];
class NO_VT v3d_t
{
public:
    f32_t x, y, z;
    v3d_t():x(0.0),y(0.0),z(0.0) {};
    v3d_t(f32_t inX, f32_t inY, f32_t inZ): x(inX), y(inY), z(inZ) {}
    v3d_t(const v3d_t &v): x(v.x), y(v.y), z(v.z) {}
    v3d_t(const f32_t f): x(f), y(f), z(f) {}
    v3d_t(const f32_t *f): x(*f), y(*(f+1)), z(*(f+2)) {}
    void RawPaste(f32_t* pr){*pr =x;	*(pr+1) = y;*(pr+2) = z;}
    void RawCopy(f32_t* pr){	x = *pr;y = *(pr+1);z = *(pr+2);}
    operator f32_t*(){return &x;}
    operator const f32_t*()const{return &x;}
    INLN  v3d_t& operator+=(const v3d_t &v){x += v.x; y += v.y; z += v.z;	return *this;}
    INLN  v3d_t& operator+=(const f32_t r){	x += r; y += r; z += r;	return *this;}
    INLN  v3d_t& operator-=(const v3d_t &v){x -= v.x ;y -= v.y; z -= v.z;return *this;}
    INLN  v3d_t& operator=(const v3d_t &v){x = v.x ;y = v.y; z = v.z;return *this;}
    //INLN  v3d_t& operator=(const f32_t* pf){x=(*pf); y=(*(pf+1)); z=(*(pf+2)); return *this;}
    INLN  void negate(){x=-x;y=-y;z=-z;};
    INLN  v3d_t& operator*=(f32_t f) {x *= f;y *= f;z *= f;	return *this;}
    INLN  v3d_t& operator/=(f32_t f) {x /= f;y /= f;z /= f;	return *this;}
    INLN  v3d_t& operator/=(int32_t f) {x /= (f32_t)f;y /= (f32_t)f;z /= (f32_t)f;return *this;}
    INLN  v3d_t& operator*=(const v3d_t& v){	x *= v.x;y *= v.y;z *= v.z;return *this;}
    f32_t getmin()const{return tmin((tmin(x,y)),z);}
    f32_t getmax()const{return tmax((tmax(x,y)),z);}
    INLN void reset(){	memset(this,0,sizeof(*this));}
    INLN void domin(v3d_t& v){	x = tmin(x, v.x);y = tmin(y, v.y);z = tmin(z, v.z);	}
    INLN  void domax(v3d_t& v){x = tmax(x, v.x);y = tmax(y, v.y);z = tmax(z, v.z);	}
    INLN void addscaled(v3d_t& v, f32_t d){	x+=v.x*d;	y+=v.y*d;	z+=v.z*d;}
    friend INLN  u_bool32 operator==(const v3d_t &a, const v3d_t &b) {return(IsZero(a.x-b.x) && IsZero(a.y-b.y) &&  IsZero(a.z-b.z));	}
    friend INLN  u_bool32 operator!=(const v3d_t &a, const v3d_t &b) {return((a.x != b.x) || (a.y != b.y) || (a.z != b.z));}
    friend INLN  v3d_t operator-(const v3d_t &a){	return v3d_t(-a.x, -a.y, -a.z);}
    friend INLN  v3d_t operator+(const v3d_t& a, const v3d_t& b) {v3d_t ret(a);ret += b;return ret;}
    friend INLN  v3d_t operator-(const v3d_t &a, const v3d_t &b) {v3d_t ret(a); ret -= b; return ret;}
    friend INLN  v3d_t operator*(const v3d_t &v, f32_t f) {return v3d_t(f * v.x, f * v.y, f * v.z);}
    friend INLN  v3d_t operator*(f32_t f, const v3d_t &v) {return v3d_t(f * v.x, f * v.y, f * v.z);}
    friend INLN  v3d_t operator/(const v3d_t &v, f32_t f) {return v3d_t(v.x / f, v.y / f, v.z / f);}
    friend INLN  v3d_t  operator/(const v3d_t &v, const v3d_t &f) {return v3d_t(v.x / f.x, v.y / f.y, v.z / f.z);}
    friend INLN  v3d_t  operator*(const v3d_t &v, const v3d_t &f) {return v3d_t(v.x * f.x, v.y * f.y, v.z * f.z);}
    friend INLN  f32_t vdist(const v3d_t& p1,const  v3d_t& p2){v3d_t tmp(p1);	tmp-=p2;return tmp.len();}
    friend INLN  f32_t vdist2(const v3d_t& p1,const  v3d_t& p2){v3d_t tmp(p1);	tmp-=p2;	return tmp.len2();}
    INLN  f32_t vdp(const v3d_t& b){  return x*b.x + y*b.y + z*b.z; }
    INLN  v3d_t&  vcp(const v3d_t &b){setval(y*b.z - z*b.y, z*b.x - x*b.z, x*b.y - y*b.x);return *this;}
    INLN  void setval(f32_t xx, f32_t yy, f32_t zz) {x = xx; y = yy; z = zz;}
    INLN  void setval(f32_t xx[3]) {x = xx[0]; y = xx[1]; z = xx[2];}
    INLN  void squeeze(f32_t val)	{if (x > -val && x < val)x = 0.00;if (y > -val && y < val)y = 0.00;if (z > -val && z < val)	z = 0.00;}
    INLN  void setval(f32_t xx) {x = xx; y = xx; z = xx;	}
    INLN  u_bool32 Valid(){	return fabs(x) != INFINIT;}
    INLN  void setval(v3d_t& v) {x = v.x; y = v.y; z = v.z;}
    INLN void interpolate(v3d_t& v1, v3d_t& v2, f32_t ammnt=.5f){
        x = v1.x+ (v2.x-v1.x)*ammnt;
        y = v1.y+ (v2.y-v1.y)*ammnt;
        z = v1.z+ (v2.z-v1.z)*ammnt;
    }
    INLN f32_t len() const {return(f32_t) sqrt(x*x + y*y + z*z);	}
    INLN f32_t len2() const{return(x*x + y*y + z*z);}
    INLN u_bool32 isnot1()const{return (x!=1 || y!=1 || z!=1);	}
    INLN u_bool32 is0(f32_t eps=EPSILON)	const{return Rabs(x) < eps && Rabs(y) < eps && Rabs(z) < eps;}
    u_bool32 isnot0()const{return Rabs(x)>EPSILON || Rabs(y)>EPSILON || Rabs(z)>EPSILON;}
    INLN u_bool32 isortonormal()const{
        if (y > .999 || y< -.999) return 1;
        if (x > .999 || x< -.999) return 1;
        if (z > .999 || z< -.999) return 1;
        return 0;
    }
    f32_t normalize(){
        f32_t m = len();
        if (m > 0.0) m = 1.0 / m;
        else m = 0.0;x *= m;
        y *= m; z *= m;
        return m;
    }
    INLN  v3d_t &norm(){	f32_t m = len();
    if (m > 0.0) m = 1.0 / m;
    else m = 0.0;x *= m;
    y *= m; z *= m;
    return *this;
    }
    INLN void cross(const v3d_t &v1,const v3d_t &v2) {
        x = v1.y * v2.z - v1.z * v2.y;
        y = v1.z * v2.x - v1.x * v2.z;
        z = v1.x * v2.y - v1.y * v2.x;
    }
    void SetLength(f32_t l)
    {
        f32_t len = sqrt(x*x + y*y + z*z);
        x *= l/len;
        y *= l/len;
        z *= l/len;
    } 
};
//---------------------------------------------------------------------------------------
extern v3d_t VX;
extern v3d_t VZ;
extern v3d_t VY;
extern v3d_t V0;
extern v3d_t V1;

//---------------------------------------------------------------------------------------
INLN  u_bool32  IsZero(const v3d_t& a, f32_t e=EPSILONBIG){
    if(a.x<-e || a.x>e)return 0;
    if(a.y<-e || a.y>e)return 0;
    if(a.z<-e || a.z>e)return 0;
    return 1;
}

//---------------------------------------------------------------------------------------
INLN   v3d_t Vcp(const v3d_t &a, const v3d_t &b){
    return v3d_t(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

//---------------------------------------------------------------------------------------
INLN  v3d_t Getnormal(const v3d_t& p0,const  v3d_t& p1,const  v3d_t& p2){
    v3d_t v = Vcp(p0-p1, p2-p1);
    v.norm();
    return v;
}

//------------------------------------------------------------------------------------
// Normalises the input vector and returns the vector normalised
INLN  v3d_t Vnrm(const v3d_t &a)
{
    v3d_t ret(a.x,a.y,a.z);
    return ret.norm();
}

//---------------------------------------------------------------------------------------
INLN  f32_t Vdp(const v3d_t &a, const v3d_t &b)
{
    return (f32_t)(a.x*b.x + a.y*b.y + a.z*b.z);
}

//---------------------------------------------------------------------------------------
INLN  void Vswp(v3d_t &a, v3d_t &b)
{
    v3d_t tmp(a);    a = b;    b = tmp;
}

//---------------------------------------------------------------------------------------
class NO_VT CLRNOA
{
public:
    CLRNOA():r(255),g(255),b(255){};
    INLN  operator u_int8_t*(){	return (u_int8_t*)&r; }
    u_int8_t r,g,b;
};

//---------------------------------------------------------------------------------------
typedef  u_int8_t    ACLR[4];
class NO_VT u_rgba_t
{
public:
    u_int8_t r,g,b,a;
    u_rgba_t():r(255),g(255),b(255),a(255){};
    u_rgba_t(u_int8_t rgb):r(rgb),g(rgb),b(rgb),a(rgb){};
    u_rgba_t(ACLR rgb):r(rgb[0]),g(rgb[1]),b(rgb[2]),a(rgb[3]){};
    u_rgba_t(const u_rgba_t& rgb):r(rgb.r),g(rgb.g),b(rgb.b),a(rgb.a){}
    u_rgba_t(u_int8_t rr, u_int8_t gg, u_int8_t bb, u_int8_t aa){r = rr;    g = gg;    b = bb;    a = aa;}
    u_rgba_t(u_int8_t rr, u_int8_t gg, u_int8_t bb){	r = rr;    g = gg;    b = bb;    a = 255;}
    u_rgba_t (uint32_t dwclr){ a=255; r= (dwclr>>16)&0xff; g= (dwclr>>8)&0xff; b= (dwclr)&0xff;}
    operator uint32_t(){uint32_t drv; drv = (r<<24)|(g<<16)|b; return drv;}
    INLN  operator u_int8_t*(){	return (u_int8_t*)&r; }
    INLN void black(){	memset(this,0,sizeof(*this));}
    INLN void white(){	memset(this,255,sizeof(*this));}
    INLN void grey(){	memset(this,128,sizeof(*this));}
    INLN void lgrey(){	memset(this,192,sizeof(*this));}
    INLN u_rgba_t& operator =(uint32_t dwclr){ a=255; r= (dwclr>>24)&0xf; g= (dwclr>>16)&0xf; b= (dwclr>>8)&0xf; return *this;}
    INLN u_rgba_t& operator =(const u_rgba_t& rr){	r = rr.r;    g = rr.g;    b = rr.b;    a = rr.a;	return *this;}
    INLN u_rgba_t& operator =(const CLRNOA& rr){	r = rr.r;    g = rr.g;    b = rr.b;    a = 255;	return *this;}
    INLN  u_rgba_t& operator -=(u_rgba_t& rgb){r-=rgb.r;g-=rgb.g;b-=rgb.b;return *this;}
    u_bool32  isnot0(){	if(r!=0)return true;if(g!=0)return true;if(b!=0)return true;return false;}
    INLN  u_rgba_t& operator +=(u_rgba_t& rgb){r+=rgb.r; g+=rgb.g; b+=rgb.b; return *this;}
    INLN  u_rgba_t& operator +=(u_int8_t rgb){r+=rgb; g+=rgb; b+=rgb; return *this;}
    INLN  u_rgba_t& operator *=(u_rgba_t& rgb){r*=rgb.r; g*=rgb.g; b*=rgb.b; return *this;}
    INLN  void setval(u_int8_t rr, u_int8_t gg, u_int8_t bb){r=rr;g=gg;b=bb;a=255;}
    INLN  void setval(const u_int8_t* by){r=by[0];g=by[1];b=by[2];a=by[3];}
    INLN  int32_t Sum()const{return (r+g+b);}
    INLN  u_rgba_t& operator *=(u_int8_t f) {r *= f;g *= f; b *= f;return *this;}
    friend INLN  u_rgba_t  operator-(u_rgba_t& a, u_rgba_t& b ){u_rgba_t vr(a);vr-=b;return vr;}
    friend INLN  u_rgba_t  operator+(u_rgba_t& a, u_rgba_t& b ){u_rgba_t vr(a);vr+=b;return vr;}
    friend INLN  u_rgba_t operator / (const u_rgba_t& v, u_int8_t f) {return u_rgba_t(v.r / f, v.g / f, v.b / f);}
    INLN void operator =(u_int8_t f){r = f; g = f;  b = f;  a = f;}
    INLN u_bool32 operator !=(u_rgba_t& vrgb){return (r!=vrgb.r || g!=vrgb.g || b!=vrgb.b || a!=vrgb.a);}
    INLN u_bool32 operator ==(const u_rgba_t& vrgb){return (r==vrgb.r && g==vrgb.g && b==vrgb.b && a==vrgb.a);}

    INLN  void interpolate(const u_rgba_t& c1, const u_rgba_t& c2, f32_t ammnt=.5f){
        r = c1.r + (c2.r-c1.r) * ammnt;
        g = c1.g + (c2.g-c1.g) * ammnt;
        b = c1.b + (c2.b-c1.b) * ammnt;
        a = c1.a + (c2.a-c1.a) * ammnt;
    }
    INLN  void ClampUp(u_rgba_t& cc){if (r>cc.r)r=cc.r;if (g>cc.g)g=cc.g;if (b>cc.b)b=cc.b;}
    INLN  void Scale(u_rgba_t& clr){r/=clr.r;g/=clr.g;b/=clr.b;}

};

//---------------------------------------------------------------------------------------
extern u_rgba_t ZBLACK;
extern u_rgba_t ZGREY;
extern u_rgba_t ZLIGHTGRAY;
extern u_rgba_t ZMEDIUMGRAY;
extern u_rgba_t ZRED;
extern u_rgba_t ZGREEN;
extern u_rgba_t ZYELLOW;
extern u_rgba_t ZBLUE;
extern u_rgba_t ZWHITE;
extern u_rgba_t ZCYAN;
extern u_rgba_t ZREDLIGHT;
extern u_rgba_t ZBLUELIGHT;
extern u_rgba_t ZGREENLIGHT;

struct UVI
{
    int32_t u;
    int32_t v;
};
//---------------------------------------------------------------------------------------
typedef  f32_t  AUV[2];
struct v2d_t
{
    f32_t u,v;
    v2d_t():u(0.00),v(0.00) {};
    v2d_t(f32_t inX, f32_t inY): u(inX), v(inY){}
    v2d_t(const v2d_t &v): u(v.u), v(v.v) {}
    v2d_t(const f32_t f): u(f), v(f) {}
    v2d_t(const f32_t *f): u(*f), v(*(f+1)) {}
    INLN  operator f32_t*(){return (f32_t*)&u;}
    INLN  v2d_t& operator += (const v2d_t &ufval) {u += ufval.u; v += ufval.v;	return *this;}
    INLN  v2d_t& operator -= (const v2d_t &ufval) {u -= ufval.u; v -= ufval.v;	return *this;}
    INLN  v2d_t& operator *= (f32_t f) {u *= f; v *= f;	return *this;}
    INLN  v2d_t& operator *= (v2d_t f) {u *= f.u; v *= f.v;	return *this;}
    INLN  v2d_t& operator /= (f32_t f) {u /= f; v /= f;	return *this;}
    INLN  v2d_t& operator /= (v2d_t& uv) {u /= uv.u; v /= uv.v;	return *this;}
    INLN  void setval(f32_t xx) {u = xx; v = xx;}
    INLN void reset(){	memset(this,0,sizeof(*this));}
    INLN  void setval(f32_t uu, f32_t vv) {u = uu; v = vv;}
    INLN  void setval(v2d_t& xx) {u = xx.u; v = xx.v;}
    INLN  void oneover() {u = 1.00/u; v = 1.00/v;}
    INLN  void domin(v2d_t& t){u = tmin(u,t.u);v = tmin(v,t.v);}
    INLN void domax(v2d_t& t){u = tmax(u,t.u);v = tmax(v,t.v);}
    INLN void interpolate(v2d_t& t1, v2d_t& t2, f32_t ammnt=.5f){u = t1.u + (t2.u-t1.u)*ammnt;v = t1.v + (t2.v-t1.v)*ammnt;}
    friend INLN  u_bool32 operator == (const v2d_t &a, const v2d_t &b) {return((a.u == b.u) && (a.v == b.v));}
    friend INLN  u_bool32 operator != (const v2d_t &a, const v2d_t &b) {return((a.u != b.u) || (a.v != b.v));}
    friend INLN  v2d_t  operator - (const v2d_t &a) {return v2d_t(-a.u, -a.v);}
    friend INLN  v2d_t  operator + (const v2d_t &a, const v2d_t &b) {v2d_t ret(a);ret += b;return ret;}
    friend INLN  v2d_t  operator - (const v2d_t &a, const v2d_t &b) {v2d_t ret(a);ret -= b;return ret;}
    friend INLN  v2d_t operator * (const v2d_t &v, f32_t f) {return v2d_t(f * v.u, f * v.v);}
    friend INLN  v2d_t operator * (f32_t f, const v2d_t &v) {return v2d_t(f * v.u, f * v.v);}
    friend INLN  v2d_t operator / (const v2d_t &v, f32_t f) {return v2d_t(v.u / f, v.v / f);}
    friend INLN  v2d_t  operator / (const v2d_t &v, const v2d_t &f) {return v2d_t(v.u / f.u, v.v / f.v);}
    friend INLN  v2d_t  operator * (const v2d_t &v, const v2d_t &f) {return v2d_t(v.u * f.u, v.v * f.v);}
};

//---------------------------------------------------------------------------------------
typedef  f32_t  AVtx[16];
struct v_vunc_t
{
    v3d_t		_xyz;    // 12 ...
    v3d_t		_nrm;    // 12 ...
    v2d_t		_uv[4];	 // 32 .. .. .. ..
    u_rgba_t	_rgb;	 // 4  .
    int32_t     _user;	 // 4 .

    v_vunc_t():_user(0){}
    v_vunc_t(const v3d_t& pct,const v2d_t& ufval){	_xyz = pct; 	_uv[0]	= ufval;	_rgb	= ZWHITE;  _user	= 0;}
    v_vunc_t(const v3d_t& pct, const v2d_t& ufval,const  u_rgba_t& rgb, int32_t c ){_xyz = pct;	_uv[0]	= ufval; _rgb= rgb;	_user	= c;}
    v_vunc_t(const AVtx& r){memcpy(this,&r,sizeof(*this));}
    v_vunc_t(const v_vunc_t& r)	{
        _xyz = r._xyz;
        _nrm = r._nrm;
        _uv[0] = r._uv[0];
        _uv[1] = r._uv[1];
        _uv[2] = r._uv[2];
        _uv[3] = r._uv[3];
        _rgb   = r._rgb  ;
        _user  = r._user ;	 }
    INLN  v_vunc_t& operator = (const v_vunc_t& r){
        if(this != & r){
            _xyz = r._xyz;
            _nrm = r._nrm;
            _uv[0] = r._uv[0];
            _uv[1] = r._uv[1];
            _uv[2] = r._uv[2];
            _uv[3] = r._uv[3];
            _rgb   = r._rgb  ;
            _user  = r._user ;
        }
        return *this;
    }
    INLN void reset(){	memset(this,0,sizeof(*this));}
    void interpolate(v_vunc_t& v1, v_vunc_t& v2, f32_t ammnt=.5f){
        _user	= v1._user;
        _xyz.interpolate(v1._xyz, v2._xyz, ammnt);
        _nrm.interpolate(v1._xyz, v2._xyz, ammnt);
        _uv[0].interpolate(v1._uv[0],v2._uv[0],ammnt);
        _uv[1].interpolate(v1._uv[1],v2._uv[1],ammnt);
        _uv[2].interpolate(v1._uv[1],v2._uv[1],ammnt);
        _uv[3].interpolate(v1._uv[1],v2._uv[1],ammnt);
        _rgb = v1._rgb;
    }
    u_bool32 operator ==(const v_vunc_t& r){return _xyz	== r._xyz;}
    int32_t size(){return sizeof(*this);}
};//  64 bytes (pow of 2) should stay like this


typedef  f32_t  AM4[16];

class NO_VT m_16t
{
public:
    f32_t   _m[16];

public:
    m_16t()     {identity();};
    m_16t(v3d_t v) {translate(v);};
    m_16t(const v3d_t &axis,f32_t angle)              {rotate(axis,angle);}
    INLN  m_16t(const m_16t &m) {
        _m[0] = m._m[0]; _m[4] = m._m[4]; _m[8] = m._m[8]; _m[12] = m._m[12];
        _m[1] = m._m[1]; _m[5] = m._m[5]; _m[9] = m._m[9]; _m[13] = m._m[13];
        _m[2] = m._m[2]; _m[6] = m._m[6]; _m[10] = m._m[10]; _m[14] = m._m[14];
        _m[3] = m._m[3]; _m[7] = m._m[7]; _m[11] = m._m[11]; _m[15] = m._m[15];
    }
    m_16t& operator=(const m_16t& m) {
        if(this != &m){
            _m[0] = m._m[0]; _m[4] = m._m[4]; _m[8] = m._m[8]; _m[12] = m._m[12];
            _m[1] = m._m[1]; _m[5] = m._m[5]; _m[9] = m._m[9]; _m[13] = m._m[13];
            _m[2] = m._m[2]; _m[6] = m._m[6]; _m[10] = m._m[10]; _m[14] = m._m[14];
            _m[3] = m._m[3]; _m[7] = m._m[7]; _m[11] = m._m[11]; _m[15] = m._m[15];
        }
        return *this;
    }
    operator f32_t*() { return (f32_t*)_m; }
    operator const f32_t*() const { return (f32_t*)_m; }
    m_16t operator+(const m_16t &m) const
    {
        m_16t ret;
        ret[0] = _m[0] + m[0]; ret[4] = _m[4] + m[4]; ret[8] = _m[8] + m[8]; ret[12] = _m[12] + m[12];
        ret[1] = _m[1] + m[1]; ret[5] = _m[5] + m[5]; ret[9] = _m[9] + m[9]; ret[13] = _m[13] + m[13];
        ret[2] = _m[2] + m[2]; ret[6] = _m[6] + m[6]; ret[10] = _m[10] + m[10]; ret[14] = _m[14] + m[14];
        ret[3] = _m[3] + m[3]; ret[7] = _m[7] + m[7]; ret[11] = _m[11] + m[11]; ret[15] = _m[15] + m[15];
        return ret;
    }
    v3d_t operator*(const v3d_t &v) const {
        v3d_t ret;
        ret[0] = _m[0] * v.x + _m[4] * v.y + _m[8] * v.z + _m[12];
        ret[1] = _m[1] * v.x + _m[5] * v.y + _m[9] * v.z + _m[13];
        ret[2] = _m[2] * v.x + _m[6] * v.y + _m[10] * v.z + _m[14];
        return ret;
    }
    m_16t operator*(f32_t f) const {
        m_16t ret;
        ret._m[0] = _m[0] * f; ret._m[4] = _m[4] * f; ret._m[8] = _m[8] * f; ret._m[12] = _m[12] * f;
        ret._m[1] = _m[1] * f; ret._m[5] = _m[5] * f; ret._m[9] = _m[9] * f; ret._m[13] = _m[13] * f;
        ret._m[2] = _m[2] * f; ret._m[6] = _m[6] * f; ret._m[10] = _m[10] * f; ret._m[14] = _m[14] * f;
        ret._m[3] = _m[3] * f; ret._m[7] = _m[7] * f; ret._m[11] = _m[11] * f; ret._m[15] = _m[15] * f;
        return ret;
    }
    m_16t operator*(const m_16t &m) const {
        m_16t ret;
        ret._m[0] = _m[0] * m._m[0] + _m[4] * m._m[1] + _m[8] * m._m[2] + _m[12] * m._m[3];
        ret._m[1] = _m[1] * m._m[0] + _m[5] * m._m[1] + _m[9] * m._m[2] + _m[13] * m._m[3];
        ret._m[2] = _m[2] * m._m[0] + _m[6] * m._m[1] + _m[10] * m._m[2] + _m[14] * m._m[3];
        ret._m[3] = _m[3] * m._m[0] + _m[7] * m._m[1] + _m[11] * m._m[2] + _m[15] * m._m[3];
        ret._m[4] = _m[0] * m._m[4] + _m[4] * m._m[5] + _m[8] * m._m[6] + _m[12] * m._m[7];
        ret._m[5] = _m[1] * m._m[4] + _m[5] * m._m[5] + _m[9] * m._m[6] + _m[13] * m._m[7];
        ret._m[6] = _m[2] * m._m[4] + _m[6] * m._m[5] + _m[10] * m._m[6] + _m[14] * m._m[7];
        ret._m[7] = _m[3] * m._m[4] + _m[7] * m._m[5] + _m[11] * m._m[6] + _m[15] * m._m[7];
        ret._m[8] = _m[0] * m._m[8] + _m[4] * m._m[9] + _m[8] * m._m[10] + _m[12] * m._m[11];
        ret._m[9] = _m[1] * m._m[8] + _m[5] * m._m[9] + _m[9] * m._m[10] + _m[13] * m._m[11];
        ret._m[10] = _m[2] * m._m[8] + _m[6] * m._m[9] + _m[10] * m._m[10] + _m[14] * m._m[11];
        ret._m[11] = _m[3] * m._m[8] + _m[7] * m._m[9] + _m[11] * m._m[10] + _m[15] * m._m[11];
        ret._m[12] = _m[0] * m._m[12] + _m[4] * m._m[13] + _m[8] * m._m[14] + _m[12] * m._m[15];
        ret._m[13] = _m[1] * m._m[12] + _m[5] * m._m[13] + _m[9] * m._m[14] + _m[13] * m._m[15];
        ret._m[14] = _m[2] * m._m[12] + _m[6] * m._m[13] + _m[10] * m._m[14] + _m[14] * m._m[15];
        ret._m[15] = _m[3] * m._m[12] + _m[7] * m._m[13] + _m[11] * m._m[14] + _m[15] * m._m[15];
        return ret;
    }

    m_16t &operator*=(f32_t f)     { return *this = *this * f; }
    m_16t &operator*=(const m_16t &m) { return *this = *this * m; }
    m_16t &operator+=(const m_16t &m) { return *this = *this + m; }

    void orthonormalize() {
        v3d_t x(_m[0],_m[1],_m[2]);
        v3d_t y(_m[3],_m[4],_m[5]);
        v3d_t z;
        x.norm();
        z.cross(x,y);
        z.norm();
        y.cross(z,x);
        y.norm();
        _m[0] = x.x; _m[3] = y.x; _m[6] = z.x;
        _m[1] = x.y; _m[4] = y.y; _m[7] = z.y;
        _m[2] = x.z; _m[5] = y.z; _m[8] = z.z;
    }

    INLN void v3transposeTransform(v3d_t& i)
    {
        v3d_t r;
        i.x -= _m[12];
        i.y -= _m[13];
        i.z -= _m[14];

        r.x = i.x * _m[0] + i.y * _m[1] + i.z * _m[2] ;
        r.y = i.x * _m[4] + i.y * _m[5] + i.z * _m[6] ;
        r.z = i.x * _m[8] + i.y * _m[9] + i.z * _m[10] ;
        i=r;
    }

    INLN void v3transform(v3d_t& v)
    {
        v3translate(v);
        v3rotate(v);
        /*
        v3d_t r;
        r.x = v.x * _m[0] + v.y * _m[4] + v.z * _m[8]  + _m[12];
        r.y = v.x * _m[1] + v.y * _m[5] + v.z * _m[9]  + _m[13];
        r.z = v.x * _m[2] + v.y * _m[6] + v.z * _m[10] + _m[14];
        v=r;
        */
    }
    INLN void v3rotate(v3d_t& v)
    {
        v3d_t r;
        r.x = v.x * _m[0] + v.y * _m[4] + v.z * _m[8] ;
        r.y = v.x * _m[1] + v.y * _m[5] + v.z * _m[9] ;
        r.z = v.x * _m[2] + v.y * _m[6] + v.z * _m[10] ;
        v=r;
    }

    INLN void v3translate(v3d_t& ret)
    {
        ret.x = ret.x + _m[12];
        ret.y = ret.y + _m[13];
        ret.z = ret.z + _m[14];
    }

    INLN void v3scale(v3d_t& ret)
    {
        ret.x = ret.x * _m[0];
        ret.y = ret.y * _m[5];
        ret.z = ret.z * _m[10];
    }

    INLN void identity() {
        _m[0] = 1.0; _m[4] = 0.0; _m[8] = 0.0; _m[12] = 0.0;
        _m[1] = 0.0; _m[5] = 1.0; _m[9] = 0.0; _m[13] = 0.0;
        _m[2] = 0.0; _m[6] = 0.0; _m[10] = 1.0; _m[14] = 0.0;
        _m[3] = 0.0; _m[7] = 0.0; _m[11] = 0.0; _m[15] = 1.0;
    }

    INLN void transp(){
        m_16t ret;
        ret._m[0] = _m[0]; ret._m[4] = _m[1]; ret._m[8] = _m[2]; ret._m[12] = _m[3];
        ret._m[1] = _m[4]; ret._m[5] = _m[5]; ret._m[9] = _m[6]; ret._m[13] = _m[7];
        ret._m[2] = _m[8]; ret._m[6] = _m[9]; ret._m[10] = _m[10]; ret._m[14] = _m[11];
        ret._m[3] = _m[12]; ret._m[7] = _m[13]; ret._m[11] = _m[14]; ret._m[15] = _m[15];
        *this = ret;
    }

    f32_t det() const {
        f32_t det;
        det = _m[0] * _m[5] * _m[10];
        det += _m[4] * _m[9] * _m[2];
        det += _m[8] * _m[1] * _m[6];
        det -= _m[8] * _m[5] * _m[2];
        det -= _m[4] * _m[1] * _m[10];
        det -= _m[0] * _m[9] * _m[6];
        return det;
    }
    m_16t inverse() const {
        m_16t ret;
        f32_t idet = 1.0f / det();
        ret._m[0] =  (_m[5] * _m[10] - _m[9] * _m[6]) * idet;
        ret._m[1] = -(_m[1] * _m[10] - _m[9] * _m[2]) * idet;
        ret._m[2] =  (_m[1] * _m[6] - _m[5] * _m[2]) * idet;
        ret._m[3] = 0.0;
        ret._m[4] = -(_m[4] * _m[10] - _m[8] * _m[6]) * idet;
        ret._m[5] =  (_m[0] * _m[10] - _m[8] * _m[2]) * idet;
        ret._m[6] = -(_m[0] * _m[6] - _m[4] * _m[2]) * idet;
        ret._m[7] = 0.0;
        ret._m[8] =  (_m[4] * _m[9] - _m[8] * _m[5]) * idet;
        ret._m[9] = -(_m[0] * _m[9] - _m[8] * _m[1]) * idet;
        ret._m[10] =  (_m[0] * _m[5] - _m[4] * _m[1]) * idet;
        ret._m[11] = 0.0;
        ret._m[12] = -(_m[12] * ret._m[0] + _m[13] * ret._m[4] + _m[14] * ret._m[8]);
        ret._m[13] = -(_m[12] * ret._m[1] + _m[13] * ret._m[5] + _m[14] * ret._m[9]);
        ret._m[14] = -(_m[12] * ret._m[2] + _m[13] * ret._m[6] + _m[14] * ret._m[10]);
        ret._m[15] = 1.0;
        return ret;
    }

    void rotate(const v3d_t &axis, f32_t angle) {
        f32_t rad = G2R(angle);
        f32_t c = Cosr(rad);
        f32_t s = Sinr(rad);
        v3d_t v = axis;
        v.norm();
        f32_t xx = v.x * v.x;
        f32_t yy = v.y * v.y;
        f32_t zz = v.z * v.z;
        f32_t xy = v.x * v.y;
        f32_t yz = v.y * v.z;
        f32_t zx = v.z * v.x;
        f32_t xs = v.x * s;
        f32_t ys = v.y * s;
        f32_t zs = v.z * s;
        _m[0] = (1.0f - c) * xx + c; _m[4] = (1.0f - c) * xy - zs; _m[8] = (1.0f - c) * zx + ys; _m[12] = 0.0;
        _m[1] = (1.0f - c) * xy + zs; _m[5] = (1.0f - c) * yy + c; _m[9] = (1.0f - c) * yz - xs; _m[13] = 0.0;
        _m[2] = (1.0f - c) * zx - ys; _m[6] = (1.0f - c) * yz + xs; _m[10] = (1.0f - c) * zz + c; _m[14] = 0.0;
        _m[3] = 0.0; _m[7] = 0.0; _m[11] = 0.0; _m[15] = 1.0;
    }
    void rotatex(f32_t angle) {
        f32_t rad = G2R(angle);
        f32_t c = Cosr(rad);
        f32_t s = Sinr(rad);
        _m[0] = 1.0; _m[4] = 0.0; _m[8] = 0.0; _m[12] = 0.0;
        _m[1] = 0.0; _m[5] = c; _m[9] = -s; _m[13] = 0.0;
        _m[2] = 0.0; _m[6] = s; _m[10] = c; _m[14] = 0.0;
        _m[3] = 0.0; _m[7] = 0.0; _m[11] = 0.0; _m[15] = 1.0;
    }
    void rotatey(f32_t angle) {
        f32_t rad = G2R(angle);
        f32_t c = Cosr(rad);
        f32_t s = Sinr(rad);
        _m[0] = c; _m[4] = 0.0; _m[8] = s; _m[12] = 0.0;
        _m[1] = 0.0; _m[5] = 1.0; _m[9] = 0.0; _m[13] = 0.0;
        _m[2] = -s; _m[6] = 0.0; _m[10] = c; _m[14] = 0.0;
        _m[3] = 0.0; _m[7] = 0.0; _m[11] = 0.0; _m[15] = 1.0;
    }
    void rotatez(f32_t angle) {
        f32_t rad = G2R(angle);
        f32_t c = Cosr(rad);
        f32_t s = Sinr(rad);
        _m[0] = c; _m[4] = -s; _m[8] = 0.0; _m[12] = 0.0;
        _m[1] = s; _m[5] = c; _m[9] = 0.0; _m[13] = 0.0;
        _m[2] = 0.0; _m[6] = 0.0; _m[10] = 1.0; _m[14] = 0.0;
        _m[3] = 0.0; _m[7] = 0.0; _m[11] = 0.0; _m[15] = 1.0;
    }
    void scale(const v3d_t &v) {
        _m[0] = v.x; _m[4] = 0.0; _m[8] = 0.0; _m[12] = 0.0;
        _m[1] = 0.0; _m[5] = v.y; _m[9] = 0.0; _m[13] = 0.0;
        _m[2] = 0.0; _m[6] = 0.0; _m[10] = v.z; _m[14] = 0.0;
        _m[3] = 0.0; _m[7] = 0.0; _m[11] = 0.0; _m[15] = 1.0;
    }
    void translate(const v3d_t &v) {
        _m[0] = 1.0; _m[4] = 0.0; _m[8] = 0.0; _m[12] = v.x;
        _m[1] = 0.0; _m[5] = 1.0; _m[9] = 0.0; _m[13] = v.y;
        _m[2] = 0.0; _m[6] = 0.0; _m[10] = 1.0; _m[14] = v.z;
        _m[3] = 0.0; _m[7] = 0.0; _m[11] = 0.0; _m[15] = 1.0;
    }
    void reflect(const v3d_t &norm, f32_t c) {
        f32_t x = norm.x;
        f32_t y = norm.y;
        f32_t z = norm.z;
        f32_t x2 = x * 2.0f;
        f32_t y2 = y * 2.0f;
        f32_t z2 = z * 2.0f;
        _m[0] = 1.0f - x * x2; _m[4] = -y * x2; _m[8] = -z * x2; _m[12] = -c * x2;
        _m[1] = -x * y2; _m[5] = 1.0f - y * y2; _m[9] = -z * y2; _m[13] = -c * y2;
        _m[2] = -x * z2; _m[6] = -y * z2; _m[10] = 1.0f - z * z2; _m[14] = -c * z2;
        _m[3] = 0.0; _m[7] = 0.0; _m[11] = 0.0; _m[15] = 1.0;
    }

    void perspective(f32_t fov,f32_t aspect,f32_t znear,f32_t zfar) {
        f32_t y = tan(fov * PI / 360.0f);
        f32_t x = y * aspect;
        _m[0] = 1.0f / x; _m[4] = 0.0; _m[8] = 0.0; _m[12] = 0.0;
        _m[1] = 0.0; _m[5] = 1.0f / y; _m[9] = 0.0; _m[13] = 0.0;
        _m[2] = 0.0; _m[6] = 0.0; _m[10] = -(zfar + znear) / (zfar - znear); _m[14] = -(2.0f * zfar * znear) / (zfar - znear);
        _m[3] = 0.0; _m[7] = 0.0; _m[11] = -1.0; _m[15] = 0.0;
    }
    void look_to(const v3d_t &eye,const v3d_t &at,const v3d_t &up) {
        v3d_t x,y,z;
        m_16t m0,m1;
        z = eye - at;
        z.norm();
        x.cross(up,z);
        x.norm();
        y.cross(z,x);
        y.norm();
        m0._m[0] = x.x; m0._m[4] = x.y; m0._m[8] = x.z; m0._m[12] = 0.0;
        m0._m[1] = y.x; m0._m[5] = y.y; m0._m[9] = y.z; m0._m[13] = 0.0;
        m0._m[2] = z.x; m0._m[6] = z.y; m0._m[10] = z.z; m0._m[14] = 0.0;
        m0._m[3] = 0.0; m0._m[7] = 0.0; m0._m[11] = 0.0; m0._m[15] = 1.0;
        m1.translate(-eye);
        *this = m0 * m1;
    }
    m_16t rotation() const {
        m_16t ret;
        ret[0] = _m[0]; ret[4] = _m[4]; ret[8] = _m[8]; ret[12] = 0;
        ret[1] = _m[1]; ret[5] = _m[5]; ret[9] = _m[9]; ret[13] = 0;
        ret[2] = _m[2]; ret[6] = _m[6]; ret[10] = _m[10]; ret[14] = 0;
        ret[3] = 0; ret[7] = 0; ret[11] = 0; ret[15] = 1;
        return ret;
    }
    m_16t transpose() const {
        m_16t ret;
        ret[0] = _m[0]; ret[4] = _m[1]; ret[8] = _m[2]; ret[12] = _m[3];
        ret[1] = _m[4]; ret[5] = _m[5]; ret[9] = _m[6]; ret[13] = _m[7];
        ret[2] = _m[8]; ret[6] = _m[9]; ret[10] = _m[10]; ret[14] = _m[11];
        ret[3] = _m[12]; ret[7] = _m[13]; ret[11] = _m[14]; ret[15] = _m[15];
        return ret;
    }
    m_16t transpose_rotation() const {
        m_16t ret;
        ret[0] = _m[0]; ret[4] = _m[1]; ret[8] = _m[2]; ret[12] = _m[12];
        ret[1] = _m[4]; ret[5] = _m[5]; ret[9] = _m[6]; ret[13] = _m[13];
        ret[2] = _m[8]; ret[6] = _m[9]; ret[10] = _m[10]; ret[14] = _m[14];
        ret[3] = _m[3]; ret[7] = _m[7]; ret[14] = _m[14]; ret[15] = _m[15];
        return ret;
    }

    void rotateradians(f32_t dAngleX, f32_t dAngleY, f32_t dAngleZ)
    {
        f32_t cr = cos( dAngleX );
        f32_t sr = sin( dAngleX );
        f32_t cp = cos( dAngleY );
        f32_t sp = sin( dAngleY );
        f32_t cy = cos( dAngleZ );
        f32_t sy = sin( dAngleZ );

        _m[0] = ( f32_t )( cp*cy );
        _m[1] = ( f32_t )( cp*sy );
        _m[2] = ( f32_t )( -sp );

        f32_t srsp = sr*sp;
        f32_t crsp = cr*sp;

        _m[4] = ( f32_t )( srsp*cy-cr*sy );
        _m[5] = ( f32_t )( srsp*sy+cr*cy );
        _m[6] = ( f32_t )( sr*cp );

        _m[8] = ( f32_t )( crsp*cy+sr*sy );
        _m[9] = ( f32_t )( crsp*sy-sr*cy );
        _m[10] = ( f32_t )( cr*cp );

    }

    void multiply (m_16t *matrix)
    {
	    float newMatrix[16];
	    const float *m1 = _m, *m2 = matrix->_m;

	    newMatrix[0] = m1[0]*m2[0] + m1[4]*m2[1] + m1[8]*m2[2];
	    newMatrix[1] = m1[1]*m2[0] + m1[5]*m2[1] + m1[9]*m2[2];
	    newMatrix[2] = m1[2]*m2[0] + m1[6]*m2[1] + m1[10]*m2[2];
	    newMatrix[3] = 0;

	    newMatrix[4] = m1[0]*m2[4] + m1[4]*m2[5] + m1[8]*m2[6];
	    newMatrix[5] = m1[1]*m2[4] + m1[5]*m2[5] + m1[9]*m2[6];
	    newMatrix[6] = m1[2]*m2[4] + m1[6]*m2[5] + m1[10]*m2[6];
	    newMatrix[7] = 0;

	    newMatrix[8] = m1[0]*m2[8] + m1[4]*m2[9] + m1[8]*m2[10];
	    newMatrix[9] = m1[1]*m2[8] + m1[5]*m2[9] + m1[9]*m2[10];
	    newMatrix[10] = m1[2]*m2[8] + m1[6]*m2[9] + m1[10]*m2[10];
	    newMatrix[11] = 0;

	    newMatrix[12] = m1[0]*m2[12] + m1[4]*m2[13] + m1[8]*m2[14] + m1[12];
	    newMatrix[13] = m1[1]*m2[12] + m1[5]*m2[13] + m1[9]*m2[14] + m1[13];
	    newMatrix[14] = m1[2]*m2[12] + m1[6]*m2[13] + m1[10]*m2[14] + m1[14];
	    newMatrix[15] = 1;

	    for (int32_t i=0; i<16; i++)
		    this->_m[i] = newMatrix[i];
    }
    void  inversetranslatevect(v3d_t& vct)
	{
		vct.x = vct.x-_m[12];
		vct.y = vct.y-_m[13];
		vct.z = vct.z-_m[14];
	}

    void inverserotatevect(v3d_t&  vct)
	{
        v3d_t vec(vct.x*_m[0]+vct.y*_m[1]+vct.z*_m[2],
		       vct.x*_m[4]+vct.y*_m[5]+vct.z*_m[6],
		       vct.x*_m[8]+vct.y*_m[9]+vct.z*_m[10]);

		vct = vec;
	}

};

class NO_VT Quat
{
public:

    Quat() : x(0), y(0), z(0), w(1) { }
    Quat(const v3d_t &dir,f32_t angle) {
        set(dir,angle);
    }
    Quat(f32_t x,f32_t y,f32_t z,f32_t angle) {
        set(x,y,z,angle);
    }
    Quat(const m_16t &m) {
        f32_t trace = m[0] + m[4] + m[8];
        if(trace > 0.0) {
            f32_t s = sqrt(trace + 1.0f);
            q[3] = 0.5f * s;
            s = 0.5f / s;
            q[0] = (m[5] - m[7]) * s;
            q[1] = (m[6] - m[2]) * s;
            q[2] = (m[1] - m[3]) * s;
        } else {
            static int32_t next[3] = { 1, 2, 0 };
            int32_t i = 0;
            if(m[4] > m[0]) i = 1;
            if(m[8] > m[3 * i + i]) i = 2;
            int32_t j = next[i];
            int32_t k = next[j];
            f32_t s = sqrt(m[3 * i + i] - m[3 * j + j] - m[3 * k + k] + 1.0f);
            q[i] = 0.5f * s;
            if(s != 0) s = 0.5f / s;
            q[3] = (m[3 * j + k] - m[3 * k + j]) * s;
            q[j] = (m[3 * i + j] + m[3 * j + i]) * s;
            q[k] = (m[3 * i + k] + m[3 * k + i]) * s;
        }
    }
    Quat& operator =(const Quat& q){x=q.x ;y=q.y ;z=q.z ;w=q.w ; return *this;}
    operator f32_t*() { return (f32_t*)&x; }
    operator const f32_t*() const { return (f32_t*)&x; }

    //f32_t& operator[](int32_t i) { return ((f32_t*)&x)[i]; }
    f32_t operator[](int32_t i) const { return ((f32_t*)&x)[i]; }

    Quat operator*(const Quat &q) const {
        Quat ret;
        ret.x = w * q.x + x * q.x + y * q.z - z * q.y;
        ret.y = w * q.y + y * q.w + z * q.x - x * q.z;
        ret.z = w * q.z + z * q.w + x * q.y - y * q.x;
        ret.w = w * q.w - x * q.x - y * q.y - z * q.z;
        return ret;
    }

    void set(const v3d_t &dir,f32_t angle) {
        f32_t length = dir.len();
        if(length != 0.0) {
            length = 1.0f / length;
            f32_t sinangle = Sinr(G2R(angle)/ 2.0f);
            x = dir.x * length * sinangle;
            y = dir.y * length * sinangle;
            z = dir.z * length * sinangle;
            w = Cosr(G2R(angle)/ 2.0f);
        } else {
            x = y = z = 0.0;
            w = 1.0;
        }
    }
    void set(f32_t x,f32_t y,f32_t z,f32_t angle) {
        set(v3d_t(x,y,z),angle);
    }

    void slerp(const Quat &q0,const Quat &q1,f32_t t) {
        f32_t k0,k1,cosomega = q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;
        Quat q;
        if(cosomega < 0.0) {
            cosomega = -cosomega;
            q.x = -q1.x;
            q.y = -q1.y;
            q.z = -q1.z;
            q.w = -q1.w;
        } else {
            q.x = q1.x;
            q.y = q1.y;
            q.z = q1.z;
            q.w = q1.w;
        }
        if(1.0 - cosomega > 1e-6) {
            f32_t omega = acos(cosomega);
            f32_t sinomega = Sinr(omega);
            k0 = Sinr((1.0f - t) * omega) / sinomega;
            k1 = Sinr(t * omega) / sinomega;
        } else {
            k0 = 1.0f - t;
            k1 = t;
        }
        x = q0.x * k0 + q.x * k1;
        y = q0.y * k0 + q.y * k1;
        z = q0.z * k0 + q.z * k1;
        w = q0.w * k0 + q.w * k1;
    }

    m_16t to_matrix() const {
        m_16t ret;
        f32_t x2 = x + x;
        f32_t y2 = y + y;
        f32_t z2 = z + z;
        f32_t xx = x * x2;
        f32_t yy = y * y2;
        f32_t zz = z * z2;
        f32_t xy = x * y2;
        f32_t yz = y * z2;
        f32_t xz = z * x2;
        f32_t wx = w * x2;
        f32_t wy = w * y2;
        f32_t wz = w * z2;
        ret[0] = 1.0f - (yy + zz); ret[3] = xy - wz; ret[6] = xz + wy;
        ret[1] = xy + wz; ret[4] = 1.0f - (xx + zz); ret[7] = yz - wx;
        ret[2] = xz - wy; ret[5] = yz + wx; ret[8] = 1.0f - (xx + yy);
        return ret;
    }

    union {
        struct {
            f32_t x,y,z,w;
        };
        f32_t q[4];
    };
};


//---------------------------------------------------------------------------------------
inline  m_16t MDirsRotate(v3d_t& f, v3d_t& u, v3d_t& s)
{
    m_16t mret;
    mret._m[8]=f.x;mret._m[9]=f.y;mret._m[10]=f.x;
    mret._m[4]=u.x;mret._m[5]=u.y;mret._m[6]=u.x;
    mret._m[0]=s.x;mret._m[1]=s.y;mret._m[2]=s.z;
    return mret;
}

//---------------------------------------------------------------------------------------
inline  m_16t MEulerRotate(f32_t a, f32_t b, f32_t c)
{
    m_16t mret;

    f32_t sa = Sinr(a);
    f32_t sb = Sinr(b);
    f32_t sy = Sinr(c);
    f32_t ca = Cosr(a);
    f32_t cb = Cosr(b);
    f32_t cy = Cosr(c);

    mret._m[0] = ca*cb*cy-sa*sy; mret._m[1] = -ca*cb*sy-sa*cy; mret._m[2] = ca*sy;
    mret._m[4] = sa*cb*cy-ca*sy; mret._m[5] = -sa*cb*sy+ca*cy; mret._m[6] = sa*sy;
    mret._m[8] = -sb*cy;         mret._m[9] = sb*sy;           mret._m[10] = cb;
    return mret;
}


//---------------------------------------------------------------------------------------
inline m_16t MRadRotate(f32_t radx, f32_t rady, f32_t radz)
{
    f32_t sinA, cosA;
    m_16t	 mret;

    if(radz != 0.0)
    {
        m_16t		mrz;
        sinA = (f32_t)Sinr(radz);
        cosA = (f32_t)Cosr(radz);
        mrz._m[0] =  cosA; mrz._m[4] = -sinA; mrz._m[8] =  0.0;
        mrz._m[1] =  sinA; mrz._m[5] =  cosA; mrz._m[9] =  0.0;
        mret*=mrz;
    }
    if(rady != 0.0)
    {
        m_16t		mry;
        sinA = (f32_t)Sinr(rady);
        cosA = (f32_t)Cosr(rady);
        mry._m[0] =  cosA;  mry._m[8] =  sinA;
        mry._m[2] = -sinA;  mry._m[10] =  cosA;
        mret*=mry;
    }

    if(radx != 0.0)
    {
        m_16t		mrx;
        sinA = (f32_t)Sinr(radx);
        cosA = (f32_t)Cosr(radx);
        mrx._m[5] =  cosA;  mrx._m[9] = -sinA;
        mrx._m[6] =  sinA;  mrx._m[10] =  cosA;
        mret*=mrx;
    }
    return mret;
}


//---------------------------------------------------------------------------------------
inline m_16t MRotAround(const v3d_t &axis, f32_t rad)
{
    v3d_t nrm = axis;
    m_16t mret;
    nrm.norm();
    f32_t sinA = (f32_t)Sinr(rad);
    f32_t cosA = (f32_t)Cosr(rad);
    f32_t invCosA = 1.0 - cosA;
    f32_t x = nrm.x;
    f32_t y = nrm.y;
    f32_t z = nrm.z;
    f32_t xSq = x * x;
    f32_t ySq = y * y;
    f32_t zSq = z * z;

    mret._m[0] = (invCosA * xSq) + (cosA);
    mret._m[4] = (invCosA * x * y) - (sinA * z );
    mret._m[8] = (invCosA * x * z) + (sinA * y );
    mret._m[1] = (invCosA * x * y) + (sinA * z);
    mret._m[5] = (invCosA * ySq) + (cosA);
    mret._m[9] = (invCosA * y * z) - (sinA * x);
    mret._m[2] = (invCosA * x * z) - (sinA * y);
    mret._m[6] = (invCosA * y * z) + (sinA * x);
    mret._m[10] = (invCosA * zSq) + (cosA);
    return mret;

}

//---------------------------------------------------------------------------------------
inline  m_16t MTranslate(f32_t x, f32_t y, f32_t z)
{
    m_16t mret;
    mret._m[12] = x; mret._m[13] = y; mret._m[14] = z;
    return mret;
}

//---------------------------------------------------------------------------------------
inline  m_16t MScale(f32_t x, f32_t y, f32_t z, f32_t w = 1.0)
{
    m_16t mret;
    mret._m[0] = x; mret._m[5] = y; mret._m[10] = z; mret._m[15] = w;
    return mret;
}

//---------------------------------------------------------------------------------------
inline m_16t MLook(const v3d_t& eye,const  v3d_t& fwd,const  v3d_t& up,const  v3d_t& side)
{
    m_16t mret;
    mret._m[0] = side.x;    mret._m[4] = side.y;    mret._m[8] = side.z;   mret._m[12] = 0.0;
    mret._m[1] = up.x;      mret._m[5] = up.y;      mret._m[9] = up.z;     mret._m[13] = 0.0;
    mret._m[2] = -fwd.x;    mret._m[6] = -fwd.y;    mret._m[10] = -fwd.z;   mret._m[14] = 0.0;
    mret._m[3] = 0.0;       mret._m[7] = 0.0;       mret._m[11] = 0.0;      mret._m[15] = 1.0;
    mret *= MTranslate(-eye.x, -eye.y, -eye.z);
    return mret;
}

//---------------------------------------------------------------------------------------
inline void GetAnglesFromVector( v3d_t &vec, f32_t &yawAng, f32_t &pitchAng ,f32_t& roll)
{
    yawAng = atan2(vec.x, vec.y) ;
    roll   = atan2(vec.z, vec.y);

    if( yawAng < 0.0 )
    {
        yawAng += DOIPI;
    }
    if( roll < 0.0 )
    {
        roll += DOIPI;
    }


    if( fabs(vec.x) > fabs(vec.y) )
    {
        pitchAng = atan2( fabs(vec.z), fabs(vec.x) );
    }
    else
    {
        pitchAng = atan2( fabs(vec.z), fabs(vec.y) );
    }

    if( vec.z < 0.0 )
    {
        pitchAng = -pitchAng;
    }
    pitchAng = (pitchAng);
    yawAng   = (yawAng);
    roll     = (roll);
}


//---------------------------------------------------------------------------------------
#define	PLANE_X						0
#define	PLANE_Y						1
#define	PLANE_Z						2
#define	PLANE_ANYX					3
#define	PLANE_ANYY					4
#define	PLANE_ANYZ					5
#define	PLANE_ANY					6

//---------------------------------------------------------------------------------------
typedef  f32_t  APlane[4];
class Plane
{
public:
    Plane():_c(0.0),_t(PLANE_ANY){};
    Plane& operator=(const Plane& pl){_n = pl._n; _c = pl._c;_t =pl._t;	return *this;};
    u_bool32 operator ==(const Plane& p){if(IsZero(p._c - _c)){	return IsZero( Vdp(_n , p._n)-1.0,  EPSILON);}	return false;}
    Plane(const v3d_t& vn, const f32_t c):_n(vn),_c(c),_t(PLANE_ANY){}
    Plane(const v3d_t& vn, const v3d_t& pt):_n(vn),_t(PLANE_ANY){	_c = Vdp(_n, pt);}
    Plane(const v3d_t& p0, const v3d_t& p1, const v3d_t& p2):_t(PLANE_ANY){CalcNormal(p0, p1, p2);}
    Plane(f32_t a, f32_t b , f32_t c, f32_t d) :_t(PLANE_ANY){_n = v3d_t(a,b,c); 	_c = d;	}
    ~Plane(){}
    INLN int32_t GetSide(const v3d_t& pct)const{f32_t r =  DistTo(pct);	if(r>EPSILON)	return 1;else if(r<-EPSILON)return -1;	return 0;}
    INLN f32_t DistTo(const v3d_t& pct)const{
        f32_t r = (Vdp(pct, _n) - _c);
        return r;
    }
    INLN f32_t DistTo(f32_t a, f32_t b, f32_t c)const{return (Vdp(v3d_t(a,b,c), _n) - _c);	}
    INLN f32_t operator-( v3d_t& pt){return DistTo(pt);}
    INLN void Set(const v3d_t& vn, const v3d_t& pct){_n = vn; _c  = Vdp(_n, pct);_t = PLANE_ANY;}
    INLN void Set(const v3d_t& vn, f32_t cnst){	_n = vn; _c=cnst;_t = PLANE_ANY;}
    INLN void Reverse(){	_n.negate();_c=-_c;	}
    INLN void CalcNormal(const v3d_t& p0, const  v3d_t& p1, const  v3d_t& p2){
        v3d_t vV1 = p0-p1;
        v3d_t vV2 = p0-p2;
        _n = Vcp(vV1, vV2);	_n.norm();
        _c = Vdp(p0,_n);
    }
    INLN char GetApproxAlignment()	{if (Rabs(_n.x) > Rabs(_n.y) && Rabs(_n.x) > Rabs(_n.z))return 'x';	if (Rabs(_n.y) > Rabs(_n.x) && Rabs(_n.y) > Rabs(_n.z))		return 'y';	return 'z';	}
    INLN f32_t FastDistTo(const v3d_t& pct)const{
        switch (_t&0xF){
        case PLANE_X: if(_n.x<0) return -(pct.x - (-_c)); return (pct.x - _c);
        case PLANE_Y: if(_n.y<0) return -(pct.y - (-_c)); return (pct.y - _c);
        case PLANE_Z: if(_n.z<0) return -(pct.z - (-_c)); return (pct.z - _c);
        }
        return (Vdp(pct, _n) - _c);
    }
    void TypeIt(){
        _t = PLANE_ANY;
        if(IsZero(Rabs(_n.x)-1.0))		_t = PLANE_X;
        else if(IsZero(Rabs(_n.y)-1.0))	_t = PLANE_Y;
        else if(IsZero(Rabs(_n.z)-1.0))	_t = PLANE_Z;
    }
    INLN u_bool32 RayIntersect( const v3d_t& vpos, const v3d_t& vdir, v3d_t& ip)const{
        f32_t dp = Vdp(_n, vdir);
        if(dp  >= 0.0)//away from plane
        {
            return false;
        }
        ip = vpos - ((vdir * DistTo(vpos)) / dp);
        return true;
    }
    u_bool32 GetIP ( const Plane &a, const Plane &b, v3d_t &v )
    {
        v3d_t   cp    = Vcp(a._n, b._n);
        f32_t denom = Vdp(_n, cp);

        if ( fabs ( denom ) < 1e-5 )
        {
            return false;
        }
        v3d_t  va = Vcp( a._n, b._n ) * -_c;
        v3d_t  vb = Vcp(b._n, _n )    * a._c ;
        v3d_t  vc = Vcp(_n, a._n )    * b._c ;
        v = ( va - vb- vc) / denom;
        return true;
    }

    /*
    int32_t GetSide(v3d_t& center, v3d_t& aex)
    {
    v3d_t  vx(aex.x,0,0);
    v3d_t  vy(0,aex.y,0);
    v3d_t  vz(0,0,aex.z);
    return GetSide(center, vx, vy, vz);
    }

      int32_t GetSide(v3d_t& center, v3d_t& ax, v3d_t& ay, v3d_t& az)
      {
      f32_t d2p =  DistTo(center);
      f32_t cd  =  Vdp(ax,_n) + Vdp(ay,_n) + Vdp(az,_n);
      if(d2p > cd)return 1;
      else if(d2p < cd)return -1;
      return 0;
      }
    */

public:
    v3d_t		_n;
    f32_t	_c;
    unsigned short	_t;
    unsigned short	_u;
    
};

//---------------------------------------------------------------------------------------
typedef  f32_t  ASphere[4];
class   NO_VT Sphere
{
public:
    Sphere(){}
    Sphere(v3d_t& center, f32_t r):_c(center), _r(r){}
    u_bool32 IsTouchingSphere( const Sphere& other){	return vdist(_c,other._c) < (other._r + _r);}
    u_bool32 GetSegIPoint(const v3d_t& start, const v3d_t& dir, v3d_t& rip){
        v3d_t&    center = _c;
        f32_t&  r = _r;
        v3d_t kDif = start - center;
        f32_t fA = dir.len2();
        f32_t fB = kDif.vdp(dir);
        f32_t fC = kDif.len2() - r*r;
        f32_t afT[2];
        f32_t fDiscr = fB*fB - fA*fC;
        if ( fDiscr < 0.0 )	{
            return false;
        }
        else if ( fDiscr > 0.0 ){
            f32_t fRoot = (f32_t)sqrt(fDiscr);
            f32_t fInvA = 1.0/fA;
            afT[0] = (-fB - fRoot)*fInvA;
            afT[1] = (-fB + fRoot)*fInvA;
        }else{
            afT[0] = -fB/fA;
        }
        rip  = start + dir*afT[0];
        return true;
    }
    f32_t DistToRay(const v3d_t& origin, const v3d_t& dir)	{
        v3d_t xv(_c);
        f32_t& r = _r;
        xv -= origin;
        f32_t lx = xv.len();
        f32_t dp = xv.vdp(dir);
        f32_t d = r*r - (lx*lx - dp*dp);
        if(d < 0) return INFINIT;
        return dp - (f32_t)sqrt(d);

    }
    u_bool32 FindRayIP(const v3d_t& start,const v3d_t& dir, v3d_t& ip);
    u_bool32 DistToSphere(const Sphere& other, f32_t* d=0){
        v3d_t& oc = (v3d_t&)other._c;
        f32_t di = (_r + other._r) - vdist(_c, oc);
        if(d)
            *d = di;
        return *d < 0;
    }
public:
    v3d_t     _c;
    f32_t   _r;
};

//---------------------------------------------------------------------------------------
typedef  f32_t  ABox[6];
class   NO_VT Box
{
public:
    Box():_min(INFINIT),_max(-INFINIT){}
    Box(const v3d_t& m, const v3d_t& mp):_min(INFINIT),_max(-INFINIT){AddPoint(m);	AddPoint(mp);}
    Box(const Box& box):_min(box._min),_max(box._max){}
    Box(const Box& box, const v3d_t& a, const v3d_t& b, f32_t ex=0):_min(box._min),_max(box._max){MakeMoveBox(a,b,ex);}
    Box(const Box& box, const v3d_t& a):_min(box._min),_max(box._max){Move(a);}
    Box(f32_t rad):_min(-rad),_max(rad){}
    void Reset(){_min =INFINIT;_max=-INFINIT;}
    Box& operator=(const Box& box){	_min=box._min;	_max=box._max;	return *this;}
    Box& operator+=(const v3d_t& pos) {_min+=pos;_max+=pos;	return *this;}
    Box& operator+=(f32_t val){_max+=val; _min+=(-val);	return *this;}
    Box& operator*=(f32_t val) {_max*=val;_min*=val;	return *this;}
    void Union(const Box& b){
        _max.x = tmax(_max.x, b._max.x);			_max.y = tmax(_max.y, b._max.y);
        _max.z = tmax(_max.z, b._max.z);			_min.x = tmin(_min.x, b._min.x);
        _min.y = tmin(_min.y, b._min.y);			_min.z = tmin(_min.z, b._min.z);
    }
    void AddPoint(const v3d_t& a){
        _max.x = tmax(_max.x, a.x);			_max.y = tmax(_max.y, a.y);
        _max.z = tmax(_max.z, a.z);			_min.x = tmin(_min.x, a.x);
        _min.y = tmin(_min.y, a.y);			_min.z = tmin(_min.z, a.z);
    }

    void MakeMoveBbox( v3d_t& start,  v3d_t& end, f32_t ex=0.0)
    {
        for (int32_t i=0 ; i<3 ; i++)
        {
            if (end[i] > start[i])
            {
                _min[i] += start[i]  - ex;
                _max[i] += end[i]    + ex;
            }
            else
            {
                _min[i] += end[i]   - ex;
                _max[i] += start[i] + ex;
            }
        }
    }
    f32_t GetRadius(){return GetMaxExtend()/2;}

    void FlatIt(v2d_t* puv){
        f32_t mx = _max.x-_min.x;
        f32_t my = _max.y-_min.y;
        f32_t mz = _max.z-_min.z;
        f32_t me =  tmin(tmin(mx,my),mz);
        if(me == mx){
            _max.x = _min.x =0;
            puv[0] = v2d_t(_min.z,_min.y);
            puv[1] = v2d_t(_max.z,_max.y);
        }
        else if(me == my){
            _max.y = _min.y =0;
            puv[0] = v2d_t(_min.x,_min.z);
            puv[1] = v2d_t(_max.x,_max.z);
        }
        else if(me == mz){
            _max.z = _min.z =0;
            puv[0] = v2d_t(_min.x,_min.y);
            puv[1] = v2d_t(_max.x,_max.y);
        }
    }
    char GetMinimAx(){
        f32_t mx = _max.x-_min.x;
        f32_t my = _max.y-_min.y;
        f32_t mz = _max.z-_min.z;
        f32_t me =  tmin(tmin(mx,my),mz);
        if(me == mx)		return 'x';
        else if(me == my)	return 'y';
        else if(me == mz)	return 'z';
        return 0;
    }

    f32_t GetMinExtend()	{
        f32_t mx = _max.x-_min.x;
        f32_t my = _max.y-_min.y;
        f32_t mz = _max.z-_min.z;
        return tmin(tmin(mx,my),mz);
    }

    f32_t GetMaxExtend()const{
        f32_t mx = _max.x-_min.x;
        f32_t my = _max.y-_min.y;
        f32_t mz = _max.z-_min.z;
        return tmax(tmax(mx,my),mz);
    }
    f32_t DistTo(const Plane& p)
    {
        const v3d_t& ext = GetExtends();
        return Rabs(ext.x*p._n.x)+Rabs(ext.y*p._n.y)+Rabs(ext.z*p._n.z);
    }
    v3d_t GetExtends()const	{	return (_max-_min);	}
    void Transform(m_16t& m){
        m.v3transform(_max);
        m.v3transform(_min);
    }
    v3d_t  GetCenter()const	{return (_min + _max)/2;}

    u_bool32 ContainBox(const Box& pct){
        return  ContainPoint(pct._max) &&
            ContainPoint(pct._min);
    }

    u_bool32 ContainPoint(const v3d_t& pct)const{
        return  (pct.x>_min.x && pct.x<_max.x &&
            pct.y>_min.y && pct.y<_max.y &&
            pct.z>_min.z && pct.z<_max.z );
    }

    void Expand0(f32_t ammnt = .1f, f32_t epsil=EPSILONBIG)
    {
        v3d_t ex = _max-_min;
        if(IsZero(ex.x, epsil)){
            _max.x+=ammnt;
            _min.x-=ammnt;
        }
        if(IsZero(ex.y,epsil)){
            _max.y+=ammnt;
            _min.y-=ammnt;
        }
        if(IsZero(ex.z,epsil)){//AUG 04
            _max.z+=ammnt;
            _min.z-=ammnt;
        }
    }

    void Expand(f32_t val){
        _min-=val;
        _max+=val;
    }

    u_bool32 IsTouchesSphere(const Sphere& other){
        const  f32_t&  rad = other._r;
        const  v3d_t&    pos = other._c;

        if (pos.x+rad >= _min.x && pos.x-rad <= _max.x)
        if (pos.y+rad >= _min.y && pos.y-rad <= _max.y)
        if (pos.z+rad >= _min.z && pos.z-rad <= _max.z)
            return true;
        return false;
    }

     u_bool32 IsTouchesSphere(v3d_t& vpos, f32_t rrad)
     {
        if (vpos.x+rrad >= _min.x && vpos.x-rrad <= _max.x)
        if (vpos.y+rrad >= _min.y && vpos.y-rrad <= _max.y)
        if (vpos.z+rrad >= _min.z && vpos.z-rrad <= _max.z)
            return true;
        return false;
    }
  
    u_bool32 IsTouchesBox(const Box& other)
    {
        if(_max.x < other._min.x)				return false;
        if(_min.x > other._max.x)				return false;
        if(_max.z < other._min.z)				return false;
        if(_min.z > other._max.z)				return false;
        if(_max.y < other._min.y)				return false;
        if(_min.y > other._max.y)				return false;
        return true;
    }

    void	FromSegment(const v3d_t& a, const v3d_t& b){
        _min.x = tmin(a.x,b.x);
        _min.y = tmin(a.y,b.y);
        _min.z = tmin(a.z,b.z);
        _max.x = tmax(a.x,b.x);
        _max.y = tmax(a.y,b.y);
        _max.z = tmax(a.z,b.z);
    }

    f32_t PlaneShift(Plane& plan)
    {
        f32_t& r = plan._c;
        v3d_t&	  pn = plan._n;

        if (pn.x > 0)
            r -= pn.x * _min.x;
        else
            r -= pn.x * _max.x;

        if (pn.y > 0)
            r -= pn.y * _min.y;
        else
            r -= pn.y * _max.y;

        if (pn.z > 0)
            r -= pn.z * _min.z;
        else
            r -= pn.z * _max.z;
        return 0;
    }

    u_bool32 PointInside(v3d_t& pct)
    {
        return ((pct.x >= _min.x) && (pct.x <= _max.x) &&
            (pct.y >= _min.y) && (pct.y <= _max.y) &&
            (pct.z >= _min.z) && (pct.z <= _max.z));
    }

    void    GetFace(v3d_t* pV, char face)//'u,d,f,b,e,w'
    {
        // return faces facing outwards
        switch(face)
        {
        case 'U':
            pV[0] = v3d_t(_min.x, _max.y, _min.z);
            pV[1] = v3d_t(_max.x, _max.y, _min.z);
            pV[2] = v3d_t(_max.x, _max.y, _max.z);
            pV[3] = v3d_t(_min.x, _max.y, _max.z);
            break;
        case 'D':
            pV[3] = v3d_t(_min.x, _min.y, _min.z);
            pV[2] = v3d_t(_max.x, _min.y, _min.z);
            pV[1] = v3d_t(_max.x, _min.y, _max.z);
            pV[0] = v3d_t(_min.x, _min.y, _max.z);
            break;
        case 'F':
            pV[0] = v3d_t(_min.x, _max.y, _max.z);
            pV[1] = v3d_t(_max.x, _max.y, _max.z);
            pV[2] = v3d_t(_max.x, _min.y, _max.z);
            pV[3] = v3d_t(_min.x, _min.y, _max.z);
            break;
        case 'B':
            pV[3] = v3d_t(_min.x, _max.y, _min.z);
            pV[2] = v3d_t(_max.x, _max.y, _min.z);
            pV[1] = v3d_t(_max.x, _min.y, _min.z);
            pV[0] = v3d_t(_min.x, _min.y, _min.z);
            break;
        case 'L':
            pV[0] = v3d_t(_min.x, _max.y, _min.z);
            pV[1] = v3d_t(_min.x, _max.y, _max.z);
            pV[2] = v3d_t(_min.x, _min.y, _max.z);
            pV[3] = v3d_t(_min.x, _min.y, _min.z);
            break;
        case 'R':
            pV[3] = v3d_t(_max.x, _max.y, _min.z);
            pV[2] = v3d_t(_max.x, _max.y, _max.z);
            pV[1] = v3d_t(_max.x, _min.y, _max.z);
            pV[0] = v3d_t(_max.x, _min.y, _min.z);
            break;
        default: break;
        }
    }

    void Move(const v3d_t& pos)
    {
        _min+=pos;_max+=pos;
    }

    void MakeMoveBox(const v3d_t& start,const  v3d_t& vend, f32_t ex=0.0)
    {
        for (int32_t i=0 ; i<3 ; i++)
        {
            if (vend[i] > start[i])
            {
                _min[i] += start[i]  - ex;
                _max[i] += vend[i]    + ex;
            }else
            {
                _min[i] += vend[i]   - ex;
                _max[i] += start[i] + ex;
            }
        }
    }
    v3d_t* GetCorners(v3d_t* pVx)
    {
        pVx[0].setval(_min);
        pVx[1].setval(_min.x,_min.y,_max.z);
        pVx[2].setval(_max.x,_min.y,_max.z);
        pVx[3].setval(_max.x,_min.y,_min.z);

        pVx[4].setval(_min.x,_max.y,_min.z);
        pVx[5].setval(_max.x,_max.y,_min.z);
        pVx[6].setval(_max);
        pVx[7].setval(_max.x,_min.y,_max.z);
        return _norms;
    }

    Plane* GetPlanes()
    {
        static Plane planes[6];

        planes[0]._n = VY;
        planes[0]._c = _max.y;

        planes[1]._n = -VY;
        planes[1]._c = _min.y;

        planes[2]._n = VX;
        planes[2]._c = _max.x;

        planes[3]._n = -VX;
        planes[3]._c = _min.x;

        planes[4]._n = VZ;
        planes[4]._c = _max.z;

        planes[5]._n = -VZ;
        planes[5]._c = _min.z;

        return planes;

    }

    f32_t Volume(){
        v3d_t extend = _max-_min;
        return Vdp(extend, extend);
    }

public:
    v3d_t	        _min;
    v3d_t	        _max;
    static v3d_t   _norms[8];
};


//---------------------------------------------------------------------------------------
INLN void    GetCornersOfPlane(v3d_t& mins, v3d_t&  maxs, v3d_t& normal, v3d_t* corners)
{
    for (REG int32_t i=0 ; i<3 ; i++)
    {
        if (normal[i] < 0)
        {
            corners[0][i] = mins[i];
            corners[1][i] = maxs[i];
        }
        else
        {
            corners[1][i] = mins[i];
            corners[0][i] = maxs[i];
        }
    }
}

//--------------------------------------------------------------------------------------------
inline v3d_t	GetMinAx(v3d_t& normal)
{
    v3d_t	rv;
    if( Rabs(normal.y) > Rabs(normal.z) )
    {
        if( Rabs(normal.z) < Rabs(normal.x) )
            rv.z = 1.f;
        else
            rv.x = 1.f;
    }
    else
    {
        if (Rabs(normal.y )<= Rabs(normal.x) )
            rv.y = 1;
        else
            rv.x = 1;
    }
    return rv;
}

//--------------------------------------------------------------------------------------------
inline v3d_t	GetMaxAx(v3d_t& normal)
{
    v3d_t	rv = normal;

    rv.x = Rabs(rv.x);
    rv.y = Rabs(rv.y);
    rv.z = Rabs(rv.z);

    if(rv.x == 1)
        return v3d_t(1,0,0);
    if(rv.y == 1)
        return v3d_t(0,1,0);
    if(rv.z == 1)
        return v3d_t(0,0,1);

    if(rv.x > rv.y && rv.x > rv.z)
        return v3d_t(1,0,0);
    else if(rv.y > rv.x && rv.y > rv.z)
        return v3d_t(0,1,0);
    return v3d_t(0,0,1);
}

INLN void	ReflectVector(v3d_t& who, v3d_t& normal, f32_t attn, v3d_t& newSpeed)
{
    newSpeed = 2.0f * (normal * Vdp(-who, normal)) +  who;
    newSpeed*=attn;
}




INLN void ClipVector(v3d_t& speed, v3d_t& normal, f32_t bounce, v3d_t& newSpeed)
{
    f32_t    reflect = Vdp(speed, normal) ;
    newSpeed = bounce * (speed - normal * reflect);
    if(Rabs(newSpeed.x)<.2f)newSpeed.x=0;
    if(Rabs(newSpeed.y)<.2f)newSpeed.y=0;
    if(Rabs(newSpeed.z)<.2f)newSpeed.z=0;
}

INLN void MakeDir(v3d_t& av, v3d_t& bv, v3d_t& dir){
    dir = bv;
    dir -=av;
    dir.norm();
}
INLN void TweakPlaneDistance(v3d_t& pos, v3d_t& norm){
    pos  += (norm * .3f);
}
INLN void HackVertBilboard(v3d_t& campos, v3d_t& pos, v3d_t& dir){
    dir	    =  campos;
    dir	    -= pos;
    dir.y	=  0.0f;
    dir.norm();
}


INLN f32_t DistPointSeg(const v3d_t& start, const v3d_t& end, const v3d_t& point)
{
    v3d_t diff = point - start;
    v3d_t	dend = end-start;
    f32_t fT = Vdp(diff, dend);

    if ( fT <= 0.0 )
    {
        fT = 0.0;
    }
    else
    {
        f32_t len2 = dend.len2();
        if ( fT >= len2)
        {
            fT = 1.0;
            diff -= dend;
        }
        else
        {
            fT /= len2;
            diff -= (dend * fT);
        }
    }
    return  diff.len();
}

INLN f32_t DistPointSeg(const v3d_t& start, const v3d_t& end, const v3d_t& point, v3d_t& ponLine)
{
    v3d_t diff = point - start;
    v3d_t	dend = end-start;
    f32_t fT = Vdp(diff, dend);

    if ( fT <= 0.0 )
    {
        fT = 0.0;
        ponLine = start;
    }
    else
    {
        f32_t len2 = dend.len2();
        if ( fT >= len2)
        {
            fT = 1.0;
            diff -= dend;
        }
        else
        {
            fT /= len2;
            diff -= (dend * fT);
        }
        ponLine = start + (end-start)*fT;
    }
    return  diff.len();
}


//----------------------------------------------------------------------------------------
INLN f32_t DistRayPlane(v3d_t& rOrigin, v3d_t& rDir, v3d_t& pOnPlan, v3d_t& pNormal)
{
    f32_t denom = Vdp(pNormal,rDir);
    if (denom == 0)
        return INFINIT;
    f32_t da = - (Vdp(pNormal,pOnPlan));
    f32_t numer = Vdp(pNormal,rOrigin) + da;
    return -(numer / denom);
}

//----------------------------------------------------------------------------------------
INLN v3d_t ClosestLinePointToPoint(v3d_t& lstart, v3d_t& lend, v3d_t& point)
{
    static v3d_t vret;
    // Determine t (the length of the v3d_t from �lstart� to �point�)
    vret = point;
    vret -= lstart;
    v3d_t xv = lend;
    xv-=lstart;

    f32_t lenxv = xv.len();
    xv.norm();

    f32_t t = Vdp(xv,vret);

    // Check to see if �t� is beyond the extents of the line segment
    if (t < 0.0f)
    {
        vret.setval(lstart);
        return vret;
    }
    if (t > lenxv)
    {
        vret.setval(lend);
        return vret;
    }
    // construct to point a+bt
    vret = lstart;
    vret += xv*t;
    return vret;
}

INLN f32_t S_PushPlane(Plane& plan, v3d_t& ex, u_bool32 upward=1)
{
    const v3d_t& normal = plan._n;
    if(upward)
    {
        plan._c += (normal.x) * ex.x;
  	    plan._c += (normal.y) * ex.y;
        plan._c += (normal.z) * ex.z;
    }
    else
    {
        plan._c -= normal.x * ex.x;
    	plan._c -= normal.y * ex.y;
        plan._c -= normal.z * ex.z;
    }
    return 0;
}

INLN f32_t ExtendsToPlane(const v3d_t& v, const Plane& p)
{
    return Rabs(v.x*p._n.x)+Rabs(v.y*p._n.y)+Rabs(v.z*p._n.z);
}

//---------------------------------------------------------------------------------------
#define FOREACH(_xtype, _coll, it_begin)    if(_coll.size()) \
                                            for(_xtype::iterator it_begin = _coll.begin(); it_begin!=_coll.end();it_begin++)




//---------------------------------------------------------------------------------------
typedef  f32_t  APos[12];
class NO_VT Pos 
{
public:
    v3d_t		_right;
    v3d_t		_up;
    v3d_t		_fwd;
    v3d_t		_pos;
    v3d_t      _euler;
    f32_t    _rot;
    v3d_t      _scale;
    int32_t     _index;
    INLN 	void	MoveUp(f32_t r){_pos   += r * _up;}
    INLN 	void	MoveSide(f32_t r){_pos += r * _right;}
    INLN 	void	MoveFwd(f32_t r){_pos  += r * _fwd;}
    INLN    void    Rotate(f32_t a, f32_t e, f32_t r)
    {
        _euler.y += a;	
	    _euler.x -= e;	
	    _euler.z += r;
	    BLOCKRANGE(_euler.x);
        Euler2Pos();
    }
 
    INLN void	SetAngles(f32_t a, f32_t e, f32_t r){
        ROLLPI(a);
        BLOCKRANGE(e);
        _euler.y=a; _euler.x=e; _euler.z=r;
        Euler2Pos();
    }

    INLN    m_16t	    LookAt(){return MLook(_pos, _fwd, _up, _right);}
    operator m_16t*()const{return ((m_16t*)this);}
    Pos& operator=(const Pos& p){
        if(&p==this)return *this;
        _right = p._right;
        _up    = p._up;
        _fwd   = p._fwd;
        _pos   = p._pos;
        _euler = p._euler;
        _rot   = p._rot;
        return *this;
    }
    void Euler2Pos(){
        f32_t CosrAz = Cosr(_euler.y);
        f32_t CosrEl = Cosr(_euler.x);
        f32_t SinrAz = Sinr(_euler.y);
        f32_t SinrEl = Sinr(_euler.x);
        f32_t CosrRl = Cosr(_euler.z);
        f32_t SinrRl = Sinr(_euler.z);
	    _fwd.x = SinrAz * CosrEl;
	    _fwd.y = SinrEl;
	    _fwd.z = CosrEl * -CosrAz;
	    _up.x = -CosrAz * SinrRl - SinrAz * SinrEl * CosrRl;
	    _up.y = CosrEl * CosrRl;
	    _up.z = -SinrAz * SinrRl - SinrEl * CosrRl * -CosrAz;
	    _right = Vcp(_fwd, _up);
    }

    INLN void Dir2Euler(u_bool32 toDeg){
        f32_t dy2 = (f32_t)sqrt(1-_fwd.y*_fwd.y);
        _euler.x = (f32_t)Sinr(_fwd.y);
        _euler.z = (f32_t)Cosr(_up.y  / dy2);
        _euler.y = (f32_t)Sinr(_fwd.x / dy2);	  // az
        ROLLPI(_euler.y);
        if(toDeg)GetEulerDegrees();
    }

    virtual void MoveAndRot(int32_t* pr, f32_t m, f32_t r){
        MoveFwd(pr[2]*m);
        MoveSide(pr[0]*m);
        MoveUp(pr[1]*m);
        Rotate( pr[3] * r , pr[4] * r, pr[5] * r);
    }
    INLN v3d_t GetEulerDegrees(int32_t b=0){
        v3d_t ret(_euler.x,_euler.y, _euler.z);
        if(1 == b){
            ret.y = 180.0-_euler.y;
        }
        else if(_fwd.z < 0.f){
            ret.y = 180.0-_euler.y;
        }
        return ret;
    }
};


//---------------------------------------------------------------------------------------
#pragma pack(pop)


#endif // __BASELIB_H__

/*
//---------------------------------------------------------------------------------------
class NO_VT Ortho
{
public:
    v3d_t		_wpos;
    v3d_t		_ldir;
    v3d_t		_lup;
    v3d_t		_lside;
    v3d_t	    _euler;
    v3d_t      _grsped;
    v3d_t      _vel;

    f32_t	_rotAngle;
    v3d_t      _rotPivot;
    v3d_t		_scalation;
    v3d_t	    _pwpos;
public:
    virtual ~Ortho(){}
    Ortho(f32_t *pos){
        _wpos.setval(pos[0],pos[1],pos[2]);
        SetAngles(pos[3], pos[4], pos[5]);
    }
    Ortho(){
        Reset();
    }
    INLN void Reset(){
        _pwpos.reset();
        _wpos.reset();
        _ldir.setval(v3d_t(0,0,-1));
        _lup.setval(v3d_t(0,1,0));
        _lside.setval(v3d_t(1,0,0));
        _rotAngle  = 0.0;
        _scalation = 1.0;
    }

    void CopyAngles(Ortho* pos){
        _euler.y = pos->_euler.y;
        _euler.x = pos->_euler.x;
        _euler.z = pos->_euler.z;
    }

    INLN void GetRawPos(f32_t* pv){
        pv[0] = _wpos.x;
        pv[1] = _wpos.y;
        pv[2] = _wpos.z;
        pv[3] = _euler.y;
        pv[4] = _euler.x;
        pv[5] = _euler.z;

    }
    INLN void SetRawPos(f32_t* pv){
        _wpos.x = pv[0];
        _wpos.y = pv[1];
        _wpos.z = pv[2];
        _euler.y = pv[3];
        _euler.x = pv[4];
        _euler.z = pv[5];
    }
    INLN void	SetAngles(f32_t a, f32_t e, f32_t r){
        ROLLPI(a);
        BLOCKRANGE(e);
        _euler.y=a; _euler.x=e; _euler.z=r;
        Euler2Dir();
    }
    INLN void BlockVertMovement(){
        _lup.setval(0.0);
        _ldir.y = 0;
        _ldir.norm();
    }
    INLN void MoveAndRot(f32_t* pr){
        MoveFwd(pr[2]);
        MoveSide(pr[0]);
        MoveUp(pr[1]);
        Rotate( pr[3],pr[4], pr[5]);
    }
    INLN void MoveAndRot(f32_t f, f32_t s, f32_t u, f32_t a, f32_t e, f32_t r)	{
        if(u)MoveUp(u);
        if(s)MoveSide(s);
        if(f)MoveFwd(f);
        Rotate( a, e, r);
    }
    INLN void Rotate(f32_t a, f32_t e, f32_t r)
    {
        _euler.y += a;
        _euler.x -= e;
        _euler.z += r;
        BLOCKRANGE(_euler.x);
        Euler2Dir();
    }
    INLN void Copy(Ortho* pr){
        _wpos      = pr->_wpos;
        _ldir      = pr->_ldir;
        _lup       = pr->_lup;
        _lside	   = pr->_lside;
        _euler.y  = pr->_euler.y;
        _euler.x  = pr->_euler.x;
        _euler.z  = pr->_euler.z;
        _rotAngle  = pr->_rotAngle;
    }

    INLN 	void	MoveUp(f32_t r){
        _wpos += r*_lup;
    }
    INLN 	void	MoveSide(f32_t r){
        _wpos += r*_lside;
    }
    INLN 	void	MoveFwd(f32_t r){
        _wpos += r*_ldir;
    }
    INLN m_16t	MakeLookAtMatrix(){
        return MLook(_wpos, _ldir, _lup, _lside);
    }
    INLN void EqualDirs(Ortho* pos){
        _wpos = pos->_wpos;
        _ldir = pos->_ldir;
        _lside = pos->_lside;
    }
    INLN void EqualAngles(Ortho* pos){
        _euler.y = pos->_euler.y;
        _euler.x = pos->_euler.x;
        _euler.z = pos->_euler.z;
    }

    INLN void Dir2Euler(u_bool32 toDeg){
        f32_t dy2 = (f32_t)sqrt(1-_ldir.y*_ldir.y);
        _euler.x = (f32_t)Sinr(_ldir.y);
        _euler.z = (f32_t)Cosr(_lup.y  / dy2);
        _euler.y = (f32_t)Sinr(_ldir.x / dy2);	  // az
        ROLLPI(_euler.y);
        if(toDeg)GetEulerDegrees();
    }

    INLN void Euler2Dir(){
        f32_t CosrAz = Cosr(_euler.y);
        f32_t CosrEl = Cosr(_euler.x);
        f32_t SinrAz = Sinr(_euler.y);
        f32_t SinrEl = Sinr(_euler.x);
        f32_t CosrRl = Cosr(_euler.z);
        f32_t SinrRl = Sinr(_euler.z);
        _ldir.x = SinrAz * CosrEl;
        _ldir.y = SinrEl;
        _ldir.z = CosrEl * -CosrAz;
        _lup.x = -CosrAz * SinrRl - SinrAz * SinrEl * CosrRl;
        _lup.y = CosrEl * CosrRl;
        _lup.z = -SinrAz * SinrRl - SinrEl * CosrRl * -CosrAz;
        _lside = Vcp(_ldir, _lup);
    }

    INLN m_16t BuildTrMatrix(){
        m_16t trmat;
        if(_wpos.isnot0())
            trmat *= MTranslate(_wpos.x,_wpos.y,_wpos.z);          // translate to desired pos
        if(_scalation.x!=1.0)
            trmat *= MScale(_scalation.x,_scalation.y,_scalation.z);     // scale it
        if(_rotAngle)
            trmat *= MRotAround(_rotPivot, G2R(_rotAngle));            // rotate around _rotPivot if _rotAngle
        if(_euler.isnot0())
            trmat *= MRadRotate(_euler.x,_euler.y,_euler.z);          // rotate
        return trmat;
    }
    INLN v3d_t GetEulerDegrees(int32_t b=0){
        v3d_t ret(_euler.x,_euler.y, _euler.z);
        if(1 == b){
            ret.y = 180.0-_euler.y;
        }
        else if(_ldir.z < 0.f){
            ret.y = 180.0-_euler.y;
        }
        return ret;
    }
};
*/

