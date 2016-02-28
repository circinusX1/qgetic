// BigTerrain.h: interface for the CBigTerrain class.
//
//////////////////////////////////////////////////////////////////////

#ifndef BIG_TERRIAN
#define BIG_TERRIAN


#include "stdafx.h"     // for GBackface
#include <bspfilestr.h>
#include "baseutils.h"
#include "texref.h"
#include "brush.h"
#include "texadapter.h"
#include "baseentity.h"

class View3D;
class View2D;
class CBigTerrain
{
public:
    CBigTerrain():n_xtiles(0),p_dummyBrush(0),n_stage(0),_clearing(0){ };
    CBigTerrain(const CBigTerrain& cbt);
    virtual ~CBigTerrain(){Clear(1);};
    void  Serialize(FileWrap* pfv, const vvector<Texture>& texNames, map<int32_t,int32_t>& texassoc);
    void  Clear(int32_t delB=0); //{v_vxes.clear();n_xtiles=0;n_ztiles=0;}
    void  Render(u_bool32 d3=false);

    void  Destroy(){Clear();}
    CBigTerrain& operator=(const CBigTerrain& cbt);
    u_int8_t* Vertextex(int32_t& ncount){ncount = v_vxes.Size(); return &v_vxes;}
    u_bool32  Exist() {return v_vxes.Size();}
    u_bool32  Create(const u_int8_t* hmap, const v3d_t& start, const v3d_t& size, int32_t xt, int32_t yt);
    Texs&   GetTexs(){
        assert(p_dummyBrush);
        return p_dummyBrush->_polys[0].GetTexs();}
    u_int32_t&  Flags(){assert(p_dummyBrush); return  p_dummyBrush->_brushflags;}
    void AnimateSurfaceTexture(v2d_t& tc0, v2d_t& tc1);

public:
    u_int32_t            n_xtiles;
    u_int32_t            n_ztiles;
    Box             b_box;

    RawBuff<u_int8_t>   v_vxes;
    Brush*          p_dummyBrush; // used to resize and move the terrain
    u_int32_t           n_stage;
    v2d_t              t_anim[2];
    u_int32_t           m_dummy[8];
    u_bool32            _clearing;
};

#endif // !BIG_TERRIAN
