



#include "ParserOptimizationProblem.h"


GoSUM::CModelVariable *GoSUM::CSimpleOptimizationProblem::pipToFind=NULL;


BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CParserOptimizationProblem)
BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CModelOptimizationVariable)
BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CSimpleOptimizationProblem)
BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::COriginalOptimizationProblem)
BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CAnalyticalOptimizationProblem)



template <class Archive>
void GoSUM::CParserOptimizationProblem::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("COptimizationProblem",boost::serialization::base_object<COptimizationProblem>(*this));
    ar & boost::serialization::make_nvp("names",names);
    ar & boost::serialization::make_nvp("fexpr",fexpr);
    ar & boost::serialization::make_nvp("gexpr",gexpr);
}


template <class Archive>
void GoSUM::CModelOptimizationVariable::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("COptimizationVariable",boost::serialization::base_object<COptimizationVariable>(*this));
    ar & boost::serialization::make_nvp("pip",pip);
}


template <class Archive>
void GoSUM::CSimpleOptimizationProblem::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CParserOptimizationProblem",boost::serialization::base_object<CParserOptimizationProblem>(*this));
    ar & boost::serialization::make_nvp("pipToFind",pipToFind);
    ar & boost::serialization::make_nvp("pIP",pIP);
    ar & boost::serialization::make_nvp("ovind",ovind);
    ar & boost::serialization::make_nvp("uvind",uvind);
    ar & boost::serialization::make_nvp("samples",samples);
}


template <class Archive>
void GoSUM::COriginalOptimizationProblem::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CSimpleOptimizationProblem",boost::serialization::base_object<CSimpleOptimizationProblem>(*this));
    ar & boost::serialization::make_nvp("pOS",pOS);
}


template <class Archive>
void GoSUM::CAnalyticalOptimizationProblem::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("COriginalOptimizationProblem",boost::serialization::base_object<COriginalOptimizationProblem>(*this));
    ar & boost::serialization::make_nvp("pAM",pAM);
}


void GoSUM::CParserOptimizationProblem::setConstraintExpression(const std::string &_gexpr,int _at)
{
    if ( !( 0<=_at && _at<int(gexpr.size()) ) )  throw "GoSUM::COptimiizationProblem::setConstraintExpression error: index out of range";
    gexpr[_at]=_gexpr;
}


void GoSUM::CParserOptimizationProblem::eraseConstraintExpression(int _at)
{
    if ( !( 0<=_at && _at<int(gexpr.size()) ) )  throw "GoSUM::COptimiizationProblem::eraseConstraintExpression: index out of range";
    gexpr.erase(gexpr.begin()+_at);
}


std::string GoSUM::CParserOptimizationProblem::roundoffEquality(std::string _expr)
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


bool GoSUM::CParserOptimizationProblem::validateExpressions()
{
    setVariableNames();

    FunctionParser test;
    if ( test.Parse(fexpr,names)>=0 )  return false;

    int i,n=int(gexpr.size());
    for ( i=0; i<n; i++ )
        if ( test.Parse(roundoffEquality(gexpr[i]),names)>=0 ) return false;

    return true;
}


void GoSUM::CParserOptimizationProblem::parseExpressions()
{
    setVariableNames();

    if ( f.Parse(fexpr,names)>=0 ) throw f.ErrorMsg();
    f.Optimize();

    int i,n=int(gexpr.size());
    g.clear();
    g.resize(n,f);
    for ( i=0; i<n; i++ )
    {
        if ( g[i].Parse(roundoffEquality(gexpr[i]),names)>=0 ) throw g[i].ErrorMsg();
        g[i].Optimize();
    }
}


int GoSUM::CSimpleOptimizationProblem::findConnectedTo(CModelVariable *_pip) const
{
    pipToFind=_pip;
    return int(find_if(ovs.begin(),ovs.end(),CSimpleOptimizationProblem::isModelVariable)-ovs.begin());
}


void GoSUM::CSimpleOptimizationProblem::setVariableNames()
{
    int i,Isize=pIP->size();
    names=(Isize==0)?"":(*pIP)(0).name();
    for ( i=1; i<Isize; i++ ) names+=","+(*pIP)(i).name();
}


void GoSUM::CParserOptimizationProblem::openOptimization()
{
    COptimizationProblem::openOptimization();

    parseExpressions();
}

void GoSUM::CSimpleOptimizationProblem::openOptimization()
{
    CParserOptimizationProblem::openOptimization();

    int dim=dimension(),Isize=pIP->size(),i;
    ovind.clear();
    ovind.resize(dim);
    for ( i=0; i<dim; i++ )
        ovind[i]=pIP->find(variableName(i));

    std::string copyname;
    uvind.clear();
    samples.clear();
    if ( dim<Isize )
    {
        CModelConstraints ucc;
        CInputParameters uncertains(&ucc);
        ucc.set(&uncertains);

        for ( i=0; i<Isize; i++ )
        {
            if ( std::find(ovind.begin(),ovind.end(),i)==ovind.end() )
            {
                uvind.push_back(i);
                copyname=(*pIP)(i).name()+"-copy";
                uncertains.add((*pIP)(i).clone(copyname));
            }
        }
        uncertains.generateSamples(samples);


    }


}


void GoSUM::CParserOptimizationProblem::closeOptimization()
{
    g.clear();

    COptimizationProblem::closeOptimization();
}


void GoSUM::CSimpleOptimizationProblem::closeOptimization()
{
    ovind.clear();
    uvind.clear();
    samples.clear();

    CParserOptimizationProblem::closeOptimization();
}


bool GoSUM::CSimpleOptimizationProblem::evaluate(const ArrayXd &_op,ArrayXd &_ep)
{
    int M=constraintsSize();
    _ep=ArrayXd::Zero(M+1);

    int dim=dimension(),Isize=pIP->size(),Ssize=samples.size();
    int fpsize,i,j,k;
    ArrayXd ip(Isize),mp;

    for ( i=0; i<dim; i++ )  ip(ovind[i])=_op(i);

    if ( dim==Isize )
    {
        fpsize=(isFeasible(ip))?1:0;
        mp=inputPoint2ModelPoint(ip);
        _ep(0)=objective(mp);
        for ( k=1; k<M+1; k++ )  _ep(k)=constraint(mp,k-1);
    }
    else
    {
        fpsize=0;
        for ( j=0; j<Ssize; j++ )
        {
            for ( i=0; i<uvind.size(); i++ )  ip(uvind[i])=samples[j](i);
            if ( isFeasible(ip) )
            {
                fpsize++;
                mp=inputPoint2ModelPoint(ip);
                _ep(0)+=objective(mp);
                for ( k=1; k<M+1; k++ )  _ep(k)+=constraint(mp,k-1);
            }
        }
        if (fpsize>0) _ep/=double(fpsize);
    }

    optimizingProgressed();

    return (fpsize>0);
}


void GoSUM::COriginalOptimizationProblem::setVariableNames()
{
    GoSUM::CSimpleOptimizationProblem::setVariableNames();
    int i,Osize=pOS->size();
    for ( i=0; i<Osize; i++ ) names+=","+(*pOS)(i).name();
}


ArrayXd GoSUM::COriginalOptimizationProblem::inputPoint2ModelPoint(const ArrayXd &_ip)
{
    int Isize=pIP->size(),Osize=pOS->size(),i;
    ArrayXd mp(Isize+Osize);
    ArrayXd os=pOS->evaluate(_ip);
    for ( i=0; i<Isize; i++ )  mp(i)=_ip(i);
    for ( i=0; i<Osize; i++ )  mp(Isize+i)=os(i);

    return mp;
}


ArrayXd GoSUM::CAnalyticalOptimizationProblem::inputPoint2ModelPoint(const ArrayXd &_ip)
{
    int Isize=pIP->size(),Osize=pOS->size(),i;
    ArrayXd mp(Isize+Osize);
    for ( i=0; i<Isize; i++ )  mp(i)=_ip(i);
    for ( i=0; i<Osize; i++ )  mp(Isize+i)=pAM->predictValue(_ip,i);

    return mp;
}
