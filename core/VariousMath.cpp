

#include "VariousMath.h"






double Cx_ZetaGammax::evaluate(double _x) const
{
    int l=7,s;
    double f,K0,const1,time,M_PI2=M_PI*M_PI;

    ArrayXd b;
    b=pow(i2,l)*a2*exp(-i2*M_PI2*_x);
    f = 2.*pow(M_PI,double(2*l))*b.sum();

    for ( s=l-1; s>1; --s )
    {
        K0=ArrayXd::LinSpaced(s,1.,double(2*s-1)).prod()/sqrt(2.*M_PI);
        const1 = (1.+pow(0.5,s+0.5))/3.;
        time = pow(2.*const1*K0/double(N)/f, 2./(3.+2.*double(s)));
        b = pow(i2,double(s))*a2*exp(-i2*M_PI2*time);
        f = 2.*pow(M_PI,double(2*s))*b.sum();
    }

    return _x-pow(2.*double(N)*sqrt(M_PI)*f, -0.4);
}


double linearInterpolation(const ArrayXd& X,const ArrayXd& Y,double x)
{
    if ( X.size()==0 || Y.size()==0 )  throw "linearInterpolation error: X and/or Y are empty";
    if ( X.size()!=Y.size() )  throw "linearInterpolation error: X and Y are of different size";
    const double *d=X.data();
    int r=int(lower_bound(d,d+X.size(),x)-d), l=r-1;
    if ( l==-1 ) return Y(r);
    else if ( r==X.size() ) return Y(l);
    else if ( fabs(X(r)-X(l))<100.*std::numeric_limits<double>::epsilon() )  throw "linearInterpolation error: X(l)==X(r)";
    else return Y(l)+(x-X(l))*(Y(r)-Y(l))/(X(r)-X(l));
}


void cumulativeSum(ArrayXd &a)
{
    int i,n=int(a.size());
    for ( i=1; i<n; i++ )  a(i) += a(i-1);
}


double variance(ArrayXd &a)
{
    int i,n=int(a.size());
    double mu, var;
    for ( mu=0., var=0., i=0; i<n; i++ )
    {
        mu+=a(i);
        var+=(a(i)*a(i));
    }
    mu/=double(n);
    var/=double(n);
    var-=mu*mu;
    return var;
}


ArrayXi permutation(int N)
{
    ArrayXi perm(N);
    int j, k;
    for ( j=0; j<N; j++ ) perm(j)=j;
    unsigned int idx;

    Ran rand(3257);
    k = N - 1;
    while (k > 0)
    {
       idx = rand.int32();
       idx %= k;
       j = perm(k);
       perm(k)=perm(idx);
       perm(idx) = j;
       --k;
    }
    return perm;
}


ArrayXXi permutations(int n, int N)
{
    ArrayXXi perm(n,N);
    int i, j, k;
    for (i=0; i<n; i++)
       for ( j=0; j<N; j++ ) perm(i,j)=j;
    unsigned int idx;

    Ran rand(3257);
    for ( i=0; i<n; i++ )
    {
       k = N - 1;
       while (k > 0)
       {
          idx = rand.int32();
          idx %= k;
          j = perm(i,k);
          perm(i,k)=perm(i,idx);
          perm(i,idx) = j;
          --k;
        }
     }
    return perm;
}


double squareDistance(const ArrayXd &a,const ArrayXd &b)
{
    double d=0.;
    for ( int i=0,n=a.size(); i<n; i++ ) d+=(a(i)-b(i))*(a(i)-b(i));
    return d;
}


int findNearest(const std::vector<ArrayXd> &v,const ArrayXd &y)
{
    int i0=0,i,n=v.size();
    double d2,d20=squareDistance(v[0],y);
    for ( i=1; i<n; i++ )
    {   d2=squareDistance(v[i],y);
        if (  d2<d20 ) { d20=d2; i0=i; }
    }
    return i0;
}
