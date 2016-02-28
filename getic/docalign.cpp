#include "stdafx.h"
#include <algorithm>
#include "document.h"
#include "view3d.h"
#include "view2d.h"
#include "geoutils.h"
#include "texsys.h"
#include "lightbulb.h"
#include "texadapter.h"

//---------------------------------------------------------------------------------------
u_bool32    GSuported=0;

//---------------------------------------------------------------------------------------
Box   Document::GetReferenceBrushBox()
{
    DeleteUncomittedNewBrushes();

    // do alignment by selection box
    _selBox.Recalc();

    if(IsSelBoxOn())
        return _selBox._box;

    if(_pSelBrushes.size())
        return _pSelBrushes.back()->_box;
    return _pSelItems.back()->GetBox();
}


//---------------------------------------------------------------------------------------
static Box GetSvlBrushesBox(PBrushes& selbrushes)
{
    Box box;
    FOREACH(PBrushes, selbrushes, ppb)
    {
        box.Union((*ppb)->_box);
    }
    return box;
}

//---------------------------------------------------------------------------------------
static v3d_t    GetFilledSpace(PBrushes& selbrushes)
{
    v3d_t allDims;
    FOREACH(PBrushes, selbrushes, ppb)
    {
        allDims+=(*ppb)->_box.GetExtends();
    }
    return allDims;
}
//---------------------------------------------------------------------------------------
// sorts brush by x displacement
int32_t SortByX(const void* b1, const void* b2)
{
    f32_t displ = ((Brush*)b1)->_box.GetCenter().x-((Brush*)b2)->_box.GetCenter().x;
    if(displ > 0)
        return 1;
    else if(displ < 0)
        return -1;
    return 0;
}

//---------------------------------------------------------------------------------------
// sorts brush by x displacement
int32_t SortByY(const void* b1, const void* b2)
{
    f32_t displ = ((Brush*)b1)->_box.GetCenter().y-((Brush*)b2)->_box.GetCenter().y;
    if(displ > 0)
        return 1;
    else if(displ < 0)
        return -1;
    return 0;

}

//---------------------------------------------------------------------------------------
// sorts brush by x displacement
int32_t SortByZ(const void* b1, const void* b2)
{
    f32_t displ = ((Brush*)b1)->_box.GetCenter().z-((Brush*)b2)->_box.GetCenter().z;
    if(displ > 0)
        return 1;
    else if(displ < 0)
        return -1;
    return 0;
}

//---------------------------------------------------------------------------------------
void    Document::DeleteUncomittedNewBrushes()
{
    FOREACH(PBrushes,  _pSelBrushes, a)
    {
        if((*a)->_brushflags & BRSH_NEW)
        {
            delete *a;
            a = _pSelBrushes.erase(a);
            if(a != _pSelBrushes.begin())
                --a;
            if(_pSelBrushes.size()==0)
                break;
        }
    }
}


//---------------------------------------------------------------------------------------
void    Document::DeleteUncomittedNewItems()
{
    if(_pSelItems.size())
    {
        FOREACH(vvector<SceItem*>, _pSelItems, ppItem)
        {
            if((*ppItem)->_flags & BRSH_NEW)
            {
                delete (*ppItem);
                ppItem = _pSelItems.erase(ppItem);
                if(ppItem != _pSelItems.begin())
                    ppItem--;
                if(_pSelItems.size()==0)
                    break;
            }
        }
    }
}


void    Document::SelectByBox()
{
    _selBox.Recalc();
    DeselectAll();

    switch(_selmode)
    {
        case SEL_BRUSH:
            {
                PBrushes* pBSs = GetPrimitives();
                FOREACH(PBrushes, (*pBSs), ppB)
                {
                    if(_curgroup > 0 && (*ppB)->_group != _curgroup)
                        continue;

                    if(_selBoxByTouch)
                    {
                        if(_selBox._box.IsTouchesBox((*ppB)->_box))
                        {
                            SelectBrush(*ppB);
                        }
                    }
                    else
                    {
                        if(_selBox._box.ContainPoint((*ppB)->_box._min) &&
                           _selBox._box.ContainPoint((*ppB)->_box._max))
                        {
                            SelectBrush(*ppB);
                        }
                    }
                }
            }
            break;
        case SEL_ITEMS:
            {

                vvector<SceItem*>* pItmsPtr = Pd->GetItems();
                FOREACH(vvector<SceItem*>, (*pItmsPtr), ppItem)
                {
                    // the movement on that coord has ben blocked by X or Y key
                    if(_selBox._box.ContainPoint((*ppItem)->_t))
                    {
                        SelectItem((*ppItem));
                    }
                }
            }
            break;
        case SEL_FACE:
            {
                PBrushes* pBSs = GetPrimitives();
                FOREACH(PBrushes, (*pBSs), ppB)
                {
                    if(_curgroup > 0 && (*ppB)->_group != _curgroup)
                        continue;

                    vvector<Poly>::iterator pPly = (*ppB)->_polys.begin();
                    vvector<Poly>::iterator be = (*ppB)->_polys.end();
                    for(; pPly != be; ++pPly)
                    {
                        if(_selBoxByTouch)
                        {
                            if(_selBox._box.IsTouchesBox(pPly->_box))
                            {
                                SelectFace(&(*pPly));
                            }
                        }
                        else
                        {
                            if(_selBox._box.ContainPoint(pPly->_box._min) &&
                               _selBox._box.ContainPoint(pPly->_box._max))
                            {
                                SelectFace(&(*pPly));
                            }
                        }
                    }
                }
            }
            break;
        case SEL_VERTEX: //_selmode
            {
                DeselectVertexes();

                PBrushes* pBSs = GetPrimitives();
                FOREACH(PBrushes, (*pBSs), ppB)
                {
                    if(_curgroup > 0 && (*ppB)->_group != _curgroup)
                        continue;

                    vvector<Poly>::iterator pPly = (*ppB)->_polys.begin();
                    vvector<Poly>::iterator be = (*ppB)->_polys.end();
                    for(; pPly != be; ++pPly)
                    {
                        FOREACH(vvector<Vtx2>, pPly->_vtci, pV)
                        {
                            if(_selBox._box.ContainPoint(pV->_xyz) )
                            {
                                SelectVertex(&(*pV), &(*pPly));
                            }
                        }
                    }
                }
            }
            break;
    }

    Invalidate();
}

