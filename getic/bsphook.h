
#include "stdafx.h"
#include <baseutils.h>
#include <geticplug.h>
#include "geticplugbsp.h"


//---------------------------------------------------------------------------------------
class FlStream
{
public:
    FlStream():_plg("bspsave.dll"){};

    virtual ~FlStream(){
        _plg.Free();
    }

    u_bool32    Open(const char* file, const char* how)
    {
        if(_plg.IsValid())
        {
            _ok = _plg->aopen(file, how);
        }
        else
        {
            _pf = ::_tfopen(file, how);
            _ok = _pf != 0;
        }
        _store = (how[0]==_G('w'));
        return _ok;
    }
    void Close(){
        if(_plg.IsValid())
        {
            if(_ok)
                _plg->close();
        }
        else
        {
            if(_pf)
                ::fclose(_pf);
            _pf=0;
        }
    }

    void Pos(u_int32_t pos, u_int32_t cseek=SEEK_SET){
        if(_plg.IsValid())
        {
            _plg->seek(pos,(SEEK_POS)cseek);
        }
        else
        {
            ::fseek(_pf,pos,cseek);
        }
    }

    void ReStart(){
        if(_plg.IsValid())
        {
            _plg->seek(0,FSEEK_SET);
        }
        else
        {
            ::fseek(_pf,0,SEEK_SET);
        }
    }

    u_int32_t GetPos(){
        if(_plg.IsValid())
        {
            return _plg->tell();
        }
        return ::ftell(_pf);
    }

    u_int32_t Getlength(){
        if(_plg.IsValid())
        {
            _plg->seek(0,FSEEK_END);
            u_int32_t flength = ftell(_pf);
            _plg->seek(0,FSEEK_SET);
            return flength;
        }
        ::fseek(_pf,0,SEEK_END);
        u_int32_t flength = ftell(_pf);
        ::fseek(_pf,0,SEEK_SET);
        return flength;
    }

    u_bool32 ReadLine(char* line, int32_t len){
        if(_plg.IsValid())
        {
            return _plg->gets(line,len)!=0;
        }
        return ::fgets(line,len,_pf)!=0;
    }

    template <class T> u_bool32 Serialize(T& t){
        if(_store)
            return Write(t);
        return Read(t);
    }

    int32_t    IsEof(){
        if(_plg.IsValid())
        {
            return _plg->eof();
        }
        return feof(_pf);
    }

    template <class T> u_bool32 Write(T& t, const char* sinfo=0){
        if(_plg.IsValid())
        {
            if(sinfo)
                _plg->start_section(sinfo);
            int32_t lenr = _plg->write(&t,1,sizeof(T));
            return (lenr == sizeof(T));
        }
        int32_t lenr = ::fwrite(&t,1,sizeof(T),_pf);
        return (lenr == sizeof(T));
    }

    template <class T> u_bool32 Read(T& t) {
        if(_plg.IsValid())
        {
            int32_t lenr = _plg->read(&t,1,sizeof(T));
            return (lenr == sizeof(T));
        }
        int32_t lenr = ::fread(&t,1,sizeof(T),_pf);
        return (lenr == sizeof(T));
    }

    u_bool32 Read(void* pv, size_t len) {
        if(_plg.IsValid())
        {
            int32_t lenr = _plg->read(pv,1,len);
            return (lenr == lenr);
        }
        int32_t lenr = ::fread(pv,1,len,_pf);
        return (lenr == lenr);
    }

    u_bool32 Write(void* pb, size_t len) {
        if(_plg.IsValid())
        {
            return (len == _plg->write(pb,1,len));
        }
        return (len == ::fwrite(pb,1,len,_pf));
    }

    u_bool32 Serialize(void* pv, int32_t len){
        if(_store) return Write(pv,len);
        return Read(pv,len);
    }

    int32_t WriteTextAsIs(const char* t){
        if(_plg.IsValid())
        {
            int16_t l = _ttcslen(t);
            Q_UNUSED(l);
            return _plg->write(t, 1, _ttcslen(t));
        }
        int16_t l = _ttcslen(t);
        return fwrite(t, 1, l,  _pf);
    }

    int32_t WriteStr(const char* t){
        if(_plg.IsValid())
        {
            int16_t l = _ttcslen(t);
            _plg->write(&l, 1, sizeof(int16_t));
            return _plg->write(t, 1, _ttcslen(t));
        }
        int16_t l = _ttcslen(t);
        ::fwrite(&l, 1, sizeof(int16_t), _pf);
        return fwrite(t, 1, _ttcslen(t),  _pf);
    }

    int32_t ReadStr(const char* t){
        if(_plg.IsValid())
        {
            int16_t len;
            _plg->read(&len, 1, sizeof(int16_t));
            int32_t rd = _plg->read((void*)t, 1, len);
            return rd=len;
        }
        int16_t len;
        ::fread(&len, 1, sizeof(int16_t), _pf);
        int32_t rd = ::fread((void*)t, 1, len,  _pf);
        return rd=len;
    }

    void start_section(const char* sectionName)
    {
        if(_plg.IsValid())
            _plg->start_section(sectionName);
    }

private:
    PlugInDll<IBSPFileHook>     _plg;
    FILE*                       _pf;
    int32_t                        _ok;
    u_bool32                        _store;
};

