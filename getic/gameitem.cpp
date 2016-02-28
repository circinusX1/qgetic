// GameItem.cpp: implementation of the GameItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gameitem.h"
#include "view2d.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GameItem::GameItem(const char* catname)
{
    _item       = ITM_GAME;
    strcpy(_catname    , catname);
}

GameItem::~GameItem()
{

}

void    GameItem::InitCommit()
{
    assert(0);
    //theApp->_TexSys.LoadRCTexture("ITM_CUSTOM",16,16);
    //this->SetTex("ITM_CUSTOM");
}

void    GameItem::Commit()
{
}

GameItem::GameItem(const GameItem& r):SceItem((const SceItem&) r)
{
    _data = r._data;
}

GameItem& GameItem::operator=(const GameItem& r)
{
    if(this != &r)
    {
        this->SceItem::operator=((const SceItem&)(r));
        _data = r._data;
    }
    return *this;
}


void    GameItem::GlDraw2D(View2D* pV)
{
    int32_t i;
    char ax = pV->_vt;
    SceItem::GlDraw2D(pV);

    glPushMatrix();

    glTranslatef(_t.x,_t.y,_t.z);

    switch(ax)
    {
        case 'x':
            glRotatef(-90,0,1,0);
            break;
        case 'y':
            glRotatef(-90,1,0,0);
            break;
        case 'z':
            break;
    }


    f32_t step = 0;
    // draw a circle from 30 x 30 degreees on xoy xoz zoy
    v3d_t  cirpctX[3];
    v3d_t  cirpctY[3];
    v3d_t  cirpctZ[3];

    for( i=0;i<3;i++)
    {
        cirpctX[i].z = 64 * sinf(step);
        cirpctX[i].y = 64 * cosf(step);

        cirpctY[i].x = 64 * sinf(step);
        cirpctY[i].z = 64 * cosf(step);

        cirpctZ[i].x = 64 * sinf(step);
        cirpctZ[i].y = 64 * cosf(step);

        step+=(PIPE6*4);
    }
    if(IsSelected())
    {
        glLineWidth(2.0f) ;
    }

    glBegin(GL_LINE_LOOP);
    for(i=0;i<3;i++)
        glVertex3f(cirpctZ[i].x, cirpctZ[i].y, cirpctZ[i].z);
    glEnd();

    if(IsSelected())
    {
        glLineWidth(1.0f) ;
    }
    if(IsSelected())
    {
        glBegin(GL_LINES);
        for(i=0;i<3;i++)
        {
            glVertex3fv((f32_t*)V0);
            glVertex3f(cirpctZ[i].x, cirpctZ[i].y, cirpctZ[i].z);
        }
        glEnd();
    }

    glPopMatrix();
}


void GameItem::Serialize(FileWrap* pfw,
                         const vvector<Texture>& texNames,
                         map<int32_t,int32_t>& texassoc)
{
    SceItem::Serialize(pfw, texNames, texassoc);

    if(pfw->_store)
    {
        int32_t       nCount = _data.size();
        pfw->Write(nCount);
        FOREACH(vvector<NameTypeValue>, _data, pdata)
        {
            pfw->Write(*pdata);
        }
    }
    else
    {
        NameTypeValue   ntv;
        int32_t             nCount = 0;

        pfw->Read(nCount);
        for(int32_t i=0; i < nCount; i++)
        {
            pfw->Read(ntv);
            _data.push_back(ntv);
        }
    }
}
