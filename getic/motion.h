// Motion.h: interface for the CMotion class.
//
//---------------------------------------------------------------------------------------
#ifndef __MOTION_H__
#define __MOTION_H__


#include <basecont.h>
#include "brush.h"

//---------------------------------------------------------------------------------------
// motion state
class CMotion;
struct MState
{
    MState(){
        _pos   = V0;
        _euler = V0;
        _trTimes[0] = 1000;
        _trTimes[1] = 1000;
    }

    f32_t        _trTimes[2];    // times transitions in this node and in transition
    v3d_t          _trRots[2];     // rotationa appyed durring times
    CMotion*    _pMotion;       // parent motion
    v3d_t          _pos;           // position
    v3d_t          _euler;         // orientation
};


//---------------------------------------------------------------------------------------
// descriobes motion of detail brushes
class Brush;
class NO_VT CMotion
{
public:
    friend class Document;


    // loop circular list, loop=0 double likked list forward and back
    CMotion(Brush* pBrush,  int32_t iNodes, u_bool32 bLoop):_iNodes(iNodes),_bLoop(bLoop),_pBrush(pBrush){
        if(iNodes)
        {
            _states.reserve(iNodes);
            Create();
            _center  = pBrush->_box.GetCenter();
        }
        _selNode = 1; // cannot select joint node
    }
    int32_t             GetNodesCount(){return _states.size();}
    MState*         GetState(int32_t i){return &_states[i];}
    u_bool32            Create();
    void            GlDraw();
    u_bool32            IsLoop(){return _bLoop;}
    void            RefreshPosition();
    void            SelectNode(int32_t idx){_selNode = idx;}
    int32_t             GetSelNode(){return _selNode ;}
    MState*         GetSelState(){return &_states[_selNode] ;};
    v3d_t&             GetCenter(){return _center;}
    Brush*          GetBrush(){return _pBrush;}

    int32_t             _iNodes;
    u_bool32            _bLoop;
    Brush*          _pBrush;
    v3d_t              _center;
    int32_t             _selNode;
    vvector<MState> _states;
};

#endif // __MOTION_H__
