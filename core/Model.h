#ifndef GOSUMMODEL_H
#define GOSUMMODEL_H



#include "ModelVariable.h"
#include "Constraints.h"

//! \brief
//! Namespace for GoSUM model.
namespace GoSUM {

class CModelEvaluator;

//! \brief
//! Class for the vector of model variables (inputs & outputs).
class CModelVariables {
protected:
    static std::string nameToFind; //!< Used in member function Find.
    static bool isModelVariableName(const CModelVariable &_aMV) { return _aMV.name()==nameToFind; } //!< Used in member function Find.
protected:
    std::string basename;  //!< Holds basename used for generating model variable unique names.
    boost::ptr_vector<CModelVariable> mvs; //!< Holds pointers to model variables.
    ArrayXd trn,scl; //!< Holds translation and scaling values for normalization of model variables.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
public:
    CModelVariables() :  basename(""), mvs(0) {}
    virtual ~CModelVariables() {}
public:
    std::string baseName() const { return basename; }  //!< Returns base name.
    void setBaseName(std::string _basename) { basename=_basename; }  //!< Sets base name.
    std::string nextName() const; //!< Returns next variable name.
    std::vector<std::string>  expandedNames() const;
    bool empty() const { return mvs.empty(); } //!< Returns true if object contains no model variables, false otherwise.
    bool emptySamples() const { return (sampleSize()==0); }  //!< Returns true if object contains no samples, false otherwise.
    void clear()  { nameToFind=""; mvs.clear(); trn.resize(0); scl.resize(0); } //! Clears data.
    void clearSamples()  { for ( int i=0; i<mvs.size(); i++ ) mvs[i].clearSample(); /*trn.resize(0); scl.resize(0);*/ } //! Clears samples.
    const CModelVariable &operator () (int _at) const { return mvs[_at]; } //!< Returns paticular model variable.
    CModelVariable &operator () (int _at) { return mvs[_at]; } //!< Returns paticular model variable.
    int find(const std::string &_name) const //!< Finds model variable by name, returns it's index.
    {  nameToFind=_name; return int(find_if(mvs.begin(),mvs.end(),CModelVariables::isModelVariableName)-mvs.begin()); }
    int size() const { return int(mvs.size()); } //!< Returns the size of model variables.
    int expandedSize() const; //!< Returns expanded size of model variables (due to categorical).
    int sampleSize() const { return (mvs.empty())? 0 : mvs[0].sampleSize(); } //!< Returns sample size.
    CModelVariable *add(CModelVariable *_pMV)  {  mvs.push_back(_pMV); return _pMV; } //!< Adds, i.e. pushes back model variable.
    CModelVariable *insert(CModelVariable *_pMV,int _before) //!< Inserts model variable in position _before.
    {   if ( !( 0<=_before && _before<=int(mvs.size()) ) )  throw "GoSUM::CModelVariable::insert: index out of range";
        if ( 0<=_before && _before<int(mvs.size()) )  mvs.insert(mvs.begin()+_before,_pMV);
        else  mvs.push_back(_pMV);
        return _pMV; }
    void erase(int _at) //!< Erases particular model variable.
    {   if ( !( 0<=_at && _at<int(mvs.size()) ) )  throw "GoSUM::CModelVariable::erase: index out of range";
        mvs.erase(mvs.begin()+_at); }
    CModelVariable *replace(int _at,CModelVariable *_pMV) //!< Replaces model variable in position _at with model variable _pMV2. The replaced variable is destroyed.
    { erase(_at); return insert(_pMV,_at); }
    void rename(int _at,const std::string &_name) //!< Renames particular model variable.
    {   if ( !( 0<=_at && _at<int(mvs.size()) ) )  throw "GoSUM::CModelVariable::rename: index out of range";
        mvs[_at].setName(_name); }
    std::vector<ArrayXd> samples(); //!< Returns all samples.
    void setSamples(const std::vector<ArrayXd> &_samples); //!< Sets samples of all model variables to values in _samples.
    void computeStatistics(int _n) {   for ( int i=0,N=int(mvs.size()); i<N; i++ ) { mvs[i].computeStatistics(_n); } } //!< Computes statistics of all model variables.
    void computeStatistics(const std::vector<int> &_selected,int _n) //!< Computes statistics of selected model variables.
    {   for ( int i=0,N=int(_selected.size()); i<N; i++ ) mvs[_selected[i]].computeStatistics(_n);  }
    void setDistributions(const std::vector<int> &_selected); //!< For selected model variables detects distribution from empirical sample data.
    void setDistributions();  //!< For all model variables detects distribution from empirical sample data.
    void setNTuple(const ArrayXd &X,int _at); //!< Sets _at sample values of all model variables.
    ArrayXd nTuple(int _at) const; //!< Returns n-tuple containing _at sample values of all model variables.
    ArrayXd expandedNTuple(int _at) const; //!< Returns expanded n-tuple containing _at sample values of all model variables.
    ArrayXd expandNTuple(const ArrayXd &X) const; //!< Expands given n-tuple.
    void setNormalization(); //! (Re)sets normalization of model variables using empirical data in samples.
    double normalize(double Vi,int i) const { return (Vi-trn(i))/scl(i); } //!< Normalizes value of the ith (expanded) model variable.
    ArrayXd normalize(const ArrayXd &V) const { return (V-trn)/scl; } //!< Normalizes values of all (expanded) model variables.
    double denormalize(double Vi,int i) const { return Vi*scl(i)+trn(i); } //!< Denoramlizes value of the ith (expanded) model variable.
    ArrayXd denormalize(const ArrayXd &V) const { return V*scl+trn; } //!< Denormalizes values of all (expanded) model variables.
    double dNormalize(int i) const { return 1./scl(i); } //!< Returns derivative of the normalization function for ith (expanded) model variable.
    ArrayXd dNormalize() const { return 1./scl; } //!< Returns derivatives of the normalization function for (expanded) model variables.
    double dDenormalize(int i) const { return scl(i); } //!< Returns derivative of the denormalization function for ith (expanded) model variable.
    ArrayXd dDenormalize() const { return scl; } //!< Returns derivatives of the denormalization function for (expanded) model variables.
    ArrayXd hcPoint2ModelPoint(const ArrayXd &x); //!< Maps hypercube point to model point.
    void eraseSelectedSamples(const std::vector<int> &sel) //!< Erases selected sample values.
    { for ( int i=0,n=int(mvs.size()); i<n; i++ ) { mvs[i].sample()->eraseSelected(sel); } }
    ArrayXd expectedValue() const; //!< Returns n-tuple containing expected values of all model variables.
};


//! \brief
//! Class for GoSUM input parameters.
class CInputParameters : public CModelVariables {
private:
    CModelConstraints *pIC; //!< Points to model constraints.
    int RSsize; //!< Holds resample size.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
private:
    CInputParameters() : CModelVariables(), pIC(NULL), RSsize(100) { basename="par"; }
public:
    CInputParameters(CModelConstraints *_pIC) : CModelVariables(), pIC(_pIC), RSsize(100) { basename="par"; }
    virtual ~CInputParameters() {}
public:
    int resampleSize() const { return RSsize; } //!< Returns resampling size.
    void setResampleSize(int _RSsize) { RSsize=_RSsize; } //!< Sets resampling size.
    void generateSamples(std::vector<ArrayXd> &_samples); //!< Generates samples on external vector of samples.
    void generateSamples(); //!< Generates samples on internal model variable samples.
    int resampleStepsSize() const; //!< Returns resample steps size.
    CModelConstraints *constraints() const { return pIC; } //!< Returns pointer to model constraints.
public:
    boost::function<void()>  progressSlot;  //!< External progress slot, later connected to signal for evalaution progress.
    void setProgressSlot(boost::function<void()> _progressSlot) { progressSlot=_progressSlot; } //!< Sets external progress slot.
};


//! \brief
//! Class for GoSUM output states.
class COutputStates : public CModelVariables {
private:
    GoSUM::CModelEvaluator *pE; //!< Holds pointer to model evalautor used for single evaluations.
    //! boost serialization
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
public:
    COutputStates() :  CModelVariables(), pE(NULL)  { basename="out"; }
    virtual ~COutputStates() {}
public:
    bool setEvaluatorCompatible(const CInputParameters &inputs);
public:
    void evaluate(const CInputParameters &inputs); //!< Starts end joins multiple threads for model function evaluation.
    void openEvaluation(const CInputParameters &inputs); //! Opens evaluation on single input parameter n-tuples.
    ArrayXd evaluate(const ArrayXd &X);  //!< Evaluates output state on single input parameter n-tuple.
    void closeEvaluation(); //! Closes evaluation on single input parameter n-tuples.
public:
    boost::function<void()>  progressSlot;  //!< External progress slot, later connected to signal for evalaution progress.
    void setProgressSlot(boost::function<void()> _progressSlot) { progressSlot=_progressSlot; } //!< Sets external progress slot.
};



} // end of namespace GoSUM

BOOST_CLASS_EXPORT_KEY(GoSUM::CModelVariables)
BOOST_CLASS_EXPORT_KEY(GoSUM::CInputParameters)
BOOST_CLASS_EXPORT_KEY(GoSUM::COutputStates)



#endif // GOSUMMODEL_H
