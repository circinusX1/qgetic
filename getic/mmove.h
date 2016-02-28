// MMove.h: interface for the MMove class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __MOVE_H__
#define __MOVE_H__

#include <QPoint>
#include <baselib.h>

enum {M_MOVED=1, M_SCALED, M_ROTATED, M_SHIFTED, M_DELETED, ITM_MOVED, ITM_SCALED, ITM_ROTATED, ITM_DELETED, F_EXTRUDED};

class MMove
{
public:
    MMove():_ldown(0),_rdown(0),_mdown(0){};
    virtual ~MMove(){};


    QPoint  _pt;
    QPoint  _ptd;
    u_bool32    _ldown;
    u_bool32    _rdown;
    u_bool32    _mdown;
    v3d_t      _wpdown;
};

#endif // __MOVE_H__
