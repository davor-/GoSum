


#include "Probability.h"


bool chiSquareTest(const CContinuousRV &_aRV,const CContinuousSample &_aS,const double chi_alpha)
{
    ArrayXd _x,_H;
    _aS.normalizedHistogram(_x,_H);
    int i,n=int(_H.size());
    ArrayXd Ht(n);
    double dx=0.5*(_x(n-1)-_x(0))/double(n-1);
    for ( i=0; i<n; i++ )  Ht(i)=_aRV.probability(_x(i)-dx,_x(i)+dx);
    double chi2=0.;
    for ( i=0; i<n; i++ )  chi2+=(_H(i)-Ht(i))*(_H(i)-Ht(i))/Ht(i);
    chi2*=_aS.sampleSize();

    boost::math::chi_squared dist(n - 1);
    double p=1-cdf(dist,chi2);
    return (p<=chi_alpha);
}


double KolmogorovSmirnovTest(const CContinuousRV &_aRV,const CContinuousSample &_aS)
{
    double KSsum=0.;

    ArrayXd _x,_cH;
    _aS.cummulativeHistogram(_x,_cH);

   int i,n=int(_cH.size());
   for ( i=0; i<n; i++ )
       KSsum+=fabs(_aRV.cumulative(_x(i))-_cH(i));

    return KSsum;
}
