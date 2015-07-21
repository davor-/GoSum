#ifndef VARIOUSMATH_H
#define VARIOUSMATH_H


#include "Utilities.h"
#include "ran.h"
#include "Ds.h"
#define TINY  100*(std::numeric_limits<double>::epsilon())




//! \brief
//! Finds _x in the interval (a,b) such that f(_x)=0.
//! \return True if root is found, false otherwise.
template <class T>
double findRoot(const T &fnc,double a,double b)
{
    double fa, fb;
    fa = fnc.evaluate(a);
    fb = fnc.evaluate(b);

    if (fa == 0)
    {  return a; }
    else if (fb == 0)
    {  return b; }
    else if ((fa > 0) == (fb > 0)) throw "findRoot error: the function values at the interval endpoints must differ in sign";

    double fc = fb;
    double c=a, d=b-a, e=d, m, toler, r, s, p, q;
    // main loop, exit from middle of the loop
    while (fb!=0 && a!=b)
    {
         // insure that b is the best result so far, a is the prvious
         // value of b, and c is on the opposite side of the zero from b.
       if ((fb > 0) == (fc > 0))
       {  c = a; fc = fa; d = b-a;  e = d; }
       if (fabs(fc) < fabs(fb))
       {  a = b; b = c; c = a; fa = fb; fb = fc; fc = fa; }
      // convergence test and possible exit
       m = 0.5*(c-b);
       toler = 2.0*2.220446049250313e-16*std::max(fabs(b), 1.0);
       if (fabs(m) <=  toler || fb == 0.0)   return b;
       // choose bisection or interpolation
       if (fabs(e) < toler || fabs(fa)<=fabs(fb))
       {  // Bisection
          d = m;  e = m;  }
       else
       {  // Interpolation
          s = fb/fa;
          if (a==c)
          {  // Linear interpolation
             p = 2.0*m*s;
             q = 1.0-s;  }
          else
          {  // Inverse quadratic interpolation
             q = fa/fc;
             r = fb/fc;
             p = s*(2.0*m*q*(q-r) - (b-a)*(r-1.0));
             q = (q-1.0)*(r-1.0)*(s-1.0); }
          if (p>0)      q = -q;
          else          p = -p;
          // Is interpolated point acceptable
          if (2.0*p<3.0*m*q-fabs(toler*q) && p<fabs(0.5*e*q))
          { e = d; d = p/q; }
          else
          { d = m; e = m; }
       }
       // next point
       a = b; fa = fb;
       if (fabs(d)>toler)     b = b+d;
       else if (b>c)          b = b-toler;
       else                   b = b+toler;
       fb = fnc.evaluate(b);
    }
    return b;
}


//! \brief
//! Class for the function f(x)=x-zeta*gamma^[7](x).
class Cx_ZetaGammax
{
private:
    int N;
    ArrayXd i2,a2;
    Cx_ZetaGammax() : N(0) {}
public:
    Cx_ZetaGammax(const int _N,const ArrayXd &_i2,const ArrayXd &_a2) : N(_N), i2(_i2), a2(_a2) {}
    double evaluate(double _x) const;
};


ArrayXi permutation(int N); //!< Generates one random permutation of (1,...,N).
ArrayXXi permutations(int n, int N); //!< Generates n random permutations of (1,...,N).
double variance(ArrayXd &a); //!< Returns variance of elements of the array.
double linearInterpolation(const ArrayXd& X,const ArrayXd& Y,double x); //!< Linear interpolation Y(x)=Yl+((Yr-Yl)/(Xr-Xl))*(x-Xl).
void cumulativeSum(ArrayXd &a); //!< Returns cummulative sum of elements of the array.
double squareDistance(const ArrayXd &a,const ArrayXd &b); //!< Returns squared distance between a and b as points.
int findNearest(const std::vector<ArrayXd> &v,const ArrayXd &y); //!< Returns index of the point beetween points v nearest to the point y.


#endif // VARIOUSMATH_H
