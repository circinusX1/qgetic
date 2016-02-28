// Curve_Class.cpp: implementation of the CCurve_Class class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "curve_class.h"




u_bool32    CCurve_Class::Compute(const vvector<VNT>& inpoints,
                              int32_t curveOrder,
                              vvector<VNT>& outpoints)
{
    _tmax = 0;
    if(curveOrder > 5) curveOrder = 5;

    f32_t*   kt = new f32_t[inpoints.size()+curveOrder+1];
    f32_t*   px = new f32_t[inpoints.size()+1];
    f32_t*   py = new f32_t[inpoints.size()+1];
    f32_t*   pz = new f32_t[inpoints.size()+1];
    f32_t*   ps = new f32_t[inpoints.size()+curveOrder+1];

    int32_t m;
    for( m=0; m < inpoints.size(); m++)
    {
        px[m] = inpoints[m].point.x;
        py[m] = inpoints[m].point.y;
        pz[m] = inpoints[m].point.z;
    }
    _TRY
    {
        if(_t == CURV_SPLINE)
        {
            SPL_Knots(m, curveOrder, px, py, pz, kt);
            SPL_Curve(px, py, pz, kt, m,curveOrder, outpoints);
        }
        else
        {
            NUR_Knots(m,curveOrder,px,py,pz, ps, kt);
            NUR_Curve(px,py,pz,kt, m, curveOrder, outpoints);
        }
    }_CATCHX()
    {
        outpoints.clear();
    }
    delete[] kt;
    delete[] px;
    delete[] py;
    delete[] pz;
    delete[] ps;
    return 1;
}

void CCurve_Class::SPL_Knots(int32_t n, int32_t curveOrder, f32_t *px, f32_t *py, f32_t *pz, f32_t* kt)
{
    int32_t i;
    for(i=0 ;i<=n+curveOrder ; i++)
    {
        if( i>curveOrder-1 )  /* is xi non multiple zero ? */
            if( i<n+1 )  /* is xi non multiple high end? */
                if(*(px+i-curveOrder) == *(px+i-curveOrder+1) &&
                    *(py+i-curveOrder) == *(py+i-curveOrder+1) &&
                    *(pz+i-curveOrder) == *(pz+i-curveOrder+1) ) /* repeated vertex ? */
                    kt[i] = kt[i-1] ;   /* then keep prev kt */
                else   /* non repeated vertex i-curveOrder ? */
                {  _tmax++;
                    kt[i] = kt[i-1] + 1.;
                /* increment kt value */
                }
                else if(i==n+1)  /* first end kt constant */
                {  _tmax++;
                    kt[n+1] = kt[n] + 1.0;
                }
                else kt[i] = kt[i-1];  /* equal end SPL_Knots */
                else kt[i] = 0.;    /* is starting equal zero kt */
    }   /* end of kt assignment loop for i*/
}

/*  B-spline SPL_Basics recursive computation given
n+1 vertices and order curveOrder for interpolant t */
f32_t CCurve_Class::SPL_Basics(int32_t i, int32_t curveOrder, f32_t t, f32_t* kt)
{
    f32_t a,b,c,d,e,f;  int32_t j,m;
    j = i+1; m=curveOrder-1;
    if( curveOrder<1 || t<0. || t > _tmax) return 0.0;
    if( curveOrder == 1)   /* order 1 */
        if( t>=kt[i] && t<=kt[i+1])  return 1.0;
        else return 0.0;
        /* Now all curveOrder > 1 */
        a =  t - kt[i];
        b =  kt[i+curveOrder-1]-kt[i];
        c =  kt[i+curveOrder] - t;
        d =  kt[i+curveOrder] - kt[i+1];
        e = SPL_Basics(i,m,t,kt);
        f = SPL_Basics(j,m,t,kt);
        if(b==0. && d==0.)
            if(e==0. && f==0.)  /* only case true */
                return 0.0;  else
            {     TRACEX("error\n");
            return 0.0;  }
            if(b==0. && a*e==0.)
                return c*f/d;
            if(d==0. && c*f==0.)
                return a*e/b;
            return e*a/b + f*c/d;
}    /* end of SPL_Basics computation */

void CCurve_Class::SPL_Curve( f32_t *rx, f32_t *ry, f32_t *rz, f32_t* kt ,int32_t n, int32_t curveOrder, vvector<VNT>& outpoints)
{
    f32_t t,b,bottom;
    int32_t i;
    VNT v;

    if(curveOrder>n+1)
    {
        TRACEX("Order of curve must be <= number of defining pts\n");
        throw 1;
    }
    for(t=0.1; t<=_tmax; t+=_tmax/20.)
    {
        v.point =V0;
        bottom=0.0;
        for(i=0;i<=n-1;i++)
        {
            b = SPL_Basics(i,curveOrder,t, kt) * 1;
            bottom += b;
            v.point.x += *(rx+i) * b;
            v.point.y += *(ry+i) * b;
            v.point.z += *(rz+i) * b;
        }
        if(bottom==0.0)
        {
            v.point=V0;
            throw 1;
        }
        outpoints<< v;
    }
}

void CCurve_Class::NUR_Knots(int32_t n,
                              int32_t curveOrder,
                              f32_t *px, f32_t *py, f32_t *pz,
                              f32_t* ps, f32_t* kt )
{
    int32_t i,mul=1, m=1;

    _tmax=n-curveOrder+2;
    for(i=0;i<=n-curveOrder+2;i++) ps[i]=0.;

    for(i=0 ;i<=n+curveOrder ; i++)
    {
        if( i>curveOrder-1 )  /* is xi non multiple zero ? */
            if( i<n+1 )  /* is xi non multiple high end? */
                if( px[i-curveOrder] == px[i-curveOrder+1] &&
                    py[i-curveOrder] == py[i-curveOrder+1] &&
                    pz[i-curveOrder] == pz[i-curveOrder+1] )  /* repeated vertex ? */
                {  kt[i] = kt[i-1] ;   /* then keep prev knot */
                _tmax=_tmax-ps[i-curveOrder]-m; mul++;
                } /* inhibit tmax knot */
                else   /* non repeated vertex i-curveOrder ? */
                    kt[i] = kt[i-1] + m +  ps[i-curveOrder];
                /* increment knot value */
                else if(i==n+1)  /* first end knot constant */
                    kt[n+1] = kt[n] + m + ps[n-curveOrder];
                else kt[i] = kt[i-1];  /* rest of equal end knots */
                else kt[i] = 0.;    /* is a repeated zero knot */
    }   /* end of knot assignment loop for i*/

}
/*  B-spline NUR_Basic recursive computation given
n+1 vertices and order curveOrder for interpolant t */
f32_t CCurve_Class::NUR_Basic(int32_t i, int32_t curveOrder, f32_t t, f32_t* kt)
{
    f32_t a,b,c,d,e,f;
    int32_t j,m;
    j = i+1;      m=curveOrder-1;
    if( curveOrder<1 || t<=0. || t > _tmax) return 0.0;
    if( curveOrder == 1)
        if( t>=kt[i] && t<=kt[i+1])  return 1.0;
        else return 0.0;
        /* Now all curveOrder > 1 */
        a =  t - kt[i];
        b =  kt[i+curveOrder-1] - kt[i];
        c =  kt[i+curveOrder] - t;
        d =  kt[i+curveOrder] - kt[i+1];
        e = NUR_Basic(i,m,t,kt);
        f = NUR_Basic(j,m,t,kt);
        if(b==0. && d==0.)
            if(e==0. && f==0.)  /* only case true */
                return 0.0;
            if(b==0. && a*e==0.)
                return c*f/d;
            if(d==0. && c*f==0.)
                return a*e/b;
            return e*a/b + f*c/d;
}    /* end of NUR_Basic computation */

void CCurve_Class::NUR_Curve( f32_t *rx, f32_t *ry, f32_t *rz,
                             f32_t* kt, int32_t n, int32_t curveOrder, vvector<VNT>& outpoints)
{
    VNT v;
    f32_t t,b,bottom;

    int32_t i;
    if(curveOrder>n+1)
        TRACEX("Order of curve must be <= number of defining pts\n");
    else
        for(t=0.1; t<=_tmax; t+=_tmax/20.)
        {
            v.point = V0;
            bottom=0.0;
            for(i=0;i<=(n-1);i++)
            {
                b = NUR_Basic(i,curveOrder,t,kt);
                bottom += b;
                v.point.x += *(rx+i) * b;
                v.point.y += *(ry+i) * b;
                v.point.z += *(rz+i) * b;
            }
            if(bottom==0.0)
            {
                TRACEX("Degenerate NUR_Basic beyond %f\n",t);
                throw 1;
            }
            outpoints << v;
        }
}

void compute_intervals(int32_t *u_int32_t, int32_t n, int32_t t);
void compute_point(int32_t *u_int32_t, int32_t n, int32_t t, double v, vvector<VNT>& control, v3d_t *output);
double blend(int32_t k, int32_t t, int32_t *u_int32_t, double v);

void bspline(int32_t n, int32_t t, vvector<VNT>& input, vvector<VNT>& output, int32_t num_output)
{
  int32_t       *u_int32_t;
  double    increment,  interval;
  VNT       calcxyz;
  int32_t       output_index;

  u_int32_t = new int32_t[n+t+8];
  compute_intervals(u_int32_t, n, t);

  output.clear();
  increment=(double) (n-t+2)/(num_output-1);  // how much parameter goes up each time
  interval=0;
  int32_t iCpcts = t;//PThis->GetDlgItemInt(EF_STPCTS);

  for (output_index=0; output_index<num_output; output_index++)
  {
      compute_point(u_int32_t, n, t, interval, input, &calcxyz.point);
      if(output_index < num_output-(iCpcts+2) )
      {
          output.push_back(calcxyz);
      }
      interval=interval+increment;  // increment our parameter
  }
  //output.push_back(input.back());   // put in the last point

  delete u_int32_t;
}

double blend(int32_t k, int32_t t, int32_t *u_int32_t, double v)  // calculate the blending value
{
  double value;

  if (t==1)			// base case for the recursion
  {
    if ((u_int32_t[k]<=v) && (v<u_int32_t[k+1]))
      value=1;
    else
      value=0;
  }
  else
  {
    if ((u_int32_t[k+t-1]==u_int32_t[k]) && (u_int32_t[k+t]==u_int32_t[k+1]))  // check for divide by zero
      value = 0;
    else
    if (u_int32_t[k+t-1]==u_int32_t[k]) // if a term's denominator is zero,use just the other
      value = (u_int32_t[k+t] - v) / (u_int32_t[k+t] - u_int32_t[k+1]) * blend(k+1, t-1, u_int32_t, v);
    else
    if (u_int32_t[k+t]==u_int32_t[k+1])
      value = (v - u_int32_t[k]) / (u_int32_t[k+t-1] - u_int32_t[k]) * blend(k, t-1, u_int32_t, v);
    else
      value = (v - u_int32_t[k]) / (u_int32_t[k+t-1] - u_int32_t[k]) * blend(k, t-1, u_int32_t, v) +
          (u_int32_t[k+t] - v) / (u_int32_t[k+t] - u_int32_t[k+1]) * blend(k+1, t-1, u_int32_t, v);
  }
  return value;
}

void compute_intervals(int32_t *u_int32_t, int32_t n, int32_t t)   // figure out the knots
{
  int32_t j;

  for (j=0; j<=n+t; j++)
  {
    if (j<t)
      u_int32_t[j]=0;
    else
    if ((t<=j) && (j<=n))
      u_int32_t[j]=j-t+1;
    else
    if (j>n)
      u_int32_t[j]=n-t+2;  // if n-t=-2 then we're screwed, everything goes to 0
  }
}

void compute_point(int32_t *u_int32_t, int32_t n, int32_t t, double v, vvector<VNT>& control, v3d_t *output)
{
    int32_t k;
    double temp;

    // initialize the variables that will hold our outputted v3d_t
    output->x=0;
    output->y=0;
    output->z=0;

    for (k=0; k<=n; k++)
    {
        temp = blend(k,t,u_int32_t,v);  // same blend is used for each dimension coordinate

        output->x = output->x + (control[k].point).x * temp;
        output->y = output->y + (control[k].point).y * temp;
        output->z = output->z + (control[k].point).z * temp;
    }


}
