#ifndef RANDOMVARIABLE_H
#define RANDOMVARIABLE_H



#include "Sample.h"



#define rvExportMacro(F,f,arg) \
public: \
ArrayXd export##F(const Array<arg,Dynamic,1> &_x) const \
{   \
    int i,n=_x.size(); \
    ArrayXd _f(n); \
    for ( i=0; i<n; i++ ) _f(i)=f(_x(i)); \
    return _f;  \
}



//! \brief
//! Abstract base class for all random variables.
class CRandomVariable {
public:
    enum distributiontype { constantdiscrete, uniformdiscrete, empiricaldiscrete, categorical, constantcontinuous, uniformcontinuous, gaussian, exponential, empiricalcontinuous  };
public:
    static distributiontype Type(std::string _stype); //!< Converts distribution name (string) to distributiontype enumerator.
    static bool IsDistributionType(std::string _stype); //!< Returns true if it is a distribution type, false otherwise.
    static bool IsEmpirical(distributiontype _type); //!< Returns true if distribution type is empirical, false otherwise.
    static bool IsTheoretical(distributiontype _type); //!< Returns true if distribution type is theoretical, false otherwise.
    static bool IsDiscrete(distributiontype _type); //!< Returns true if distribution type is discrete, false otherwise.
    static bool IsContinuous(distributiontype _type); //!< Returns true if distribution type is continuous, false otherwise.
    static int DistributionParametersSize(distributiontype _type); //!< Returns size (number of) distribution parameters.
private:
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version) {}
public:
    CRandomVariable() {}
    virtual ~CRandomVariable() {}
    CRandomVariable(const CRandomVariable &O) {}
protected:
    virtual double doQuantile(double _p) const=0; //!< Quantile, formula implementation without checking argument.
public:
    virtual double quantile(double _p) const=0;  //!< Returns quantile (inverse cumulative distribution function, probit function) of _p in [0,1].
    virtual int expandedSize() const { return 1; } //! Returns number of variables after expansion for SVM and similar, 1 except in cateogrical variable.
    virtual double minValue() const=0; //!< Returns minimal value of the random variable.
    virtual double maxValue() const=0; //!< Returns maximal value of the random variable.
    virtual double expectedValue() const=0; //!< Returns expected value of the random variable.
    virtual double variance() const=0; //!< Returns variance of the random variable.
    virtual distributiontype distributionType() const=0;  //!< Returns enum type of the random variable distribution.
    virtual std::string distributionName() const=0;  //!< Returns name of the random variable distribution.
    virtual bool isDistributionDefined() const { return true; } //!< Returns true if distribution is defined, false otherwise.
};


//! \brief
//! Template of an abstract class - covers two types of random variables:
//! discrete (t=int,T=CDiscreteSample) or continuous (t=double,T=CContinuousSample).
template <typename t,class T>
class TRandomVariable : public CRandomVariable {
private:
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version)
    {   ar & boost::serialization::make_nvp("CRandomVariable",boost::serialization::base_object<CRandomVariable>(*this)); }
public:
    TRandomVariable() : CRandomVariable() {}
    virtual ~TRandomVariable() {}
    TRandomVariable(const TRandomVariable &O) : CRandomVariable(O) {}
public:
    virtual void setDistribution(t _dp1,t _dp2=0) {} //!< Sets distribution parameters of the random variable.
    virtual void setDistribution(const T &_aS) {} //!< Sets distribution of the random variable from sample.
    virtual double probability(t _x) const=0; //!< Returns probability mass function (PMF)  or probability density function (PDF) for the random variable value _x.
    virtual double cumulative(t _x) const=0; //!< Returns cumulative distribution function (CDF) for the random variable value _x.
    rvExportMacro(Probability,probability,t) //!< Exports probability for an array of x values.
    rvExportMacro(Cumulative,cumulative,t) //!< Exports cumulative for an array of x values.
    rvExportMacro(Quantile,quantile,double) //!< Exports quantile for an array of p values.
    virtual Array<t,Dynamic,1> exportDomain() const=0; //!< Exports domain of the random variable.
};


//! \brief
//! Abstract class for all discrete random variables
//! derived as specialization for t=int, T=TDiscreteSample.
typedef TRandomVariable<int,CDiscreteSample> CTDiscreteRV;
class CDiscreteRV : public CTDiscreteRV {
private:
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version)
    {   ar & boost::serialization::make_nvp("CTDiscreteRV",boost::serialization::base_object<CTDiscreteRV>(*this));  }
public:
    CDiscreteRV() : CTDiscreteRV() {}
    CDiscreteRV(const CDiscreteRV &O) : CTDiscreteRV(O) {}
public:
    virtual double quantile(double _p) const
    { if ( 0<=_p && _p<=1 ) return doQuantile(_p);
      else throw "CDiscreteRV::quantile error: probability must be in [0,1] interval"; }  //!< Quantile, only argument checking.
};


//! \brief
//! Abstract class for all continuous random variables
//! derived as specialization with t=double, T=CContinuousSample.
typedef TRandomVariable<double,CContinuousSample> CTContinuousRV;
class CContinuousRV : public CTContinuousRV {
private:
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version)
{   ar & boost::serialization::make_nvp("CTContinuousRV",boost::serialization::base_object<CTContinuousRV>(*this));  }
public:
    CContinuousRV() : CTContinuousRV() {}
    CContinuousRV(const CContinuousRV &O) : CTContinuousRV(O) {}
public:
    double probability(double _x1,double _x2) const { return cumulative(_x2)-cumulative(_x1); } //!< Returns probablilty that random variable has value between _x1 and _x2.
    virtual double quantile(double _p) const
    { if ( 0.<_p && _p<1. ) return doQuantile(_p);
      else throw "CContinuousRV::quantile error: probability must be in (0,1) interval"; }  //!< Quantile, only argument checking.
};


//! \brief
//! Class for constant discrete random variables
//! derived from discrete random variables.
class CConstantDRV : public CDiscreteRV {
private:
    int c; //!< Distribution parameter: constant integer value of the random variable.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
public:
    CConstantDRV() { setDistribution(0); }
    virtual ~CConstantDRV() {}
    CConstantDRV(const CConstantDRV &O) : CDiscreteRV(O) { setDistribution(O.c); }
protected:
    virtual double doQuantile(double _p) const { return double(c); } //!< Quantile, formula implementation.
public:
    virtual void setDistribution(int _c,int _dp2=0); //!< Set distribution parameter.
    virtual void setDistribution(const CDiscreteSample &_aS) { setDistribution(_aS.minValue()); } //!< Computes PMF and CDF from sample histogram.
    virtual double probability(int _k) const { return (_k==c)?1.:0.; } //!< Function that returns PMF.
    virtual double cumulative(int _k) const { return (_k>=c)?1.:0.; } //!< Function that returns CDF.
    virtual double minValue() const { return c; } //!< Returns minimal value of the random variable.
    virtual double maxValue() const { return c; } //!< Returns maximal value of the random variable.
    virtual double expectedValue() const { return c; } //!< Returns expected value of the random variable.
    virtual double variance() const { return 0.; } //!< Returns variance of the random variable.
    virtual distributiontype distributionType() const { return constantdiscrete; }  //!< Returns enum type of the random variable distribution.
    virtual std::string distributionName() const { return "constant discrete"; } //!< Returns name of the random variable distribution.
    virtual ArrayXi exportDomain() const; //!< Exports domain of the random variable.
    int constantValue() const { return c; } //!< Returns value of constant random variable.
    void setConstantValue(int _c)  { setDistribution(_c); } //!< Sets value of constant random variable.
};


//! \brief
//! Class for uniform discrete random variables
//! derived from discrete random variables.
class CUniformDRV : public CDiscreteRV {
private:
    int a,b,n;  //!< Distribution parameters: random variable can have one of the n=b-a+1 integer values between a and b.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
public:
    CUniformDRV() { setDistribution(1,2); }
    virtual ~CUniformDRV() {}
    CUniformDRV(const CUniformDRV &O) : CDiscreteRV(O) { setDistribution(O.a,O.b); }
protected:
    virtual double doQuantile(double _p) const { return floor(double(a)+double(n)*_p); } //!< Quantile, formula implementation.
public:
    virtual void setDistribution(int _a,int _b);  //!< Set distribution parameters.
    virtual double probability(int _k) const { return (a<=_k && _k<=b)?1./double(n):0.; }  //!< Function that returns PMF.
    virtual double cumulative(int _k) const { return (_k<a)?0.:((_k<b)?double(_k-a+1)/double(n):1.); } //!< Function that returns CDF.
    virtual double minValue() const { return a; } //!< Returns minimal value of the random variable.
    virtual double maxValue() const { return b; } //!< Returns maximal value of the random variable.
    virtual double expectedValue() const { return 0.5*double(a+b); } //!< Returns expected value of the random variable.
    virtual double variance() const { return double(b-a)*double(b-a)/12.; } //!< Returns variance of the random variable.
    virtual distributiontype distributionType() const { return uniformdiscrete; }  //!< Returns enum type of the random variable distribution.
    virtual std::string distributionName() const { return "uniform discrete"; } //!< Returns name of the random variable distribution.
    virtual ArrayXi exportDomain() const; //!< Exports domain of the random variable.
    int lowerBound() const { return a; } //!< Returns lower bound of the uniform radnom variable.
    int upperBound() const { return b; } //!< Returns upper bound of the uniform radnom variable.
    void setLowerBound(int _a)  { setDistribution(_a,b); } //!< Sets lower bound of the uniform radnom variable.
    void setUpperBound(int _b)  { setDistribution(a,_b); } //!< Sets upper bound of the uniform radnom variable.
};


//! \brief
//! Class for categorical discrete random variables
//! derived from discrete random variables.
class CEmpiricalDRV : public CDiscreteRV {
private:
    ArrayXd p,cdf; //!< PMF (p) and CDF (cdf) data.
    double mu,stdv;
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
public:
    CEmpiricalDRV():  mu(0.), stdv(0.) {}
    virtual ~CEmpiricalDRV() {}
    CEmpiricalDRV(const CEmpiricalDRV &O) : CDiscreteRV(O), p(O.p), cdf(O.cdf), mu(O.mu), stdv(O.stdv) {}
protected:
    virtual double doQuantile(double _p) const; //!< Quantile, formula implementation.
public:
    virtual void setDistribution(const CDiscreteSample &_aS); //!< Computes PMF and CDF from sample histogram.
    virtual double probability(int _k) const { return (0<=_k && _k<p.size()) ? p(_k) : 0.; } //!< Function that returns PMF.
    virtual double cumulative(int _k) const { return (0<=_k && _k<cdf.size()) ? cdf(_k) : ( (_k<0) ? 0. : 1. ); } //!< Function that returns CDF.
    //virtual int expandedSize() const { return int(p.size()); } //! After expansion, number of new variables is equal to the number of categories.
    virtual double minValue() const { return 0; } //!< Returns minimal value of the random variable.
    virtual double maxValue() const { return (p.size()==0)?0.:double(p.size()-1); } //!< Returns maximal value of the random variable.
    virtual double expectedValue() const { return mu; } //!< Returns expected value of the random variable.
    virtual double variance() const { return stdv*stdv; }  //!< Returns variance of the random variable.
    virtual distributiontype distributionType() const { return empiricaldiscrete; }  //!< Returns enum type of the random variable distribution.
    virtual std::string distributionName() const { return "empiricaldiscrete"; } //!< Returns name of the random variable distribution.
    virtual ArrayXi exportDomain() const; //!< Exports domain of the random variable.
    virtual bool isDistributionDefined() const { return (p.size()>0 && cdf.size()>0); } //!< Returns true if distribution is defined, false otherwise.
};


//! \brief
//! Class for categorical discrete random variables
//! derived from discrete random variables.
class CCategoricalDRV : public CDiscreteRV {
private:
    ArrayXd p,cdf; //!< PMF (p) and CDF (cdf) data.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
public:
    CCategoricalDRV() {}
    virtual ~CCategoricalDRV() {}
    CCategoricalDRV(const CCategoricalDRV &O) : CDiscreteRV(O), p(O.p), cdf(O.cdf) {}
protected:
    virtual double doQuantile(double _p) const; //!< Quantile, formula implementation.
public:
    virtual void setDistribution(const CDiscreteSample &_aS); //!< Computes PMF and CDF from sample histogram.
    virtual double probability(int _k) const { return (0<=_k && _k<p.size()) ? p(_k) : 0.; } //!< Function that returns PMF.
    virtual double cumulative(int _k) const { return (0<=_k && _k<cdf.size()) ? cdf(_k) : ( (_k<0) ? 0. : 1. ); } //!< Function that returns CDF.
    virtual int expandedSize() const { return int(p.size()); } //! After expansion, number of new variables is equal to the number of categories.
    virtual double minValue() const { return 0; } //!< Returns minimal value of the random variable.
    virtual double maxValue() const { return (p.size()==0)?0.:double(p.size()-1); } //!< Returns maximal value of the random variable.
    virtual double expectedValue() const { return (p.size()==0)?0.:int((p.size()-1)*0.5); } //!< Returns expected value of the random variable.
    virtual double variance() const { return 1.; }  //!< Returns variance of the random variable.
    virtual distributiontype distributionType() const { return categorical; }  //!< Returns enum type of the random variable distribution.
    virtual std::string distributionName() const { return "categorical"; } //!< Returns name of the random variable distribution.
    virtual ArrayXi exportDomain() const; //!< Exports domain of the random variable.
    virtual bool isDistributionDefined() const { return (p.size()>0 && cdf.size()>0); } //!< Returns true if distribution is defined, false otherwise.
};


//! \brief
//! Class for constant continuous random variables
//! derived from continuous random variables.
class CConstantCRV : public CContinuousRV {
private:
    double xc; //!< Distribution parameter: constant integer value of the random varialbe.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
public:
    CConstantCRV() { setDistribution(0.); }
    virtual ~CConstantCRV() {}
    CConstantCRV(const CConstantCRV &O) : CContinuousRV(O) { setDistribution(O.xc); }
protected:
    virtual double doQuantile(double _p) const { return xc; } //!< Quantile, formula implementation.
public:
    virtual void setDistribution(double _xc,double _dp2=0); //!< Set distribution parameter.
    virtual void setDistribution(const CContinuousSample &_aS) { setDistribution(_aS.minValue()); } //!< Set distribution parameters from sample empirical parameters.
    virtual double probability(double _x) const { return (_x==xc)?1./*std::numeric_limits<double>::infinity()*/:0.; } //!< Function that returns PMF.
    virtual double cumulative(double _x) const { return (_x>=xc)?1.:0.; } //!< Function that returns CDF.
    virtual double minValue() const { return xc; } //!< Returns minimal value of the random variable.
    virtual double maxValue() const { return xc; } //!< Returns maximal value of the random variable.
    virtual double expectedValue() const { return xc; } //!< Returns expected value of the random variable.
    virtual double variance() const { return 0.; } //!< Returns variance of the random variable.
    virtual distributiontype distributionType() const { return constantcontinuous; }  //!< Returns enum type of the random variable distribution.
    virtual std::string distributionName() const { return "constant continuous"; } //!< Returns name of the random variable distribution.
    virtual ArrayXd exportDomain() const; //!< Exports domain of the random variable.
    double constantValue() const { return xc; } //!< Returns value of constant random variable.
    void setConstantValue(double _xc)  { setDistribution(_xc); } //!< Sets value of constant random variable.
};


//! \brief
//! Class for uniform continuous random variables
//! derived from continuous random variables.
class CUniformCRV : public CContinuousRV {
private:
    double a,b,d; //!< Distribution parameters: random variable can have any value between a and b, (d=b-a).
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
public:
    CUniformCRV() { setDistribution(0.,1.); }
    virtual ~CUniformCRV() {}
    CUniformCRV(const CUniformCRV &O) : CContinuousRV(O) { setDistribution(O.a,O.b); }
protected:
    virtual double doQuantile(double _p) const { return a+d*_p; } //!< Quantile, formula implementation.
public:
    virtual void setDistribution(double _a,double _b);  //!< Set distribution parameters.
    virtual void setDistribution(const CContinuousSample &_aS) { setDistribution(_aS.minValue(),_aS.maxValue()); } //!< Set distribution parameters from sample empirical parameters.
    virtual double probability(double _x) const { return (a<=_x && _x<=b)?1./d:0.; } //!< Function that returns PDF.
    virtual double cumulative(double _x) const { return (_x<a)?0.:((_x<b)?(_x-a)/d:1.); } //!< Function that returns CDF.
    virtual double minValue() const { return a; } //!< Returns minimal value of the random variable.
    virtual double maxValue() const { return b; } //!< Returns maximal value of the random variable.
    virtual double expectedValue() const { return 0.5*(a+b); } //!< Returns expected value of the random variable.
    virtual double variance() const { return d*d/12.; } //!< Returns variance of the random variable.
    virtual distributiontype distributionType() const { return uniformcontinuous; }  //!< Returns enum type of the random variable distribution.
    virtual std::string distributionName() const { return "uniform continuous"; } //!< Returns name of the random variable distribution.
    virtual ArrayXd exportDomain() const; //!< Exports domain of the random variable.
    double lowerBound() const { return a; } //!< Returns lower bound of the uniform radnom variable.
    double upperBound() const { return b; } //!< Returns upper bound of the uniform radnom variable.
    void setLowerBound(double _a)  { setDistribution(_a,b); } //!< Sets lower bound of the uniform radnom variable.
    void setUpperBound(double _b)  { setDistribution(a,_b); } //!< Sets upper bound of the uniform radnom variable.
};


//! \brief
//! Class for Gaussian continuous random variables
//! derived from continuous random variables.
class CGaussianCRV : public CContinuousRV {
private:
    double mu,sigma,sigma2,sigmasqrt2;  //!< Distribution parameters: mean (mu) and standard deviation (sigma).
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
public:
    CGaussianCRV() { setDistribution(0.,1.); }
    virtual ~CGaussianCRV() {}
    CGaussianCRV(const CGaussianCRV &O) : CContinuousRV(O) { setDistribution(O.mu,O.sigma);}
protected:
    virtual double doQuantile(double _p) const { return mu+sigmasqrt2*boost::math::erf_inv(2.*_p-1.); } //!< Quantile, formula implementation.
public:
    virtual void setDistribution(double _mu,double _sigma);  //!< Set distribution parameters.
    virtual void setDistribution(const CContinuousSample &_aS) { setDistribution(_aS.mean(),_aS.standardDeviation()); } //!< Set distribution parameters from sample empirical parameters.
    virtual double probability(double _x) const { return exp(-0.5*(_x-mu)*(_x-mu)/sigma2)/(sigmasqrt2*sqrt(M_PI)); } //!< Function that returns PDF.
    virtual double cumulative(double _x) const { return 0.5*(1.+boost::math::erf((_x-mu)/sigmasqrt2)); }  //!< Function that returns CDF.
    virtual double minValue() const { return mu-3.*sigma; } //!< Returns minimal value of the random variable.
    virtual double maxValue() const { return mu+3.*sigma; } //!< Returns maximal value of the random variable.
    virtual double expectedValue() const { return mu; } //!< Returns expected value of the random variable.
    virtual double variance() const { return sigma*sigma; }  //!< Returns variance of the random variable.
    virtual distributiontype distributionType() const { return gaussian; }  //!< Returns enum type of the random variable distribution.
    virtual std::string distributionName() const { return "gaussian"; } //!< Returns name of the random variable distribution.
    virtual ArrayXd exportDomain() const; //!< Exports domain of the random variable.
    double mean() const { return mu; } //!< Returns mean of Gaussian random variable.
    double standardDeviation() const { return sigma; } //!< Returns standard deviation of Gaussian random variable.
    void setMean(double _mu)  { setDistribution(_mu,sigma); } //!< Sets mean of Gaussian random variable.
    void setStandardDeviation(double _sigma)  { setDistribution(mu,_sigma); } //!< Sets standard deviation of Gaussian random variable.
};


//! \brief
//! Class for exponential continuous random variables
//! derived from continuous random variables.
class CExponentialCRV : public CContinuousRV {
private:
    double lambda,beta; //!< Distribution parameters: rate parameter beta=1/lambda.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
public:
    CExponentialCRV() { setDistribution(1.); }
    virtual ~CExponentialCRV() {}
    CExponentialCRV(const CExponentialCRV &O) : CContinuousRV(O) { setDistribution(O.lambda);}
protected:
    virtual double doQuantile(double _p) const { return -beta*log(1.-_p); } //!< Quantile, formula implementation.
public:
    virtual void setDistribution(double _lambda,double _dp2=0); //!< Set distribution parameters.
    virtual void setDistribution(const CContinuousSample &_aS) { setDistribution(1./_aS.mean()); } //!< Set distribution parameters from sample empirical parameters.
    virtual double probability(double _x) const { return (_x>=0.)?lambda*exp(-lambda*_x):0.; } //!< Function that returns PDF.
    virtual double cumulative(double _x) const { return (_x>=0.)?(1.-exp(-lambda*_x)):0.; } //!< Function that returns CDF.
    virtual double minValue() const { return 0.; } //!< Returns minimal value of the random variable.
    virtual double maxValue() const { return quantile(0.99); } //!< Returns maximal value of the random variable.
    virtual double expectedValue() const { return beta; } //!< Returns expected value of the random variable.
    virtual double variance() const { return beta*beta; }  //!< Returns variance of the random variable.
    virtual distributiontype distributionType() const { return exponential; }  //!< Returns enum type of the random variable distribution.
    virtual std::string distributionName() const { return "exponential"; } //!< Returns name of the random variable distribution.
    virtual ArrayXd exportDomain() const; //!< Exports domain of the random variable.
    double rateParameter() const { return lambda; } //!< Returns rate parameter of exponential random variable.
    void setRateParameter(double _lambda)  { setDistribution(_lambda); } //!< Sets rate parameter of exponential random variable.
};


//! \brief
//! Class for empirical continuous random variables
//! derived from continuous random variables.
class CEmpiricalCRV : public CContinuousRV {
private:
    ArrayXd x,p,cdf;  //!< PDF (x,p) and CDF (x,cdf) data.
    double pbandwidth,qbandwidth; //!< Bandwidths of PDF and CDF, as computed in kernel density estimator.
    double mu,stdv;
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
public:
    CEmpiricalCRV() : pbandwidth(0.), qbandwidth(0.), mu(0.), stdv(0.) {}
    virtual ~CEmpiricalCRV() {}
    CEmpiricalCRV(const CEmpiricalCRV &O) : CContinuousRV(O), x(O.x), p(O.p), cdf(O.cdf),
        pbandwidth(O.pbandwidth), qbandwidth(O.qbandwidth), mu(O.mu), stdv(O.stdv) {}
protected:
    virtual double doQuantile(double _p)  const; //!< Quantile, formula implementation.
public:
    virtual void setDistribution(const CContinuousSample &_aS); //!< Computes PMF and CDF from sample histogram, application of kernel densitiy estimation algorithm.
    virtual double probability(double _x)  const { return linearInterpolation(x,p,_x); } //!< Function that returns PDF.
    virtual double cumulative(double _x)  const { return linearInterpolation(x,cdf,_x); } //!< Function that returns CDF.
    virtual double minValue() const { return (x.size()==0)?0.:x(0); } //!< Returns minimal value of the random variable.
    virtual double maxValue() const { return (x.size()==0)?0.:x(x.size()-1); } //!< Eeturns maximal value of the random variable.
    virtual double expectedValue() const { return mu;/*(x.size()==0)?0.:0.5*(x(0)+x(x.size()-1));*/ } //!< Returns expected value of the random variable.
    virtual double variance() const { return stdv*stdv; }  //!< Returns variance of the random variable.
    virtual distributiontype distributionType() const { return empiricalcontinuous; }  //!< Returns enum type of the random variable distribution.
    virtual std::string distributionName() const { return "empirical continuous"; } //!< Returns name of the random variable distribution.
    virtual ArrayXd exportDomain() const; //!< Exports domain of the random variable.
    virtual bool isDistributionDefined() const { return (x.size()>0 && p.size()>0 && cdf.size()>0); }  //!< Returns true if distribution is defined, false otherwise.
    double mean() const { return mu; } //!< Returns mean of empirical random variable.
    double standardDeviation() const { return stdv; } //!< Returns standard deviation of empirical random variable.
};


BOOST_CLASS_EXPORT_KEY(CConstantDRV)
BOOST_CLASS_EXPORT_KEY(CUniformDRV)
BOOST_CLASS_EXPORT_KEY(CEmpiricalDRV)
BOOST_CLASS_EXPORT_KEY(CCategoricalDRV)
BOOST_CLASS_EXPORT_KEY(CConstantCRV)
BOOST_CLASS_EXPORT_KEY(CUniformCRV)
BOOST_CLASS_EXPORT_KEY(CGaussianCRV)
BOOST_CLASS_EXPORT_KEY(CExponentialCRV)
BOOST_CLASS_EXPORT_KEY(CEmpiricalCRV)


#endif // RANDOMVARIABLE_H
