
#ifndef __CUST_ITEM_CATS__
#define __CUST_ITEM_CATS__

#include <baseutils.h>
#include <basecont.h>
#include <bspfilestr.h>

inline VAR FromSignature(const char* sig)
{
    if(!strcmp(sig,"u_int8_t"))
        return V_BYTE;
    if(!strcmp(sig,"CHAR"))
        return V_CHAR;
    if(!strcmp(sig,"TEXT"))
        return V_TEXT;
    if(!strcmp(sig,"FILENAME"))
        return V_FILENAME;
    if(!strcmp(sig,"INT"))
        return V_INT;
    if(!strcmp(sig,"float"))
        return V_REAL;
    if(!strcmp(sig,"f32_t"))
        return V_REAL;
    if(!strcmp(sig,"u_rgba_t"))
        return V_CLR;
    if(!strcmp(sig,"v3d_t"))
        return V_V3;
    if(!strcmp(sig,"v2d_t"))
        return V_UV;
    if(!strcmp(sig,"DOUBLE"))
        return V_DOUBLE;
    return V_BUFFER;
};


inline int32_t TypeSize(VAR typ)
{
    switch(typ)
    {
        case V_BYTE:
            return sizeof(u_int8_t);
        case V_CHAR:
            return sizeof(char);
        case V_TEXT:
        case V_FILENAME:
            return MAX_SLEN*sizeof(char);
        case V_INT:
            return sizeof(int32_t);
        case V_REAL:
            return sizeof(f32_t);
        case V_CLR:
            return sizeof(u_rgba_t);
        case V_V3:
            return sizeof(v3d_t);
        case V_UV:
            return sizeof(v2d_t);
        case V_DOUBLE:
            return sizeof(double);
        case V_UNKNOWN:
        case V_VOID:
        case V_BUFFER:
            break;
    }
    assert(0);
    return 0;
};

struct  NameTypeValue
{
    NameTypeValue(){
        _name[0]=0;
        _type=(_VAR)-1;
        memset(_value,0,sizeof(_value));
        memset(_valueMax,0,sizeof(_value));
        memset(_valueMin,0,sizeof(_value));
    }

    char    _name[MAX_SLEN];
    VAR     _type;
    u_int8_t    _value[MAX_SLEN];
    u_int8_t    _valueMax[MAX_SLEN];
    u_int8_t    _valueMin[MAX_SLEN];

    template <class T> void SetValue(T& t)
    {
        T*  pt = (T*)_value;
        *pt = t;
    }

    template <char> void SetValue(char* t)
    {
        SCopy((char*)_value,t, 32);
    }

    template <class T>void  GetValue(T& t)
    {
        T*  pt = (T*)_value;
        t = *pt ;
    }
    template <char>void GetValue(char* t)
    {
        SCopy(t,(char*)_value, 32);
    }
//
    template <class T> void SetMaxValue(T& t)
    {
        T*  pt = (T*)_value;
        *pt = t;
    }
    template <char> void SetMaxValue(char* t)
    {
        SCopy((char*)_value,t, 32);
    }

    template <class T>void  GetMaxValue(T& t)
    {
        T*  pt = (T*)_value;
        t = *pt ;
    }
    template <char>void GetMaxValue(char* t)
    {
        SCopy(t,(char*)_value, 32);
    }

//
    template <class T> void SetMinValue(T& t)
    {
        T*  pt = (T*)_value;
        *pt = t;
    }
    template <char> void SetMinValue(char* t)
    {
        SCopy((char*)_value,t, 32);
    }

    template <class T>void  GetMinValue(T& t)
    {
        T*  pt = (T*)_value;
        t = *pt ;
    }
    template <char>void GetMinValue(char* t)
    {
        SCopy(t,(char*)_value, 32);
    }



};

//-----------------------------------------------------------------------------
// item category. realisation into specific items
class ItemCat
{
public:
    ItemCat(){
        _catname[0]=0;
        _itemname[0]=0;
    }
    char                     _catname[MAX_SLEN];  //
    char                     _itemname[MAX_SLEN]; //
    vvector<NameTypeValue>   _classprops;         // class config dialog
    vvector<NameTypeValue>   _objprops;           // per object properties


};

#endif // __CUST_ITEM_CATS__
