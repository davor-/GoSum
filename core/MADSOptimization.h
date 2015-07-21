#ifndef MADSOPTIMIZATION_H
#define MADSOPTIMIZATION_H


#include "OptimizationProblem.h"
#include "../nomad/src/Mads.hpp"


//! \brief
//! Class for the mesh addaptive direct serach, i.e. interface for NOMAD.
class CMADS {
protected:
    COptimizationProblem *pOP; //!< Points to the optimization problem.
    int maxeval; //!< Holds maximal number of objective & constraints evaluations during optimization.
    int lh0,lhi;
    double ims,mps;
    ArrayXd Xbest;
    double ybest;
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
public:
    CMADS() : pOP(NULL), maxeval(10000), lh0(100), lhi(10), ims(0.1), mps(1.e-6) {}
    virtual ~CMADS() {}
public:
    ArrayXd optimize(COptimizationProblem *_pOP,std::ostream &_out=std::cout); //!< Solves the optimizations problem.
    static NOMAD::Point ArrayXd2NOMADPoint(const ArrayXd &x);  //!< Utility: converts ArrayXd to NOMAD::Point.
    static ArrayXd NOMADPoint2ArrayXd(const NOMAD::Point &p);  //!< Utility: converts NOMAD::Point to ArrayXd.
    int evaluationSize() const { return maxeval; } //!< Returns maximal number of evaluations.
    void setEvaluationSize(int _maxeval) { maxeval=_maxeval; } //!< Sets maximal number of evaluations.
    int initialLHSearch() const { return lh0; }  //!< Returns initial value for LH search.
    void setInitialLHSearch(int _lh0) { lh0=_lh0; }  //!< Sets initial value for LH search.
    int iterationLHSearch() const { return lhi; }  //!< Returns iteration value for LH search.
    void setIterationLHSearch(int _lhi) { lhi=_lhi; }  //!< Sets iteration value for LH search.
    double initalMeshSize() const { return ims; } //!< Returns initial mesh size factor.
    void setInitialMeshSize(double _ims) { ims=_ims; } //!< Sets initial mesh size factor.
    double minimalPollSize() const { return mps; }//!< Returns minimal poll size factor.
    void setMinimalPollSize(double _mps) { mps=_mps; }//!< Sets minimal poll size factor.
    int progressStepsSize() const { return maxeval; }  //!< Returns progress steps size.
};


//! \brief
//! Subclass of the NOMAD::Evaluator class.
class CMADSEvaluator : public NOMAD::Evaluator {
protected:
    COptimizationProblem *pOP; //!< Points to the optimization problem.
public:
    CMADSEvaluator(const NOMAD::Parameters &p,COptimizationProblem *_pOP) : NOMAD::Evaluator(p), pOP(_pOP) {}
    virtual ~CMADSEvaluator() {}
public:
    virtual bool eval_x(NOMAD::Eval_Point &x, const NOMAD::Double &h_max, bool &count_eval) const; //!< Overload of the NOMAD::Evaluator evaluation member function.
};


BOOST_CLASS_EXPORT_KEY(CMADS)


#endif // MADSOPTIMIZATION_H
