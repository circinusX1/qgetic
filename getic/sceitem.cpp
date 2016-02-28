
#include "stdafx.h"
#include "geticapp.h"
#include "sceitem.h"
#include "document.h"
#include "texadapter.h"
#include "poly.h"
#include "scriptitem.h"
#include "startplayitem.h"
#include "sounditem.h"
#include "lightbulb.h"
#include "triggeritem.h"
#include "sounditem.h"
#include "gameitem.h"
#include "view2d.h"
#include "view3d.h"

//-----------------------------------------------------------------------------
v_vunc_t    SceItem::_Vtci[4];
u_bool32    SceItem::_BInitalized = false;
int32_t    SceItem::_GItemID     = 0; // cd


ItemSig IS[]={
    {"."},
    {"L"},
    {"T"},
    {"S"},
    {"."},
    {"."},
    {"."},
    {"."},
    {"."},
    {"."},
    {"."},
    {"."},
    {"."},
    {"."},
    {"."},
    {"."},
    {"."},
    {"."},
    {"."},
    {"."},
    {"."},
    {"."},
    {"."},
    {"."},
    {"."},
    {"."},
    {"."},
    {"."},
    {"."},
    {"."},
    {"U"},
    {"X"},
};



SceItem::SceItem():_item(ITM_NONE),_props(0),_flags(0),_colorD(ZWHITE)
{
    _leafIdx     = -1;
    _selected    = false;
    if(_BInitalized == false)
    {
        _BInitalized = true;
        // on yoz facing to us
//        _Vtci[0] = v_vunc_t(v3d_t(-16, 16,0), v2d_t(1,1), ZWHITE,0);
//        _Vtci[1] = v_vunc_t(v3d_t( 16, 16,0), v2d_t(1,0), ZWHITE,0);
//        _Vtci[2] = v_vunc_t(v3d_t( 16,-16,0), v2d_t(0,0), ZWHITE,0);
//        _Vtci[3] = v_vunc_t(v3d_t(-16,-16,0), v2d_t(0,1), ZWHITE,0);


        _Vtci[0] = v_vunc_t(v3d_t(-16, 16,0), v2d_t(0,1), ZWHITE,0);
        _Vtci[1] = v_vunc_t(v3d_t( 16, 16,0), v2d_t(1,1), ZWHITE,0);
        _Vtci[2] = v_vunc_t(v3d_t( 16,-16,0), v2d_t(1,0), ZWHITE,0);
        _Vtci[3] = v_vunc_t(v3d_t(-16,-16,0), v2d_t(0,0), ZWHITE,0);

    }
    _group    = 0;
    _id       = _GItemID++;
    _s        = V1;
    _euler    = V0;
    _zonearea= -1;
    _tstprintf(_name,"%s",MKSTR("Item%d",_id));
}

SceItem::SceItem(const SceItem& r)
{
    if(GNewUID)
    {
        _id    = _GItemID++;
         char name[64];
        _ttcscpy(name,r._name);
        StripDigits(name);
        if(strlen(name))
            _tstprintf(_name, "%s%d", name, _id);
        else
            _tstprintf(_name, "%d", _id);

    }
    else
    {
        _id    = r._id;
        _ttcscpy(_name,r._name);
    }
    strcpy(_catname,r._catname);
    _item       = r._item    ;
    _props      = r._props   ;
    _flags      = r._flags   ;
    _selected   = r._selected;
    _colorD     = r._colorD  ;
    _zonearea   = r._zonearea;
    _t          = r._t       ;
    _s          = r._s       ;
    _euler      = r._euler   ;
    _tl         = r._tl      ;
    _sl         = r._sl      ;
    _dirl       = r._dirl    ;
    _group      = r._group   ;
    _leafIdx    = r._leafIdx ;
    _combine    = r._combine;
    _textures   = r._textures;

}

SceItem& SceItem::operator=(const SceItem& r)
{
    if(this!=&r)
    {
        if(GNewUID)
        {
            _id    = _GItemID++;
             char name[64];
            _ttcscpy(name,r._name);
            StripDigits(name);
            if(strlen(name))
                _tstprintf(_name, "%s%d", name, _id);
            else
                _tstprintf(_name, "%d", _id);

        }
        else
        {
            _id    = r._id;
            _ttcscpy(_name,r._name);
        }
        strcpy(_catname,r._catname);
        _item       = r._item    ;
        _props      = r._props   ;
        _flags      = r._flags   ;
        _selected   = r._selected;
        _colorD     = r._colorD  ;
        _zonearea   = r._zonearea;
        _t          = r._t       ;
        _s          = r._s       ;
        _euler      = r._euler   ;
        _tl         = r._tl      ;
        _sl         = r._sl      ;
        _dirl       = r._dirl    ;
        _group      = r._group   ;
        _leafIdx    = r._leafIdx ;
        _combine    = r._combine;
        _textures   = r._textures;

    }
    return *this;
}


void    SceItem::GlDraw2D(View2D* pV)
{
    char ax = pV->_vt;

    if(_flags & BRSH_NEW)
        glColor4ubv(CLR_NEW_BRITEM);
    else
    {
        if(!IsSelected())
            glColor4ub(_colorD.r, _colorD.g, _colorD.b, 255);
        else
            glColor4ubv(CLR_SELECT);
    }

    glPushMatrix();
    glTranslatef(_t.x,_t.y,_t.z);
    char s[32];

    if(_item == ITM_GAME)
    {
        SCopy(s, (char*)_catname, sizeof(s)-1);
        if(!IsSelected())
            s[3]=0;
    }
    else
    {
        _ttcscpy(s, IS[_item].text);
    }


    if(_flags & BRSH_NEW)
    {
        u_rgba_t color(CLR_NEW_BRITEM[0],CLR_NEW_BRITEM[1],CLR_NEW_BRITEM[2]);
        VIEW3D(0,0)->PrintOut(V0, pV->_vt, "Hit Enter To Commit", color);
    }
    else
    {
        if(Pd->_selmode== SEL_NONE)
            VIEW3D(0,0)->PrintOut(V0, pV->_vt, ".", _colorD);
        else
            VIEW3D(0,0)->PrintOut(V0, pV->_vt, s, _colorD);
    }

    glPopMatrix();

    if(!IsSelected())
        glColor4ub(_colorD.r/3, _colorD.g/3, _colorD.b/3,255);
    else
        glColor4ubv(CLR_WHITE);


}


SceItem::~SceItem()
{
}

Htex& SceItem::GetTexture()
{
    Htex& rv = GetHtex(0);
     if(rv) return rv;
    return  _itmqtex;
}


void SceItem::GlDraw(View3D* pV)
{
    int32_t i;
    u_rgba_t cd  = _colorD;

    cd.a    = 254;
    glColor4ubv(cd);
    f32_t modelview[16];

    glPushMatrix();
    glEnable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glDepthMask(GL_FALSE);
    glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);

    Ta.Bind(GetTexture(), 0);
    glTranslatef(_t.x,_t.y,_t.z);

    glGetFloatv(GL_MODELVIEW_MATRIX , modelview);
    v3d_t xUp(modelview[1],modelview[5],modelview[9]);
    for( i=0; i<3; i++ ){
        for(int32_t j=0; j<3; j++ ){
            if ( i==j )
               modelview[i*4+j] = 1;
            else
                modelview[i*4+j] = 0.0f;
        }
    }
    glLoadMatrixf(modelview);

    glBegin(GL_POLYGON);

    for(i=0;i<4;i++)
    {
        glTexCoord2f(_Vtci[i]._uv[GUtex].u, _Vtci[i]._uv[GUtex].v);
        glVertex3f(_Vtci[i]._xyz.x, _Vtci[i]._xyz.y, _Vtci[i]._xyz.z);
    }

    glEnd();

    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glPopMatrix();


}

void SceItem::GlDrawSel(View3D* pV)
{
    int32_t i;
    f32_t modelview[16];
    glPushMatrix();
    glTranslatef(_t.x,_t.y,_t.z);

    Ta.Disable();
    glGetFloatv(GL_MODELVIEW_MATRIX , modelview);
    for( i=0; i<3; i++ ){
        for(int32_t j=0; j<3; j++ ){
            if ( i==j )
               modelview[i*4+j] = 1.2f; //scale a bit
            else
                modelview[i*4+j] = 0.0f;
        }
    }
    glLoadMatrixf(modelview);

    glBegin(GL_LINE_LOOP);
    for( i=0;i<4;i++)
        glVertex3f(_Vtci[i]._xyz.x, _Vtci[i]._xyz.y, _Vtci[i]._xyz.z);
    glEnd();
    Ta.Restore();
    glPopMatrix();

    u_rgba_t sc = _colorD;
    _colorD = ZWHITE;
    GlDraw(pV);
    _colorD = sc;

    Pd->DrawOrtho(_t, V0, 40.0, .64);
    Pd->DrawOrtho(_t, _euler, 64, 1);

}

void    SceItem::Rotate(v3d_t& r, u_bool32 bao)
{
    if(r.x== -1 && r.y==-1 && r.z==-1)
    {
        _euler.reset();
        _m.identity();
    }
    else
    {
        _euler += r;

        ROLL2PI(_euler.y);
        ROLL2PI(_euler.x);
        ROLL2PI(_euler.z);

        _m.identity();
        if(_euler.y)  _m  *= MRotAround(VY, _euler.y);
        if(_euler.x)  _m  *= MRotAround(VX, _euler.x);
        if(_euler.z)  _m  *= MRotAround(VZ, _euler.z);
    }
}


/*static */
SceItem* SceItem::CreateSci(int32_t type)
{
    switch(type)
    {
        case ITM_LIGTBULB:
            return new CLightBulb();
        case ITM_TRIGER:
            return new TriggerItem();
        case ITM_GAME:
            return new GameItem();
        case ITM_SNDSOURCE:
            return new CSoundItem();
        case ITM_STARTPLAY:
            return new CStartPlayItem();
        default:
            break;
    }
    return 0;
}

void SceItem::Serialize(FileWrap* pfw,
                        const vvector<Texture>& texNames,
                        map<int32_t,int32_t>& texassoc)
{
    pfw->Serialize(_item);
    _catname[15] = 0;
    pfw->Serialize(_catname,16);
    _catname[15] = 0;
    _name[15] = 0;
    pfw->Serialize(_name,16);
    _name[15] = 0;
    pfw->Serialize(_id);
    pfw->Serialize(_props);
    pfw->Serialize(_flags);
    pfw->Serialize(_selected);
    pfw->Serialize(_colorD);
    pfw->Serialize(_zonearea);
    pfw->Serialize(_t);
    pfw->Serialize(_s);
    pfw->Serialize(_euler);
    pfw->Serialize(_group);

    if(pfw->_store)
    {
        u_int32_t dwTmp;
        for(int32_t k=0;k<4;k++)
        {
            map<int32_t, int32_t>::iterator fi = texassoc.find((int32_t)this->GetHtex(k));
            if(fi != texassoc.end())
            {
                pfw->Write((int32_t&) texassoc[this->GetHtex(k)]);
            }
            else
            {
                int32_t  it = -1;
                pfw->Write((int32_t&) it);
            }
            dwTmp = this->Apply(k);
            pfw->Write(dwTmp);
        }
        dwTmp = this->Combine();
        pfw->Write(dwTmp);
    }
    else
    {
        int32_t      tmpVal;
        u_int32_t    dwTmpVal;

        for(int32_t k=0; k<4; k++)
        {
            pfw->Read(tmpVal);
            pfw->Read(dwTmpVal);

            if(tmpVal >=0 && (u_int32_t)tmpVal < texNames.size())
            {
                this->SetApply(dwTmpVal,k);
                this->SetTex((char*)texNames[tmpVal].filename, k, texNames[tmpVal].hTex.glTarget);

                if(this->GetHtex(k) <= 0 || this->GetHtex(k)==_dumptex)
                {
                    theApp->_errors.push_back(texNames[tmpVal].filename);
                }
            }
        }
        u_int32_t combine;
        pfw->Read(combine);//this->_combine);
        this->SetCombine(combine);
        SetPos(_t);
        SceItem::_GItemID = tmax(SceItem::_GItemID,(int32_t)_id);
    }

}




