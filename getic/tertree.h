#ifndef __TER_TREE_H__
#define __TER_TREE_H__

#include <baselib.h>
#include "texadapter.h"

//---------------------------------------------------------------------------------------
class NO_VT TerNode
{
public:
    TerNode(){}
    ~TerNode(){}

    int32_t     n_idx;
    int32_t     l_idx;
    Box     b_box;
    int32_t     i_nodes[2];   // if index is - is an index in leafs struct
    int32_t     i_parentNode;
    u_int32_t   x_flags;    // x_ for dword types
    int32_t     d_split;    // direction split 0 on x 1 on (z/y)
};

#define LEAF_LIT    0x1
//---------------------------------------------------------------------------------------
class NO_VT TerLeaf
{
public:
    TerLeaf(){}
    ~TerLeaf();//{theApp->_TexSys.RemoveTexture(lm_Tex);}
    SIZE            s_tiles[2];     // start tile, end tile
    LmInfo          lm_info;        // in ttree lmapbuffer
    Htex            lm_Tex;         // lm texture
    Box             b_box;          // leaf box
    int32_t             n_idx;          // node index
    int32_t             t_idx;          // leaf index
    u_int32_t           x_flags;
};

//---------------------------------------------------------------------------------------
class CBigTerrain;
class TerTree
{
public:
    TerTree();
    virtual ~TerTree();

    virtual void	Clear();
    u_bool32            Compile(CBigTerrain* pbt);
    u_bool32            Exist(){return p_leafs.size();}
    void            Render(u_bool32 d3);
    u_bool32            SegmentIntersect(const v3d_t& a, const v3d_t& b);
protected:
    void _RenderLeaf(const TerLeaf* pLeaf, int32_t blm, int32_t tilemode, u_bool32 d3, u_rgba_t& color, int32_t clrdiv=1);
    void _R_Render(const v3d_t& pos, int32_t node, u_bool32 d3);
    int32_t  _R_Compile(TerNode  *pParentNode,
                          const SIZE& startSz,
                          const SIZE& tiles,
                          const v3d_t& ext,
                          const v3d_t& corner, int32_t& depth);


public:
    vvector<f32_t>       v_heights;
    vvector<TerLeaf*>   p_leafs;
    vvector<TerNode*>   p_nodes;
    vvector<CLRNOA>     g_light;        // gouraoud light
    static TerTree*     p_TerrTree;
    static CBigTerrain* p_Terrain;
    u_int32_t               x_flags;
};


#endif // !__TER_TREE_H__



/*

*/

