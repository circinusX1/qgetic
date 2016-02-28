#ifndef DIALOGNEWBRUSH_H
#define DIALOGNEWBRUSH_H
#include <baselib.h>
#include "brush.h"
#include <QDialog>

namespace Ui {
class DialogNewBrush;
}
struct SNewBrush
{
    SNewBrush()
    {
        _tm = TM_LIN;
    }
    u_int8_t    _vstrips;
    u_int8_t    _radstrips;
    v3d_t      _boxdim;
    f32_t    _thikness;
    TEX_MAP _tm;

    u_bool32    _isSolid    : 1;
    u_bool32    _detail     : 1;
    u_bool32    _reversed   : 1;
    u_bool32	_cutAll     : 1;
    u_bool32	_splitter   : 1;
    u_bool32    _zone       : 1;
    u_bool32    _skyDom     : 1;
    u_bool32    _detached   : 1;

};

class DialogNewBrush : public QDialog
{
    Q_OBJECT

public:
    explicit DialogNewBrush(QWidget *parent = 0);
    ~DialogNewBrush();

private slots:

    void on_IDOK8_clicked();
    void on_CK_SETDETAIL_clicked(bool checked);
    void on_CK_SHELL_clicked(bool checked);
    void on_IDCANCEL9_clicked();
    void showEvent(QShowEvent *);
    void on_CK_SPLITTER_clicked(bool checked);
    void on_CK_ZONE_clicked(bool checked);
    void on_CK_SKYDOM_clicked(bool checked);

private:
    Ui::DialogNewBrush *ui;

public:
    SNewBrush   _brshSett;
    bool        _hideZone;
};

#endif // DIALOGNEWBRUSH_H
