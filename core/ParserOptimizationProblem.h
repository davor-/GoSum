#ifndef GOSUMPARSEROPTIMIZATIONPROBLEM_H
#define GOSUMPARSEROPTIMIZATIONPROBLEM_H



#include "OptimizationProblem.h"
#include "fparser.hh"
#include "AnalyticalModel.h"


//! \brief
//! Namespace for GoSUM model.
namespace GoSUM {


//! \brief
//! Class for the optimization problem with parser functions.
class CParserOptimizationProblem : public COptimizationProblem {
protected:
    std::string names; //!< Holds all names that are permited in objective and constraint expressions.
    std::string fexpr; //!< Holds objective function expression.
    std::vector<std::string> gexpr; //!< Holds expressions for constraints.
    FunctionParser f; //!< Holds function parser for objective function.
    std::vector<FunctionParser> g; //!< Holds function parsers for constraints.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
protected:
    virtual void setVariableNames()=0; //!< Sets variable names for the parser.
public:
    CParserOptimizationProblem() : COptimizationProblem(), names(""), fexpr(""), gexpr(0), g(0)
    {  f.AddConstant("pi", 3.1415926535897932384626433832795);
       f.AddConstant("e", 2.7182818284590452353602874713527); }
    virtual ~CParserOptimizationProblem() {}
public:
    void setObjectiveExpression(const std::string &_fexpr) { fexpr=_fexpr; } //!< Sets objective expression.
    std::string objectiveExpression() { return fexpr; } //!< Returns objective expression.
    virtual int constraintsSize() const { return int(gexpr.size()); } //!< Returns size of the constraints.
    void clearConstraintExpressions() { gexpr.clear(); } //!< Clears constraints expressions.
    void addConstraintExpression(const std::string &_gexpr) { gexpr.push_back(_gexpr); } //!< Adds constraint expression.
    void eraseConstraintExpression(int _at); //!< Erases particular constraint expression.
    void setConstraintExpression(const std::string &_gexpr,int _at); //!< Sets particular constraint expression.
    std::string constraintExpression(int _at) { return gexpr[_at]; } //!< Returns particular constraint expression.
    std::string roundoffEquality(std::string _expr); //!< If _expr is an equality expression left=right it returns expression abs(left-right)<=TINY.
    bool validateExpressions(); //!< Validates all expressions.
    void parseExpressions(); //!< Parses all expressions.
    virtual void openOptimization(); //! Opens, i.e. prepares optimization.
    virtual void closeOptimization(); //! Closes optimization, i.e. closes what was opened in openOptimization.
    double objective(const ArrayXd &_mv) {  return f.Eval(_mv.data()); } //!< Evaluates objective function value from model variables values.
    double constraint(const ArrayXd &_mv,int _at) { return  g[_at].Eval(_mv.data()); } //!< Evalautes particular constraint value from model variables values.
public:
    boost::signal<void()> optimizingProgressed; //!< Signal for optimizing progress, emitted on single objective evaluation.
};


class CModelOptimizationVariable : public COptimizationVariable {
protected:
    CModelVariable *pip; //!< Holds pointer to the input parameter to which model optimization variable is connected.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
    CModelOptimizationVariable() : COptimizationVariable(), pip(NULL) {}
public:
    CModelOptimizationVariable(CModelVariable *_pip) : COptimizationVariable(_pip->minValue(),_pip->maxValue(),_pip->expectedValue()), pip(_pip) {}
    CModelOptimizationVariable(CModelOptimizationVariable &_O) : COptimizationVariable(_O), pip(_O.pip) {}
    virtual ~CModelOptimizationVariable() {}
public:
    CModelVariable *inputParameter() const { return pip; } //!< Returns pointer to the input parameter to which model optimization variable is connected.
    void reset() { set(pip->minValue(),pip->maxValue(),pip->expectedValue()); } //!< Resets lower bound, upper bound and inital value.
};

//! \brief
//! Class for the optimization problem based only on GoSUM input parameters.
class CSimpleOptimizationProblem : public CParserOptimizationProblem {
protected:
    static CModelVariable *pipToFind; //!< Used in member function Find.
    static bool isModelVariable(const COptimizationVariable &_aOV) { return ((CModelOptimizationVariable *)(&_aOV))->inputParameter()==pipToFind; } //!< Used in member function Find.
protected:
    CInputParameters *pIP; //!< Points to input parameters.
    std::vector<int> ovind,uvind; //!< Vectors of indices of optimization variables (ovind) and uncertain variables (uvind) among input parameters.
    std::vector<ArrayXd> samples; //!< Samples for the uncertain varibles.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
    CSimpleOptimizationProblem() : CParserOptimizationProblem(), pIP(NULL) {}
protected:
    virtual void setVariableNames(); //!< Sets variable names for the parser.
public:
    CSimpleOptimizationProblem(CInputParameters *_pIP) : CParserOptimizationProblem(), pIP(_pIP) {}
    virtual ~CSimpleOptimizationProblem() {}
public:
    virtual void clear() { ovind.clear(); uvind.clear(); samples.clear(); CParserOptimizationProblem::clear();  } //!< Clears all.
    virtual void clearHistory() { ovind.clear(); uvind.clear(); samples.clear(); CParserOptimizationProblem::clearHistory(); } //!< Clears results.
    int findConnectedTo(CModelVariable *_pip) const; //!< Finds optimization variable connected to a particular input parameter.
    const CModelOptimizationVariable &variable(int _at) const { return (CModelOptimizationVariable &)ovs[_at]; } //!< Returns paticular optimization variable.
    CModelOptimizationVariable &variable(int _at) { return (CModelOptimizationVariable &)ovs[_at]; } //!< Returns paticular optimization variable.
    std::string variableName(int _at) { return variable(_at).inputParameter()->name(); } //!< Returns name of the particular optimization variable.
    virtual bool isFeasible(const ArrayXd &_ip) { return (!pIP->constraints())? true:pIP->constraints()->constraintsSatisfied(_ip); } //!< Returns true if model variables is admissible, false otherwise.
    virtual void openOptimization(); //! Opens, i.e. prepares optimization.
    virtual void closeOptimization(); //! Closes optimization, i.e. closes what was opened in openOptimization.
    virtual bool evaluate(const ArrayXd &_hp,ArrayXd &_ep); //!< Evaluates objective and all constraints from optimization variables values and returns true if it is feasible, false otherwise.
    virtual ArrayXd inputPoint2ModelPoint(const ArrayXd &_ip) { return _ip; } //!< Converts input parameter point to model point.
};


//! \brief
//! Class for the optimization problem based on GoSUM analyitical model.
class COriginalOptimizationProblem : public CSimpleOptimizationProblem {
protected:
    COutputStates *pOS; //!< Points to output states.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
    COriginalOptimizationProblem() : CSimpleOptimizationProblem(), pOS(NULL) {}
protected:
    virtual void setVariableNames(); //!< Sets variable names for the parser.
public:
    COriginalOptimizationProblem(CInputParameters *_pIP,COutputStates *_pOS) : CSimpleOptimizationProblem(_pIP), pOS(_pOS) {}
    virtual ~COriginalOptimizationProblem() {}
public:
    virtual void openOptimization() { CSimpleOptimizationProblem::openOptimization(); pOS->openEvaluation(*pIP); } //! Opens, i.e. prepares optimization.
    virtual void closeOptimization() { pOS->closeEvaluation(); CSimpleOptimizationProblem::closeOptimization();  } //! Closes optimization, i.e. closes what was opened in openOptimization.
    virtual ArrayXd inputPoint2ModelPoint(const ArrayXd &_ip); //!< Converts input parameter point to model point.
};


//! \brief
//! Class for the optimization problem based on GoSUM analyitical model.
class CAnalyticalOptimizationProblem : public COriginalOptimizationProblem {
protected:
    CAnalyticalModel *pAM; //!< Points to analytical model.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
    CAnalyticalOptimizationProblem() : COriginalOptimizationProblem(), pAM(NULL) {}
public:
    CAnalyticalOptimizationProblem(CInputParameters *_pIP,COutputStates *_pOS,CAnalyticalModel *_pAM) : COriginalOptimizationProblem(_pIP,_pOS), pAM(_pAM) {}
    virtual ~CAnalyticalOptimizationProblem() {}
public:
    virtual ArrayXd inputPoint2ModelPoint(const ArrayXd &_ip); //!< Converts input parameter point to model point.
};


} // end of namespace GoSUM


BOOST_CLASS_EXPORT_KEY(GoSUM::CParserOptimizationProblem)
BOOST_CLASS_EXPORT_KEY(GoSUM::CModelOptimizationVariable)
BOOST_CLASS_EXPORT_KEY(GoSUM::CSimpleOptimizationProblem)
BOOST_CLASS_EXPORT_KEY(GoSUM::COriginalOptimizationProblem)
BOOST_CLASS_EXPORT_KEY(GoSUM::CAnalyticalOptimizationProblem)



#endif // PARSEROPTIMIZATIONPROBLEM_H
