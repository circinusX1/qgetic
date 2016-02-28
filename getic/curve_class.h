// Curve_Class.h: interface for the CCurve_Class class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_CURVE_CLASS_H__C61EDFCB_8925_404D_BD12_57659C76261A__INCLUDED_)
#define AFX_CURVE_CLASS_H__C61EDFCB_8925_404D_BD12_57659C76261A__INCLUDED_

#include <basecont.h>

struct  VNT
{
    VNT():sel(0){
    }
    u_bool32   sel;
    v3d_t  point;
    v3d_t  normal;
};

class CCurve_Class
{
public:
    typedef enum _CURVTYPE
    {
        CURV_SPLINE,
        CURV_NURB,

    }CURVTYPE;

public:
    CCurve_Class(CURVTYPE t){_t=t;};
    virtual ~CCurve_Class(){};
    u_bool32    Compute(const vvector<VNT>& inpoints, int32_t step, vvector<VNT>& outpoints);

private:
    void SPL_Knots(int32_t n, int32_t curveOrder, f32_t *px, f32_t *py, f32_t *pz, f32_t*);
    f32_t SPL_Basics(int32_t i, int32_t curveOrder, f32_t t, f32_t*);
    void SPL_Curve( f32_t *rx, f32_t *ry, f32_t *rz, f32_t*, int32_t n, int32_t curveOrder, vvector<VNT>& outpoints);

    void NUR_Knots(int32_t n, int32_t curveOrder, f32_t *px, f32_t *py, f32_t *pz, f32_t* ps, f32_t* kt );
    f32_t NUR_Basic(int32_t i, int32_t curveOrder, f32_t t, f32_t*);
    void NUR_Curve( f32_t *rx, f32_t *ry, f32_t *rz, f32_t*, int32_t n, int32_t curveOrder, vvector<VNT>& outpoints);


    CURVTYPE    _t;
    f32_t        _tmax;
};

void bspline(int32_t n, int32_t t, vvector<VNT>& input, vvector<VNT>& output, int32_t num_output);

#endif // !defined(AFX_CURVE_CLASS_H__C61EDFCB_8925_404D_BD12_57659C76261A__INCLUDED_)
