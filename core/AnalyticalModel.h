#ifndef GOSUMANALYTICALMODEL_H
#define GOSUMANALYTICALMODEL_H




#include "Model.h"
#include "MADSOptimization.h"
#include "GAOptimization.h"
#include "svm.h"





//! \brief
//! Namespace for GoSUM model.
namespace GoSUM {


class CAnalyticalModel;
//! \brief
//! Base class for the analyitical model for single output state,
//! interface between analytical model and SVM.
class CSingleAM : public COptimizationProblem {
protected:
    static CInputParameters *pIP; //!< Points to input parameters.
    static COutputStates *pOS; //!< Points to output states.
    static ArrayXi perm; //!< Permutation of sample indices.
    static struct svm_problem prob; //!< SVM format specific.
    static std::vector<struct svm_node> xspace; //!< SVM format specific.
    static int maxlearnsize;
    static double learnfrac;
public:
    static GoSUM::CSingleAM *New(const CAnalyticalModel *_pAM); //!< Returns new SingleAM of type defined in _pAM.
    static void OpenSVM(CInputParameters *_pIP,COutputStates *_pOS); //!< Opens SVM learning (sets prob and xspace).
    static void CloseSVM(); //!< Closes SVM learning.
    static void SetMaximalLearningSize(int _maxlearnsize) { maxlearnsize=_maxlearnsize; } //!< Sets maximal (sample) size used in learning.
    static int MaximalLearningSize() { return maxlearnsize; } //!< Returns maximal (sample) size  used in learning.
    static void SetLearningFraction(int _learnfrac) { learnfrac=_learnfrac; } //!< Sets (sample) fraction  used in learning.
    static int LearningFraction() { return learnfrac; } //!< Returns (sample) fraction  used in learning.
protected:
    struct svm_parameter param; //!< SVM format specific.
    struct svm_model * model;  //!< SVM format specific.
protected:
    int osi; //!< Index of the related output state.
    double gamma,C,p,nu; //!< SVM exponent constant in the RBF kernel case.
    double coef0; //!< SVM free coeficient in the polynomial kernel case.
    int degree; //!< SVM degree in the polynomial kernel case.
    int Nsvm; //!< SVM number of support vectors.
    ArrayXd coeff; //!< SVM coefficients.
    std::vector<ArrayXd> Xsv; //!< SVM support vectors.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
protected:
    double rbfKernel(const ArrayXd& x,int j) const {  VectorXd d=(x.matrix()-Xsv[j].matrix());  return exp(-gamma*d.squaredNorm());  } //!< Returns RBF kernel for jth support vector.
public:
    CSingleAM() : model(NULL), osi(-1), gamma(0.), C(0.), p(0.), nu(0.), coef0(0.), degree(0), Nsvm(0), Xsv(0) {}
    virtual ~CSingleAM() {}
public:
    void learn(int _osi,CMADS &_mads,std::ostream &_out=std::cout); //!< Learns SVM models for all output states.
    void learn(int _osi,CGAModelOptimization &_gamo,std::ostream &_out=std::cout); //!< Learns SVM models for all output states.
    virtual int constraintsSize() const { return 0; } //!< Returns size of the constraints.
    virtual double constraint(const ArrayXd &mv,int _at) { return 0.; } //!< Evaluates particular constraint value from model variables values.
    virtual ArrayXd modelVariables(const ArrayXd &x) const { return x; } //!< Returns model variables values from optimization variables values.
    virtual double objective(const ArrayXd &ov); //!< Evaluates objective function value from optimization variables values.
    virtual void openOptimization(); //!< Opens optmization.
    virtual void optimizationPoint2SVMParam(const ArrayXd &ov) {} //!< Converts optimization point to SVM parameters.
    double f(const ArrayXd& x) const; //!< Returns value of the output state according to the single output state analytical model.
    ArrayXd df(const ArrayXd& x) const; //!< Returns gradient of the output state according to the single output state the analytical model.
public:
    boost::signal<void()> learningProgressed; //!< Signal for learning progress, emitted on every objective evaluation.
};


//! \brief
//! Class for the analyitical model for single output state, SVC type.
class CCSvcSAM : public CSingleAM {
protected:
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
public:
    CCSvcSAM() : CSingleAM() {}
    virtual ~CCSvcSAM() {}
};


//! \brief
//! Class for the analyitical model for single output state, nu-SVC type.
class CNuSvcSAM : public CSingleAM {
protected:
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
public:
    CNuSvcSAM() : CSingleAM() {}
    virtual ~CNuSvcSAM() {}
};


//! \brief
//! Class for the analyitical model for single output state, one class type.
class COneClassSAM : public CSingleAM {
protected:
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
public:
    COneClassSAM() : CSingleAM() {}
    virtual ~COneClassSAM() {}
};


//! \brief
//! Class for the analyitical model for single output state, epsilon-SVR type.
class CEpsSvrSAM : public CSingleAM {
protected:
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
public:
    CEpsSvrSAM() : CSingleAM() {}
    virtual ~CEpsSvrSAM() {}
public:
    virtual void openOptimization(); //!< Opens optmization.
    virtual void optimizationPoint2SVMParam(const ArrayXd &ov); //!< Converts optimization point to SVM parameters.
};


//! \brief
//! Class for the analyitical model for single output state, nu-SVR type.
class CNuSvrSAM : public CSingleAM {
protected:
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
public:
    CNuSvrSAM() : CSingleAM() {}
    virtual ~CNuSvrSAM() {}
public:
    virtual void openOptimization(); //!< Opens optmization.
    virtual void optimizationPoint2SVMParam(const ArrayXd &ov); //!< Converts optimization point to SVM parameters.
};


//! \brief
//! Class for analytical representation of the model.
class CAnalyticalModel {
public:
    enum svmtype {epsilonsvr,nusvr}; //!< Enumerator for the possible analytic model types.
private:
    svmtype etype; //!< Holds chosen analytical model type.
    CInputParameters *pIP; //!< Points to input parameters.
    COutputStates *pOS; //!< Points to output states.
    boost::ptr_vector<CSingleAM> sam; //!< Holds pointers to snigle output state analytical models.
    std::vector<ArrayXd> X,Y; //!< Input parameter and output state samples for prediction.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
private:
    CAnalyticalModel() : etype(epsilonsvr), pIP(NULL), pOS(NULL), sam(0), X(0), Y(0)  {}
public:
    CAnalyticalModel(CInputParameters *_pIP,COutputStates *_pOS) : etype(epsilonsvr), pIP(_pIP), pOS(_pOS), sam(0), X(0), Y(0) {}
    virtual ~CAnalyticalModel() {}
public:
    void setSvmType(svmtype _etype) { etype=_etype; } //!< Sets chosen svm type.
    svmtype svmType() const { return etype; } //!< Returns chosen svm type.
    bool empty() const { return sam.empty(); } //!< Returns true if object is empty, true otherwise.
    void clear() { sam.clear(); X.clear(); Y.clear(); }  //!< Clears data.
    void learn(CMADS &_mads,std::ostream &_out=std::cout); //!< Learns SVM models for all output states.
    void learn(CGAModelOptimization &_gamo,std::ostream &_out=std::cout); //!< Learns SVM models for all output states.
    void setInputSamples(const std::vector<ArrayXd> &_X) { X=_X; } //!< Sets internal input parameter samples.
    const std::vector<ArrayXd> &inputSamples() { return X; } //!< Returns internal input parameter samples.
    const std::vector<ArrayXd> &outputSamples() { return Y; } //!< Returns internal output state samples.
    void predict(); //!< Computes predicted values on internal input/output samples.
    double predictValue(const ArrayXd& _X, int i) const; //!< Predicts value of the ith (expanded) output state, _X must be unexpanded.
    ArrayXd predictDerivative(const ArrayXd& _X,int i)  const; //!< Predicts gradient of the ith (expanded) output state, _X must be unexpanded.
    ArrayXd predictValues(const ArrayXd& _X) const; //!< Predicts values of all output states, _X must be unexpanded.
    ArrayXXd predictDerivatives(const ArrayXd& _X) const; //!< Predicts gradients of all (expanded) output states, _X must be unexpanded.
    ArrayXd exportExpectedCurve(const ArrayXd &_x,int _i,int _o) const; //!< Exports _o output vs. _i input curve, while other inputs are set to their expected values.
    ArrayXd exportIntersectionCurve(int _s,const ArrayXd &_x,int _i,int _o) const; //!< Exports _o output vs. _i input curve, while other inputs are set to values in _x.
    ArrayXd evaluate(const ArrayXd& _X); //!< Predicts values of all output states, _X must be unexpanded.
public:
    boost::function<void()>  progressSlot;  //!< External progress slot, later connected to signal for optimization progress.
    void setProgressSlot(boost::function<void()> _progressSlot) { progressSlot=_progressSlot; } //!< Sets external progress slot.
};


} // end of namespace GoSUM

BOOST_CLASS_EXPORT_KEY(GoSUM::CSingleAM)
BOOST_CLASS_EXPORT_KEY(GoSUM::CCSvcSAM)
BOOST_CLASS_EXPORT_KEY(GoSUM::CNuSvcSAM)
BOOST_CLASS_EXPORT_KEY(GoSUM::COneClassSAM)
BOOST_CLASS_EXPORT_KEY(GoSUM::CEpsSvrSAM)
BOOST_CLASS_EXPORT_KEY(GoSUM::CNuSvrSAM)
BOOST_CLASS_EXPORT_KEY(GoSUM::CAnalyticalModel)



#endif // ANALYTICALMODEL_H

