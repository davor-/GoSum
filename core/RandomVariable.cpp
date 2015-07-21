

#include "RandomVariable.h"
#include "FFTWLibrary.h"


BOOST_CLASS_EXPORT_IMPLEMENT(CConstantDRV)
BOOST_CLASS_EXPORT_IMPLEMENT(CUniformDRV)
BOOST_CLASS_EXPORT_IMPLEMENT(CEmpiricalDRV)
BOOST_CLASS_EXPORT_IMPLEMENT(CCategoricalDRV)
BOOST_CLASS_EXPORT_IMPLEMENT(CConstantCRV)
BOOST_CLASS_EXPORT_IMPLEMENT(CUniformCRV)
BOOST_CLASS_EXPORT_IMPLEMENT(CGaussianCRV)
BOOST_CLASS_EXPORT_IMPLEMENT(CExponentialCRV)
BOOST_CLASS_EXPORT_IMPLEMENT(CEmpiricalCRV)




CRandomVariable::distributiontype CRandomVariable::Type(string _str)
{
    if ( _str=="constantdiscrete" )          return constantdiscrete;
    else if ( _str=="uniformdiscrete" )      return uniformdiscrete;
    else if ( _str=="empiricaldiscrete" )    return empiricaldiscrete;
    else if ( _str=="categorical" )          return categorical;
    else if ( _str=="constantcontinuous" )   return constantcontinuous;
    else if ( _str=="uniformcontinuous" )    return uniformcontinuous;
    else if ( _str=="gaussian" )             return gaussian;
    else if ( _str=="exponential" )          return exponential;
    else if ( _str=="empiricalcontinuous" )  return empiricalcontinuous;
    else  throw "CRandomVariable::Type error: unrecognised type";
}


bool CRandomVariable::IsEmpirical(CRandomVariable::distributiontype _type)
{
    return ( _type==empiricaldiscrete || _type==categorical || _type==empiricalcontinuous );
}


bool CRandomVariable::IsTheoretical(CRandomVariable::distributiontype _type)
{
    return ( _type==constantdiscrete || _type==uniformdiscrete ||
             _type==constantcontinuous || _type==uniformcontinuous ||
             _type==gaussian || _type==exponential );
}


bool CRandomVariable::IsDistributionType(std::string _stype)
{
    return ( _stype=="constantdiscrete" || _stype=="uniformdiscrete" ||
             _stype=="empiricaldiscrete" || _stype=="categorical" ||
             _stype=="constantcontinuous" || _stype=="uniformcontinuous" ||
             _stype=="gaussian" || _stype=="exponential" ||
             _stype=="empiricalcontinuous" );
}


bool CRandomVariable::IsDiscrete(CRandomVariable::distributiontype _type)
{
    return ( _type==constantdiscrete || _type==uniformdiscrete || _type==empiricaldiscrete || _type==categorical );
}


bool CRandomVariable::IsContinuous(CRandomVariable::distributiontype _type)
{
    return ( _type==constantcontinuous || _type==uniformcontinuous || _type==gaussian ||
             _type==exponential || _type==empiricalcontinuous );
}


int CRandomVariable::DistributionParametersSize(CRandomVariable::distributiontype _type)
{
    if ( _type==constantdiscrete )          return 1;
    else if ( _type==uniformdiscrete )      return 2;
    else if ( _type==empiricaldiscrete )    return 0;
    else if ( _type==categorical )          return 0;
    else if ( _type==constantcontinuous )   return 1;
    else if ( _type==uniformcontinuous )    return 2;
    else if ( _type==gaussian )             return 2;
    else if ( _type==exponential )          return 1;
    else if ( _type==empiricalcontinuous )  return 2;
    else  throw "CRandomVariable::DistributionParametersSize error: unrecognised type";
}


template <class Archive>
void CConstantDRV::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CDiscreteRV",boost::serialization::base_object<CDiscreteRV>(*this));
    ar & boost::serialization::make_nvp("c",c);
}


template <class Archive>
void CUniformDRV::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CDiscreteRV",boost::serialization::base_object<CDiscreteRV>(*this));
    ar & boost::serialization::make_nvp("a",a);
    ar & boost::serialization::make_nvp("b",b);
    ar & boost::serialization::make_nvp("n",n);
}


template <class Archive>
void CEmpiricalDRV::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CDiscreteRV",boost::serialization::base_object<CDiscreteRV>(*this));
    ar & boost::serialization::make_nvp("p",p);
    ar & boost::serialization::make_nvp("cdf",cdf);
    ar & boost::serialization::make_nvp("mu",mu);
    ar & boost::serialization::make_nvp("stdv",stdv);
}


template <class Archive>
void CCategoricalDRV::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CDiscreteRV",boost::serialization::base_object<CDiscreteRV>(*this));
    ar & boost::serialization::make_nvp("p",p);
    ar & boost::serialization::make_nvp("cdf",cdf);
}


template <class Archive>
void CConstantCRV::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CContinuousRV",boost::serialization::base_object<CContinuousRV>(*this));
    ar & boost::serialization::make_nvp("xc",xc);
}


template <class Archive>
void CUniformCRV::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CContinuousRV",boost::serialization::base_object<CContinuousRV>(*this));
    ar & boost::serialization::make_nvp("a",a);
    ar & boost::serialization::make_nvp("b",b);
    ar & boost::serialization::make_nvp("d",d);
}


template <class Archive>
void CGaussianCRV::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CContinuousRV",boost::serialization::base_object<CContinuousRV>(*this));
    ar & boost::serialization::make_nvp("mu",mu);
    ar & boost::serialization::make_nvp("sigma",sigma);
    ar & boost::serialization::make_nvp("sigma2",sigma2);
    ar & boost::serialization::make_nvp("sigmasqrt2",sigmasqrt2);
}


template <class Archive>
void CExponentialCRV::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CContinuousRV",boost::serialization::base_object<CContinuousRV>(*this));
    ar & boost::serialization::make_nvp("lambda",lambda);
    ar & boost::serialization::make_nvp("beta",beta);
}


template <class Archive>
void CEmpiricalCRV::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CContinuousRV",boost::serialization::base_object<CContinuousRV>(*this));
    ar & boost::serialization::make_nvp("x",x);
    ar & boost::serialization::make_nvp("p",p);
    ar & boost::serialization::make_nvp("cdf",cdf);
    ar & boost::serialization::make_nvp("pbandwidth",pbandwidth);
    ar & boost::serialization::make_nvp("qbandwidth",qbandwidth);
    ar & boost::serialization::make_nvp("mu",mu);
    ar & boost::serialization::make_nvp("stdv",stdv);
}


void CConstantDRV::setDistribution(int _c,int _dp2)
{
    c=_c;
}


void CUniformDRV::setDistribution(int _a,int _b)
{
    if ( _a>=_b ) throw "CUniformDRV::setDistribution error: bad distribution parameters";
    a=_a; b=_b; n=b-a+1;
}


void CEmpiricalDRV::setDistribution(const CDiscreteSample &_aS)
{
    cout<<"in empiricalDRV setdistribution"<<endl;
    if ( _aS.sampleSize()==0 )  throw "CEmpiricalDRV::setDistribution error: sample is empty";
    ArrayXi _x;
    _aS.normalizedHistogram(_x,p);
    cdf=p; cumulativeSum(cdf);

    mu=_aS.mean();
    stdv=_aS.standardDeviation();
}


void CCategoricalDRV::setDistribution(const CDiscreteSample &_aS)
{
    if ( _aS.sampleSize()==0 )  throw "CCategoricalDRV::setDistribution error: sample is empty";
    ArrayXi _x;
    _aS.normalizedHistogram(_x,p);
    cdf=p; cumulativeSum(cdf);
}


double CEmpiricalDRV::doQuantile(double _p) const
{
    if (!cdf.data())  throw "CEmpiricalDRV::doQuantile error: empty distribution";
    const double *d=cdf.data();
    return double(lower_bound(d,d+cdf.size(),_p)-d);
}


double CCategoricalDRV::doQuantile(double _p) const
{
    if (!cdf.data())  throw "CCategoricalDRV::doQuantile error: empty distribution";
    const double *d=cdf.data();
    return double(lower_bound(d,d+cdf.size(),_p)-d);
}


void CConstantCRV::setDistribution(double _xc,double _dp2)
{
    xc=_xc;
}


void CUniformCRV::setDistribution(double _a,double _b)
{
    if ( _a>=_b ) throw "CUniformCRV::setDistribution error: bad distribution parameters";
    a=_a; b=_b; d=b-a;
}


void CGaussianCRV::setDistribution(double _mu,double _sigma)
{
    if ( fabs(_sigma)<TINY ) throw "CGaussianCRV::setDistribution error: bad distribution parameters";
    mu=_mu; sigma=_sigma; sigma2=_sigma*_sigma; sigmasqrt2=_sigma*sqrt(2.);
}


void CExponentialCRV::setDistribution(double _lambda,double _dp2)
{
    if ( fabs(_lambda)<TINY ) throw "CExponentialCRV::setDistribution error: bad distribution parameters";
    lambda=_lambda; beta=1./_lambda;
}


void CEmpiricalCRV::setDistribution(const CContinuousSample &_aS)
{
    int N=_aS.sampleSize();
    if ( N==0 ) throw "CEmpiricalCRV::setDistribution error: sample is empty";

    CFFTW fftw;

    ArrayXd initial_data;
    double range=_aS.normalizedHistogram(x,initial_data);
    int n=x.size();
    double iN=1./double(N),M_PI2=M_PI*M_PI;

    ArrayXd a = fftw.discreteCosineTransform(initial_data);

    ArrayXd i0n=ArrayXd::LinSpaced(n,0.,double(n-1));
    ArrayXd i2=i0n.tail(n-1)*i0n.tail(n-1);
    ArrayXd a2=0.25*a.tail(n-1)*a.tail(n-1);
    Cx_ZetaGammax xzg(N,i2,a2);
    double t_star;
    try          {   t_star=findRoot(xzg,iN*iN/9.,0.1);  }
    catch (...)  {   t_star = 0.28*pow(N,-0.4); cout<<"find root failed"<<endl; }

    ArrayXd a_t=a*exp(-0.5*i0n*i0n*M_PI2*t_star);
    p = fftw.inverseDiscreteCosineTransform(a_t); p/=range;
    p=p.cwiseMax(0.);
    pbandwidth = sqrt(t_star)*range;

    ArrayXd iaExp=i2*a2*exp(-i2*M_PI2*t_star);
    double t_cdf=pow(sqrt(M_PI)*(2.*M_PI2*iaExp.sum())*double(N), -2./3.);

    ArrayXd a_cdf=a*exp(-0.5*i0n*i0n*M_PI2*t_cdf);
    cdf = fftw.inverseDiscreteCosineTransform(a_cdf);
    cdf=cdf.cwiseMax(0.);
    cumulativeSum(cdf);
    double tmp=cdf(0);
    cdf-=tmp;
    tmp=cdf(cdf.size()-1);
    cdf/=tmp;
    qbandwidth = sqrt(t_cdf)*range;

    mu=_aS.mean();
    stdv=_aS.standardDeviation();
}


double CEmpiricalCRV::doQuantile(double _p)  const
{
    if (!cdf.data())  throw "CEmpiricalCRV::doQuantile error: empty distribution";
    return linearInterpolation(cdf,x,_p);
}


ArrayXi CConstantDRV::exportDomain() const
{
    ArrayXi x(3);
    x(0)=c-1;
    x(1)=c;
    x(2)=c+1;
    return x;
}


ArrayXi CUniformDRV::exportDomain() const
{
    ArrayXi x(n+2);
    for ( int i=0; i<n+2; i++ )   x(i)=a-1+i;
    return x;
}


ArrayXi CEmpiricalDRV::exportDomain() const
{
    ArrayXi x;
    int n=p.size();
    if (n>0) x=ArrayXi::LinSpaced(n+2,-1,n);
    return x;
}


ArrayXi CCategoricalDRV::exportDomain() const
{
    ArrayXi x;
    int n=p.size();
    if (n>0) x=ArrayXi::LinSpaced(n+2,-1,n);
    return x;
}


ArrayXd CConstantCRV::exportDomain() const
{
    ArrayXd x(5);
    x(0)=xc-1.;
    x(1)=xc-.001;
    x(2)=xc;
    x(3)=xc+.001;
    x(4)=xc+1.;
    return x;
}


ArrayXd CUniformCRV::exportDomain() const
{
    ArrayXd x(6);
    x(0)=a-1.;
    x(1)=a-.001;
    x(2)=a;
    x(3)=b;
    x(4)=b+.001;
    x(5)=b+1.;
    return x;
}


ArrayXd CGaussianCRV::exportDomain() const
{
    ArrayXd x=ArrayXd::LinSpaced(100,minValue(),maxValue());
    return x;
}


ArrayXd CExponentialCRV::exportDomain() const
{
    ArrayXd x=ArrayXd::LinSpaced(100,minValue(),maxValue());
    return x;
}


ArrayXd CEmpiricalCRV::exportDomain() const
{
    return x;
}
