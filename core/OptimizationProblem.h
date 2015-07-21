#ifndef OPTIMIZATIONPROBLEM_H
#define OPTIMIZATIONPROBLEM_H


#include "Utilities.h"


class COptimizationVariable {
protected:
    double xL,xU,x0; //!< Holds lower bound, upper bound and initial value of a optimization variable.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version)
    {
        ar & boost::serialization::make_nvp("x0",x0);
        ar & boost::serialization::make_nvp("xL",xL);
        ar & boost::serialization::make_nvp("xU",xU);
    }
public:
    COptimizationVariable() : xL(0.), xU(0.), x0(0.) {}
    COptimizationVariable(double _xL,double _xU,double _x0) : xL(_xL), xU(_xU), x0(_x0) {}
    COptimizationVariable(COptimizationVariable &_O) : xL(_O.xL), xU(_O.xU), x0(_O.x0) {}
    virtual ~COptimizationVariable() {}
public:
    double lowerBound() const { return xL; }  //!< Returns lower bound.
    double upperBound() const { return xU; }  //!< Returns upper bound.
    double initialValue() const { return x0; }  //!< Returns initial value.
    void setLowerBound(double _xL); //!< Sets lower bound.
    void setUpperBound(double _xU); //!< Sets upper bound.
    void setInitialValue(double _x0); //!< Sets inital value.
    void set(double _xL,double _xU,double _x0); //!< Sets lower bound, upper bound, initial value.
};


//! \brief
//! Class for the optimization problem.
class COptimizationProblem {
protected:
    bool minimize; //!< Indicates if it is a minimization problem.
    boost::ptr_vector<COptimizationVariable> ovs; //!< Holds all optimization variables.
    std::vector< std::pair<ArrayXd,double> > history; //!< Holds optimization history, i.e. sequence of best results up to every evaluation.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version)
    {
        ar & boost::serialization::make_nvp("minimize",minimize);
        ar & boost::serialization::make_nvp("ovs",ovs);
        ar & boost::serialization::make_nvp("history",history);
    }
public:
    COptimizationProblem() : minimize(true) {}
    virtual ~COptimizationProblem() {}
protected:
public:
    virtual void clear() { ovs.clear(); history.clear(); } //!< Clears all.
    virtual void clearHistory() { history.clear(); } //!< Clears results.
    bool emptyHistory() const { return history.size()==0; } //!< Returns true if results are empty. false otherwise.
    bool isMinimization() { return minimize; } //!< Returns true if it is a minimization problem, false otherwise.
    bool isMaximization() { return !minimize; } //!< Returns true if it is a maximization problem, false otherwise.
    void setMinimization()  { minimize=true; } //!< Sets optimization problem to minimization.
    void setMaximization()  { minimize=false; } //!< Sets optimization problem to maximization.
    int dimension() const { return int(ovs.size()); }  //!< Returns dimension of the problem.
    COptimizationVariable *addVariable(COptimizationVariable *_pOV) { ovs.push_back(_pOV); return _pOV; } //!< Adds optimization variable.
    COptimizationVariable *variable(int _at) { return &(ovs[_at]); } //!< Returns optimization variable.
    void eraseVariable(int _at); //!< Erases particular optimization variable.
    void setLowerBound(const ArrayXd &_xL); //!< Sets lower bound of the optimization variables.
    void setUpperBound(const ArrayXd &_xU); //!< Sets upper bound of the optimization variables.
    void setInitialValue(const ArrayXd &_x0); //!< Sets initial value of the optimization variables.
    ArrayXd lowerBound() const; //!< Returns lower bound of the optimization variables.
    ArrayXd upperBound() const; //!< returns upper bound of the optimization variables.
    ArrayXd initialValue() const; //!< Returns initial value of the optimization variables.
    void setLowerBound(int _at,double _xL) { return ovs[_at].setLowerBound(_xL); } //!< Sets lower bound of the optimization variables.
    void setUpperBound(int _at,double _xU) { return ovs[_at].setUpperBound(_xU); } //!< Sets upper bound of the optimization variables.
    void setInitialValue(int _at,double _x0) { return ovs[_at].setInitialValue(_x0); } //!< Sets initial value of the optimization variables.
    double lowerBound(int _at) const { return ovs[_at].lowerBound(); } //!< Returns lower bound of the optimization variables.
    double upperBound(int _at) const { return ovs[_at].upperBound(); } //!< Returns upper bound of the optimization variables.
    double initialValue(int _at) const { return ovs[_at].initialValue(); } //!< Returns initial value of the optimization variables.
    virtual double objective(const ArrayXd &_ov)=0; //!< Evaluates objective function value from optimization variables values.
    virtual bool isFeasible(const ArrayXd &_ov) { return true; } //!< Returns true if optimization variable is feasible, false otherwise.
    virtual int constraintsSize() const=0;  //!< Returns size of the constraints.
    virtual double constraint(const ArrayXd &_ov,int _at)=0; //!< Evalautes particular constraint value from optimization variables values.
    virtual bool evaluate(const ArrayXd &_hp,ArrayXd &_ep); //!< Evaluates objective and all constraints from optimization variables values and returns true if it is feasible, false otherwise.
    virtual void openOptimization() { clearHistory(); } //! Opens, i.e. prepares optimization.
    virtual void closeOptimization() {} //! Closes optimization, i.e. closes what was opened in openOptimization.
    void writeHistory(const ArrayXd &_ov,double _res);
    int historySize() const { return int(history.size()); } //!< Returns history size.
    double objectiveHistory(int _i) const { return history[_i].second; } //!< Returns particular objective history.
    double variableHistory(int _i,int _j) const { return history[_i].first(_j); } //!< Returns particular variable history.
    ArrayXd exportObjectiveHistory() const; //!< Exports objective history.
};



BOOST_CLASS_EXPORT_KEY(COptimizationVariable)
BOOST_CLASS_EXPORT_KEY(COptimizationProblem)


#endif // OPTIMIZATIONPROBLEM_H
