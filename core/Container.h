#ifndef CONTAINER_H
#define CONTAINER_H


#include "Reduction.h"
#include "MADSOptimization.h"
#include "GAOptimization.h"
#include "ParserOptimizationProblem.h"
#include "OriginalModel.h"
#include "Hypercube.h"


//! \brief
//! Namespace for GoSUM model.
namespace GoSUM {


//! \brief
//! Class for the GoSUM main project.
class CContainer {
public:
    enum projecttype {samplegeneration,simpleanalysis,modelanalysis,dataanalysis,simpleoptimization,modeloptimization,learnedmodeloptimization,learneddataoptimization }; //!< Enumerator for the possible project types.
    enum optimizationmethodtype {mads,ga}; //!< Enumerator for the possible optimization method types.
    static projecttype ProjectType(const std::string &_stype); //!< Converts project type name (string) to projecttype enumerator-
    static optimizationmethodtype OptimizationMethodType(const std::string &_stype); //!< Converts optimization method name (string) to optimizationmethodtype enumerator.
private:
    int computestatSize;
    std::string prjPath,prjName; //!< Project path and name.
    projecttype prjType; //!< Project type.
    optimizationmethodtype omType; //!< Optimization method type.
    CHypercube hycube; //!< Basic hypercube, used for resampling.
    CModelConstraints inconsts; //!< Model constraints, i.e. constraints on input paramters.
    CInputParameters inputs; //!< Input parameters of the project.
    COutputStates outputs; //!<  Output states of the project.
    CEvaluator evaluator; //!<  Basic evalautor of the project, used only for model based projects.
    CAnalyticalModel analytical; //!< Analytic model of the project.
    CSensitivityAnalysis sensitivity; //!< Sensistivity analysis of the analytical model.
    CReduction reduced; //!< Reduction operator.
    CSimpleOptimizationProblem *pOP; //!< Defined optimization problem.
    CMADS MADS; //!< Mesh Adaptive Direct Search optimization method.
    CGAModelOptimization GAMO; //!< Genetic Algorithms optimization method.
    std::vector<int> selectedsamples; //!< Vector of selected sample values indices.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
public:
    CContainer();
    virtual ~CContainer();

// add/delete model variables
public:
    void addDefaultVariable(CModelVariables *pMVs,CRandomVariable::distributiontype _dtype); //!< Adds one model variable, input and output, with default distribution parameter values.
    void addVariable(CModelVariables *pMVs,CRandomVariable::distributiontype _dtype,double _a=0.,double _b=0.); //!< Adds one model variable, input and output.
    void addVariable(CModelVariables *pMVs,const std::string &_name,CRandomVariable::distributiontype _dtype,double _a=0.,double _b=0.); //!< Adds one model variable, input and output.
    void eraseVariable(CModelVariables *pMVs,std::string _name); //!< Erases model variable with _name.
    void cloneVariable(CModelVariables *pMVs,int _at); //!< Clones particular model variable.
    void addVariables(CModelVariables *pMVs,int _N,CRandomVariable::distributiontype _dtype,double _a=0.,double _b=0.); //!< Adds multiple model variables, input and output.
    void addVariables(CModelVariables *pMVs,const std::string &_name,int _N,CRandomVariable::distributiontype _dtype,double _a=0.,double _b=0.); //!< Adds multiple model variables, input and output.
    void addInput(const std::string &_name,CRandomVariable::distributiontype _type,double _a=0.,double _b=0.) { addVariable(&inputs,_name,_type,_a,_b); } //!< Adds one input parameter.
    void addOutput(const std::string &_name,CRandomVariable::distributiontype _type,double _a=0.,double _b=0.) { addVariable(&outputs,_name,_type,_a,_b); } //!< Adds one output state.
    void addInputs(int _N,const std::string &_name,CRandomVariable::distributiontype _type,double _a=0.,double _b=0.) { addVariables(&inputs,_name,_N,_type,_a,_b); } //!< Adds multiple input parameters.
    void addOutputs(int _N,const std::string &_name,CRandomVariable::distributiontype _type,double _a=0.,double _b=0.) { addVariables(&outputs,_name,_N,_type,_a,_b); } //!< Adds multiple output states.

// save/load
public:
    void save(); //!< Saves project to binary format.
    void load(); //!< Loads project from binary format.
    void saveXml(); //!< Saves project to xml format.
    void loadXml(); //!< Loads project from xml format.
    void saveTxt(); //!< Saves project to txt format.
    void loadTxt(); //!< Loads project from txt format.

// import/export
public:
    bool containsTheoreticalViarables(const std::string &_fname); //!< Returns true if file _fname contains theoretical variables, false otherwise.
    bool containsNamedTheoreticalViarables(const std::string &_fname); //!< Returns true if file _fname contains named theoretical variables, false otherwise.
    bool containsEmpiricalViarables(const std::string &_fname,int &SSize,std::vector<CRandomVariable::distributiontype> &dtypes); //!< Returns true if file _fname contains empirical variables, false otherwise.
    bool containsNamedEmpiricalViarables(const std::string &_fname,int &SSize,std::vector<CRandomVariable::distributiontype> &dtypes); //!< Returns true if file _fname contains named empirical variables, false otherwise.
    bool containsDeclaredEmpiricalViarables(const std::string &_fname,int &SSize,std::vector<CRandomVariable::distributiontype> &dtypes); //!< Returns true if file _fname contains explicitly typed empirical variables, false otherwise.
    bool containsNamedDeclaredEmpiricalViarables(const std::string &_fname,int &SSize,std::vector<CRandomVariable::distributiontype> &dtypes); //!< Returns true if file _fname contains named explicitly typed empirical variables, false otherwise.
    bool containsPredictionSamples(const std::string &_fname,int &Ncols,int &Nrows); //!< Returns true if file _fname contains predicition samples, false otherwise.
    void importTheoreticalVariables(GoSUM::CModelVariables *pMVs,const std::string &_fname); //!< Imports multiple theoretical model variables from a file.
    void importNamedTheoreticalVariables(GoSUM::CModelVariables *pMVs,const std::string &_fname); //!< Imports multiple theoretical model variables from a file.
    void importEmpiricalVariables(GoSUM::CModelVariables *pMVs,const std::string &_fname,int &SSize,std::vector<CRandomVariable::distributiontype> &dtypes); //!< Imports multiple empirical model variables from a file.
    void importNamedEmpiricalVariables(GoSUM::CModelVariables *pMVs,const std::string &_fname,int &SSize,std::vector<CRandomVariable::distributiontype> &dtypes); //!< Imports multiple empirical model variables from a file.
    void importDeclaredEmpiricalVariables(GoSUM::CModelVariables *pMVs,const std::string &_fname,int &SSize,std::vector<CRandomVariable::distributiontype> &dtypes); //!< Imports multiple empirical model variables from a file.
    void importNamedDeclaredEmpiricalVariables(GoSUM::CModelVariables *pMVs,const std::string &_fname,int &SSize,std::vector<CRandomVariable::distributiontype> &dtypes); //!< Imports multiple empirical model variables from a file.
    void importVariables(GoSUM::CModelVariables *pMVs,const std::string &_fname); //!< Imports multiple model variables from a file, input and output.
    void exportSamples(CModelVariables *pMVs,const std::string &_fname); //!< Exports samples for model variables, input and output.
    void importInputs(const std::string &_fname) { importVariables(&inputs,_fname); } //!< Imports multiple input parameters.
    void importOutputs(const std::string &_fname) { importVariables(&outputs,_fname); }  //!< Imports multiple output states.
    void exportInputSamples(const std::string &_fname) { exportSamples(&inputs,_fname); } //!< Imports input samples from file.
    void exportOutputSamples(const std::string &_fname) { exportSamples(&outputs,_fname); } //!< Imports output samples from file.
    bool importPredictionInputSamples(const std::string &_fname); //!< Imports prediction input samples.
    void exportPredictionOutputSamples(const std::string &_fname);  //!< Exports prediction output samples.
    void exportDerivativeSensitivity(const std::string &_fname); //!< Exports derivative sensitivity.
    void exportAverageDerivative(const std::string &_fname); //!< Exports average derivative sensitivity.
    void exportAbsoluteAverageDerivative(const std::string &_fname); //!< Exports absolute average derivative sensitivity.
    void exportVarianceSensitivity(const std::string &_fname); //!< Exports variance sensitivity.
    void exportFirstOrderANOVA(const std::string &_fname); //!< Exports first order ANOVA sensitivity.
    void exportOptimizationMethod(const std::string &_fname); //!< Exports optimization method.
    void exportOptimizationHistory(const std::string &_fname); //!< Exports optimization history.
    void importModelConstraints(const std::string &_fname); //!< Imports model constraint expressions.
    void importOptimizationConstraints(const std::string &_fname); //!< Imports optimization constraint expressions.
    void importLowerBound(const std::string &_fname); //!< Imports lower bounds for optimization variables.
    void importUpperBound(const std::string &_fname); //!< Imports upper bounds for optimization variables.
    void importInitialValue(const std::string &_fname); //!< Imports inital values for optimization variables.

// main operators
public:
    void resampleInputs(); //!< Resamples input parameters.
    void evaluateOutputs(); //!< Evaluates output states using external executable.
    void learnModel(); //!< Learns model.
    void predict() {  analytical.predict(); } //!< Predicts (using analytical model).
    void predictMean(ArrayXd &ymu,ArrayXd &yvar); //!< Predicts mean and variance using learned model.
    void computeSensitivities(); //!< Computes sensitivities.
    void optimize(); //!< Optimizes.
    void reduce(); //!< Reduces model.
    void minimize(); //!< Minimizes.
    void maximize(); //!< Maximizes.

// clear functions
public:
    void clear(); //!< Clears all project content.
    void clearResults(); //!< Clears all project results.
    void clearForReducing(); //!< Clears all results except reducer.
    void clearSamplingResults(CModelVariables *_pMVs); //!< Clears project results  starting from particular sampling.
    void clearLearningResults(); //!< Clears project results, starting from learning.
    void clearSensitivityResults(); //!< Clears project results, starting from senstivity.
    void clearOptimizationResults(); //!< Clears optimization results.

// empty question
public:
    bool emptyResults() const; //!< Returns true if results are empty, false otherwise.
    bool emptySamplingResults(CModelVariables *_pMVs) const; //!< Returns true if results starting from the _pMVs sampling are empty, false otherwise.
    bool emptyLearningResults() const; //!< Returns true if learning results are empty, false otherwise.
    bool emptySensitivityResults() const; //!< Returns true if sensitivity results are empty, false otherwise.
    bool emptyOptimizationResults() const; //!< Returns true if optimization results are empty, false otherwise.
    bool emptySelectedSamples() const { return selectedsamples.empty(); } //!< Returns true if selected samples are empty, false otherwise.

// various set/get
public:
    void setProjectPath(const std::string &_prjPath)  { prjPath=_prjPath; } //!< Sets project path.
    std::string projectPath()  { return prjPath; } //!< Returns project path.
    void setProjectName(const std::string &_prjName)  { prjName=_prjName; } //!< Sets project name.
    std::string  projectName()  { return prjName; } //!< Returns project name.
    std::string  longProjectName(); //!< Returns long project name.
    void setProjectType(projecttype _prjType); //!< Sets chosen project type.
    projecttype projectType() {return prjType; } //!< Returns chosen project type.
    void setOptimizationMethod(optimizationmethodtype _omType) { omType=_omType; } //!< Sets chosen optimization method type.
    optimizationmethodtype optimizationMethod() { return omType; } //!< Returns chosen optimization method type.
    int learnEvaluationSize() { return MADS.evaluationSize()*outputs.size(); } //!< Returns maximal number of learn optimization evaluations.
    void setThreadSize(int _trdN) { evaluator.SetThreadSize(_trdN); } //!< Sets chosen thread size.
    void setMatLabPath(const std::string &_matlabPath)  { CMATLAB::SetPath(_matlabPath); } //!< Sets MatLab path.
    void setRNG(CRandomGenerator::rngtype _type) { CRandomGenerator::Set(_type); } //!< Sets chosen rng type.
    void setResampleType(CHypercube::hctype _type) { hycube.SetType(_type); } //!< Sets resample type.
    void setResampleSize(int _N) { inputs.setResampleSize(_N); } //!< Sets resample size.
    void setVoronoiOptions(int _maxiter,int _q,double _alpha2,double _beta2) { hycube.SetVoronoiOptions(_maxiter,_q,_alpha2,_beta2); } //!< Sets Voronoi options.
    void setModelEvaluator(GoSUM::CModelEvaluator::evaluatortype _me,const std::string &_filename); //!< Sets model evaluator.
    void setSensitivityOptions(int _N, double _eps1=0.005, double _eps2=0.01, double _eps3=0.01); //!< Sets sensitivity parameters.
    void setReductionType(GoSUM::CReduction::reductiontype _rtype) { reduced.SetReductionType(_rtype); } //!< Sets model reduction type.
    void setReductionOutputs(const std::vector<std::string> &_selOS) {  reduced.selectOutputs(_selOS); } //!< Sets reduction outputs.
    void setReductionCutoffSize(int _cutip) { reduced.setCutoffSize(_cutip); }  //!< Sets reduction inputs cutoff size.
    void setReductionCutoffValue(double _cutval) { reduced.setCutoffValue(_cutval); } //!< Sets reduction inputs cut value.
    void resetOptimizationVariable(CModelVariables *pMVs,int _at); //!< Resets values in the particular optimization variable.
    void setObjective(const std::string &_fexpr); //!< Sets objective of the optimization problem.
    void addOptimizationConstraint(const std::string &_gexpr); //!< Adds constraint of the optimization problem.
    void setLowerBound(const ArrayXd &_xL); //!< Sets lower bound of the optimization variables.
    void setUpperBound(const ArrayXd &_xU); //!< Sets upper bound of the optimization variables.
    void setInitialValue(const ArrayXd &_x0); //!< Sets initial value of the optimization variables.
    void setMadsMaxEvaluation(int _maxeval) { MADS.setEvaluationSize(_maxeval); } //!< Sets MADS maximal number of evaluations.
    void setMadsLHSearch(int _lh0,int _lhi) { MADS.setInitialLHSearch(_lh0); MADS.setIterationLHSearch(_lhi); } //!< Sets MADS lh search paraemters.
    void setMadsInitMeshSize(double _ims) { MADS.setInitialMeshSize(_ims); } //!< Sets MADS initial mesh size.
    void setMadsMinPollSize(double _mps) { MADS.setMinimalPollSize(_mps); } //!< Sets MADS final poll size.

// select/cut tail
public:
    bool isSampleSelected(int _i) const; //!< Returns true if particular sample value is selected, false otherwise.
    void selectSamples(CModelVariable *pmv,double _left,double _right); //!< Selects sample values of variable pmv that are contained in [_left,_right] interval.
    void separateBySelection(const ArrayXd &X,std::vector<double> &x,std::vector<double> &selx) const; //!< Separates array X into two vectors based on the index (not contained/contained in selected samples).
    void eraseSelectedSamples(); //!< Erases selected sample values in all model variables.

// axcess to main objects
public:
    int inputsSize() { return inputs.size(); } //!< Returns input parameters size.
    int outputsSize() { return outputs.size(); } //!< Returns output states size.
    CInputParameters &inputParameters() { return inputs; } //!< Returns input parameters.
    CModelConstraints &modelConstraints() { return inconsts; } //!< Returns model constraints.
    CHypercube &hyperCube() { return hycube; } //!< Returns hypercube.
    CModelVariable &inputParameter(int _at) { return inputs(_at); } //!< Returns particular input parameter.
    CModelVariable &outputState(int _at) { return outputs(_at); } //!< Returns particular output state.
    COutputStates &outputStates() { return outputs; } //!< Returns output states.
    CEvaluator &modelEvaluator() { return evaluator; } //!< Returns evaluator.
    CAnalyticalModel &analyticalModel() { return analytical; } //!< Returns analytical model.
    CSensitivityAnalysis &sensitivityAnalysis() { return sensitivity; } //!< Returns sensitivity analysis.
    CReduction &reducer() { return reduced; } //!< Returns reduction tool.
    CSimpleOptimizationProblem *optimizationProblem()  { return pOP; }  //!< Returns optimization problem.
    CMADS &madsOptimizer() { return MADS; } //!< Returns MADS.
    CGAModelOptimization &gaOptimizer() { return GAMO; } //!< Returns MADS.
    std::vector<int> &selectedSamples() { return selectedsamples; }

// ready
public:
    bool readyToSampleInputs(); //!< Returns true if project is ready to sample inputs, false otherwise.
    bool readyToEvaluateOutputs(); //!< Returns true if project is ready to evaluate outputs, false otherwise.
    bool readyToLearnModel(); //!< Returns true if project is ready to learn model, false otherwise.
    bool readyToComputeSensitivities(); //!< Returns true if project is ready to compute sensitivities, false otherwise.
    bool readyToReduceModel(); //!< Returns true if project is ready to reduce model, false otherwise.
    bool readyToSetOptimization(); //!< Returns true if project is ready to set optimization, false otherwise.
    bool readyToOptimize(); //!< Returns true if project is ready to optimize, false otherwise.

// start threads
 public:
    boost::thread thrd; //!< Thread for computations.
    void startResampling(); //!< Starts sampling inputs.
    void startEvaluating(); //!< Starts evaluating outputs.
    void startLearning(); //!< Starts learning model.
    void startSensitivityComputing(); //!< Starts sensitivity computing.
    void startOptimizing(); //!< Starts optimizing.
    void join(); //!< Joins thread for computation.

// progress signals
public:
    int optimizationStepsSize() const { return (omType==mads)?MADS.progressStepsSize():GAMO.progressStepsSize(); } //!< Returns maximal number of optimization evaluations.
    int learningStepsSize() const { return MADS.progressStepsSize()*outputs.size(); } //!< Returns learning steps size.
    boost::signal<void()> resamplingFinished; //!< Signal for sampling inputs finshed.
    boost::signal<void()> evaluatingFinished; //!< Signal for evaluating outputs finshed.
    boost::signal<void()> learningFinished; //!< Signal for learning model finshed.
    boost::signal<void()> sensitivityComputingFinished; //!< Signal for sensitivity computing finshed.
    boost::signal<void()> optimizingFinished; //!< Signal for optimizing finshed.
};


} // end namespace GoSUM



#endif // CONTAINER_H
