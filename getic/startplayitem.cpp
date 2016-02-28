// LightBulb.cpp: implementation of the CStartPlayItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "startplayitem.h"
#include "texadapter.h"
#include "document.h"
#include "view2d.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStartPlayItem::CStartPlayItem():SceItem()
{
    _euler          = v3d_t(0,0,0);           // spheric no spot
    _item           = ITM_STARTPLAY;
    strcpy(_catname , "ITM_STARTPLAY") ;
    _tstprintf(_name,"Start%d",_id);
    _team            = "";
}

CStartPlayItem::CStartPlayItem(const CStartPlayItem& r):SceItem((const SceItem&)r)
{
    _team   = r._team;
}

CStartPlayItem& CStartPlayItem::operator=(const CStartPlayItem& r)
{
    if(this != &r)
    {
        this->SceItem::operator=((const SceItem&)(r));
        _team   = r._team;
    }
    return *this;
}

CStartPlayItem::~CStartPlayItem()
{

}

void    CStartPlayItem::GlDraw2D(View2D* pV)
{
    SceItem::GlDraw2D(pV);
    Pd->DrawOrtho(_t, _euler, 64, 1);
}

Htex& CStartPlayItem::GetTexture()
{
    static Htex h;
    assert(0);
    return h; //_startPosTex;
}

void CStartPlayItem::Serialize(FileWrap* pfv,const vvector<Texture>& texNames, map<int32_t,int32_t>& texassoc)
{
    SceItem::Serialize(pfv, texNames, texassoc);
    if(pfv->_store)
    {
        if(_team.size()==0)
                _team="*";
        pfv->WriteStr(_team.c_str());
    }
    else
    {
        char enough[512];
        pfv->ReadStr(enough);
        _team = " ";
    }
}
