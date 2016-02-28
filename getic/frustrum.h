#ifndef FRUSTRUM_H
#define FRUSTRUM_H

#include <baselib.h>
#include <baseutils.h>


class Camera;
class Frustrum
{
public:
    Frustrum():_ihulls(0){};
    virtual ~Frustrum(){};

    void		AddFrustrumPlan(const v3d_t& a,const v3d_t& b,const v3d_t& c){
        if(_ihulls<32)
        {
            Plane p(a,b,c);
            AddFrustrumPlan(p);
        }
    }
    void		AddFrustrumPlan(const v3d_t& norm, const v3d_t& ponpl){
        if(_ihulls<32)
        {
            Plane p(norm,ponpl);
            AddFrustrumPlan(p);

        }
    }
    void		AddFrustrumPlan(const Plane& plane){
        if(_ihulls<32)
        {
            int32_t c = _ihulls;
            while (--c>=0)
            {
                if(_hulls[c] == plane)
                    return;
            }
            _hulls[_ihulls++] = plane;
        }
    }
    u_bool32        IsCulled(const v3d_t* point);
    u_bool32		IsCulled(const v3d_t* aabbMin, const v3d_t* aabbMax);
    void        UpdateFrustrumPlanes(v3d_t& pos, v3d_t& ldir, v3d_t& lup, v3d_t& lside);


    Plane		_hulls[32];		// maxim clipping hulls
    int32_t			_ihulls;		// number of hulls
    v3d_t			_viewfdots[5];	// main frustrum corners
    f32_t		_farZ;			// far

};

#endif // FRUSTRUM_H
