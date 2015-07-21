


#include "OptimizationProblem.h"


BOOST_CLASS_EXPORT_IMPLEMENT(COptimizationVariable)
BOOST_CLASS_EXPORT_IMPLEMENT(COptimizationProblem)


void COptimizationVariable::setLowerBound(double _xL)
{
    if ( _xL>xU || _xL>x0 ) throw "COptimizationVariable::setLowerBound error";
    xL=_xL;
}

void COptimizationVariable::setUpperBound(double _xU)
{
    if ( _xU<xL || _xU<x0 ) throw "COptimizationVariable::setUpperBound error";
    xU=_xU;
}

void COptimizationVariable::setInitialValue(double _x0)
{
    if ( _x0<xL || _x0>xU ) throw "COptimizationVariable::setInitialValue error";
    x0=_x0;
}

void COptimizationVariable::set(double _xL,double _xU,double _x0)
{
    if (!( _xL<=_x0 && _x0<=_xU ))   throw "COptimizationVariable::set error";
    xL=_xL; xU=_xU; x0=_x0;
}


void COptimizationProblem::eraseVariable(int _at)
{
    if ( !( 0<=_at && _at<int(ovs.size()) ) )  throw "COptimizationProblem::eraseVariable error: index out of range";
    ovs.erase(ovs.begin()+_at);
}


void COptimizationProblem::setLowerBound(const ArrayXd &_xL)
{
    if ( _xL.size()!=ovs.size() )  throw "COptimizationProblem::setLowerBound error: wrong dimension";

    int i,dim=int(ovs.size());
    for ( i=0; i<dim; i++ )  ovs[i].setLowerBound(_xL[i]);
}


void COptimizationProblem::setUpperBound(const ArrayXd &_xU)
{
    if ( _xU.size()!=ovs.size() )  throw "COptimizationProblem::setUpperBound error: wrong dimension";

    int i,dim=int(ovs.size());
    for ( i=0; i<dim; i++ )  ovs[i].setUpperBound(_xU[i]);
}


void COptimizationProblem::setInitialValue(const ArrayXd &_x0)
{
    if ( _x0.size()!=ovs.size() )  throw "COptimizationProblem::setInitialValue error: wrong dimension";

    int i,dim=int(ovs.size());
    for ( i=0; i<dim; i++ )  ovs[i].setInitialValue(_x0[i]);
}


ArrayXd COptimizationProblem::lowerBound() const
{
    int i,dim=int(ovs.size());

    ArrayXd _xL(dim);
    for ( i=0; i<dim; i++ )  _xL(i)=ovs[i].lowerBound();

    return _xL;
}


ArrayXd COptimizationProblem::upperBound() const
{
    int i,dim=int(ovs.size());

    ArrayXd _xU(dim);
    for ( i=0; i<dim; i++ )  _xU(i)=ovs[i].upperBound();

    return _xU;
}


ArrayXd COptimizationProblem::initialValue() const
{
    int i,dim=int(ovs.size());

    ArrayXd _x0(dim);
    for ( i=0; i<dim; i++ )  _x0(i)=ovs[i].initialValue();

    return _x0;
}


bool COptimizationProblem::evaluate(const ArrayXd &_op,ArrayXd &_ep)
{
    int M=constraintsSize(),i;
    bool fp=(isFeasible(_op));

    _ep.setZero(M+1);
    _ep(0)=objective(_op);
    for ( i=1; i<M+1; i++ )  _ep(i)=constraint(_op,i-1);

    return fp;
}


ArrayXd COptimizationProblem::exportObjectiveHistory() const
{
    ArrayXd h;
    int i,n=int(history.size());
    h.resize(n);
    for ( i=0; i<n; i++ )
        h(i)=history[i].second;

    return h;
}


void COptimizationProblem::writeHistory(const ArrayXd &_ov,double _res)
{
    ArrayXd ovbest;
    double resbest;

    if ( history.empty() )
    {
        ovbest=_ov;
        resbest=_res;
    }
    else
    {
        ovbest=history.back().first;
        resbest=history.back().second;
        if ( (minimize && _res<=resbest) || (!minimize && _res>=resbest) )
        {
            ovbest=_ov;
            resbest=_res;
        }
    }

    history.push_back(std::pair<ArrayXd,double>(ovbest,resbest));
}

