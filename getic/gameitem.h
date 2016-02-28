#if !defined(AFX_GAMEITEM_H__3B6BEA24_0EAE_4DFD_BA81_9068ACD7F347__INCLUDED_)
#define AFX_GAMEITEM_H__3B6BEA24_0EAE_4DFD_BA81_9068ACD7F347__INCLUDED_


#include "sceitem.h"
#include "custitemcat.h"
#include <string>
#include <map>

using namespace std;
class GameItem : public SceItem
{
public:
    GameItem(){_item    = ITM_GAME;};
    GameItem(const char* catname);
    GameItem(const GameItem& r);
    GameItem& operator=(const GameItem& r);

    OVERWRITE void Serialize(FileWrap* pfv, const vvector<Texture>& texNames, map<int32_t,int32_t>& texassoc);

    void InitCommit();

    void    GlDraw2D(View2D* pV);

    virtual ~GameItem();
    void    Commit();

    vvector<NameTypeValue>    _data;

};

#endif // !defined(AFX_GAMEITEM_H__3B6BEA24_0EAE_4DFD_BA81_9068ACD7F347__INCLUDED_)
