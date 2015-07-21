

#include "Script.h"
//#include <QFileInfo>


GoSUM::CScript::CScript() : pContainer(NULL)
{
    cmds.push_back(make_pair("resample_inputs",(paction)&GoSUM::CScript::resampleInputs));
    cmds.push_back(make_pair("evaluate_outputs",(paction)&GoSUM::CScript::evaluateOutputs));
    cmds.push_back(make_pair("learn_model",(paction)&GoSUM::CScript::learnModel));
    cmds.push_back(make_pair("predict",(paction)&GoSUM::CScript::predict));
    cmds.push_back(make_pair("compute_sensitivities",(paction)&GoSUM::CScript::computeSensitivities));
    cmds.push_back(make_pair("reduce",(paction)&GoSUM::CScript::reduce));
    cmds.push_back(make_pair("minimize",(paction)&GoSUM::CScript::minimize));
    cmds.push_back(make_pair("maximize",(paction)&GoSUM::CScript::maximize));
    cmds.push_back(make_pair("save",(paction)&GoSUM::CScript::save));
    cmds.push_back(make_pair("load",(paction)&GoSUM::CScript::load));
    cmds.push_back(make_pair("save_xml",(paction)&GoSUM::CScript::saveXml));
    cmds.push_back(make_pair("load_xml",(paction)&GoSUM::CScript::loadXml));
    cmds.push_back(make_pair("save_txt",(paction)&GoSUM::CScript::saveTxt));
    cmds.push_back(make_pair("load_txt",(paction)&GoSUM::CScript::loadTxt));
    noeqI=int(cmds.size());
    cmds.push_back(make_pair("set_project_path",(paction)&GoSUM::CScript::setProjectPath));
    cmds.push_back(make_pair("set_project_name",(paction)&GoSUM::CScript::setProjectName));
    cmds.push_back(make_pair("set_project_type",(paction)&GoSUM::CScript::setProjectType));
    cmds.push_back(make_pair("set_optimization_method",(paction)&GoSUM::CScript::setOptimizationMethod));
    cmds.push_back(make_pair("set_core_size",(paction)&GoSUM::CScript::setCoreSize));
    cmds.push_back(make_pair("set_matlab_path",(paction)&GoSUM::CScript::setMatLabPath));
    cmds.push_back(make_pair("set_rng",(paction)&GoSUM::CScript::setRNG));
    cmds.push_back(make_pair("set_model_evaluator",(paction)&GoSUM::CScript::setModelEvaluator));
    cmds.push_back(make_pair("add_input",(paction)&GoSUM::CScript::addInput));
    cmds.push_back(make_pair("add_output",(paction)&GoSUM::CScript::addOutput));
    cmds.push_back(make_pair("add_inputs",(paction)&GoSUM::CScript::addInputs));
    cmds.push_back(make_pair("add_outputs",(paction)&GoSUM::CScript::addOutputs));
    cmds.push_back(make_pair("import_inputs",(paction)&GoSUM::CScript::importInputs));
    cmds.push_back(make_pair("import_outputs",(paction)&GoSUM::CScript::importOutputs));
    cmds.push_back(make_pair("set_resample_type",(paction)&GoSUM::CScript::setResampleType));
    cmds.push_back(make_pair("set_resample_size",(paction)&GoSUM::CScript::setResampleSize));
    cmds.push_back(make_pair("set_voronoi_options",(paction)&GoSUM::CScript::setVoronoiOptions));
    cmds.push_back(make_pair("export_input_samples",(paction)&GoSUM::CScript::exportInputSamples));
    cmds.push_back(make_pair("export_output_samples",(paction)&GoSUM::CScript::exportOutputSamples));
    cmds.push_back(make_pair("import_prediction_input_samples",(paction)&GoSUM::CScript::importPredictionInputSamples));
    cmds.push_back(make_pair("export_prediction_output_samples",(paction)&GoSUM::CScript::exportPredictionOutputSamples));
    cmds.push_back(make_pair("set_sensitivity_options",(paction)&GoSUM::CScript::setSensitivityOptions));
    cmds.push_back(make_pair("set_reduction_type",(paction)&GoSUM::CScript::setReductionType));
    cmds.push_back(make_pair("set_reduction_outputs",(paction)&GoSUM::CScript::setReductionOutputs));
    cmds.push_back(make_pair("set_reduction_cutoff_size",(paction)&GoSUM::CScript::setReductionCutoffSize));
    cmds.push_back(make_pair("set_reduction_cutoff_value",(paction)&GoSUM::CScript::setReductionCutoffValue));
    cmds.push_back(make_pair("set_objective",(paction)&GoSUM::CScript::setObjective));
    cmds.push_back(make_pair("add_optimization_constraint",(paction)&GoSUM::CScript::addOptimizationConstraint));
    cmds.push_back(make_pair("set_const_lower_bound",(paction)&GoSUM::CScript::setConstLowerBound));
    cmds.push_back(make_pair("set_lower_bound",(paction)&GoSUM::CScript::setLowerBound));
    cmds.push_back(make_pair("set_const_upper_bound",(paction)&GoSUM::CScript::setConstUpperBound));
    cmds.push_back(make_pair("set_upper_bound",(paction)&GoSUM::CScript::setUpperBound));
    cmds.push_back(make_pair("set_const_initial_value",(paction)&GoSUM::CScript::setConstInitialValue));
    cmds.push_back(make_pair("set_initial_value",(paction)&GoSUM::CScript::setInitialValue));
    cmds.push_back(make_pair("set_mads_max_evaluation",(paction)&GoSUM::CScript::setMadsMaxEvaluation));
    cmds.push_back(make_pair("set_mads_lh_search",(paction)&GoSUM::CScript::setMadsLHSearch));
    cmds.push_back(make_pair("set_mads_init_mesh_size",(paction)&GoSUM::CScript::setMadsInitMeshSize));
    cmds.push_back(make_pair("set_mads_min_poll_size",(paction)&GoSUM::CScript::setMadsMinPollSize));
    cmds.push_back(make_pair("export_derivative_sensitivity",(paction)&GoSUM::CScript::exportDerivativeSensitivity));
    cmds.push_back(make_pair("export_average_derivative",(paction)&GoSUM::CScript::exportAverageDerivative));
    cmds.push_back(make_pair("export_absolute_average_derivative",(paction)&GoSUM::CScript::exportAbsoluteAverageDerivative));
    cmds.push_back(make_pair("export_variance_sensitivity",(paction)&GoSUM::CScript::exportVarianceSensitivity));
    cmds.push_back(make_pair("export_ANOVA1",(paction)&GoSUM::CScript::exportANOVA1));
    cmds.push_back(make_pair("export_optimization_method",(paction)&GoSUM::CScript::exportOptimizationMethod));
    cmds.push_back(make_pair("export_optimization_history",(paction)&GoSUM::CScript::exportOptimizationHistory));

}



std::string GoSUM::CScript::line2Path(std::istringstream &_line)
{
    std::string _path;
    if ( !std::getline(_line,_path)) throw "getPath error: bad path in input line";
    unsigned first= unsigned(_path.find_first_not_of(' '));
    if (first==std::string::npos) throw "getPath error: empty input line";
    unsigned last= unsigned(_path.find_last_not_of(' '));
    if (last==std::string::npos) throw "getPath error: empty input line";
    return _path.substr(first,last);
}


void GoSUM::CScript::setProjectPath(std::istringstream &iscl)
{
    pContainer->setProjectPath(line2Path(iscl));
}


void GoSUM::CScript::setProjectName(std::istringstream &iscl)
{
    std::string _name;

    if ( !(iscl>>_name) ) throw "GoSUM::CScript::setProjectPath error: bad project path";
    pContainer->setProjectName(_name);
}


void GoSUM::CScript::setProjectType(std::istringstream &iscl)
{
    std::string _stype;

    if ( !(iscl>>_stype) ) throw "GoSUM::CScript::setProjectType error: bad project type";
    pContainer->setProjectType(CContainer::ProjectType(_stype));
}


void GoSUM::CScript::setOptimizationMethod(std::istringstream &iscl)
{
    std::string _stype;

    if ( !(iscl>>_stype) ) throw "GoSUM::CScript::setOptimizationMethod error: bad optimization method type";
    pContainer->setOptimizationMethod(pContainer->OptimizationMethodType(_stype));
}


void GoSUM::CScript::setCoreSize(std::istringstream &iscl)
{
    int _trdN;

    if ( !(iscl>>_trdN) ) throw "GoSUM::CScript::setCoreSize error: bad core size";
    pContainer->setThreadSize(_trdN);
}


void GoSUM::CScript::setMatLabPath(std::istringstream &iscl)
{
    pContainer->setMatLabPath(line2Path(iscl));
}


void GoSUM::CScript::setRNG(std::istringstream &iscl)
{
    std::string _stype;

    if ( !(iscl>>_stype) ) throw "GoSUM::CScript::setRNG error: bad rng type";
    pContainer->setRNG(CRandomGenerator::Type(_stype));
}


void GoSUM::CScript::addVariable(std::istringstream &iscl)
{
    std::string _name;
    std::string _stype;

    if ( !(iscl>>_name) ) throw "GoSUM::CScript::addVariable error: bad name";
    if ( !(iscl>>_stype) ) throw "GoSUM::CScript::addVariable error: bad type";
    CRandomVariable::distributiontype _type=CRandomVariable::Type(_stype);

    int n=CRandomVariable::DistributionParametersSize(_type);
    if ( n==0 )
    {
        (pContainer->*padd)(_name,_type,0.,0.);
    }
    else if ( n==1 )
    {
        double _a;
        if ( !(iscl>>_a) ) throw "GoSUM::CScript::addVariable error: bad distribution parameter";
        (pContainer->*padd)(_name,_type,_a,0);
    }
    else //if ( n==2 )
    {
        double _a,_b;
        if ( !(iscl>>_a>>_b) ) throw "GoSUM::CScript::addVariable error: bad distribution parameters";
        (pContainer->*padd)(_name,_type,_a,_b);
    }
}


void GoSUM::CScript::addVariables(std::istringstream &iscl)
{
    int _N;
    std::string _name;
    std::string _stype;

    if ( !(iscl>>_N) ) throw "GoSUM::CScript::addVariables error: bad size";
    if ( _N<0 )  throw "GoSUM::CScript::addVariables error: negative size";
    if ( !(iscl>>_name) ) throw "GoSUM::CScript::addVariables error: bad name";
    if ( !(iscl>>_stype) ) throw "GoSUM::CScript::addVariables error: bad type";
    CRandomVariable::distributiontype _type=CRandomVariable::Type(_stype);

    int n=CRandomVariable::DistributionParametersSize(_type);
    if ( n==0 )
    {
        (pContainer->*padds)(_N,_name,_type,0.,0.);
    }
    else if ( n==1 )
    {
        double _a;
        if ( !(iscl>>_a) ) throw "GoSUM::CScript::addVariables error: bad distribution parameter";
        (pContainer->*padds)(_N,_name,_type,_a,0);
    }
    else //if ( n==2 )
    {
        double _a,_b;
        if ( !(iscl>>_a>>_b) ) throw "GoSUM::CScript::addVariables error: bad distribution parameters";
        (pContainer->*padds)(_N,_name,_type,_a,_b);
    }
}


void GoSUM::CScript::importVariables(std::istringstream &iscl)
{
    (pContainer->*pimport)(line2Path(iscl));
}


//void GoSUM::CScript::importEmpiricalVariables(std::istringstream &iscl)
//{
//    (pContainer->*pimport)(line2Path(iscl));
//}


void GoSUM::CScript::setResampleType(std::istringstream &iscl)
{
    std::string _stype;

    if ( !(iscl>>_stype) ) throw "GoSUM::CScript::setResampleType error: bad resample type";
    pContainer->setResampleType(CHypercube::Type(_stype));
}


void GoSUM::CScript::setResampleSize(std::istringstream &iscl)
{
    int _N;
    if ( !(iscl>>_N) ) throw "GoSUM::CScript::setResampleSize error: bad size";

    pContainer->setResampleSize(_N);
}


void GoSUM::CScript::setVoronoiOptions(std::istringstream &iscl)
{
    int _maxiter,_q;
    double _alpha2,_beta2;
    if ( !(iscl>>_maxiter) ) throw "GoSUM::CScript::setVoronoiOptions error: bad maxiter";
    if ( !(iscl>>_q) ) throw "GoSUM::CScript::setVoronoiOptions error: bad parameter";
    if ( !(iscl>>_alpha2) ) throw "GoSUM::CScript::setVoronoiOptions error: bad parameter";
    if ( !(iscl>>_beta2) ) throw "GoSUM::CScript::setVoronoiOptions error: bad parameter";

    pContainer->setVoronoiOptions(_maxiter,_q,_alpha2,_beta2);
}


void GoSUM::CScript::resampleInputs(std::istringstream &iscl)
{
    if ( !pContainer->readyToSampleInputs() ) throw "GoSUM::CScript::resampleInputs error: not ready to resample inputs (empty input parameters)";
    pContainer->resampleInputs();
}


void GoSUM::CScript::setModelEvaluator(std::istringstream &iscl)
{
    std::string _stype;
    if ( !(iscl>>_stype) )  throw "GoSUM::CScript::setModelEvaluator error: bad model evaluator type";

    pContainer->setModelEvaluator(GoSUM::CModelEvaluator::Type(_stype),line2Path(iscl));
}


void GoSUM::CScript::evaluateOutputs(std::istringstream &iscl)
{
    if ( !pContainer->readyToEvaluateOutputs() ) throw "GoSUM::CScript::evaluateOutputs error: not ready to evaluate outputs (empty output states or input samples)";
    pContainer->evaluateOutputs();
}


void GoSUM::CScript::exportInputSamples(std::istringstream &iscl)
{
    pContainer->exportInputSamples(line2Path(iscl));
}


void GoSUM::CScript::exportOutputSamples(std::istringstream &iscl)
{
    pContainer->exportOutputSamples(line2Path(iscl));
}



void GoSUM::CScript::learnModel(std::istringstream &iscl)
{
    if ( !pContainer->readyToLearnModel() ) throw "GoSUM::CScript::learnModel error: not ready to learn model (empty output or input samples)";
    pContainer->learnModel();
}


void GoSUM::CScript::setSensitivityOptions(std::istringstream &iscl)
{
    int _N;
    double _eps1,_eps2,_eps3;

    if ( !(iscl>>_N) ) throw "GoSUM::CScript::computeSensitivities error: bad size";
    if ( !(iscl>>_eps1) ) throw "GoSUM::CScript::computeSensitivities error: bad eps1";
    if ( !(iscl>>_eps2) ) throw "GoSUM::CScript::computeSensitivities error: bad eps2";
    if ( !(iscl>>_eps3) ) throw "GoSUM::CScript::computeSensitivities error: bad eps3";

    pContainer->setSensitivityOptions(_N,_eps1,_eps2,_eps3);
}


void GoSUM::CScript::computeSensitivities(std::istringstream &iscl)
{
    if ( !pContainer->readyToComputeSensitivities() ) throw "GoSUM::CScript::computeSensitivities error: not ready to compute sensitivities (empty analytical model)";
    pContainer->computeSensitivities();
}



void GoSUM::CScript::importPredictionInputSamples(std::istringstream &iscl)
{
    pContainer->importPredictionInputSamples(line2Path(iscl));
}


void GoSUM::CScript::exportPredictionOutputSamples(std::istringstream &iscl)
{
    pContainer->exportPredictionOutputSamples(line2Path(iscl));
}


void GoSUM::CScript::predict(std::istringstream &iscl)
{
    pContainer->predict();
}

void GoSUM::CScript::setReductionType(std::istringstream &iscl)
{
    std::string _stype;

    if ( !(iscl>>_stype) ) throw "GoSUM::CScript::setReductionType error: bad reduction type";
    pContainer->setReductionType(GoSUM::CReduction::ReductionType(_stype));
}


void GoSUM::CScript::setReductionOutputs(std::istringstream &iscl)
{
    int i,n;
    if ( !(iscl>>n) ) throw "GoSUM::CScript::setReductionOutputs error: bad reduction outputs size";
    if ( n<=0 ) throw "GoSUM::CScript::setReductionOutputs error: nonpositive reduction outputs size";

    std::vector<std::string> _topos(n);
    for ( i=0; i<n; i++ )
        if ( !(iscl>>_topos[i]) ) throw "GoSUM::CScript::setReductionOutputs error: bad reduction output name";

    pContainer->setReductionOutputs(_topos);
}


void GoSUM::CScript::setReductionCutoffSize(std::istringstream &iscl)
{
    int _cutip;

    if ( !(iscl>>_cutip) ) throw "GoSUM::CScript::setReductionCut error: bad reduction cut";
    pContainer->setReductionCutoffSize(_cutip);
}


void GoSUM::CScript::setReductionCutoffValue(std::istringstream &iscl)
{
    double _c;

    if ( !(iscl>>_c) ) throw "GoSUM::CScript::setReductionCutValue error: bad reduction cut value";
    pContainer->setReductionCutoffValue(_c);
}


void GoSUM::CScript::reduce(std::istringstream &iscl)
{
    pContainer->reduce();
}


void GoSUM::CScript::setObjective(std::istringstream &iscl)
{
    std::string _fexpr;
    if (!(iscl>>_fexpr)) throw "GoSUM::CScript::setObjective error: bad objective";
    pContainer->setObjective(_fexpr);
}


void GoSUM::CScript::addOptimizationConstraint(std::istringstream &iscl)
{
    std::string _gexpr;
    if (!(iscl>>_gexpr)) throw "GoSUM::CScript::addConstraint error: bad constraint";
    pContainer->addOptimizationConstraint(_gexpr);
}


void GoSUM::CScript::setConstLowerBound(std::istringstream &iscl)
{
    double _xl;
    if (!(iscl>>_xl)) throw "GoSUM::CScript::setConstantLowerBound error: bad constant lower bound";
    pContainer->setLowerBound(ArrayXd::Constant(pContainer->inputsSize(),_xl));
}


void GoSUM::CScript::setLowerBound(std::istringstream &iscl)
{
    int i,N=pContainer->inputsSize();
    ArrayXd _xL(N);
    for ( i=0; i<N; i++ )
        if (!(iscl>>_xL(i))) throw "GoSUM::CScript::setLowerBound error: bad lower bound";
    pContainer->setLowerBound(_xL);
}


void GoSUM::CScript::setConstUpperBound(std::istringstream &iscl)
{
    double _xu;
    if (!(iscl>>_xu)) throw "GoSUM::CScript::setConstantUpperBound error: bad constant upper bound";
    pContainer->setUpperBound(ArrayXd::Constant(pContainer->inputsSize(),_xu));
}


void GoSUM::CScript::setUpperBound(std::istringstream &iscl)
{
    int i,N=pContainer->inputsSize();
    ArrayXd _xU(N);
    for ( i=0; i<N; i++ )
        if (!(iscl>>_xU(i))) throw "GoSUM::CScript::setUpperBound error: bad upper bound";
    pContainer->setUpperBound(_xU);
}


void GoSUM::CScript::setConstInitialValue(std::istringstream &iscl)
{
    double _x0;
    if (!(iscl>>_x0)) throw "GoSUM::CScript::setConstantInitialValue error: bad constant initial value";
    pContainer->setInitialValue(ArrayXd::Constant(pContainer->inputsSize(),_x0));
}


void GoSUM::CScript::setInitialValue(std::istringstream &iscl)
{
    int i,N=pContainer->inputsSize();
    ArrayXd _x0(N);
    for ( i=0; i<N; i++ )
        if (!(iscl>>_x0(i))) throw "GoSUM::CScript::setInitialValue error: bad initial value";
    pContainer->setInitialValue(_x0);
}


void GoSUM::CScript::setMadsMaxEvaluation(std::istringstream &iscl)
{
    int _maxeval;

    if ( !(iscl>>_maxeval) ) throw "GoSUM::CScript::setMadsMaxEvaluation error: bad maxeval";

    pContainer->setMadsMaxEvaluation(_maxeval);
}


void GoSUM::CScript::setMadsLHSearch(std::istringstream &iscl)
{
    int _lh0,_lhi;

    if ( !(iscl>>_lh0) ) throw "GoSUM::CScript::setMadsLHSearch error: bad lh0";
    if ( !(iscl>>_lhi) ) throw "GoSUM::CScript::setMadsLHSearch error: bad lhi";

    pContainer->setMadsLHSearch(_lh0,_lhi);
}


void GoSUM::CScript::setMadsInitMeshSize(std::istringstream &iscl)
{
    double _ims;

    if ( !(iscl>>_ims) ) throw "GoSUM::CScript::setMadsInitMeshSize error: bad ims";

    pContainer->setMadsInitMeshSize(_ims);
}


void GoSUM::CScript::setMadsMinPollSize(std::istringstream &iscl)
{
    double _mps;

    if ( !(iscl>>_mps) ) throw "GoSUM::CScript::setMadsMinPollSize error: bad mps";

    pContainer->setMadsMinPollSize(_mps);
}


void GoSUM::CScript::minimize(std::istringstream &iscl)
{
    pContainer->minimize();
}


void GoSUM::CScript::maximize(std::istringstream &iscl)
{
    pContainer->maximize();
}


void GoSUM::CScript::save(std::istringstream &iscl)
{
    pContainer->save();
}


void GoSUM::CScript::load(std::istringstream &iscl)
{
    pContainer->load();
}


void GoSUM::CScript::saveXml(std::istringstream &iscl)
{
    pContainer->saveXml();
}


void GoSUM::CScript::loadXml(std::istringstream &iscl)
{
    pContainer->loadXml();
}


void GoSUM::CScript::saveTxt(std::istringstream &iscl)
{
    pContainer->saveTxt();
}


void GoSUM::CScript::loadTxt(std::istringstream &iscl)
{
    pContainer->loadTxt();
}

void GoSUM::CScript::exportDerivativeSensitivity(std::istringstream &iscl)
{
    pContainer->exportDerivativeSensitivity(line2Path(iscl));
}


void GoSUM::CScript::exportAverageDerivative(std::istringstream &iscl)
{
    pContainer->exportAverageDerivative(line2Path(iscl));
}


void GoSUM::CScript::exportAbsoluteAverageDerivative(std::istringstream &iscl)
{
    pContainer->exportAbsoluteAverageDerivative(line2Path(iscl));
}


void GoSUM::CScript::exportVarianceSensitivity(std::istringstream &iscl)
{
    pContainer->exportVarianceSensitivity(line2Path(iscl));
}


void GoSUM::CScript::exportANOVA1(std::istringstream &iscl)
{
    pContainer->exportFirstOrderANOVA(line2Path(iscl));
}


void GoSUM::CScript::exportOptimizationMethod(std::istringstream &iscl)
{
    pContainer->exportOptimizationMethod(line2Path(iscl));
}


void GoSUM::CScript::exportOptimizationHistory(std::istringstream &iscl)
{
    pContainer->exportOptimizationHistory(line2Path(iscl));
}


void GoSUM::CScript::read(GoSUM::CContainer *_pContainer,const std::string &_fileName)
{
    if (!_pContainer)  throw "GoSUM::CScript::read error: null container pointer";
    pContainer=_pContainer;

    std::ifstream ifs(_fileName);
    if (!ifs.is_open()) throw "CSCript::read error: unable to open script file";

    std::string cl;
    while ( std::getline(ifs,cl) )
    {
        if ( cl.empty() )  continue;
        std::istringstream iscl(cl);
        std::string cmd,eqs;
        if ( !(iscl>>cmd) ) throw "GoSUM::CScript::read error: no script command";
        if ( cmd!="#" )
        {
            int i,n=int(cmds.size());
            for ( i=0; i<noeqI; i++ )  if ( cmd==cmds[i].first )  break;
            if ( i<noeqI )   (this->*cmds[i].second)(iscl);
            else
            {   for ( i=noeqI; i<n; i++ )  if ( cmd==cmds[i].first )  break;
                if ( i<n )
                {   if ( !(iscl>>eqs) || eqs!="=" ) throw "GoSUM::CScript::read error: no equal sign";
                    (this->*cmds[i].second)(iscl);
                }
                else { cout<<cmd<<endl; throw "GoSUM::CScript::read error: unknown script command";  }
            }
        }
    }

}


