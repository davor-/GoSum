#ifndef GOSUMSCRIPT_H
#define GOSUMSCRIPT_H

#include "Container.h"


//! \brief
//! Namespace for GoSUM model.
namespace GoSUM {

class CScript;
typedef void (CScript::*paction)(std::istringstream &);  //!< Typdef for pointer to member function of CScript that is the response to speccific script commands.

//! \brief
//! Class for the GoSUM script format.
class CScript {
private:
    vector< pair<std::string,paction> > cmds; //!< Vector of pairs: first is string of the admissible script command, second is pointer to action function.
    int noeqI;
    CContainer *pContainer; //!< Pointer to GoSUM project container.
    void (CContainer::*padd)(const std::string &,CRandomVariable::distributiontype ,double ,double ); //!< Pointer to member function that tells project to add model variable.
    void (CContainer::*padds)(int ,const std::string & ,CRandomVariable::distributiontype ,double ,double ); //!< Pointer to member function that tells project to add model variables.
    void (CContainer::*pimport)(const std::string &_fname); //!< Pointer to member function that tells project to import data from import file.
private:
    std::string line2Path(std::istringstream &_line); //!< Returns path from the input line.
    void addVariable(std::istringstream &iscl); //!< Tells project to add model variable.
    void addVariables(std::istringstream &iscl); //!< Tells project to add model variables.
    void importVariables(std::istringstream &iscl); //!< Tells project to import model variables.
//    void importEmpiricalVariables(std::istringstream &iscl); //!< Tells project to import empirical model variables.
//    void importVariablesAndSamples(std::istringstream &iscl); //!< Tells project to import model variables and samples.
private:
    //! Tells project to set project path. Format of the related script command is: \n
    //! **set_project_path  =  pp** \n
    //! where **pp** is the path which contains all the project related files, must end with a backslash.
    void setProjectPath(std::istringstream &iscl);

    //! Tells project to set project name. Format of the related script command is: \n
    //! **set_project_name  =  pn** \n
    //! where **pn** is the name of the project.
    void setProjectName(std::istringstream &iscl);

    //! Tells project to set project type. Format of the related script command is: \n
    //! **set_project_type  =  pt** \n
    //! where **pt** is one of the following project types:
    //! *samplegeneration*, *modelanalysis*, *dataanalysis*, *simpleopt*, *modelopt*, *learnedmodelopt*, *learneddataopt*.
    void setProjectType(std::istringstream &iscl);

    //! Tells project to set optimization method. Format of the related script command is: \n
    //! **set_optimization_method  =  om** \n
    //! where **om** is one of the following optimization methods:
    //! *mads*, *ga*.
    void setOptimizationMethod(std::istringstream &iscl);

    //! Tells project to set core size. Format of the related script command is: \n
    //! **set_core_size  =  cs** \n
    //! where **cs** is the core size, i.e. number of cores to be used when multithreading is used in computations.
    //! The optimal value is one less than the number of computer cores.
    void setCoreSize(std::istringstream &iscl);

    //! Tells project to set MatLab path. Format of the related script command is: \n
    //! **set_matlab_path  =  mp** \n
    //! where **mp** is the MatLab path, i.e. the path of the MATLAB.exe.
    void setMatLabPath(std::istringstream &iscl);

    //! Tells project to set random number generator (RNG) type. Format of the related script command is: \n
    //! **set_rng  =  rng** \n
    //! where **rng** is one of the following RNG types:
    //! *smallperiod* (10^8), *mediumperiod* (2 * 10^18), *nonlinearcongruential*, *largeperiod* (3.138*10^57), *dsample* (default), *mads*.
    void setRNG(std::istringstream &iscl);

    //! Tells project to add one input parameter. Format of the related script command is: \n
    //! **add_input  =  nm  tp  a  b** \n
    //! where **nm** is the input parameter name (must be unique), **tp** is the distribution type, and **a** and **b** are distribution parameters,
    //! for some types they must be omitted.
    //! More precisely, following distribution types, and related distribution parameters can be used: \n
    //! - *constantdiscrete*, then **a** is the constant value of that random variable, **b** must be omitted, \n
    //! - *uniformdiscrete*, then **a** and **b** are the lower and the upper boound of that random variable, \n
    //! - *categorical*, then **a** and **b** must be omitted, \n
    //! - *constantcontinuous*, then **a** is the constant value of that random variable, **b** must be omitted, \n
    //! - *uniformcontinuous*, then **a** and **b** are the lower and the upper boound of that random variable, \n
    //! - *gaussian*, then **a** is the mean and **b** is the standard deviation of the gaussian distribution, \n
    //! - *exponential*, then **a** is the rate parameter of the exponential parameter, **b** must be omitted, \n
    //! - *empiricalcontinuous*, then **a** and **b** must be omitted.
    void addInput(std::istringstream &iscl) { padd=&CContainer::addInput; addVariable(iscl); }

    //! Tells project to add one output state. Format of the related script command is: \n
    //! **add_output  =  nm  tp  a  b** \n
    //! where **nm** is the output state name (must be unique), **tp** is the distribution type, and **a** and **b** are distribution parameters,
    //! for some types they must be omitted.
    //! For more details on distribution types and parameters, see description of **add_input** script command.
    void addOutput(std::istringstream &iscl) { padd=&CContainer::addOutput; addVariable(iscl); }

    //! Tells project to add multiple input parameters. Format of the related script command is: \n
    //! **add_inputs  =  sz  nm  tp  a  b** \n
    //! where **sz** is the number of input parameters to be added, **nm** is the common part for the input parameters name (must be unique),
    //! **tp** is the distribution type, and **a** and **b** are distribution parameters, for some types they must be omitted.
    //! Multiple adding of input parameters is possible only for input parameters that have the same distribution,
    //! and their names are generated by adding a suffix to the common part of the name.
    //! For more details on distribution types and parameters, see description of **add_input** script command.
    void addInputs(std::istringstream &iscl) { padds=&CContainer::addInputs; addVariables(iscl); }

    //! Tells project to add multiple output states. Format of the related script command is: \n
    //! **add_outputs  =  sz  nm  tp  a  b** \n
    //! where **sz** is the number of output states to be added, **nm** is the common part for the output state name (must be unique),
    //! **tp** is the distribution type, and **a** and **b** are distribution parameters, for some types they must be omitted.
    //! Multiple adding of output states is possible only for output states that have the same distribution,
    //! and their names are generated by adding a suffix to the common part of the name.
    //! For more details, see description of **add_input** script command.
    void addOutputs(std::istringstream &iscl) { padds=&CContainer::addOutputs; addVariables(iscl); }

    //! Tells project to import input parameters. Format of the related script command is: \n
    //! **import_inputs  =  fn** \n
    //! where **fn** is the name of the import file.
    //! Import file must have one of the following four formats.
    //! (1) In the case of **named theoretical variables format** each line must contain variable name, variable distribution type, and distribution parameters.
    //! (2) In the case of **theoretical variables format** each line must contain variable distribution type and distribution parameters.
    //! (3) In the case of **named empirical variables format** first line must begin with a # sign and must contain names of all imported variables.
    //! The rest of the file must contain sample data, with columns as variables, and rows as samples.
    //! (4) In the case of **empirical variables format** file must contain sample data, with columns as variables, and rows as samples.
    //! For more details on distribution types and distribution parameters see description of **add_input** script command.
    //! (5) In the case of **named declared empirical variables format** first line must begin with a # sign and must contain names of all imported variables,
    //! and second line must begin with a double # sign and must contain types of all imported variables.
    //! The rest of the file must contain sample data, with columns as variables, and rows as samples.
    //! (6) In the case of **declared empirical variables format** first line must begin with a  double # sign and must contain types of all imported variables.
    //! The rest of the file must contain sample data, with columns as variables, and rows as samples.
    //! For more details on distribution types and distribution parameters see description of **add_input** script command.
    void importInputs(std::istringstream &iscl) { pimport=&CContainer::importInputs; importVariables(iscl); }

    //! Tells project to import output states. Format of the related script command is: \n
    //! **import_outputs  =  fn** \n
    //! where **fn** is the name of the import file.
    //! For the description of the import file format see cescrition of **import_inputs** command, format cases (3), (4), (5), and (6).
    void importOutputs(std::istringstream &iscl) { pimport=&CContainer::importOutputs; importVariables(iscl); }

    //! Tells project to set resample type. Format of the related script command is: \n
    //! **set_resample_type  =  rg** \n
    //! where **rg** is one of the following sample generator types: *dsample*, *montecarlo*, *cvoronoi*, *lcvoronoi*.
    void setResampleType(std::istringstream &iscl);

    //! Tells project to set resample size. Format of the related script command is: \n
    //! **set_resample_size  =  sz** \n
    //! where **sz** is the resampling size, i.e. number of the samples to be generated.
    void setResampleSize(std::istringstream &iscl);

    //! Tells project to set Centralized Voronoi Tessellation (CVT) options. Format of the related script command is: \n
    //! **set_voronoi_options  =  maxiter  q alpha2 beta2** \n
    //! where **maxiter** is maximal number of iterations in CVT, and **q**, **alpha2**, and **beta2** are CVT parameters.
    void setVoronoiOptions(std::istringstream &iscl);

    //! Tells project to resample input parameters. Format of the related script command is: \n
    //! **resample_inputs** \n
    void resampleInputs(std::istringstream &iscl);

    //! Tells project to set model evaluator. Format of the related script command is: \n
    //! **set_model_evaluator  =  me  exe** \n
    //! where **me** is on of the following model evaluator types: *exeascii*, *exemat*, *matlabshell*, *matlabengine*;
    //! and **exe** is the name of the executable.
    void setModelEvaluator(std::istringstream &iscl);

    //! Tells project to evaluate output states. Format of the related script command is: \n
    //! **evaluate_outputs** \n
    void evaluateOutputs(std::istringstream &iscl);

    //! Tells project to export input samples. Format of the related script command is: \n
    //! **export_input_samples  =  fn** \n
    //! where **fn** is the name of the export file.
    void exportInputSamples(std::istringstream &iscl);

    //! Tells project to export output samples. Format of the related script command is: \n
    //! **export_output_samples  =  fn** \n
    //! where **fn** is the name of the export file.
    void exportOutputSamples(std::istringstream &iscl);

    //! Tells project to learn analytical model from the original model. Format of the related script command is: \n
    //! **learn_model** \n
    void learnModel(std::istringstream &iscl);

    //! Tells project to import prediction input samples. Format of the related script command is: \n
    //! **import_prediction_input_samples  =  fn** \n
    //! where **fn** is the name of the import file.
    void importPredictionInputSamples(std::istringstream &iscl);

    //! Tells project to export prediction output samples. Format of the related script command is: \n
    //! **export_prediction_output_samples  =  fn** \n
    //! where **fn** is the name of the export file.
    void exportPredictionOutputSamples(std::istringstream &iscl);

    //! Tells project to predict using analytical model. Format of the related script command is: \n
    //! **predict** \n
    void predict(std::istringstream &iscl);

    //! Tells project to set sensitivity options. Format of the related script command is: \n
    //! **set_sensitivity_options  =  sz  eps1  eps2  eps3** \n
    //! where **sz** is the resized sample size, **eps1** is the sensitivity error (default 0.005),
    //! **eps2** is the separability error (default 0.01), and **eps3** is the coupling error (default 0.01).
    void setSensitivityOptions(std::istringstream &iscl);

    //! Tells project to compute sensitivities. Format of the related script command is: \n
    //! **compute_sensitivities** \n
    void computeSensitivities(std::istringstream &iscl);

    //! Tells project to set model reduction type. Format of the related script command is: \n
    //! **set_reduction_type  =  rt** \n
    //! where **rt** is one of the following model reduction types:
    //! *derivative*, *averagederivative*, *absoluteaveragederivative*, *variance*.
    void setReductionType(std::istringstream &iscl);

    //! Tells project to set/choose outputs for the model reduction. Format of the related script command is: \n
    //! **set_reduction_outputs  =  n  o1  o2  ...** \n
    //! where **rn** is size of chosen outputs, and **o1**, **o2**, etc. are names of the chosen outputs.
    void setReductionOutputs(std::istringstream &iscl);

    //! Tells project to set reduction cut. Format of the related script command is: \n
    //! **set_reduction_cutoff_size  =  c** \n
    //! where **c** is size of chosen top intputs.
    void setReductionCutoffSize(std::istringstream &iscl);

    //! Tells project to set reduction cut value. Format of the related script command is: \n
    //! **set_reduction_cutoff_value  =  c** \n
    //! where **c** is the cutoff value for choosing top inputs.
    void setReductionCutoffValue(std::istringstream &iscl);

    //! Tells project to reduce model. Format of the related script command is: \n
    //! **reduce** \n
    void reduce(std::istringstream &iscl);

    //! Tells project to set objective. Format of the related script command is: \n
    //! **set_objective  =  fexpr** \n
    //! where **fexpr** is the analytical expression of the objective function.
    void setObjective(std::istringstream &iscl);

    //! Tells project to add an optimization constraint. Format of the related script command is: \n
    //! **add_optimization_constraint  =  gexpr** \n
    //! where **gexpr** is the analytical expression of the constraint.
    void addOptimizationConstraint(std::istringstream &iscl);

    //! Tells project to set constant lower bounds. Format of the related script command is: \n
    //! **set_const_lower_bound  =  xl** \n
    //! where **xl** is the same lower bound value for all decision variables.
    void setConstLowerBound(std::istringstream &iscl);

    //! Tells project to set lower bounds for all decision variables. Format of the related script command is: \n
    //! **set_lower_bound  =  x1_L x2_L ...** \n
    //! where **x1_L** , **x2_L** , etc. are lower bounds for decision variables.
    void setLowerBound(std::istringstream &iscl);

    //! Tells project to set constant upper bound. Format of the related script command is: \n
    //! **set_const_upper_bound  =  xu** \n
    //! where **xu** is the same upper bound value for all decision variables.
    void setConstUpperBound(std::istringstream &iscl);

    //! Tells project to set upper bounds for all decision variables. Format of the related script command is: \n
    //! **set_upper_bound  =  x1_U x2_U ...** \n
    //! where **x1_U** , **x2_U** , etc. are upper bounds for decision variables.
    void setUpperBound(std::istringstream &iscl);

    //! Tells project to set constant initial value. Format of the related script command is: \n
    //! **set_const_upper_bound  =  x0** \n
    //! where **x0** is the same initial value for all decision variables.
    void setConstInitialValue(std::istringstream &iscl);

    //! Tells project to set initial values for all decision variables. Format of the related script command is: \n
    //! **set_initial_value  =  x1_0 x2_0 ...** \n
    //! where **x1_0** , **x2_0** , etc. are initial values for decision variables.
    void setInitialValue(std::istringstream &iscl);

    //! Tells project to set maximal number of evaluations in MADS. Format of the related script command is: \n
    //! **set_mads_max_evaluation  =  maxeval** \n
    //! where **maxeval** is the maximal number of evaluations.
    void setMadsMaxEvaluation(std::istringstream &iscl);

    //! Tells project to set LH search parameters in MADS. Format of the related script command is: \n
    //! **set_mads_lh_search  =  lh0  lhi** \n
    //! where **lh0** is the inital LH value, and **lhi** is the LH iteration.
    void setMadsLHSearch(std::istringstream &iscl);

    //! Tells project to set inital mesh size in MADS. Format of the related script command is: \n
    //! **set_mads_init_mesh_size  =  ims** \n
    //! where **ims** is the inital mesh size.
    void setMadsInitMeshSize(std::istringstream &iscl);

    //! Tells project to set minimal poll size in MADS. Format of the related script command is: \n
    //! **set_mads_min_poll_size  =  mps** \n
    //! where **mps** is the minimal poll size.
    void setMadsMinPollSize(std::istringstream &iscl);

    //! Tells project to find minimum of the previously defined optimization problem. Format of the related script command is: \n
    //! **minimize** \n
    void minimize(std::istringstream &iscl);

    //! Tells project to find maximum of the previously defined optimization problem. Format of the related script command is: \n
    //! **maximize** \n
    void maximize(std::istringstream &iscl);

    //! Tells project to save to binary format. Format of the related script command is: \n
    //! **save** \n
    void save(std::istringstream &iscl);

    //! Tells project to load from binary format. Format of the related script command is: \n
    //! **load** \n
    void load(std::istringstream &iscl);

    //! Tells project to save to xml format. Format of the related script command is: \n
    //! **save_xml** \n
    void saveXml(std::istringstream &iscl);

    //! Tells project to load from xml format. Format of the related script command is: \n
    //! **load_xml** \n
    void loadXml(std::istringstream &iscl);

    //! Tells project to save to txt format. Format of the related script command is: \n
    //! **save_txt** \n
    void saveTxt(std::istringstream &iscl);

    //! Tells project to load from txt format. Format of the related script command is: \n
    //! **load_txt** \n
    void loadTxt(std::istringstream &iscl);

    //! Tells project to export derivative sensitivities. Format of the related script command is: \n
    //! **export_derivative_sensitivity  =  fn** \n
    //! where **fn** is the name of the export file.
    void exportDerivativeSensitivity(std::istringstream &iscl);

    //! Tells project to export average derivative. Format of the related script command is: \n
    //! **export_average_derivative  =  fn** \n
    //! where **fn** is the name of the export file.
    void exportAverageDerivative(std::istringstream &iscl);

    //! Tells project to export absolute average derivative. Format of the related script command is: \n
    //! **export_absolute_average_derivative  =  fn** \n
    //! where **fn** is the name of the export file.
    void exportAbsoluteAverageDerivative(std::istringstream &iscl);

    //! Tells project to export variance sensitivities. Format of the related script command is: \n
    //! **export_variance_sensitivity  =  fn** \n
    //! where **fn** is the name of the export file.
    void exportVarianceSensitivity(std::istringstream &iscl);

    //! Tells project to export first order ANOVA. Format of the related script command is: \n
    //! **export_ANOVA1  =  fn** \n
    //! where **fn** is the name of the export file.
    void exportANOVA1(std::istringstream &iscl);

    //! Tells project to export optimization method. Format of the related script command is: \n
    //! **export_optimization_method  =  fn** \n
    //! where **fn** is the name of the export file.
    void exportOptimizationMethod(std::istringstream &iscl);

    //! Tells project to export optimization history. Format of the related script command is: \n
    //! **export_optimization_history  =  fn** \n
    //! where **fn** is the name of the export file.
    void exportOptimizationHistory(std::istringstream &iscl);
public:
    CScript();
    virtual ~CScript() {}
public:
    void read(GoSUM::CContainer *_pContainer,const std::string &_fileName); //!< Reads script file and interprets all script commands.
};


} // end namespace GoSUM


#endif // SCRIPT_H
