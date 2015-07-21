#ifndef CONSTRAINTS_H
#define CONSTRAINTS_H


#include "Utilities.h"
#include "VariousMath.h"
#include "fparser.hh"


//! \brief
//! Namespace for GoSUM model.
namespace GoSUM {


//! \brief
//! Class for the constraints with parser functions.
class CConstraints {
protected:
    std::string names; //!< Holds all names that are permited in objective and constraint expressions.
    std::vector<std::string> gexpr; //!< Holds expressions for constraints.
    FunctionParser f; //!< Holds base function parser.
    std::vector<FunctionParser> g; //!< Holds function parsers for constraints.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
protected:
    virtual void setVariableNames()=0; //!< Sets variable names for the parser.
public:
    CConstraints() : names(""), gexpr(0), g(0)
    {  f.AddConstant("pi", 3.1415926535897932384626433832795);
        f.AddConstant("e", 2.7182818284590452353602874713527);  }
    virtual ~CConstraints() {}
public:
    virtual void clear() { names.clear(); gexpr.clear(); g.clear(); } //!< Clears object.
    int size() const { return int(gexpr.size()); } //!< Returns size of the constraints.
    void addExpression(const std::string &_gexpr) { gexpr.push_back(_gexpr); } //!< Adds contraint expression.
    void eraseExpression(int _at); //!< Erases particular constraint expression.
    void setExpression(const std::string &_gexpr,int _at); //!< Sets particular cosntraint expression.
    std::string expression(int _at) const { return gexpr[_at]; } //!< Returns particular constraint expression.
    std::string roundoffEquality(std::string _expr); //!< If _expr is an equality expression left=right it returns expression abs(left-right)<=TINY.
    bool validateExpressions(); //!< Validates all expressions.
    void parseExpressions(); //!< Parses all expressions.
    double evaluate(const ArrayXd &_x,int _at) { return  g[_at].Eval(_x.data()); } //!< Evalautes particular constraint value from variables values.
    bool constraintsSatisfied(const ArrayXd &_x); //!< Returns true if _x satisfies constraints, false otherwise.
    bool findInExpressions(const std::string &_name) const; //!< Returns true if _name is found in constraint expressions, false otherwise.
};


class CInputParameters;
class CModelConstraints : public CConstraints {
protected:
    CInputParameters *pIP; //!< Points to input parameters.
    std::vector<bool> status; //!< Status (constrained or not) for each input parameter.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
protected:
    virtual void setVariableNames(); //!< Sets variable names for the parser.
public:
    CModelConstraints() : CConstraints(), pIP(NULL) {}
    virtual ~CModelConstraints() {}
public:
    virtual void clear() { CConstraints::clear(); status.clear(); } //!< Clears object.
    void set(CInputParameters *_pIP); //!< Sets input parameters pointer.
    bool isConstrained(int _at) const { return status[_at]; } //!< Returns status (cosntrained or not) for particular input parameter.
};

} // end of namespace GoSUM


BOOST_CLASS_EXPORT_KEY(GoSUM::CConstraints)
BOOST_CLASS_EXPORT_KEY(GoSUM::CModelConstraints)



#endif // CONSTRAINTS_H
