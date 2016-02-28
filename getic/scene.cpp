// Scene.cpp: implementation of the Scene class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "winwrap.h"
#include "scene.h"
#include "view3d.h"
#include "custitemcat.h"
#include "sounditem.h"
#include "bigterrain.h"


int32_t	Scene::GetPolyCnt()
{
    int32_t polys = 0;

    FOREACH(PBrushes, _primitives, pp)
    {
        Brush& pB = (*pp)->GetResult();

        if(pB.IsSolid())
            polys+=pB._polys.size();
    }
    return polys;
}

int32_t Scene::FindSoundByName(const char* psz)
{
    int32_t index = 0;
    FOREACH(vvector<CSoundItem*>,_sounds, pps)
    {
        if((*pps)->_soundfile==psz)
        {
            return index;
        }
        ++index;
    }
    return -1;
}


int32_t StringBetween(char* str, char s, char e, char* out)
{
    int32_t i;
    char* po  = out;
    u_bool32  app = (s == 0) ? true : false; // from the begining

    for( i=0; str[i] != 0; i++)
    {
        if(str[i] == e && app)
            break;

        if(app==false && str[i] == s )
        {
            app = true;
            continue;
        }
        if(app)
        {
            *(out++)=str[i];
        }
    }
    *(out++)=0;
    return i;
}

void TrimBoth(char* slineprp)
{
    while(slineprp[0] && (slineprp[0] == ' ' || slineprp[0] == '\t'))
        strcpy(slineprp,slineprp+1);

    while(slineprp[0] &&(
          slineprp[strlen(slineprp)-1] == ' '  ||
          slineprp[strlen(slineprp)-1] == '\t' ||
          slineprp[strlen(slineprp)-1] == '\r' ||
          slineprp[strlen(slineprp)-1] == '\n'))
          slineprp[strlen(slineprp)-1] = 0;

}

u_bool32 ExtractEquals(char* slineprp,
                   vvector<string>& lefts,
                   vvector<string>& rights)
{
    char left[128] = {0};
    char right[128]= {0};

    int32_t leftlen     = 0;
    int32_t rightlen    = 0;
    u_bool32 leftdone = false;
    u_bool32 rightdone = false;
    int32_t  quote     = 0;
    u_int32_t    i;

    //test line
    if(0==strlen(slineprp))
    {
        return 0;
    }
    for(i=1;i < strlen(slineprp)-1;++i)
    {
        if(slineprp[i]=='=' &&
           (slineprp[i-1] ==' ' || slineprp[i-1] =='\t' ||
           slineprp[i+1] ==' ' || slineprp[i+1] =='\t'))
        {
            return false;
        }
    }

    for(i=0;i<strlen(slineprp);i++)
    {
        if(!leftdone)
        {
            if(slineprp[i]=='=')
            {
                leftdone = true;
                left[leftlen]=0;
            }
            else
                left[leftlen++]=slineprp[i];
            continue;
        }
        else if(!rightdone)
        {

            if(slineprp[i]=='"')
            {
                if(quote==0)
                    quote++;
                else
                {
                    rightdone = true;
                    right[rightlen]=0;
                }
                continue;
            }
            if(0==quote)
            {
                if(slineprp[i]==' ')
                {
                    rightdone = true;
                    right[rightlen]=0;
                }
            }
            right[rightlen++]=slineprp[i];
            continue;
        }
        lefts << left;
        rights << right;

        rightlen= 0;
        leftlen = 0;
        quote   = 0;
        rightdone = false;
        leftdone = false;
    }
    if(rightdone && leftdone)
    {
        lefts << left;
        rights << right;
    }
    return 1;
}

/*
Get_Item:class= Particles Generator
@int32_t="Sparks" value="32"
@u_rgba_t="Color" value="255,255,0"
@f32_t="Life Time" value="4"

Get_Class:name= Particles Generator
@FILENAME="Mesh File" value="~quad"
@v2d_t="velocity" value="3200,5600"  tmin=33333 tmax="asdf"
@f32_t="dispersion" value="45"
*/
int32_t ComaCount(const char* s)
{
    int32_t coma = 0;
    while(s = strchr(s,','))
    {
        ++s;
        ++coma;
    }
    return coma;
}

void NameTypeSetValue(int32_t mvM, NameTypeValue& n, string& val)
{
    Q_UNUSED(mvM);
    switch(n._type)
    {
        case V_TEXT:
        case V_FILENAME:
            {
                SCopy((char*)n._value,(char*)val.c_str(), sizeof(n._value));
            }
            break;
        case V_INT:
            {
                int32_t* pi = (int32_t*)n._value;
                *pi = atoi(val.c_str());
            }
            break;
        case V_REAL:
            {
                f32_t* r = (f32_t*)n._value;
                *r = atof(val.c_str());
            }
            break;
        case V_V3:
            {
                v3d_t* pv = (v3d_t*)n._value;
                ::sscanf(val.c_str(), "%f,%f,%f", &pv->x, &pv->y, &pv->z);
            }
            break;
        case V_UV:
            {
                v2d_t* pv = (v2d_t*)n._value;
                ::sscanf(val.c_str(), "%f,%f", &pv->u, &pv->v);
            }
            break;
        case V_DOUBLE:
            {
                double* pi = (double*)n._value;
                *pi = (double)atof(val.c_str());
            }
            break;
        case V_BYTE:
            {
                u_int8_t* pi = (u_int8_t*)n._value;
                *pi = (u_int8_t)atoi(val.c_str());
            }
            break;
        case V_CHAR:
            {
                char* pi = (char*)n._value;
                *pi = val[0];
            }
            break;
        case V_CLR:
            {
                int32_t    rgb[4] = {0};
                u_rgba_t*   pclr = (u_rgba_t*)n._value;
                int32_t cc = ComaCount(val.c_str());
                if(cc==3)
                    ::sscanf(val.c_str(), "%d,%d,%d,%d", &rgb[0],  &rgb[1], &rgb[2], &rgb[3]);
                else if (cc==2)
                {
                    ::sscanf(val.c_str(), "%d,%d,%d", &rgb[0],  &rgb[1], &rgb[2]);
                    rgb[3] = 255;
                }

                pclr->r = rgb[0]; pclr->g = rgb[1];
                pclr->b = rgb[2];  pclr->a = rgb[3];
            }
            break;
    }

}


u_bool32    Scene::ParseSection(string& errors,
                            vvector<char*>& classes,
                            vvector<char*>& items)
{
    int32_t         jumpover;
    char        slineprp[128];
    ItemCat     cat;
    ItemCat*     pCatExist;

    for(int32_t cc=0;cc<2;cc++)
    {
        vvector<char*>* pcoll = (cc==0) ? &classes : &items;

        FOREACH(vvector<char*>, (*pcoll), clsstring)
        {
            char* ph = *clsstring;
            jumpover = StringBetween(ph, '=', '@', slineprp);
            TrimBoth(slineprp);

            pCatExist = GetCat(slineprp);

            if(0 == pCatExist)
                pCatExist = &cat;

            SCopy(pCatExist->_catname, slineprp, sizeof(cat._catname));

            ph += jumpover;

            do{
                jumpover = StringBetween(ph, '@', '@', slineprp);
                ph       += jumpover;

                vvector<string> lefts;
                vvector<string> rights;

                if(!ExtractEquals(slineprp, lefts, rights))
                {
                    continue;
                }

                if(lefts.size() != rights.size())
                {
                    _errstring += slineprp;
                    continue;
                }
                //build the cat from equals
                // break types, tmin tmax and value
                vvector<string>::iterator il = lefts.begin();
                vvector<string>::iterator ir = rights.begin();
                int32_t idx = 0;

                NameTypeValue ntv;

                for(;il!=lefts.end();ir++,il++,idx++)
                {
                    string& key  = *il;
                    string&  val = *ir;

                    if(0 == idx)
                    {
                        ntv._type = FromSignature(key.c_str());
                        if(ntv._type >=0)
                        {
                            SCopy(ntv._name, (char*)val.c_str(), sizeof(ntv._name));
                            continue;
                        }
                        else
                        {
                            _errstring += slineprp;
                            return  0;
                        }

                    }
                    //SCopy(ntv._name , (char*)key.c_str(), sizeof(ntv._name));
                    if(key=="value")
                    {
                        NameTypeSetValue(0, ntv, val);
                    }
                    else if(key=="tmax")
                    {
                        NameTypeSetValue(1, ntv, val);
                    }
                    else if(key=="tmin")
                    {
                        NameTypeSetValue(-1, ntv, val);
                    }

                }
                if(cc == 0)
                    pCatExist->_classprops << ntv;
                else
                    pCatExist->_objprops << ntv;

            }while(*ph=='@');

            if( pCatExist == &cat)
                _gamecats << cat;
        }
    }
    return true;
}



u_bool32    Scene::SearchCats(const char* fileName)
{
    if(0==fileName)
        return false;

    if(fileName[0]=='\0')
        return false;

    u_bool32        rv=false;
    FileWrap    fw;
    char*       wholeFile   = 0;
    string      errorstr;
    vvector     <char*>     cats;
    vvector     <char*>     items;
    char		stacksaver[512];
    _TRY
    {
        stacksaver[0]=0;
        ClearCats();
        if(fw.Open(fileName,"rb"))
        {
            int32_t    lenFile = fw.Getlength();
            wholeFile   = new char[lenFile+1];
            if(!wholeFile)
            {
                fw.Close();
                return false;
            }
            fw.Read(wholeFile, lenFile);
            fw.Close();
            wholeFile[lenFile] = 0;

            char* pHead = wholeFile;



            while(pHead)
            {
                char* ps = strstr(pHead, "PROPERTIES");
                if(0==ps)
                    ps = strstr(pHead, "ATTRIBUTES");
                if(0==ps)
                    break;

                char* pe = strstr(pHead+7, "PROPERTIES");
                if(0==pe)
                    pe = strstr(pHead+7, "ATTRIBUTES");
                if(pe)
                {
                    *(pe-1)=0;
                }

                if(!::memcmp("PROPERTIES",pHead,8))
                {
                    string sh = string((char*)pHead);

                    if(find(items.begin(),items.end(),sh) == items.end())
                    {
                        items << (char*)pHead;
                    }
                }
                else
                if(!::memcmp("ATTRIBUTES",pHead,9))
                {
                    string sh = string((char*)pHead);
                    if(find(cats.begin(),cats.end(),sh) == cats.end())
                    {
                        cats<<(char*)pHead;
                    }
                }
                pHead = pe;
            }

            ParseSection(errorstr, cats, items);
        }
        rv=true;
        assert(stacksaver[0]==0);
    }
    _CATCHX()
    {
        TAfxMessageBox("Error parsing Custom Game Item Section", 0);
    }
    delete[] wholeFile;
    return rv;
}




void    Scene::ClearCats()
{
    _gamecats.clear();
}


void Scene::ClearPrimitives()
{
    FOREACH(PBrushes,_primitives, ppb)
    {
        if((*ppb)->_brushflags & BRSH_BIGTERRAIN)
        {
            _terrain.Clear();
        }
        delete (*ppb);
    }
    _primitives.clear();
}
void Scene::ClearBrushes()
{
    FOREACH(PBrushes,_brushes, ppb)
    {
        if(!((*ppb)->_brushflags & BRSH_REF))
            delete (*ppb);
    }
    _brushes.clear();
}

