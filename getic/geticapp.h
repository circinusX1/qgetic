#ifndef GETICAPP_H
#define GETICAPP_H

#include "stdafx.h"
#include <QApplication>
#include <QImage>
#include <QBrush>
#include <QIcon>
#include <QSettings>
#include <QKeyEvent>
#include <QDebug>
#include <qnamespace.h>
#include <geticplug.h>
#include <baselib.h>
#include "texadapter.h"
#include <map>
#include <set>

using namespace std;



class GeticEditor : public IGeticEditor
{
public:
    int32_t GetTextureInfo(_Plg_Texture* pt);
    void  Clear();
    map<string, _Plg_Texture>  _gc;
};


class GeticEventFilter;
class QFileDialog;
class GeticApp : public QApplication
{
    Q_OBJECT
public:
    explicit GeticApp(int32_t argc, char *argv[]);
    void key_event(int, int);
    void some_event(QObject* object,QEvent* event);
    void mouse_event(QMouseEvent* e);
    void DocsDir(const tchar* psz)
    {
        _docsDir=psz;
    }
    void BspsDir(const tchar* psz)
    {
        _bspDir=psz;
    }
    void TexDir(const tchar* psz)
    {
        _texDir=psz;
    }
    void SndDir(const tchar* psz)
    {
        _sndDir=psz;
    }
    void PrefsDir(const tchar* psz)
    {
        _prefsDir=psz;
    }
    void HomeDir(const tchar* hd);
    void ScriptsDir(const tchar* psz)     {_scriptsDir=psz;}
    void CompilerDir(const tchar* psz)    {_compilerDir=psz;}
    void ServerDir(const tchar* psz)      {_serverDir=psz;}
    void XsdCatItmFile(const tchar* psz)  {_catFile=psz;}
    void StartDir(const tchar* psz)       {_curDir = psz;}


    const tchar* InstallDir(){return _instDir.c_str();}
    const tchar* StartDir(){return _curDir.c_str();}
    const tchar* DocsDir(){return _docsDir.c_str();}
    const tchar* HomeDir(){return _homeDir.c_str();}

    const tchar* BspsDir(){return _bspDir.c_str();}
    const tchar* TexDir(){return _texDir.c_str();}
    const tchar* SndDir(){return _sndDir.c_str();}
    const tchar* PrefsDir(){return _prefsDir.c_str();}
    const tchar* ServerDir()  {return _serverDir.c_str();}
    const tchar* ScriptsDir(){return _scriptsDir.c_str();}
    const tchar* CompilerDir(){return _compilerDir.c_str();}
    const tchar* XsdCatItmFile(){return _catFile.c_str();}

    void LoadCustomProfileSettings();
    void WriteCustomProfileSettings();

    void  ReDir(){::chdir(_curDir.c_str()); }
    void CleanTexMan();
    int32_t DoFileDialog(QFileDialog& d);
    void StampDll(int32_t runs);
    void PumpMessage(u_int32_t msg, u_int32_t tout=-1);

    int32_t GetProfileInt(const char*, const char*, int32_t def);
    QString GetProfileString(const char*, const char*, const char* def);
    void WriteProfileInt(const char*, const char*, int32_t);
    void WriteProfileString(const char*, const char*, const char* value);
    u_bool32 isKeyDown(int32_t key);
    void keyMenu(int key, int state);
signals:
    
public slots:
protected:
    static int32_t _keyboardModifiers();
    static int32_t _mouseButtons();

public:

   vvector<string>  _errors;
   QWidget*       p_wndFocused;
   GeticEditor _gEditor;
   TexRef      _TexSys;
   QBrush      _brush[4];
   COLORREF    _color[4]; //

   COLORREF    _txtcolor;
   QImage     _butBmp;

   string      _cName;      //class window name
   string      _cNameViews; //class window name

   // toolbar values
   u_int32_t       _bShowMakeTbar;
   u_int32_t       _bShowSelectTbar;
   u_int32_t       _bShowMoveTbar;
   u_int32_t       _bShowAlignTbar;
   u_int32_t       _bShowViewsTbar;
   u_int32_t       _bShowCsgTbar;
   u_int32_t       _bShowBuildTbar;
   u_int32_t       _bShowCurvesbar;
   QImage		_msgBoxBitmap;

   QBrush      _slpEditBrush;
   QImage		_slpEditBitmap;
   QImage		_txtEditBitmap;
   QBrush      _txtEditBrush;


   QRect        _rtMakeTbar;
   QRect        _rtSelectTbar;
   QRect        _rtMoveTbar;
   QRect        _rtAlignTbar;
   QRect        _rtViewsTbar;
   QRect        _rtCsgTbar;
   QRect        _rtBuildTbar;
   QRect        _rtCurvesTbar;

   u_int32_t       _dwpMakeTbar;
   u_int32_t       _dwpSelectTbar;
   u_int32_t       _dwpMoveTbar;
   u_int32_t       _dwpAlignTbar;
   u_int32_t       _dwpViewsTbar;
   u_int32_t       _dwpCsgTbar;
   u_int32_t       _dwpBuildTbar;
  // CToolFrame*	_pToolFrame;
   u_bool32        _bToolOn;
   QRect            _rectMiniTool;
   u_int32_t        _cmdPlugIn;
   u_bool32         _returnVal;
   int32_t         _runs;
   QIcon           _colexpic[2];
   std::set<int>    _keys;
private:

   tstring     _instDir;
   tstring     _curDir;
   tstring     _docsDir;
   tstring     _homeDir;
   tstring     _bspDir;
   tstring     _texDir;
   tstring     _sndDir;
   tstring     _prefsDir;
   tstring     _scriptsDir;
   tstring     _compilerDir;
   tstring     _serverDir;
   tstring     _catFile;
   QSettings   _config;


public:
    QCursor _compilingCur;
    QCursor _cursors[16];

};

extern GeticApp* theApp;


inline int32_t Inl_GetTexCount()
{
    int32_t textures = 0;
    TexSys::iterator bt = theApp->_TexSys.begin();
    TexSys::iterator et = theApp->_TexSys.end();
    for(;bt != et; bt++)
    {
        Texture& tex      = (*bt).second;
        if(!strncmp(tex.filename, "@L",2))
            continue;
        ++textures;
    }
    return textures;
}


class GeticEventFilter: public QObject
{
public:
  GeticEventFilter():QObject(){}
  ~GeticEventFilter(){}

  bool eventFilter(QObject* object,QEvent* event)
  {
       bool br =  QObject::eventFilter(object,event);
       if(event->type() == QEvent::KeyPress)
       {
            QKeyEvent* pk=reinterpret_cast<QKeyEvent*>(event);
            theApp->key_event(pk->key(),1);
       }else if(event->type() == QEvent::KeyRelease)
       {
            QKeyEvent* pk=reinterpret_cast<QKeyEvent*>(event);
            theApp->key_event(pk->key(),0);
       }
       else if(event->type() == QEvent::MouseButtonRelease)
       {
           QMouseEvent* pk=reinterpret_cast<QMouseEvent*>(event);
           theApp->mouse_event(pk);
       }

       return br;
  }
};


#define	REDIR()	 theApp->ReDir()

#endif // GETICAPP_H
