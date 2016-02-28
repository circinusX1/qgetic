// Scene.h: interface for the Scene class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(SCENE_H_)
#define SCENE_H_

#include "stdafx.h"
#include <bspfilestr.h>
#include "camera.h"
#include "poly.h"
#include "brush.h"
#include "texsys.h"
#include "sceitem.h"
#include "skydom.h"
#include "scriptitem.h"
#include "motion.h"
#include "custitemcat.h"    /// item categories
#include "bigterrain.h"
#include "sounditem.h"



//---------------------------------------------------------------------------------------
class Scene  : public SceItem
{
public:
    Scene(){
        Initialize();
    }

    void Initialize()
    {
        strcpy(_si.author,"John Doe");
        strcpy(_si.comments,"Getic 2.3");
        _si.levelType = LEV_QUEST;
        _si.maxActors = 8;
        _si.maxFrags  = 100;
        _si.maxTime   = 15;
        _si.defLumin  = u_rgba_t(128,128,128);
        _si.gravAcc   = 9.81;
        _si.camFar    = 65535;
        _si.camFov    = 64.0;
        _si.sceSpeed  = 2.0;
        _si.sceFlags  = 0;
        _si.fogNear   = 0;
        _si.fogFar     = 0;
        _si.fogColor   = ZWHITE;
        _si.fogDens    = 0;
        _si.fogFallOff =0;

        _ttcscpy(_name, "scene");
        //_si.camOrient1
        //_si.camOrient2

    };
    virtual ~Scene(){
        Clear();
    }

    void Clear(){
        // items first. they may release owned brushes
        Initialize();
        ClearItems();
        ClearBrushes();
        ClearMotions();
        ClearScripts();
        ClearSounds();
        ClearCats();
        ClearPrimitives();
        _terrain.Clear();
        _descs.clear();
        _errstring="";
    }
    void ClearScripts(){_scripts.deleteelements();}
    void ClearSounds(){_sounds.deleteelements();}
    void ClearItems(){_items.deleteelements();}
    void ClearMotions(){_motions.deleteelements();}
    void ClearPrimitives();
    void ClearBrushes();
    int32_t	 GetPolyCnt();
    PBrushes*   GetPrimitives(){return &_primitives;};
    void        AddBrush(Brush* pB){_brushes <<(pB);}
    PBrushes*   GetBrushes(){return &_brushes;};
    void        AddPrimitive(Brush* pB){pB->_brushflags &= ~BRSH_NEW;_primitives << (pB);}
    Brush*      GetPrimitive(int32_t brSel){if(brSel < (int32_t)_primitives.size()) return _primitives[brSel];return 0;}
    Brush*      GetBrush(int32_t brSel){if(brSel < (int32_t)_brushes.size()) return _brushes[brSel];return 0;}
    Brush*      GetBrushByID(int32_t brushID){FOREACH(PBrushes, _primitives, ppb){if((*ppb)->_unicID == brushID){return *ppb;}}return 0;}
    int32_t         AddItem(SceItem* pi){int32_t sz = _items.size();_items << pi;return sz;}
    void        DelItem(SceItem* pi){
        vvector<SceItem*>::iterator fi = _items.findelement(pi);
        if(fi != _items.end()){_items.erase(fi);delete pi;}
    }
    SceItem*  GetItem(int32_t idx){return (idx < (int32_t)_items.size()) ? _items[idx] : 0;}
    vvector<SceItem*>*   GetItems(){return &_items;}
    SceItem*  GetItemByID(int32_t itemID){
        FOREACH(vvector<SceItem*>, _items, ppi){
            if((*ppi)->_id == itemID){
                return *ppi;
            }
        }
        return 0;
    }

   int32_t GetItemsByType(SceItem** ppArray, u_int32_t type, int32_t imax)
   {
       Q_UNUSED(type);
       int32_t retval = 0;
        FOREACH(vvector<SceItem*>, _items, ppi){
            if((*ppi)->_item==ITM_LIGTBULB){
                ppArray[retval++] = (*ppi);
                if(retval >= imax)
                    break;
            }
        }
        return retval;
   }
/*
   SceItem*  GetItemByPtr(u_int32_t itemPtr){
        FOREACH(vvector<SceItem*>, _items, ppi){
            if((u_int32_t)(*ppi) == itemPtr){
                return *ppi;
            }
        }
        return 0;
    }
*/

   int32_t    GetIndex(SceItem* pi)
   {
       return _items.getindex(pi);
   }

// motions
    int32_t	GetIndex(CMotion* pi)
    {
        return _motions.getindex(pi);
    }


    int32_t AddMotion(CMotion* pi)
    {
        int32_t sz = _items.size();
        _motions << pi;
        return sz;
    }

    void DelMotion(CMotion* pi){
        vvector<CMotion*>::iterator fi = _motions.findelement(pi);
        if(fi != _motions.end())
        {
            delete pi;
            _motions.erase(fi);
        }
    }
    vvector<CMotion*>*   GetMotions(){return &_motions;}
    int32_t AddScript(ScriptItem* pi)
    {
        int32_t sz = _scripts.size();
        _scripts << pi;
        return sz;
    }

    void DelScript(ScriptItem* pi)
    {
        vvector<ScriptItem*>::iterator fi = _scripts.findelement(pi);
        if(fi != _scripts.end())
        {
            delete pi;
            _scripts.erase(fi);
        }
    }

    ScriptItem*   GetScript(int32_t index){
        return _scripts[index];
    }


    vvector<ScriptItem*>*   GetScripts(){
        return &_scripts;
    }

   int32_t    GetIndex(ScriptItem* pi){
        vvector<ScriptItem*>::iterator fi = _scripts.findelement(pi);
        if(fi != _scripts.end())
        {
            return fi-_scripts.begin();
        }
        return -1;
    }

    int32_t FindSoundByName(const char* psz);

    int32_t AddSound(CSoundItem* pi)
    {
        int32_t sz = _sounds.size();
        _sounds << pi;
        return sz;
    }

    void DelSound(CSoundItem* pi)
    {
        vvector<CSoundItem*>::iterator fi = _sounds.findelement(pi);
        if(fi != _sounds.end())
        {
            delete pi;
            _sounds.erase(fi);
        }
    }

    vvector<CSoundItem*>*   GetSounds(){
        return &_sounds;
    }

   int32_t    GetIndex(CSoundItem* ps){
        vvector<CSoundItem*>::iterator fi = _sounds.findelement(ps);
        if(fi != _sounds.end())
        {
            return fi-_sounds.begin();
        }
        return -1;
    }

    CSoundItem*   GetSound(int32_t idx){
        return _sounds[idx];
    }

// item cathegories managemenet (per game)
    u_bool32    HasCats(){return _gamecats.size()!=0;}
    u_bool32    SearchCats(const char* fileName);
    void    ClearCats();
    vvector<ItemCat>* GetCats(){return &_gamecats;}
    ItemCat* GetCat(const char* catname)
    {
        FOREACH(vvector<ItemCat>,_gamecats,pc)
        {
            if(!strcmp(pc->_catname,catname)){
                return &(*pc);
            }
        }
        return 0;
    }

    CBigTerrain&    Terrain(){return _terrain;}
    map<string ,string>& Descriptions(){ return  _descs;}
    void SetCam(const v3d_t& pos, const v3d_t& elr){_campos=pos; _cameuler=elr;};
private:

    u_bool32    ParseSection(string& errors, vvector<char*>& classes, vvector<char*>& items);


private:
    vvector<Brush*>			_primitives;
    vvector<Brush*>			_brushes;
    vvector<SceItem*>		_items;
    vvector<CMotion*>		_motions;
    vvector<ScriptItem*>	_scripts;
    vvector<CSoundItem*>    _sounds;
    vvector<ItemCat>        _gamecats;
    CBigTerrain             _terrain;
    map<string ,string>     _descs;
    string                  _errstring;
public:
    BST_SceneInfo			_si;
    SkyDom					_dom;
    int32_t                     _GCarving;
    int32_t						_dummy[3];
    v3d_t                      _campos;
    v3d_t                      _cameuler;
};


#endif // SCENE_H_

