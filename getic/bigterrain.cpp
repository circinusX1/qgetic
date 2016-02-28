// BigTerrain.cpp: implementation of the CBigTerrain class.
//
//---------------------------------------------------------------------------------------

#include "stdafx.h"
#include "geticapp.h"
#include "bigterrain.h"
#include "view3d.h"
#include "view2d.h"
#include "document.h"
#include "texadapter.h"
#include "texsys.h"
#include "texref.h"

//---------------------------------------------------------------------------------------
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//---------------------------------------------------------------------------------------
void CBigTerrain::Serialize(FileWrap* pfv,
                            const vvector<Texture>& texNames,
                            map<int32_t,int32_t>& texassoc)
{

    pfv->Serialize(n_xtiles);
    pfv->Serialize(n_ztiles);
    pfv->Serialize(b_box);
    pfv->Serialize(n_stage);
    pfv->Serialize(t_anim[0]);
    pfv->Serialize(t_anim[1]);

    pfv->Serialize(m_dummy[0]);
    pfv->Serialize(m_dummy[1]);
    pfv->Serialize(m_dummy[2]);
    pfv->Serialize(m_dummy[3]);
    pfv->Serialize(m_dummy[4]);
    pfv->Serialize(m_dummy[5]);
    pfv->Serialize(m_dummy[6]);
    pfv->Serialize(m_dummy[7]);

    if(pfv->_store)
    {
        int32_t nVxes = v_vxes.Size();
        pfv->Write(nVxes);
        for(int32_t i=0;i<nVxes;i++)
        {
            pfv->Write(v_vxes[i]);
        }
    }
    else
    {
        u_int8_t hm;
        int32_t nVxes;
        pfv->Read(nVxes);
        v_vxes.ObjReserve(nVxes);
        for(int32_t i=0; i<nVxes ;i++)
        {
            pfv->Read(hm);
            v_vxes[i] = hm;
        }
    }
}

//---------------------------------------------------------------------------------------
void  CBigTerrain::Render(u_bool32 d3)
{
    if(!n_xtiles || 0==p_dummyBrush) return;
    v3d_t              ex = b_box.GetExtends();
    f32_t            xS = (ex.x) / (f32_t)n_xtiles;
    f32_t            zS = (ex.z) / (f32_t)n_ztiles;
    v3d_t              vWalk1,vWalk2, vmin = b_box._min;
    u_int8_t            lc[4];
    v2d_t              tc0,tc1;
    u_int32_t           texAstage = 1<<GUtex;
    u_bool32            untt[4] = {Flags()&TERR_UTT0,
                               Flags()&TERR_UTT1,
                               Flags()&TERR_UTT2,
                               Flags()&TERR_UTT3};


    if(p_dummyBrush->IsSelected())
        glColor4ubv(CLR_WHITE);
    else
    {
        if(d3==0)
            glColor4ubv(CLR_TERRAIN);
        else
            glColor4ubv(CLR_NORMAL);
    }

    if(d3)
    {
        Ta.Reset();
        Ta.Bind(Texs(), GUtex);
    }

    for(int32_t x=0 ; x < n_xtiles; x++)
    {
        if(d3)
        {
            glBegin(GL_TRIANGLE_STRIP);
        }
        else
            glBegin(GL_LINE_STRIP);

        vWalk1.x = vmin.x + x * xS;
        vWalk2.x = vmin.x + (x+1) * xS;

        for(int32_t z=0 ; z < n_ztiles; z++)
        {

            vWalk1.z = vmin.z + z * zS;
            vWalk1.y = *(element(&v_vxes, x, z, n_xtiles));
            vWalk1.y /= 255.0;
            vWalk1.y *= ex.y;
            vWalk1.y += vmin.y;

            vWalk2.z = vWalk1.z;
            vWalk2.y = *(element(&v_vxes, (x+1), z, n_xtiles));
            vWalk2.y /= 255.0;
            vWalk2.y *= ex.y;
            vWalk2.y += vmin.y;

            if(GCullTerrCell && Pd->_cam.IsCulled(&vWalk1) && Pd->_cam.IsCulled(&vWalk2))
            {
                continue;
            }

            if(d3==0 && !p_dummyBrush->IsSelected())
            {
                lc[0] = CLR_TERRAIN[0] + ((vWalk1.y-b_box._min.z) / b_box.GetExtends().z * 64);
                lc[1] = CLR_TERRAIN[1] + ((vWalk1.y-b_box._min.z) / b_box.GetExtends().z * 64);
                lc[2] = CLR_TERRAIN[2] + ((vWalk1.y-b_box._min.z) / b_box.GetExtends().z * 64);
                lc[3] = CLR_TERRAIN[3];
                glColor4ubv(lc);
            }

            if(untt[GUtex])
            {
                tc0.setval((vWalk1.x-vmin.x)/ex.x, 1-(vWalk1.z-vmin.z)/ex.z);
            }
            else
            {
                if((z&1)==0)
                    tc0.setval(0,0);
                else
                    tc0.setval(0,1);
            }

            if(untt[GUtex])
            {
                tc1.setval((vWalk2.x-vmin.x)/ex.x, 1-(vWalk2.z-vmin.z)/ex.z);
            }
            else
            {
            if((z&1)==0)
                tc1.setval(1,0);
            else
                tc1.setval(1,1);
            }
            if(d3 && (n_stage & texAstage))
            {
                AnimateSurfaceTexture(tc0, tc1);
            }

            glTexCoord2fv((f32_t*)tc0);
            glVertex3fv((f32_t*)vWalk1);
            glTexCoord2fv((f32_t*)tc1);
            glVertex3fv((f32_t*)vWalk2);

        }
        glEnd();
    }
}

//---------------------------------------------------------------------------------------
void CBigTerrain::AnimateSurfaceTexture(v2d_t& tc0, v2d_t& tc1)
{
    u_int32_t animbits = n_stage & 0xF0000000;
    if(animbits == TERR_TA_SHIFT)
    {
        v2d_t depl(t_anim[0].u * PaintCounter, t_anim[0].v * PaintCounter);
        tc0 += depl;
        tc1 += depl;
    }
    else if(animbits == TERR_TA_OSC) //trmo
    {
        v2d_t depl(t_anim[0].u*Sinr((f32_t)PaintCounter/16.0), t_anim[0].v*Cosr((f32_t)PaintCounter/16.0));
        tc0 += depl;
        tc1 += depl;
    }
}




//---------------------------------------------------------------------------------------
u_bool32  CBigTerrain::Create(const u_int8_t* hmap, const v3d_t& start, const v3d_t& size,
                          int32_t xt, int32_t yt)
{
    Clear(1);

    p_dummyBrush  = new Brush();
    p_dummyBrush->MakeCube((v3d_t&)size);
    p_dummyBrush->Recalc();
    p_dummyBrush->_brushflags    = BRSH_BIGTERRAIN|TERR_GOR|TERR_UTT0;
    p_dummyBrush->_pUsrData = this;
    p_dummyBrush->Reverse();
    p_dummyBrush->_pUsrData=this;
    strcpy(p_dummyBrush->_name,"terrain");


    n_xtiles     = xt;
    n_ztiles     = yt;
    b_box._min   = start;
    b_box._max   = start + size;
    u_int8_t* pwhmap = (u_int8_t*)hmap;
    int32_t k=0;

    u_int8_t  bmin=255;
    u_int8_t  bmax=0;

    v_vxes.ObjReserve((n_xtiles+1)*(n_ztiles+1));
    for(int32_t x=0 ; x <= n_xtiles; x++)
    {
        for(int32_t z=0 ; z <= n_ztiles; z++)
        {
            bmin = tmin(*pwhmap,bmin);
            bmax = tmax(*pwhmap,bmax);
            v_vxes[k++] = *pwhmap++;
        }
    }
    return 1;
}

//---------------------------------------------------------------------------------------
void  CBigTerrain::Clear(int32_t delB)
{
    if(delB && p_dummyBrush)
        Pd->DeleteBrush(p_dummyBrush,0);
    p_dummyBrush = 0;
    v_vxes.Destroy();
    n_xtiles=0;
    n_ztiles=0;
}

//---------------------------------------------------------------------------------------
CBigTerrain& CBigTerrain::operator = (const CBigTerrain& cbt)
{
    if(&cbt != this)
    {
        Clear(1);
        if( ((CBigTerrain&)cbt).Exist() )
        {
            p_dummyBrush = new Brush(*cbt.p_dummyBrush);
            p_dummyBrush->_pUsrData=this;
        }
        v_vxes       = cbt.v_vxes;
        n_xtiles     = cbt.n_xtiles;
        n_ztiles     = cbt.n_ztiles;
        b_box        = cbt.b_box;
        n_stage      = cbt.n_stage;
        t_anim[0]    = cbt.t_anim[0];
        t_anim[1]    = cbt.t_anim[1];
        memcpy(m_dummy,cbt.m_dummy,sizeof(m_dummy));
        _clearing=0;
    }
    return *this;
}

//---------------------------------------------------------------------------------------
CBigTerrain::CBigTerrain(const CBigTerrain& cbt )
{
    if( ((CBigTerrain&)cbt).Exist())
    {
        p_dummyBrush = new Brush(*cbt.p_dummyBrush);
        p_dummyBrush->_pUsrData=this;
    }
    v_vxes       = cbt.v_vxes;
    n_xtiles     = cbt.n_xtiles;
    n_ztiles     = cbt.n_ztiles;
    b_box        = cbt.b_box;
    n_stage      = cbt.n_stage;
    t_anim[0]    = cbt.t_anim[0];
    t_anim[1]    = cbt.t_anim[1];
    memcpy(m_dummy,cbt.m_dummy,sizeof(m_dummy));
    _clearing = 0;
}


/*
    anim textures u and v is shift
*/
