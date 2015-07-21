


#include "Reduction.h"


enum GoSUM::CReduction::reductiontype GoSUM::CReduction::redtype=GoSUM::CReduction::derivative;
enum GoSUM::CReduction::cutofftype GoSUM::CReduction::cuttype=GoSUM::CReduction::cutoffsize;


BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CReduction)


template <class Archive>
void GoSUM::CReduction::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("redtype",redtype);
    ar & boost::serialization::make_nvp("cuttype",cuttype);
    ar & boost::serialization::make_nvp("pIP",pIP);
    ar & boost::serialization::make_nvp("pOS",pOS);
    ar & boost::serialization::make_nvp("pAM",pAM);
    ar & boost::serialization::make_nvp("pSA",pSA);
    ar & boost::serialization::make_nvp("rankIP",rankIP);
    ar & boost::serialization::make_nvp("rankOS",rankOS);
    ar & boost::serialization::make_nvp("cutval",cutval);
    ar & boost::serialization::make_nvp("cutip",cutip);
    ar & boost::serialization::make_nvp("cutos",cutos);
}


bool compareFirst(std::pair<double,int> pair1,std::pair<double,int> pair2)
{
    return pair1.first>pair2.first;
}


void GoSUM::CReduction::selectInputParameters()
{
    if ( cuttype==cutoffsize )
    {
        int i,Isize=pIP->size();
        std::vector< std::pair<double,int> > rnk(Isize);
        for ( i=0; i<Isize; i++ )
        {   rnk[i].first=rankIP[i].first;
            rnk[i].second=i;
            rankIP[i].second=false; }

        std::sort(rnk.begin(),rnk.end(),compareFirst);

        for ( i=0; i<cutip; i++ )
            rankIP[rnk[i].second].second=true;

    }
    else if ( cuttype==cutoffvalue )
    {
        cutip=0;
        int i,Isize=pIP->size();
        for ( i=0; i<Isize; i++ )
        {
            if ( rankIP[i].first>=cutval ) { rankIP[i].second=true; cutip++;  }
            else                           { rankIP[i].second=false; }
        }

    }
}

void GoSUM::CReduction::evaluateMaximalSensitivities()
{
    if ( pSA->empty() )  throw "GoSUM::CReduction::evaluateMaximalSensitivities error: empty sensitivities";

    int i,j,Isize=pIP->size(),Osize=pOS->size();

    ArrayXXd A;
    if ( redtype==derivative )  A=pSA->derivativeSensitivity();
    else if ( redtype==averagederivative )  A=pSA->averageDerivative();
    else if ( redtype==absoluteaveragederivative )  A=pSA->absoluteAverageDerivative();
    else if ( redtype==variance )  A=pSA->varianceSensitivity();
    else if ( redtype==anova )  A=pSA->firstOrderANOVA();

    for ( i=0; i<Isize; i++ )
    {
        rankIP[i].first=0.;
        for ( j=0; j<Osize; j++ )
        {   if ( rankOS[j] )
            { rankIP[i].first=std::max(rankIP[i].first,A(j,i));  }   }
    }

}


GoSUM::CReduction::reductiontype GoSUM::CReduction::ReductionType(const std::string &_stype)
{
    if ( _stype=="derivative" )                          return derivative;
    else if ( _stype=="averagederivative" )              return averagederivative;
    else if ( _stype=="absoluteaveragederivative" )      return absoluteaveragederivative;
    else if ( _stype=="variance" )                       return variance;
    else if ( _stype=="anova" )                          return anova;
    else  throw "GoSUM::CReduction::ReductionType error: unrecognised type";
}


const ArrayXXd & GoSUM::CReduction::sensitivityIndex()
{
    if ( redtype==derivative )                        return pSA->derivativeSensitivity();
    else if ( redtype==averagederivative )            return pSA->averageDerivative();
    else if ( redtype==absoluteaveragederivative )    return pSA->absoluteAverageDerivative();
    else if ( redtype==anova                     )    return pSA->firstOrderANOVA();
    else                                              return pSA->varianceSensitivity();
}


void GoSUM::CReduction::initialize()
{
    if ( pIP->empty() || pOS->empty() || pSA->empty() )   throw "GoSUM::CReduction::initialize error: empty model or sensitivity";

    int i,j,Isize=pIP->size(),Osize=pOS->size();

    redtype=derivative;
    cuttype=cutoffsize;

    rankOS.resize(Osize);
    for ( int o=0; o<Osize; o++ )
    {   rankOS[o]=true; }

    ArrayXXd A;
    //A=pSA->derivativeSensitivity();
    A=pSA->varianceSensitivity();

    rankIP.resize(Isize);
    for ( i=0; i<Isize; i++ )
    {
        rankIP[i].second=false;
        rankIP[i].first=0.;
        for ( j=0; j<Osize; j++ )
        {   if ( rankOS[j] )
            { cout << i<<" "<<j<<"\n";rankIP[i].first=std::max(rankIP[i].first,A(j,i));  }   }
    }

    cutos=Osize;
    cutip=0; cutval=0.;
}


void GoSUM::CReduction::setReductionType(enum reductiontype _redtype)
{
    redtype=_redtype;

    evaluateMaximalSensitivities();
    selectInputParameters();
}

void GoSUM::CReduction::setCutoffSize(int _cutip)
{
    if ( rankIP.size()==0 )  throw "GoSUM::CReduction::setCutofSize error: bad rankIP size";
    if ( _cutip<0 || _cutip>pIP->size() ) throw "GoSUM::CReduction::setCutofSize error: bad _cutip";

    cutip=_cutip;
    cuttype=cutoffsize;

    selectInputParameters();
}


void GoSUM::CReduction::setCutoffValue(double _cutval)
{
    if ( rankIP.size()==0 )  throw "GoSUM::CReduction::setCutoffValue error: bad rankIP size";

    cutval=_cutval;
    cuttype=cutoffvalue;

    selectInputParameters();
}


void GoSUM::CReduction::selectOutputState(int _o)
{
    if ( rankOS.size()==0 )  throw "GoSUM::CReduction::selectOutputState error: empty rankOS";
    else if ( 0<=_o && _o<rankOS.size() )  rankOS[_o] = !rankOS[_o];
    else        throw "GoSUM::CReduction::selectOutputState error: output state index out of range";

    evaluateMaximalSensitivities();
    selectInputParameters();
}


void GoSUM::CReduction::selectOutputs(const std::vector<std::string> &_selOS)
{
    if ( rankOS.size()==0 )  throw "GoSUM::CReduction::selectOutputs error: empty rankOS";
    if ( _selOS.empty() )  throw "GoSUM::CReduction::selectOutputs error: empty list of selected output states";

    cutos=int(_selOS.size());

    int o,Osize=pOS->size();
    for ( o=0; o<Osize; o++ )
    {   rankOS[o] = ( std::find(_selOS.begin(),_selOS.end(),(*pOS)(o).name())!=_selOS.end() ); }

    evaluateMaximalSensitivities();
    selectInputParameters();
}


bool GoSUM::CReduction::isSelectedOutputState(int _o) const
{
    if ( rankOS.size()==0 )  return false;
    else if ( 0<=_o && _o<rankOS.size() )  return rankOS[_o];
    else        throw "GoSUM::CReduction::isSelectedOutputState error: output state index out of range";
}


bool GoSUM::CReduction::isSelectedInputParameter(int _i) const
{
    if ( rankIP.size()==0 )  return false;
    else if ( 0<=_i && _i<rankIP.size() )  return rankIP[_i].second;
    else        throw "GoSUM::CReduction::isSelectedInputParameter error: input parameter index out of range";
}

double GoSUM::CReduction::maximalSensitivity(int _i)
{
    if ( rankIP.size()==0 )  throw "GoSUM::CReduction::maximalSensitivityIndex error: empty rankIP";
    else if ( 0<=_i && _i<rankIP.size() )  return rankIP[_i].first;
    else throw "GoSUM::CReduction::maximalSensitivityIndex error: input parameter index out of range";
}


double GoSUM::CReduction::maximalSensitivity()
{
    if ( rankIP.size()==0 )  throw "GoSUM::CReduction::maximalSensitivityIndex error: empty rankIP";

    int i,Isize=pIP->size();
    std::vector< std::pair<double,int> > rnk(Isize);
    for ( i=0; i<Isize; i++ )
    {   rnk[i].first=rankIP[i].first;
        rnk[i].second=i; }

    std::sort(rnk.begin(),rnk.end(),compareFirst);

    return rnk[0].first;
}


void GoSUM::CReduction::eraseNonSelectedVariables()
{
    int i,Isize=int(rankIP.size()),Osize=int(rankOS.size());

    for ( i=Isize-1; i>=0; i-- )
    {  if ( !rankIP[i].second ) { pIP->erase(i); } }
    for ( i=Osize-1; i>=0; i-- )
    {  if ( !rankOS[i] ) { pOS->erase(i); } }

    clear();
}



bool GoSUM::CReduction::admissible()
{
    int i,Isize=int(rankIP.size()),Osize=int(rankOS.size());
    int rIsize=Isize,rOsize=Osize;
    for ( i=Isize-1; i>=0; i-- )
    {  if ( !rankIP[i].second ) { rIsize--; } }
    for ( i=Osize-1; i>=0; i-- )
    {  if ( !rankOS[i] ) { rOsize--; } }

    return (rIsize>0 && rOsize>0);
 }


