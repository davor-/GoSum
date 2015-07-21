#ifndef GOSUMREDUCTION_H
#define GOSUMREDUCTION_H


#include "SensitivityAnalysis.h"


//! \brief
//! Namespace for GoSUM model.
namespace GoSUM {


//! \brief
//! Class for sensitivity analyis of the model.
class CReduction {
public:
    enum reductiontype {derivative, averagederivative, absoluteaveragederivative, variance, anova}; //!< Enumerator that holds types of sensitivity analysis that can be used as base for model reduction.
    enum cutofftype {cutoffvalue,cutoffsize}; //!< Enumerator that holds types of cutoff criteria that can be used as base for model reduction.
    static enum reductiontype redtype; //!< Holds actual reduction type.
    static enum cutofftype cuttype; //!< Holds actual criteria type.
    static reductiontype ReductionType(const std::string &_stype); //!< Converts reduction type name (string) to reductiontype enumerator.
    static reductiontype ReductionType() { return redtype; } //!< Returns actual reduction type.
    static cutofftype CutoffType() { return cuttype; } //!< Returns actual criteria type.
    static void SetReductionType(enum reductiontype _redtype) { redtype=_redtype; } //!< Sets reduction type.
    static void SetCutoffType(enum cutofftype _cuttype) { cuttype=_cuttype; } //!< Sets reduction type.
private:
    CInputParameters *pIP; //!< Points to input parameters.
    COutputStates *pOS; //!< Points to output states.
    CAnalyticalModel *pAM; //!< Points to the analytical model.
    CSensitivityAnalysis *pSA; //!< Points to the sensitivity analysis.
    std::vector< std::pair<double,bool> > rankIP; //!< Holds pairs of input parameter appropriate maximal sensitivity indices (relative to selected output states), it sorted in descending order  and appropraite selection status.
    std::vector<bool> rankOS; //!< Holds pairs of output state names and appropriate selection status.
    double cutval; //!< Holds cutoff value.
    int cutip; //!< Holds size of the selected inputs.
    int cutos; //!< Holds size of the selected outputs.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
private:
    CReduction() : pIP(NULL), pOS(NULL), pAM(NULL), pSA(NULL), cutval(0.), cutip(0), cutos(0)  {}
public:
    CReduction(CInputParameters *_pIP,COutputStates *_pOS,CAnalyticalModel *_pAM,CSensitivityAnalysis *_pSA) :
        pIP(_pIP), pOS(_pOS), pAM(_pAM), pSA(_pSA), cutval(0.), cutip(0), cutos(0)
    {  if (!_pIP)  throw "GoSUM::CReduction constructor error: null pointer to inputs";
       if (!_pOS)  throw "GoSUM::CReduction constructor error: null pointer to outputs";
       if (!_pAM)  throw "GoSUM::CReduction constructor error: null pointer to anayltical model";
       if (!_pSA)  throw "GoSUM::CReduction constructor error: null pointer to sensitivity analysis"; }
    virtual ~CReduction() {}
public:
    void clear() { rankIP.clear(); rankOS.clear(); cuttype=cutoffsize; cutval=0.; cutip=0; cutos=0;  } //!< Clears data.
    void initialize(); //!< Initializes reducing.
    const ArrayXXd & sensitivityIndex(); //!< Returns actual sensitivitiy index.
    void setReductionType(enum reductiontype _redtype); //!< Sets reduction type.
    void evaluateMaximalSensitivities(); //!< Evaluates maximal indices for all input parameters relative to selected output states.
    void selectInputParameters(); //!< Cuts by actual cutoff criteria.
    int cutoffSize() const { return cutip; }  //!< Returns cutoff size.
    double cutoffValue() const { return cutval; }  //!< return cutoff value.
    void setCutoffSize(int _cutip); //!< Sets cutoff size.
    void setCutoffValue(double _cutval); //!< Sets cutoff value.
    void selectOutputState(int _o); //!< Selects output state for reduction criteria.
    void selectOutputs(const std::vector<std::string> &_selOS); //!< Selects output states named in the _selOS.
    bool isSelectedOutputState(int _o) const; //!< Returns true if particular output state is selected, false otherwise.
    bool isSelectedInputParameter(int _i) const; //!< Returns true if particular input parameter is selected, false otherwise.
    double maximalSensitivity(int _i); //!< Returns maximal  (relative to selected output states) sensitivity index for particular input parameter.
    double maximalSensitivity(); //!< Returns maximal  (relative to selected output states) sensitivity index.
    void eraseNonSelectedVariables(); //!< Reduces the model, i.e. deletes all input paramters and output states that are not selected.
    bool admissible(); //!< Returns true if reduction is admissible, i.e. if the reduced inputs and outputs will not be empty; false otherwise.
};

} // end of namespace GoSUM

BOOST_CLASS_EXPORT_KEY(GoSUM::CReduction)




#endif // REDUCTION_H
