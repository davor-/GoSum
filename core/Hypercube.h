#ifndef HYPERCUBE_H
#define HYPERCUBE_H


//#include "VariousMath.h"
#include "RandomGenerators.h"
#include "Model.h"



//! \brief
//! Namespace for GoSUM model.
namespace GoSUM {


class CModelHypercube;

class CHypercube {
public:
    enum hctype { dsample, montecarlo, cvoronoi, lcvoronoi };  //!< Enumerator for the possibe hypercube types.
protected:
    static hctype etype; //!< Holds hypercube type.
    static int maxiter,q; //!< Holds CVT parameters.
    static double alpha2,beta2; //!< Holds CVT parameters.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
public:
    CHypercube() {}
    virtual ~CHypercube() {}
public:
    static hctype Type(const std::string &_stype); //!< Returns hypercube type enumerator from hypercube type name.
    static hctype Type() { return etype; }  //!< Returns hypercube type.
    static void SetType(hctype _etype); //!< Sets hypercube type.
    static void VoronoiOptions(int &_maxiter,int &_q,double &_alpha2,double &_beta2) //!< Returns Voronoi options.
    {  _maxiter=maxiter; _q=q; _alpha2=alpha2; _beta2=beta2; }
    static void SetVoronoiOptions(int _maxiter,int _q,double _alpha2,double _beta2) //!< Sets Voronoi options.
    {  maxiter=_maxiter; q=_q; alpha2=_alpha2; beta2=_beta2; }
    static int CvtIterationSize() { return maxiter; }  //!< Returns CVT iteration size.
    static void SetCvtIterationSize(int _maxiter) { maxiter=_maxiter; }  //!< Sets CVT iteration size.
    static int CvtPointsSize() { return q; }  //!< Returns CVT points size.
    static void SetCvtPointsSize(int _q) { q=_q; }  //!< Sets CVT points size.
    static double CvtOldCenterCoefficient() { return alpha2; }  //!< Returns CVT parameter #1.
    static void SetCvtOldCenterCoefficient(double _alpha2) { alpha2=_alpha2; }  //!< Sets CVT paramter #1.
    static double CvtNewCenterCoefficient() { return beta2; }  //!< Returns CVT paramter #2.
    static void SetCvtNewCenterCoefficient(double _beta2) { beta2=_beta2; }  //!< Sets CVT paramter #2.
    static GoSUM::CModelHypercube *New(CInputParameters *_pIP); //!< Returns new model hypercube.
    static int ProgressSize(int _rssize,int _dim); //!< Returns progress steps size.
public:
    boost::signal<void()> generatingProgressed; //!< Signal for centralize progress.
};


class CModelHypercube : public CHypercube {
protected:
    CInputParameters *pIP; //!< Holds pointer to input parameters.
    CModelConstraints *pIC; //!< Holds pointer to input parameter constraints.
    int maxtries; //!< Holds maximal number of tries to satisfy model constraints.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
protected:
    ArrayXd &generateHCPoint(ArrayXd &x)  //!< Generates single hypercube point.
    { for ( int i=0; i<x.size(); i++ )    { x(i)=CRandomGenerator::Rnd(); } return x; }
    void generateHCPoints(int N,int dim,std::vector<ArrayXd> &y); //!< Generates N hypercube points.
    void generateModelPoints(int N,int dim,std::vector<ArrayXd> &y); //!< Generates N model points.
    void checkHCPoints(std::vector<ArrayXd> &y); //!< Checks if hypercube points when mapped to model points satisfy constraints.
    void hcPoints2ModelPoints(std::vector<ArrayXd> &y); //!< Maps hypercube points to model points.
    virtual void doGenerate(int _rssize,int _dim,std::vector<ArrayXd> &_samples)=0; //!< Core of the generation.
protected:
    CModelHypercube() : pIP(NULL), pIC(NULL), maxtries(100000) {}
public:
    CModelHypercube(CInputParameters *_pIP) : pIP(_pIP), pIC(pIP->constraints()), maxtries(100000) {}
    virtual ~CModelHypercube() {}
public:
    virtual void generate(int _rssize,int _dim,std::vector<ArrayXd> &_samples); //!< Generates _samples: _rssize model points (satisifing constraints) of dimension _dim.
};


class CDSampleHC : public CModelHypercube {
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
protected:
    virtual void doGenerate(int _rssize,int _dim,std::vector<ArrayXd> &_samples);
protected:
    CDSampleHC() : CModelHypercube() {}
public:
    CDSampleHC(CInputParameters *_pIP) : CModelHypercube(_pIP) {}
    virtual ~CDSampleHC() {}
};


class CMonteCarloHC : public CModelHypercube {
protected:
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
protected:
    virtual void doGenerate(int _rssize,int _dim,std::vector<ArrayXd> &_samples); //!< Core of the generation.
protected:
    CMonteCarloHC() : CModelHypercube() {}
public:
    CMonteCarloHC(CInputParameters *_pIP) : CModelHypercube(_pIP) {}
    virtual ~CMonteCarloHC() {}
};


class CCVoronoiHC : public CModelHypercube {
protected:
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
protected:
    virtual void doGenerate(int _rssize,int _dim,std::vector<ArrayXd> &_samples); //!< Core of the generation.
    void centralize(std::vector<ArrayXd> &_samples); //!< Centralizes model points _samples.
protected:
    CCVoronoiHC() : CModelHypercube() {}
public:
    CCVoronoiHC(CInputParameters *_pIP) : CModelHypercube(_pIP) {}
    virtual ~CCVoronoiHC() {}
};


class CLCVoronoiHC : public CCVoronoiHC {
protected:
    static int coo; //!< Used in compcoo.
    static bool compcoo(ArrayXd &a,ArrayXd &b) { return a(coo)<b(coo); } //!< Compare points by coo cordinate value.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
protected:
    virtual void doGenerate(int _rssize,int _dim,std::vector<ArrayXd> &_samples); //!< Core of the generation.
    void latinize(std::vector<ArrayXd> &_samples); //!< Latinizes model points _samples.
protected:
    CLCVoronoiHC() : CCVoronoiHC() {}
public:
    CLCVoronoiHC(CInputParameters *_pIP) : CCVoronoiHC(_pIP) {}
    virtual ~CLCVoronoiHC() {}
};


} // end of namespace GoSUM


BOOST_CLASS_EXPORT_KEY(GoSUM::CHypercube)
BOOST_CLASS_EXPORT_KEY(GoSUM::CModelHypercube)
BOOST_CLASS_EXPORT_KEY(GoSUM::CDSampleHC)
BOOST_CLASS_EXPORT_KEY(GoSUM::CMonteCarloHC)
BOOST_CLASS_EXPORT_KEY(GoSUM::CCVoronoiHC)
BOOST_CLASS_EXPORT_KEY(GoSUM::CLCVoronoiHC)


#endif // HYPERCUBE_H
