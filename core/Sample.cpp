

#include "Sample.h"


BOOST_CLASS_EXPORT_IMPLEMENT(CDiscreteSample)
BOOST_CLASS_EXPORT_IMPLEMENT(CContinuousSample)
BOOST_CLASS_EXPORT_IMPLEMENT(CNumericalSample)
BOOST_CLASS_EXPORT_IMPLEMENT(CCategoricalSample)


int CSample::maxdiscretesize=11;

template <class Archive>
void CDiscreteSample::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("intTSample",boost::serialization::base_object<CTDiscreteSample>(*this));
}


template <class Archive>
void CContinuousSample::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("doubleTSample",boost::serialization::base_object<CTContinuousSample>(*this));
}


template <class Archive>
void CNumericalSample::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CDiscreteSample",boost::serialization::base_object<CDiscreteSample>(*this));
    ar & boost::serialization::make_nvp("values",values);
}


template <class Archive>
void CCategoricalSample::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CDiscreteSample",boost::serialization::base_object<CDiscreteSample>(*this));
    ar & boost::serialization::make_nvp("categories",categories);
}


void CNumericalSample::readSampleValue(std::ifstream &_ifs,int _at)
{
    double val;
    _ifs>>val;
    if ( _ifs.fail() )  throw "CNumericalSample::readSampleValue error: std::ifstream fail";
    std::vector<double>::iterator it=std::find(values.begin(),values.end(),val);
    if ( it==values.end() )
    {   values.push_back(val);
        sample[_at]=int(values.size())-1;  }
    else  sample[_at]=int(it-values.begin());
}


void CCategoricalSample::readSampleValue(std::ifstream &_ifs,int _at)
{
    std::string ctg;
    _ifs>>ctg;
    if ( _ifs.fail() )  throw "CCategoricalSample::readSampleValue error: std::ifstream fail";
    std::vector<std::string>::iterator it=std::find(categories.begin(),categories.end(),ctg);
    if ( it==categories.end() )
    {   categories.push_back(ctg);
        sample[_at]=int(categories.size())-1;  }
    else  sample[_at]=int(it-categories.begin());
}


void CDiscreteSample::computeNormalizedHistogram(int _n)
{
    int i,N=int(sample.size()),n;
    if ( N==0 ) return;

    std::vector<int> _x(sample.data(),sample.data()+sample.size());
    sort(_x.begin(),_x.end());

    if ( _x.front()==_x.back() )
    {
        x=ArrayXi::Constant(1,_x.front());
        H=ArrayXd::Constant(1,1.);
        range=0;
    }
    else
    {
        std::vector<double> _H;
        _H.push_back(1.);
        for ( i=1; i<N; i++ )
        {   if ( _x[i]==_x[i-1] )  _H.back()+=1.;
            else                   _H.push_back(1.);  }
        n=int(std::unique(_x.begin(),_x.end())-_x.begin());
        x=ArrayXi::Map(&_x.front(),n);
        H=ArrayXd::Map(&_H.front(),n);
        H/=double(N);
        range=_x.back()-_x.front();
    }

    cH=H;
    cumulativeSum(cH);
}


void CDiscreteSample::computeNormalizedSubHistogram(ArrayXd &_subH,const std::vector<int> &subset) const
{
    int N=int(sample.size());
    if ( N==0 ) return;

    int i,j,n=x.size(),m=subset.size();
    _subH=ArrayXd::Zero(n);
    std::vector<int> _x(x.data(),x.data()+x.size());

    for ( i=0; i<m; i++ )
    {
        if (  0<=subset[i] && subset[i]<N )
        {
            j=std::find(_x.begin(),_x.end(),sample(subset[i]))-_x.begin();
            if ( j<n ) _subH(j)+=1.;
        }
    }
    _subH/=double(N);
}


void CDiscreteSample::exportSubHistogram(ArrayXd &_subH,const std::vector<int> &subset) const
{
    if ( x.size()==0 )
    {
        _subH.resize(0);
    }
    else if ( range==0. )
    {
        _subH.resize(1);
        _subH(0)=double(subset.size())/double(sample.size());
    }
    else
    {
        computeNormalizedSubHistogram(_subH,subset);
        _subH=_subH*(double(H.size()-1)/range);
    }
}


bool CContinuousSample::hasDiscreteData()
{
    int N=int(sample.size()),n;
    if ( N==0 ) return false;
    std::vector<double> _x(sample.data(),sample.data()+sample.size());
    sort(_x.begin(),_x.end());
    n=int(std::unique(_x.begin(),_x.end())-_x.begin());
    return ( n<=maxdiscretesize );
}


CNumericalSample::CNumericalSample(const CContinuousSample &O)
{
    int i,N=int(O.sampleSize());
    double val;
    values.resize(0);
    sample.resize(N);
    for ( i=0; i<N; i++ )
    {
        val=O.sampleValue(i);
        std::vector<double>::iterator it=std::find(values.begin(),values.end(),val);
        if ( it==values.end() )
        {   values.push_back(val);
            sample[i]=int(values.size())-1;
        }
        else
        {
            sample[i]=int(it-values.begin());
        }
    }
}


void CContinuousSample::computeNormalizedHistogram(int _n)
{
    int N=int(sample.size());
    if ( N==0 ) return;
    if ( _n<=0 ) throw "CContinuousSample::computeNormalizedHistogram error: bad histogram size";

    std::vector<double> _x(sample.data(),sample.data()+sample.size());
    sort(_x.begin(),_x.end());
    double mind=_x.front();
    double maxd=_x.back();
    range=maxd-mind;

    if ( fabs(range)<TINY )
    {
        x=ArrayXd::Constant(1,mind);
        H=ArrayXd::Constant(1,1.);
        range=0.;
    }
    else
    {
        int i,n = 1 << std::max(2,int(ceil(log(double(_n))/log(2.))+0.5));
        double dx =range/double(n-1);
        x.setLinSpaced(n,mind-0.5*range/double(N),maxd+0.5*range/double(N));
        H=ArrayXd::Zero(n);
        for ( i=0; i<N; i++ ) H(int(0.5 + floor((sample(i)-mind)/dx)))++;
        H /= double(N);
    }

    cH=H;
    cumulativeSum(cH);
}


void CContinuousSample::computeNormalizedSubHistogram(ArrayXd &_subH,const std::vector<int> &subset) const
{
    int N=int(sample.size());
    if ( N==0 ) return;

    int i,j,n=x.size(),m=subset.size();
    double mind=x(0)+0.5*range/double(N);
    double dx =range/double(n-1);

    _subH=ArrayXd::Zero(n);

    for ( i=0; i<m; i++ )
    {
        if (  0<=subset[i] && subset[i]<N )
        {
            _subH(int(0.5 + floor((sample(subset[i])-mind)/dx)))++;
        }
    }
    _subH/=double(N);
}


void CContinuousSample::exportSubHistogram(ArrayXd &_subH,const std::vector<int> &subset) const
{
    if ( x.size()==0 )
    {
        _subH.resize(0);
    }
    else if ( range==0. )
    {
        _subH.resize(1);
        _subH(0)=double(subset.size())/double(sample.size());
    }
    else
    {
        computeNormalizedSubHistogram(_subH,subset);
        _subH=_subH*(double(H.size()-1)/range);
    }
}


void CNumericalSample::computeMeanAndStandardDeviation()
{
    int i, N=int(sample.size());
    if ( N==0 ) return; //throw "CNumericalSample::computeMeanAndStandardDeviation error: empty sample";
    for ( mu=0., i=0; i<N; i++ ) mu+=sample(i);
    mu/=double(N);
    for ( stdv=0., i=0; i<N; i++ ) stdv+=(sample(i)-mu)*(sample(i)-mu);
    stdv=sqrt(stdv/double(N));
}


void CContinuousSample::computeMeanAndStandardDeviation()
{
    int i, N=int(sample.size());
    if ( N==0 ) return; //throw "CContinuousSample::computeMeanAndStandardDeviation error: empty sample";
    for ( mu=0., i=0; i<N; i++ ) mu+=sample(i);
    mu/=double(N);
    for ( stdv=0., i=0; i<N; i++ ) stdv+=(sample(i)-mu)*(sample(i)-mu);
    stdv=sqrt(stdv/double(N));
}


