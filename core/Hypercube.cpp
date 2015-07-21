


#include "Hypercube.h"
#include "Model.h"





GoSUM::CHypercube::hctype GoSUM::CHypercube::etype=GoSUM::CHypercube::dsample;
int GoSUM::CHypercube::maxiter=10000;
int GoSUM::CHypercube::q=1;
double GoSUM::CHypercube::alpha2=0.;
double GoSUM::CHypercube::beta2=1.;

int GoSUM::CLCVoronoiHC::coo;


BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CHypercube)
BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CModelHypercube)
BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CDSampleHC)
BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CMonteCarloHC)
BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CCVoronoiHC)
BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CLCVoronoiHC)


template <class Archive>
void GoSUM::CHypercube::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("etype",etype);
    ar & boost::serialization::make_nvp("maxiter",maxiter);
    ar & boost::serialization::make_nvp("q",q);
    ar & boost::serialization::make_nvp("alpha2",alpha2);
    ar & boost::serialization::make_nvp("beta2",beta2);
}


template <class Archive>
void GoSUM::CModelHypercube::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CHypercube",boost::serialization::base_object<CHypercube>(*this));
    ar & boost::serialization::make_nvp("pIP",pIP);
    ar & boost::serialization::make_nvp("pIC",pIC);
    ar & boost::serialization::make_nvp("maxtries",maxtries);
}


template <class Archive>
void GoSUM::CDSampleHC::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CModelHypercube",boost::serialization::base_object<CModelHypercube>(*this));
}


template <class Archive>
void GoSUM::CMonteCarloHC::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CModelHypercube",boost::serialization::base_object<CModelHypercube>(*this));
}


template <class Archive>
void GoSUM::CCVoronoiHC::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CModelHypercube",boost::serialization::base_object<CModelHypercube>(*this));
}


template <class Archive>
void GoSUM::CLCVoronoiHC::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CCVoronoiHC",boost::serialization::base_object<CCVoronoiHC>(*this));
    ar & boost::serialization::make_nvp("coo",coo);
}


GoSUM::CHypercube::hctype GoSUM::CHypercube::Type(const std::string &_stype)
{
    if ( _stype=="dsample" )              return dsample;
    else if ( _stype=="montecarlo" )      return montecarlo;
    else if ( _stype=="cvoronoi" )        return cvoronoi;
    else if ( _stype=="lcvoronoi" )       return lcvoronoi;
    else  throw "CHypercube::Type error: unrecognised type";
}


void GoSUM::CHypercube::SetType(hctype _etype)
{
    etype=_etype;
    if ( etype==dsample )   CRandomGenerator::Set(CRandomGenerator::dynamicsystem);
    else                    CRandomGenerator::Set(CRandomGenerator::largeperiod);
}


GoSUM::CModelHypercube *GoSUM::CHypercube::New(CInputParameters* _pIP)
{
    if ( etype==dsample )              return new GoSUM::CDSampleHC(_pIP);
    else if ( etype==montecarlo )      return new GoSUM::CMonteCarloHC(_pIP);
    else if ( etype==cvoronoi )        return new GoSUM::CCVoronoiHC(_pIP);
    else if ( etype==lcvoronoi )       return new GoSUM::CLCVoronoiHC(_pIP);
    else  throw "GoSUM::CModelHypercube::New error: unrecognised type";
}


int GoSUM::CHypercube::ProgressSize(int _rssize,int _dim)
{
    if ( etype==dsample )              return _rssize+1;
    else if ( etype==montecarlo )      return _rssize+1;
    else if ( etype==cvoronoi )        return 1+maxiter+1;
    else if ( etype==lcvoronoi )       return 1+maxiter+_dim+1+1;
    else  throw "GoSUM::CModelHypercube::CvtProgressSize error: unrecognised type";
}


void GoSUM::CModelHypercube::generateHCPoints(int _N,int _dim,std::vector<ArrayXd> &y)
{
    int count=0;
    maxtries=_N*100;

    ArrayXd x(_dim);
    do
    {
        generateHCPoint(x);
        if ( pIC->constraintsSatisfied(pIP->hcPoint2ModelPoint(x)) )  y.push_back(x);
    }
    while ( y.size()<_N && (++count)<maxtries );

    if ( y.size()<_N )  y.clear();
}


void GoSUM::CModelHypercube::generateModelPoints(int _N,int _dim,std::vector<ArrayXd> &y)
{
    int count=0;
    maxtries=_N*100;

    ArrayXd x(_dim);
    do
    {
        x=pIP->hcPoint2ModelPoint(generateHCPoint(x));
        if ( pIC->constraintsSatisfied(x) )
        {
            y.push_back(x);
            generatingProgressed();
        }
    }
    while ( y.size()<_N && (++count)<maxtries );

    if ( y.size()<_N )  y.clear();
}


void GoSUM::CModelHypercube::checkHCPoints(std::vector<ArrayXd> &y)
{
    int i,N=y.size();
    for ( i=0; i<N; i++ )
    {  if ( !pIC->constraintsSatisfied(pIP->hcPoint2ModelPoint(y[i])) )  break;  }

    if ( i<N )  y.clear();
}


void GoSUM::CModelHypercube::hcPoints2ModelPoints(std::vector<ArrayXd> &y)
{
    int i,N=y.size();
    ArrayXd x;
    for ( i=0; i<N; i++ )
    {
        x=pIP->hcPoint2ModelPoint(y[i]);
        if ( pIC->constraintsSatisfied(x) )  y[i]=x;
        else break;
    }

    if ( i<N )  y.clear();
}


void GoSUM::CModelHypercube::generate(int _rssize,int _dim,std::vector<ArrayXd> &_samples)
{
    if ( _rssize<=0 ) throw "GoSUM::CModelHypercube::generate error: bad number of points";
    if ( _dim<=0 ) throw "GoSUM::CModelHypercube::generate error: bad dimension";

    _samples.clear();

    CRandomGenerator::rngtype oldrngtype=CRandomGenerator::Type();

    doGenerate(_rssize,_dim,_samples);

    CRandomGenerator::Set(oldrngtype);
}


void GoSUM::CDSampleHC::doGenerate(int _rssize,int _dim,std::vector<ArrayXd> &_samples)
{
    CRandomGenerator::SetDynamicSystem(_dim);
    generateModelPoints(_rssize,_dim,_samples);
}


void GoSUM::CMonteCarloHC::doGenerate(int _rssize,int _dim,std::vector<ArrayXd> &_samples)
{
    CRandomGenerator::SetLargePeriod();
    generateModelPoints(_rssize,_dim,_samples);
}


void GoSUM::CCVoronoiHC::centralize(std::vector<ArrayXd> &_samples)
{
    int _rssize=_samples.size(),_dim=_samples[0].size();
    double alpha1=1.-alpha2;
    double beta1=1.-beta2;
    int iter,i,j,inear;

    ArrayXi J=ArrayXi::Ones(_rssize);

    for ( iter=0; iter<maxiter; iter++ )
    {
        std::vector<ArrayXd> u(_rssize,ArrayXd::Zero(_dim));
        ArrayXi I=ArrayXi::Zero(_rssize);

        int ysize=q;
        std::vector<ArrayXd> y;
        generateHCPoints(ysize,_dim,y);

        if ( !y.empty() )
        {
            std::vector<ArrayXd> tmp=_samples;

            for ( j=0; j<ysize; j++ )
            {   inear=findNearest(tmp,y[j]);
                u[inear]=(I[inear]*u[inear]+y[j])/double(I[inear]+1);
                I[inear]++;
            }

            for ( i=0; i<_rssize; i++ )
            {   if ( I[i]>0 )
                {   tmp[i]=((alpha1*J[i]+beta1)*tmp[i]+(alpha2*J[i]+beta2)*u[i])/double(J[i]+1);
                    J[i]++;
                }
            }

            checkHCPoints(tmp);
            if (!tmp.empty()) _samples=tmp;
        }
        generatingProgressed();
    }
}


void GoSUM::CCVoronoiHC::doGenerate(int _rssize,int _dim,std::vector<ArrayXd> &_samples)
{
    CRandomGenerator::SetLargePeriod();
    generateHCPoints(_rssize,_dim,_samples);
    generatingProgressed();

    if ( !_samples.empty() )
    {
        centralize(_samples);
        hcPoints2ModelPoints(_samples);
    }
}


void GoSUM::CLCVoronoiHC::latinize(std::vector<ArrayXd> &_samples)
{
    int _rssize=int(_samples.size()),_dim=int(_samples[0].size());
    int i;
    for ( coo=0; coo<_dim; coo++ )
    {
        if ( !pIC->isConstrained(coo) )
        {
            sort(_samples.begin(),_samples.end(),CLCVoronoiHC::compcoo);
            for ( i=0; i<_rssize; i++ )  _samples[i](coo)=(double(i+1)-CRandomGenerator::Rnd())/double(_rssize);
        }
        generatingProgressed();
    }
}


void GoSUM::CLCVoronoiHC::doGenerate(int _rssize,int _dim,std::vector<ArrayXd> &_samples)
{
    CRandomGenerator::SetLargePeriod();
    generateHCPoints(_rssize,_dim,_samples);
    generatingProgressed();

    if ( !_samples.empty() )
    {
        centralize(_samples);
        latinize(_samples);
        hcPoints2ModelPoints(_samples);
        generatingProgressed();
    }
}










