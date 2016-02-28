#include <QDebug>
#include <QStatusBar>
#include "geticapp.h"
#include "document.h"
#include <QFileDialog>

#ifdef Q_WS_WIN
#   include <windows.h>
#   include <shellapi.h>
#else /* Q_WS_X11 */
#   include <X11/Xlib.h>
#   include <QX11Info>
#endif


GeticApp* theApp = 0;

GeticApp::GeticApp(int32_t argc, char *argv[]) :
    QApplication(argc, argv)
{
    char cd[256];
    getcwd(cd,255);
    _curDir=cd;


    _compilingCur.setShape(Qt::WaitCursor);
    _cursors[CUR_SCROLL].setShape(Qt::CrossCursor);
    _cursors[CUR_MOVE].setShape(Qt::ClosedHandCursor);
    _cursors[CUR_SCALE].setShape(Qt::SizeAllCursor);
    _cursors[CUR_ROTATE].setShape(Qt::ForbiddenCursor);
    _cursors[CUR_NS].setShape(Qt::SizeVerCursor);
    _cursors[CUR_EW].setShape(Qt::SizeHorCursor);
    _cursors[CUR_NWSE].setShape(Qt::SizeFDiagCursor);
    _cursors[CUR_NESW].setShape(Qt::SizeBDiagCursor);

    theApp = this;
}

void GeticApp::keyMenu(int key, int state)
{


}

void GeticApp::some_event(QObject* object,QEvent* event)
{
    qDebug() << event->type() << " some event\n";
}


void GeticApp::mouse_event(QMouseEvent* e)
{
    qDebug("mouse event\n");

}

void GeticApp::key_event(int key, int state)
{
    qDebug("key event\n");
    if(key==Qt::Key_Escape)
    {
        _keys.clear();
        Pd->OnEscape();
        _Hmain->statusBar()->showMessage(tr("Ready..."));
    }

    if(key==Qt::Key_Return)
    {
        _keys.clear();
        Pd->OnEnter();
        Pd->Invalidate();
        _Hmain->statusBar()->showMessage(tr("Ready..."));
    }

}

u_bool32 GeticApp::isKeyDown(int32_t key)
{
    std::set<int>::iterator k = _keys.begin();
    for(;k!=_keys.end();++k)
    {
        if(key==*k)
            return 1;
    }
    return 0;
}

int32_t GeticApp::_keyboardModifiers()
{

}

int32_t GeticApp::_mouseButtons()
{

}

void GeticApp::LoadCustomProfileSettings()
{
}

void GeticApp::WriteCustomProfileSettings()
{
}

void GeticApp::CleanTexMan()
{
}

int32_t GeticApp::DoFileDialog(QFileDialog& d)
{
    return d.exec();
}

void GeticApp::StampDll(int32_t runs)
{
}

void GeticApp::PumpMessage(u_int32_t msg, u_int32_t tout)
{
     installEventFilter(new GeticEventFilter());
}




int32_t GeticEditor::GetTextureInfo(_Plg_Texture* pt)
{
    TexHandler      th;
    char            local[_MAX_PATH] = {0};

    pt->pBuffer     = 0;
    pt->dims[0]     = 0;
    pt->dims[1]     = 0;
    pt->bpp         = 0;

    _getcwd(local,_MAX_PATH);

    if(pt->filename[0])
    {
        map<string, _Plg_Texture>::iterator it = _gc.find(pt->filename);
        if(it != _gc.end())
        {
            pt->dims[0] = _gc[pt->filename].dims[0];
            pt->dims[1] = _gc[pt->filename].dims[1];
            pt->bpp     = _gc[pt->filename].bpp    ;
            pt->pBuffer = _gc[pt->filename].pBuffer;

            return 0;
        }

        if(!::strchr((const char*)pt->filename, '.'))
        {
            ::sprintf(local,"%s.tga", pt->filename);
            if(th.LoadThisFile(local,0))
            {
                pt->dims[0] = th.n_x;
                pt->dims[1] = th.n_y;
                pt->bpp     = th.n_bpp;
                if(pt->flags & WANT_TEX_BUFEER)
                {
                    pt->pBuffer = new u_int8_t[th.n_x* th.n_y * th.n_bpp];
                    if(pt->pBuffer)
                    {
                        ::memcpy(pt->pBuffer, th.Buffer(), th.n_x* th.n_y * th.n_bpp);
                    }
                }
                _gc[pt->filename] = *pt;
                return 0;
            }

            ::sprintf(local,"%s.bmp", pt->filename);
            if(th.LoadThisFile(local,0))
            {
                pt->dims[0] = th.n_x;
                pt->dims[1] = th.n_y;
                pt->bpp     = th.n_bpp;
                if(pt->flags & WANT_TEX_BUFEER)
                {
                    pt->pBuffer = new u_int8_t[th.n_x* th.n_y * th.n_bpp];
                    if(pt->pBuffer)
                    {
                        ::memcpy(pt->pBuffer, th.Buffer(), th.n_x* th.n_y * th.n_bpp);
                    }
                }
                _gc[pt->filename] = *pt;
                return 0;
            }

            ::sprintf(local,"%s.jpg", pt->filename);
            if(th.LoadThisFile(local,0))
            {
                pt->dims[0] = th.n_x;
                pt->dims[1] = th.n_y;
                pt->bpp     = th.n_bpp;
                if(pt->flags & WANT_TEX_BUFEER)
                {
                    pt->pBuffer = new u_int8_t[th.n_x* th.n_y * th.n_bpp];
                    if(pt->pBuffer)
                    {
                        ::memcpy(pt->pBuffer, th.Buffer(), th.n_x* th.n_y * th.n_bpp);
                    }
                }
                _gc[pt->filename] = *pt;
                return 0;
            }
        }
    }
    return -1;  // error cannot load texture
}

void  GeticEditor::Clear()
{
    for(map<string, _Plg_Texture>::iterator it = _gc.begin();
        it != _gc.end(); it++)
    {
        if((it->second).pBuffer)
        {
            delete[] (it->second).pBuffer;
        }
    }
}


int32_t GeticApp::GetProfileInt(const char* section, const char* entry, int32_t def)
{
    _config.beginGroup(section);
    int32_t val = _config.value(entry).toInt();
    _config.endGroup();
    return val;

}


QString GeticApp::GetProfileString(const char* section, const char* entry, const char* def)
{
    _config.beginGroup(section);
    QString val = _config.value(entry).toString();
    _config.endGroup();
    return val;

}

void GeticApp::WriteProfileInt(const char* section, const char* entry, int32_t value)
{
    _config.beginGroup(section);
    _config.setValue(entry, QVariant(value));
    _config.endGroup();
}


void GeticApp::WriteProfileString(const char* section, const char* entry, const char* value)
{
    _config.beginGroup(section);
    _config.setValue(entry, QVariant(value));
    _config.endGroup();
}

