

#include "Constraints.h"
#include "Model.h"



BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CConstraints)
BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CModelConstraints)



template <class Archive>
void GoSUM::CConstraints::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("names",names);
    ar & boost::serialization::make_nvp("gexpr",gexpr);
}


template <class Archive>
void GoSUM::CModelConstraints::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CConstraints",boost::serialization::base_object<CConstraints>(*this));
//    ar & boost::serialization::make_nvp("pIP",pIP);
}


void GoSUM::CConstraints::setExpression(const std::string &_gexpr,int _at)
{
    if ( !( 0<=_at && _at<int(gexpr.size()) ) )  throw "GoSUM::COptimiizationProblem::setConstraintExpression error: index out of range";
    gexpr[_at]=_gexpr;
}


void GoSUM::CConstraints::eraseExpression(int _at)
{
    if ( !( 0<=_at && _at<int(gexpr.size()) ) )  throw "GoSUM::COptimiizationProblem::eraseConstraintExpression: index out of range";
    gexpr.erase(gexpr.begin()+_at);
}


std::string GoSUM::CConstraints::roundoffEquality(std::string _expr)
{
    size_t eqpos=_expr.find("=");
    if ( eqpos==std::string::npos )   return _expr;

    std::string _oper=_expr.substr(eqpos-1,2);
    if ( _oper=="<=" || _oper==">=" ) return _expr;

    std::string left=_expr.substr(0,eqpos);
    std::string right=_expr.substr(eqpos+1,std::string::npos);
    std::ostringstream oss;
    oss<<"abs("<<left<<"-"<<right<<")<="<<TINY;
    return oss.str();
}


bool GoSUM::CConstraints::validateExpressions()
{
    setVariableNames();

    FunctionParser test;

    int i,n=int(gexpr.size());
    for ( i=0; i<n; i++ )
        if ( test.Parse(roundoffEquality(gexpr[i]),names)>=0 )    return false;

    return true;
}


void GoSUM::CConstraints::parseExpressions()
{
    setVariableNames();

    int i,n=int(gexpr.size());
    g.clear();
    g.resize(n,f);
    for ( i=0; i<n; i++ )
    {
        if ( g[i].Parse(roundoffEquality(gexpr[i]),names)>=0 ) throw g[i].ErrorMsg();
        g[i].Optimize();
    }
}


bool GoSUM::CConstraints::findInExpressions(const std::string &_name) const
{
    int i,n=int(gexpr.size());
    for ( i=0; i<n; i++ )
    {
        if (gexpr[i].find(_name)!=string::npos) return true;
    }
    return false;

}


bool GoSUM::CConstraints::constraintsSatisfied(const ArrayXd &X)
{
    int i,M=size();
    for ( i=0; i<M; i++ )  if ( evaluate(X,i)==0. ) return false;
    return true;
}


void GoSUM::CModelConstraints::set(CInputParameters *_pIP)
{
    pIP=_pIP;
}


void GoSUM::CModelConstraints::setVariableNames()
{
    int i,Isize=pIP->size();
    names=(Isize==0)?"":(*pIP)(0).name();
    for ( i=1; i<Isize; i++ ) names+=","+(*pIP)(i).name();

    status.resize(Isize);
    for ( i=0; i<Isize; i++ )
    {  status[i]=findInExpressions((*pIP)(i).name());  }
}
