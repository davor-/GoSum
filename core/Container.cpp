

#include "Container.h"




GoSUM::CContainer::projecttype GoSUM::CContainer::ProjectType(const std::string &_stype)
{
    if ( _stype=="samplegeneration" )                 return samplegeneration;
    else if ( _stype=="simpleanalysis" )              return simpleanalysis;
    else if ( _stype=="modelanalysis" )               return modelanalysis;
    else if ( _stype=="dataanalysis" )                return dataanalysis;
    else if ( _stype=="simpleoptimization" )          return simpleoptimization;
    else if ( _stype=="modeloptimization" )           return modeloptimization;
    else if ( _stype=="learnedmodeloptimization" )    return learnedmodeloptimization;
    else if ( _stype=="learneddataoptimization" )     return learneddataoptimization;
    else  throw "GoSUM::CContainer::ProjectType error: unrecognised type";
}


GoSUM::CContainer::optimizationmethodtype GoSUM::CContainer::OptimizationMethodType(const std::string &_stype)
{
    if ( _stype=="mads" )      return mads;
    else if ( _stype=="ga" )   return ga;
    else  throw "GoSUM::CContainer::OptimizationMethodType error: unrecognised type";
}


template <class Archive>
void GoSUM::CContainer::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("prjType",prjType);
    ar & boost::serialization::make_nvp("omType",omType);
    ar & boost::serialization::make_nvp("rng",CRandomGenerator::Type());
    ar & boost::serialization::make_nvp("matlab",CMATLAB::Path());
    ar & boost::serialization::make_nvp("hycube",hycube);
    ar & boost::serialization::make_nvp("inconsts",inconsts);
    ar & boost::serialization::make_nvp("inputs",inputs);
    ar & boost::serialization::make_nvp("outputs",outputs);
    ar & boost::serialization::make_nvp("evaluator",evaluator);
    ar & boost::serialization::make_nvp("analytical",analytical);
    ar & boost::serialization::make_nvp("sensitivity",sensitivity);
    ar & boost::serialization::make_nvp("reduced",reduced);
    ar & boost::serialization::make_nvp("pOP",pOP);
    ar & boost::serialization::make_nvp("MADS",MADS);
    ar & boost::serialization::make_nvp("GAMO",GAMO);
    ar & boost::serialization::make_nvp("selectedsamples",selectedsamples);
}

GoSUM::CContainer::CContainer(): computestatSize(1<<14),
                                 prjPath("C:"), prjName("untitled"), prjType(samplegeneration), omType(mads),
                                 inputs(&inconsts), analytical(&inputs,&outputs), sensitivity(&inputs,&outputs,&analytical),
                                 reduced(&inputs,&outputs,&analytical,&sensitivity), pOP(NULL)
{
    inconsts.set(&inputs);
}


GoSUM::CContainer::~CContainer()
{
    if (pOP) delete pOP;
}


void GoSUM::CContainer::clear()
{
    inconsts.clear();
    inputs.clear();
    outputs.clear();
    evaluator.clear();
    analytical.clear();
    sensitivity.clear();
    reduced.clear();
    if (pOP) { pOP->clear(); }
    selectedsamples.clear();
}


bool GoSUM::CContainer::emptyResults() const
{
    return ((!pOP || pOP->emptyHistory()) && sensitivity.empty() && analytical.empty() && outputs.emptySamples() && inputs.emptySamples() );
}


bool GoSUM::CContainer::emptySamplingResults(CModelVariables *_pMVs) const
{
    if ( _pMVs == &inputs )
        return ( (!pOP || pOP->emptyHistory()) && sensitivity.empty() && analytical.empty() && outputs.emptySamples() && inputs.emptySamples() );
    else //if ( _pMVs == &outputs )
        return ( (!pOP || pOP->emptyHistory()) && sensitivity.empty() && analytical.empty() && outputs.emptySamples() );
}


bool GoSUM::CContainer::emptyLearningResults() const
{
    return ( (!pOP || pOP->emptyHistory()) && sensitivity.empty() && analytical.empty() );
}


bool GoSUM::CContainer::emptySensitivityResults() const
{
    return ( (!pOP || pOP->emptyHistory()) && sensitivity.empty() );
}


bool GoSUM::CContainer::emptyOptimizationResults() const
{
    return (!pOP || pOP->emptyHistory());
}


void GoSUM::CContainer::clearResults()
{
    inputs.clearSamples();
    outputs.clearSamples();
    analytical.clear();
    sensitivity.clear();
    reduced.clear();
    if (pOP) { pOP->clearHistory(); }
    selectedsamples.clear();
}


void GoSUM::CContainer::clearForReducing()
{
    if (pOP) { delete pOP; pOP=NULL; }
    sensitivity.clear();
    analytical.clear();
}


void GoSUM::CContainer::clearSamplingResults(CModelVariables *_pMVs)
{
    selectedsamples.clear();
    if (pOP) { pOP->clearHistory(); }
    reduced.clear();
    sensitivity.clear();
    analytical.clear();
    if ( _pMVs == &inputs )
    {
        outputs.clearSamples();
        inputs.clearSamples();
    }
    else if ( _pMVs == &outputs )
    {
        outputs.clearSamples();
    }
}


void GoSUM::CContainer::clearLearningResults()
{
    if (pOP) { pOP->clearHistory(); }
    reduced.clear();
    sensitivity.clear();
    analytical.clear();
}


void GoSUM::CContainer::clearSensitivityResults()
{
    if (pOP) { pOP->clearHistory(); }
    reduced.clear();
    sensitivity.clear();
}


void GoSUM::CContainer::clearOptimizationResults()
{
    if (pOP) { pOP->clearHistory(); }
}


void GoSUM::CContainer::setProjectType(GoSUM::CContainer::projecttype _prjType)
{
    prjType=_prjType;

    if (pOP) { delete pOP; pOP=NULL; }

    if ( prjType==samplegeneration )                  { ; }
    else if ( prjType==simpleanalysis )               { ; }
    else if ( prjType==modelanalysis )                { ; }
    else if ( prjType==dataanalysis )                 { ; }
    else if ( prjType==simpleoptimization )           { pOP=new GoSUM::CSimpleOptimizationProblem(&inputs);  }
    else if ( prjType==modeloptimization )            { pOP=new GoSUM::COriginalOptimizationProblem(&inputs,&outputs); }
    else if ( prjType==learnedmodeloptimization )     { pOP=new GoSUM::CAnalyticalOptimizationProblem(&inputs,&outputs,&analytical); }
    else if ( prjType==learneddataoptimization )      { pOP=new GoSUM::CAnalyticalOptimizationProblem(&inputs,&outputs,&analytical); }
    else  throw "GoSUM::CContainer::setProjectType error: unrecognised type";

}


std::string GoSUM::CContainer::longProjectName()
{
    ostringstream oss;

    oss<<prjName<<" (";
    if ( prjType==samplegeneration )                  { oss<<"Sample Generation Project"; }
    else if ( prjType==simpleanalysis )               { oss<<"Analysis Project"; }
    else if ( prjType==modelanalysis )                { oss<<"Model Analysis Project"; }
    else if ( prjType==dataanalysis )                 { oss<<"Data Analysis Project"; }
    else if ( prjType==simpleoptimization )           { oss<<"Optimization Project";  }
    else if ( prjType==modeloptimization )            { oss<<"Model Optimization Project"; }
    else if ( prjType==learnedmodeloptimization )     { oss<<"Model Anlysis and Optimization Project"; }
    else if ( prjType==learneddataoptimization )      { oss<<"Data Analyis and Optimization Project"; }
    else  throw "GoSUM::CContainer::longProjectName error: unrecognised type";
    oss<<") - GoSUM";
    return oss.str();
}


void GoSUM::CContainer::addDefaultVariable(GoSUM::CModelVariables *pMVs,CRandomVariable::distributiontype _dtype)
{
    CModelVariable *pMV;
    if ( CRandomVariable::IsDiscrete(_dtype) )
        pMV=new CDiscreteMV(pMVs->nextName(),_dtype);
    else if ( CRandomVariable::IsContinuous(_dtype) )
        pMV=new CContinuousMV(pMVs->nextName(),_dtype);
    else throw "GoSUM::CContainer::addVariable error: unrecognized model variable type";

    pMVs->add(pMV);
    if ( pOP && pMVs==&inputs )     pOP->addVariable(new GoSUM::CModelOptimizationVariable(pMV));
}


void GoSUM::CContainer::addVariable(GoSUM::CModelVariables *pMVs,CRandomVariable::distributiontype _type,double _a,double _b)
{
    CModelVariable *pMV;

    if ( CRandomVariable::IsDiscrete(_type) )
    {   pMV=new CDiscreteMV(pMVs->nextName(),_type);
        ((CDiscreteMV *)pMV)->setDistributionParameters(int(_a),int(_b));  }
    else if ( CRandomVariable::IsContinuous(_type) )
    {   pMV=new CContinuousMV(pMVs->nextName(),_type);
        ((CContinuousMV *)pMV)->setDistributionParameters(_a,_b);  }
    else throw "GoSUM::CContainer::addVariable error: unrecognized model variable type";

    pMVs->add(pMV);
    if ( pOP && pMVs==&inputs )     pOP->addVariable(new GoSUM::CModelOptimizationVariable(pMV));
}


void GoSUM::CContainer::addVariable(GoSUM::CModelVariables *pMVs,const std::string &_name,CRandomVariable::distributiontype _type,double _a,double _b)
{
    CModelVariable *pMV;

    if ( CRandomVariable::IsDiscrete(_type) )
    {   pMV=new CDiscreteMV(_name,_type);
        ((CDiscreteMV *)pMV)->setDistributionParameters(int(_a),int(_b));  }
    else if ( CRandomVariable::IsContinuous(_type) )
    {   pMV=new CContinuousMV(_name,_type);
        ((CContinuousMV *)pMV)->setDistributionParameters(_a,_b);  }
    else throw "GoSUM::CContainer::addVariable error: unrecognized model variable type";

    pMVs->add(pMV);
    if ( pOP && pMVs==&inputs )     pOP->addVariable(new GoSUM::CModelOptimizationVariable(pMV));
}


void GoSUM::CContainer::resetOptimizationVariable(CModelVariables *pMVs,int _at)
{
    if ( pOP && pMVs==&inputs )
    {
        int ovi=pOP->findConnectedTo(&(*pMVs)(_at));
        pOP->variable(ovi).reset();
    }
}


void GoSUM::CContainer::eraseVariable(CModelVariables *pMVs,std::string _name)
{
    int _at=pMVs->find(_name);
    if ( !(0<=_at && _at<pMVs->size()) )  throw "GoSUM::CContainer::eraseVariable error: no such variable";

    if ( pOP && pMVs==&inputs )
    {
        int ovi=pOP->findConnectedTo(&(*pMVs)(_at));
        if ( 0<=ovi && ovi<pOP->dimension() ) pOP->eraseVariable(ovi);
    }

    pMVs->erase(_at);
}


void GoSUM::CContainer::cloneVariable(CModelVariables *pMVs,int _at)
{
    if ( !(0<=_at && _at<pMVs->size()) )  throw "GoSUM::CContainer::cloneVariable error: index out of range";

    CModelVariable *clone=pMVs->add((*pMVs)(_at).clone(pMVs->nextName()));

    if ( pOP && pMVs==&inputs )
    {
        int ovi=pOP->findConnectedTo(&(*pMVs)(_at));
        if ( 0<=ovi && ovi<pOP->dimension() ) pOP->addVariable(new GoSUM::CModelOptimizationVariable(clone));
//        else                                  pOP->addVariable(new GoSUM::CModelOptimizationVariable(&(*pMVs)(_at)));
    }
}


void GoSUM::CContainer::addVariables(GoSUM::CModelVariables *pMVs,int _N,CRandomVariable::distributiontype _dtype,double _a,double _b)
{
    for ( int i=0; i<_N; i++ )        addVariable(pMVs,_dtype,_a,_b);
}


void GoSUM::CContainer::addVariables(GoSUM::CModelVariables *pMVs,const std::string &_name,int _N,CRandomVariable::distributiontype _dtype,double _a,double _b)
{
    std::string oldname=pMVs->baseName();
    pMVs->setBaseName(_name);
    for ( int i=0; i<_N; i++ )        addVariable(pMVs,_dtype,_a,_b);
    pMVs->setBaseName(oldname);

}


bool GoSUM::CContainer::containsTheoreticalViarables(const std::string &_fname)
{
    std::ifstream ifs(_fname.c_str());
    if ( ifs.fail() ) return false;
    std::string ln,_stype;

    try
    {
        while ( std::getline(ifs,ln) )
        {
            if ( ln.empty() )  continue;
            std::istringstream isln(ln);
            if ( !(isln>>_stype) || CRandomVariable::IsEmpirical(CRandomVariable::Type(_stype)) )    throw "format error";

            int n=CRandomVariable::DistributionParametersSize(CRandomVariable::Type(_stype));
            if ( n==0 )
            {
            }
            else if ( n==1 )
            {
                double _a;
                if ( !(isln>>_a) ) throw "format error";
            }
            else //if ( n==2 )
            {
                double _a,_b;
                if ( !(isln>>_a>>_b) ) throw "format error";
            }
        }
    }
    catch (const char *str)
    {
        ifs.close();
        ifs.clear();
        return false;
    }

    ifs.close();
    ifs.clear();
    return true;
}


bool GoSUM::CContainer::containsNamedTheoreticalViarables(const std::string &_fname)
{
    std::ifstream ifs(_fname.c_str());
    if ( ifs.fail() ) return false;
    std::string ln,_name,_stype;

    try
    {
        while ( std::getline(ifs,ln) )
        {
            if ( ln.empty() )  continue;
            std::istringstream isln(ln);
            if ( !(isln>>_name>>_stype) || CRandomVariable::IsEmpirical(CRandomVariable::Type(_stype)) )    throw "format error";
            if ( inputs.find(_name)<inputs.size() ) throw "name already used";

            int n=CRandomVariable::DistributionParametersSize(CRandomVariable::Type(_stype));
            if ( n==0 )
            {
            }
            else if ( n==1 )
            {
                double _a;
                if ( !(isln>>_a) ) throw "format error";
            }
            else //if ( n==2 )
            {
                double _a,_b;
                if ( !(isln>>_a>>_b) ) throw "format error";
            }
        }
    }
    catch (const char *str)
    {
        ifs.close();
        ifs.clear();
        return false;
    }

    ifs.close();
    ifs.clear();
    return true;
}


bool GoSUM::CContainer::containsEmpiricalViarables(const std::string &_fname,int &SSize,std::vector<CRandomVariable::distributiontype> &dtypes)
{
    std::ifstream ifs(_fname.c_str());
    if ( ifs.fail() ) return false;
    std::string ln,d;
    int lnNcols=0,Ncols;

    dtypes.clear();
    Ncols=0; SSize=0;

    try
    {
        while ( std::getline(ifs,ln) )
        {
            if ( ln.empty() )  continue;
            ++SSize;
            std::istringstream isln(ln);
            lnNcols=0;
            while ( isln >> d )
            {
                if ( CRandomVariable::IsDistributionType(d) ) throw "format error";

                lnNcols++;
                if ( SSize==1 )  dtypes.push_back(CRandomVariable::empiricalcontinuous);

                std::stringstream ds(d);
                double x;
                if ( !(ds>>x) )  dtypes[lnNcols-1]=CRandomVariable::categorical;
            }
            if ( SSize == 1 )  Ncols = lnNcols;
            else if ( lnNcols != Ncols ) throw "format error";
        }
    }
    catch (const char *str)
    {
        ifs.close();
        ifs.clear();
        return false;
    }

    ifs.close();
    ifs.clear();
    return true;

}


bool GoSUM::CContainer::containsNamedEmpiricalViarables(const std::string &_fname,int &SSize,std::vector<CRandomVariable::distributiontype> &dtypes)
{
    std::ifstream ifs(_fname.c_str());
    if ( ifs.fail() ) return false;
    std::string ln,d;
    int lnNcols=0,Ncols;

    dtypes.clear();
    Ncols=0; SSize=0;

    try
    {
        while ( std::getline(ifs,ln) && ln.empty() ) {;}
        if ( ln.empty() ) throw "format error";
        std::istringstream isln(ln);
        if ( !(isln>>d) ) throw "format error";
        if ( d!="#" ) throw "format error";
        lnNcols=0;
        while ( isln>>d )
        {
            if ( CRandomVariable::IsDistributionType(d) ) throw "format error";
            lnNcols++;
        }
        Ncols=lnNcols;
        dtypes.resize(Ncols,CRandomVariable::empiricalcontinuous);

        while ( std::getline(ifs,ln) )
        {
            if ( ln.empty() )  continue;
            ++SSize;
            std::istringstream isln(ln);
            lnNcols=0;
            while ( isln >> d )
            {
                if ( CRandomVariable::IsDistributionType(d) ) throw "format error";
                lnNcols++;
                std::stringstream ds(d);
                double x;
                if ( !(ds>>x) )  dtypes[lnNcols-1]=CRandomVariable::categorical;
            }
            if ( lnNcols != Ncols ) throw "format error";
        }
    }
    catch (const char *str)
    {
        ifs.close();
        ifs.clear();
        return false;
    }

    ifs.close();
    ifs.clear();
    return true;
}


bool GoSUM::CContainer::containsNamedDeclaredEmpiricalViarables(const std::string &_fname,int &SSize,std::vector<CRandomVariable::distributiontype> &dtypes)
{
    std::ifstream ifs(_fname.c_str());
    if ( ifs.fail() ) return false;
    std::string ln,d;
    int lnNcols=0,Ncols;

    dtypes.clear();
    Ncols=0; SSize=0;

    try
    {
        while ( std::getline(ifs,ln) && ln.empty() ) {;}
        if ( ln.empty() ) throw "format error";
        std::istringstream isln1(ln);
        if ( !(isln1>>d) ) throw "format error";
        if ( d!="#" ) throw "format error";
        lnNcols=0;
        while ( isln1>>d )
        {
            if ( CRandomVariable::IsDistributionType(d) ) throw "format error";
            lnNcols++;
        }
        Ncols=lnNcols;
        dtypes.resize(Ncols,CRandomVariable::empiricalcontinuous);

        while ( std::getline(ifs,ln) && ln.empty() ) {;}
        if ( ln.empty() ) throw "format error";
        std::istringstream isln2(ln);
        if ( !(isln2>>d) ) throw "format error";
        if ( d!="##" ) throw "format error";
        lnNcols=0;
        while ( isln2>>d && lnNcols<Ncols )
        {
            dtypes[lnNcols]=CRandomVariable::Type(d);
            lnNcols++;
        }
        if ( lnNcols != Ncols ) throw "format error";


        while ( std::getline(ifs,ln) )
        {
            if ( ln.empty() )  continue;
            ++SSize;
            std::istringstream isln(ln);
            lnNcols=0;
            while ( isln >> d )
            {
                if ( CRandomVariable::IsDistributionType(d) ) throw "format error";
                lnNcols++;
            }
            if ( lnNcols != Ncols ) throw "format error";
        }
    }
    catch (const char *str)
    {
        ifs.close();
        ifs.clear();
        return false;
    }

    ifs.close();
    ifs.clear();
    return true;
}


bool GoSUM::CContainer::containsDeclaredEmpiricalViarables(const std::string &_fname,int &SSize,std::vector<CRandomVariable::distributiontype> &dtypes)
{
    std::ifstream ifs(_fname.c_str());
    if ( ifs.fail() ) return false;
    std::string ln,d;
    int lnNcols=0,Ncols;

    dtypes.clear();
    Ncols=0; SSize=0;

    try
    {
        while ( std::getline(ifs,ln) && ln.empty() ) {;}
        if ( ln.empty() ) throw "format error";
        std::istringstream isln(ln);
        if ( !(isln>>d) ) throw "format error";
        if ( d!="##" ) throw "format error";
        lnNcols=0;
        while ( isln>>d )
        {
            dtypes.push_back(CRandomVariable::Type(d));
            lnNcols++;
        }
        Ncols=lnNcols;

        while ( std::getline(ifs,ln) )
        {
            if ( ln.empty() )  continue;
            ++SSize;
            std::istringstream isln(ln);
            lnNcols=0;
            while ( isln >> d )
            {
                if ( CRandomVariable::IsDistributionType(d) ) throw "format error";
                lnNcols++;
            }
            if ( lnNcols != Ncols ) throw "format error";
        }
    }
    catch (const char *str)
    {
        ifs.close();
        ifs.clear();
        return false;
    }

    ifs.close();
    ifs.clear();
    return true;
}


bool GoSUM::CContainer::containsPredictionSamples(const std::string &_fname,int &Ncols,int &Nrows)
{
    std::ifstream ifs(_fname.c_str());
    if ( ifs.fail() ) return false;
    std::string ln,d;
    int lnNcols=0;

    Ncols=0; Nrows=0;

    try
    {
        while ( std::getline(ifs,ln) )
        {
            if ( ln.empty() )  continue;
            ++Nrows;
            std::istringstream isln(ln);
            lnNcols=0;
            while ( isln >> d )
            {
                lnNcols++;
            }
            if ( Nrows == 1 )  Ncols = lnNcols;
            else if ( lnNcols != Ncols ) throw "format error";
        }
    }
    catch (const char *str)
    {
        ifs.close();
        ifs.clear();
        return false;
    }

    ifs.close();
    ifs.clear();
    return true;
}


void GoSUM::CContainer::importTheoreticalVariables(GoSUM::CModelVariables *pMVs,const std::string &_fname)
{
    if ( pMVs==(&outputs) ) return;

    ifstream ifs(_fname.c_str());

    std::string ln;
    while ( std::getline(ifs,ln) )
    {
        if ( ln.empty() )  continue;
        std::istringstream isln(ln);

        std::string _stype;
        isln>>_stype;
        int n=CRandomVariable::DistributionParametersSize(CRandomVariable::Type(_stype));
        if ( n==0 )
        {
            addVariable(pMVs,CRandomVariable::Type(_stype),0.,0.);
        }
        else if ( n==1 )
        {
            double _a;
            isln>>_a;
            addVariable(pMVs,CRandomVariable::Type(_stype),_a,0.);
        }
        else //if ( n==2 )
        {
            double _a,_b;
            isln>>_a>>_b;
            addVariable(pMVs,CRandomVariable::Type(_stype),_a,_b);
        }
    }
}


void GoSUM::CContainer::importNamedTheoreticalVariables(GoSUM::CModelVariables *pMVs,const std::string &_fname)
{
    if ( pMVs==(&outputs) ) return;

    ifstream ifs(_fname.c_str());

    std::string ln;


    while ( std::getline(ifs,ln) )
    {
        if ( ln.empty() )  continue;
        std::istringstream isln(ln);

        std::string _name,_stype;
        isln>>_name>>_stype;
        int n=CRandomVariable::DistributionParametersSize(CRandomVariable::Type(_stype));
        if ( n==0 )
        {
            addVariable(pMVs,_name,CRandomVariable::Type(_stype),0.,0.);
        }
        else if ( n==1 )
        {
            double _a;
            isln>>_a;
            addVariable(pMVs,_name,CRandomVariable::Type(_stype),_a,0.);
        }
        else //if ( n==2 )
        {
            double _a,_b;
            isln>>_a>>_b;
            addVariable(pMVs,_name,CRandomVariable::Type(_stype),_a,_b);
        }
    }
}


void GoSUM::CContainer::importEmpiricalVariables(GoSUM::CModelVariables *pMVs,const std::string &_fname,int &SSize,std::vector<CRandomVariable::distributiontype> &dtypes)
{
    int i,j;
    int VSize=dtypes.size();
    if ( VSize<=0 || SSize<=0 )  throw "GoSUM::CContainer::importVariables error: bad number of columns or rows";

    int oldsize=pMVs->size(),oldsamplesize=pMVs->sampleSize();
    for ( j=0; j<VSize; j++ )  addVariable(pMVs,dtypes[j]);
    int newsize=pMVs->size();

    for ( j=oldsize; j<newsize; j++ ) (*pMVs)(j).setSampleSize(SSize);

    std::ifstream ifs(_fname.c_str());
    if (ifs.fail())  throw "GoSUM::CContainer::importVariables error: std::ifstream constructor failed";
    for ( i=0; i<SSize; i++ )
        for ( j=oldsize; j<newsize; j++ )
            (*pMVs)(j).readSampleValue(ifs,i);
    ifs.close();

    for ( j=oldsize; j<newsize; j++ )
        if ( (*pMVs)(j).isContinuous() && (*pMVs)(j).cast2ContinuousMV()->hasDiscreteData() )
            pMVs->replace(j,new GoSUM::CDiscreteMV(*(*pMVs)(j).cast2ContinuousMV()));

    for ( j=oldsize; j<newsize; j++ ) (*pMVs)(j).computeStatistics(computestatSize);
    for ( j=oldsize; j<newsize; j++ ) (*pMVs)(j).setDistribution();

    if ( pOP )
    {   for ( j=oldsize; j<newsize; j++ )
        {   i=pOP->findConnectedTo(&(*pMVs)(j));
            if ( i<pOP->dimension() ) pOP->variable(i).reset();  }
    }

    if ( (oldsize>0 && oldsamplesize!=SSize) || prjType==simpleoptimization || prjType==modeloptimization )
        for ( j=oldsize; j<newsize; j++ ) (*pMVs)(j).clearSample();
}


void GoSUM::CContainer::importNamedEmpiricalVariables(GoSUM::CModelVariables *pMVs,const std::string &_fname,int &SSize,std::vector<CRandomVariable::distributiontype> &dtypes)
{
    int i,j,VSize=dtypes.size();
    if ( VSize<=0 || SSize<=0 )  throw "GoSUM::CContainer::importVariables error: bad number of columns or rows";

    int oldsize=pMVs->size(),oldsamplesize=pMVs->sampleSize();

    std::ifstream ifs(_fname.c_str());
    if (ifs.fail())  throw "GoSUM::CContainer::importVariables error: std::ifstream constructor failed";

    std::string ln,d,_name;

    while ( std::getline(ifs,ln) && ln.empty() ) {;}
    std::istringstream isln(ln);
    isln>>d;
    for ( j=0; j<VSize; j++ )
    {
        isln>>_name;
        addVariable(pMVs,_name,dtypes[j]);
    }
    int newsize=pMVs->size();

    for ( j=oldsize; j<newsize; j++ ) (*pMVs)(j).setSampleSize(SSize);

    for ( i=0; i<SSize; i++ )
        for ( j=oldsize; j<newsize; j++ )
            (*pMVs)(j).readSampleValue(ifs,i);
    ifs.close();

    for ( j=oldsize; j<newsize; j++ )
    {
        if ( (*pMVs)(j).isContinuous() && (*pMVs)(j).cast2ContinuousMV()->hasDiscreteData() )
            pMVs->replace(j,new GoSUM::CDiscreteMV(*(*pMVs)(j).cast2ContinuousMV()));
    }

    for ( j=oldsize; j<newsize; j++ ) (*pMVs)(j).computeStatistics(computestatSize);
    for ( j=oldsize; j<newsize; j++ ) (*pMVs)(j).setDistribution();

    if ( pOP )
    {   for ( j=oldsize; j<newsize; j++ )
        {   i=pOP->findConnectedTo (&(*pMVs)(j));
            if ( i<pOP->dimension() ) pOP->variable(i).reset();  }
    }

    if ( (oldsize>0 && oldsamplesize!=SSize) || prjType==simpleoptimization || prjType==modeloptimization )
        for ( j=oldsize; j<newsize; j++ ) (*pMVs)(j).clearSample();
}


void GoSUM::CContainer::importNamedDeclaredEmpiricalVariables(GoSUM::CModelVariables *pMVs,const std::string &_fname,int &SSize,std::vector<CRandomVariable::distributiontype> &dtypes)
{
    int i,j,VSize=dtypes.size();
    if ( VSize<=0 || SSize<=0 )  throw "GoSUM::CContainer::importVariables error: bad number of columns or rows";

    int oldsize=pMVs->size(),oldsamplesize=pMVs->sampleSize();

    std::ifstream ifs(_fname.c_str());
    if (ifs.fail())  throw "GoSUM::CContainer::importVariables error: std::ifstream constructor failed";

    std::string ln,d,_name;

    while ( std::getline(ifs,ln) && ln.empty() ) {;}
    std::istringstream isln(ln);
    isln>>d;
    for ( j=0; j<VSize; j++ )
    {
        isln>>_name;
        addVariable(pMVs,_name,dtypes[j]);
    }
    while ( std::getline(ifs,ln) && ln.empty() ) {;}

    int newsize=pMVs->size();
    for ( j=oldsize; j<newsize; j++ ) (*pMVs)(j).setSampleSize(SSize);

    for ( i=0; i<SSize; i++ )
        for ( j=oldsize; j<newsize; j++ )
            (*pMVs)(j).readSampleValue(ifs,i);
    ifs.close();

    for ( j=oldsize; j<newsize; j++ )
    {
        if ( (*pMVs)(j).isContinuous() && (*pMVs)(j).cast2ContinuousMV()->hasDiscreteData() )
            pMVs->replace(j,new GoSUM::CDiscreteMV(*(*pMVs)(j).cast2ContinuousMV()));
    }

    for ( j=oldsize; j<newsize; j++ ) (*pMVs)(j).computeStatistics(computestatSize);
    for ( j=oldsize; j<newsize; j++ )
    {
        (*pMVs)(j).setDistribution();
     }

    if ( pOP )
    {   for ( j=oldsize; j<newsize; j++ )
        {   i=pOP->findConnectedTo (&(*pMVs)(j));
            if ( i<pOP->dimension() ) pOP->variable(i).reset();  }
    }

    if ( (oldsize>0 && oldsamplesize!=SSize) || prjType==simpleoptimization || prjType==modeloptimization )
        for ( j=oldsize; j<newsize; j++ ) (*pMVs)(j).clearSample();
}


void GoSUM::CContainer::importDeclaredEmpiricalVariables(GoSUM::CModelVariables *pMVs,const std::string &_fname,int &SSize,std::vector<CRandomVariable::distributiontype> &dtypes)
{
    int i,j,VSize=dtypes.size();
    if ( VSize<=0 || SSize<=0 )  throw "GoSUM::CContainer::importVariables error: bad number of columns or rows";

    int oldsize=pMVs->size(),oldsamplesize=pMVs->sampleSize();

    std::ifstream ifs(_fname.c_str());
    if (ifs.fail())  throw "GoSUM::CContainer::importVariables error: std::ifstream constructor failed";

    std::string ln;
    while ( std::getline(ifs,ln) && ln.empty() ) {;}

    for ( j=0; j<VSize; j++ )  addVariable(pMVs,dtypes[j]);

    int newsize=pMVs->size();

    for ( j=oldsize; j<newsize; j++ ) (*pMVs)(j).setSampleSize(SSize);

    for ( i=0; i<SSize; i++ )
        for ( j=oldsize; j<newsize; j++ )
            (*pMVs)(j).readSampleValue(ifs,i);
    ifs.close();

    for ( j=oldsize; j<newsize; j++ )
    {
        if ( (*pMVs)(j).isContinuous() && (*pMVs)(j).cast2ContinuousMV()->hasDiscreteData() )
            pMVs->replace(j,new GoSUM::CDiscreteMV(*(*pMVs)(j).cast2ContinuousMV()));
    }

    for ( j=oldsize; j<newsize; j++ ) (*pMVs)(j).computeStatistics(computestatSize);
    for ( j=oldsize; j<newsize; j++ ) (*pMVs)(j).setDistribution();

    if ( pOP )
    {   for ( j=oldsize; j<newsize; j++ )
        {   i=pOP->findConnectedTo (&(*pMVs)(j));
            if ( i<pOP->dimension() ) pOP->variable(i).reset();  }
    }

    if ( (oldsize>0 && oldsamplesize!=SSize) || prjType==simpleoptimization || prjType==modeloptimization )
        for ( j=oldsize; j<newsize; j++ ) (*pMVs)(j).clearSample();
}


void GoSUM::CContainer::importVariables(GoSUM::CModelVariables *pMVs,const std::string &_fname)
{
    int SSize;
    std::vector<CRandomVariable::distributiontype> dtypes;

    if ( containsNamedTheoreticalViarables(_fname) )
    {
        importNamedTheoreticalVariables(pMVs,_fname);
    }
    else if ( containsTheoreticalViarables(_fname) )
    {
        importTheoreticalVariables(pMVs,_fname);
    }
    else if ( containsNamedEmpiricalViarables(_fname,SSize,dtypes) )
    {
        importNamedEmpiricalVariables(pMVs,_fname,SSize,dtypes);
    }
    else if ( containsEmpiricalViarables(_fname,SSize,dtypes) )
    {
        importEmpiricalVariables(pMVs,_fname,SSize,dtypes);
    }
    else if ( containsNamedDeclaredEmpiricalViarables(_fname,SSize,dtypes) )
    {
        importNamedDeclaredEmpiricalVariables(pMVs,_fname,SSize,dtypes);
    }
    else if ( containsDeclaredEmpiricalViarables(_fname,SSize,dtypes) )
    {
        importDeclaredEmpiricalVariables(pMVs,_fname,SSize,dtypes);
    }

}


void GoSUM::CContainer::exportSamples(CModelVariables *pMVs,const std::string &_fname)
{
    std::ofstream ofs(_fname.c_str());
    if (!ofs.is_open()) throw "GoSUM::CContainer::exportSamples error: unable to open export file";

    int i,j,Ncols=pMVs->size(),Nrows=pMVs->sampleSize();
    for ( i=0; i<Nrows; i++ )
    {   for ( j=0; j<Ncols; j++ )
        {   (*pMVs)(j).writeSampleValue(ofs,i); }
        ofs<<endl;
    }
    ofs.close();
}


void GoSUM::CContainer::importModelConstraints(const std::string &_fname)
{
    std::ifstream ifs(_fname.c_str());
    if ( ifs.fail() )  throw "GoSUM::CContainer::importModelConstraints error: unable to open import file";

    std::string ln;
    while ( std::getline(ifs,ln) )
    {
        if ( ln.empty() )  continue;
        inconsts.addExpression(ln);
    }
}


void GoSUM::CContainer::importLowerBound(const std::string &_fname)
{
    if (!pOP || pOP->dimension()==0)  throw "GoSUM::CContainer::importLowerBound error: no optimization problem";
    std::ifstream ifs(_fname.c_str());
    if ( ifs.fail() )  throw "GoSUM::CContainer::importLowerBound error: unable to open import file";

    ArrayXd xL(pOP->dimension());
    for ( int i=0; i<pOP->dimension(); i++ )
        if (!(ifs>>xL(i))) throw "GoSUM::CContainer::importLowerBound error: wrong format";

    pOP->setLowerBound(xL);
}


void GoSUM::CContainer::importUpperBound(const std::string &_fname)
{
    if (!pOP || pOP->dimension()==0)  throw "GoSUM::CContainer::importUpperBound error: no optimization problem";
    std::ifstream ifs(_fname.c_str());
    if ( ifs.fail() )  throw "GoSUM::CContainer::importUpperBound error: unable to open import file";

    ArrayXd xU(pOP->dimension());
    for ( int i=0; i<pOP->dimension(); i++ )
        if (!(ifs>>xU(i))) throw "GoSUM::CContainer::importUpperBound error: wrong format";

    pOP->setUpperBound(xU);
}


void GoSUM::CContainer::importInitialValue(const std::string &_fname)
{
    if (!pOP || pOP->dimension()==0)  throw "GoSUM::CContainer::importInitialValue error: no optimization problem";
    std::ifstream ifs(_fname.c_str());
    if ( ifs.fail() )  throw "GoSUM::CContainer::importInitialValue error: unable to open import file";

    ArrayXd x0(pOP->dimension());
    for ( int i=0; i<pOP->dimension(); i++ )
        if (!(ifs>>x0(i))) throw "GoSUM::CContainer::importInitialValue error: wrong format";

    pOP->setInitialValue(x0);
}


void GoSUM::CContainer::importOptimizationConstraints(const std::string &_fname)
{
    if (!pOP)  throw "GoSUM::CContainer::importOptimizationConstraints error: no optimization problem";
    std::ifstream ifs(_fname.c_str());
    if ( ifs.fail() )  throw "GoSUM::CContainer::importOptimizationConstraints error: unable to open import file";

    std::string ln;
    while ( std::getline(ifs,ln) )
    {
        if ( ln.empty() )  continue;
        pOP->addConstraintExpression(ln);
    }
}


void GoSUM::CContainer::resampleInputs()
{
    inconsts.parseExpressions();

    if ( CRandomGenerator::Type()==CRandomGenerator::dynamicsystem )  CRandomGenerator::SetSeed(inputs.size());
    inputs.generateSamples();
    inputs.computeStatistics(computestatSize);

    resamplingFinished();
}


void GoSUM::CContainer::setModelEvaluator(GoSUM::CEvaluator::evaluatortype _me,const std::string &_filename)
{
    CModelEvaluator::SetType(_me);
    CModelEvaluator::SetExternalEvaluator(_filename);
}


void GoSUM::CContainer::evaluateOutputs()
{
    outputs.evaluate(inputs);
    outputs.computeStatistics(computestatSize);
    outputs.setDistributions();

    evaluatingFinished();
}


void GoSUM::CContainer::learnModel()
{
    std::ostringstream oss;
    oss<<prjPath<<"\\"<<prjName<<"-log.txt";
    std::ofstream logfile(oss.str(),ios_base::out | ios_base::app);

    if ( CRandomGenerator::Type()==CRandomGenerator::dynamicsystem )  CRandomGenerator::SetSeed(inputs.size());
    analytical.learn(MADS,logfile);

    learningFinished();
}


void GoSUM::CContainer::predictMean(ArrayXd &ymu,ArrayXd &yvar)
{
    int N=outputs.size();
    int M=inputs.sampleSize();

    ArrayXd y(N);
    ymu=ArrayXd::Zero(N);
    yvar=ArrayXd::Zero(N);

    for ( int i=0; i<M; i++ )
    {
      y=analytical.predictValues(inputs.nTuple(i));
      ymu+=y;
      yvar+=y.square();
    }

    ymu/=M;
    yvar/=M;
    yvar-=ymu.square();
}


bool GoSUM::CContainer::importPredictionInputSamples(const std::string &_fname)
{
    std::vector<ArrayXd> _X;

    int i,s,Isize=0,Ssize=0;
    if ( !containsPredictionSamples(_fname,Isize,Ssize) ) return false;
    if ( Isize!=inputs.size() ) return false;
    _X.resize(Ssize);
    for ( s=0; s<Ssize; s++ ) _X[s].resize(Isize);

    std::ifstream ifs(_fname.c_str());
    if (ifs.fail())  return false;

    for ( s=0; s<Ssize; s++ )
    {   for ( i=0; i<Isize; i++ )  ifs>>_X[s](i);  }
    ifs.close();

    analytical.setInputSamples(_X);
    return true;
}


void GoSUM::CContainer::exportPredictionOutputSamples(const std::string &_fname)
{
    std::ofstream ofs(_fname.c_str());
    if (!ofs.is_open()) throw "GoSUM::CContainer::exportPredictedOutputSamples error: unable to open export file";

    const std::vector<ArrayXd> &_Y=analytical.outputSamples();
    int Ssize=int(_Y.size());
    if (Ssize>0)
    {
        int i,s,Osize=_Y[0].size();
        for ( s=0; s<Ssize; s++ )
        {   for ( i=0; i<Osize; i++ )  ofs<<_Y[s](i)<<"   ";
            ofs<<endl;    }
    }
    ofs.close();
}


void GoSUM::CContainer::setSensitivityOptions(int _N, double _eps1, double _eps2, double _eps3)
{
    sensitivity.setSensitivityOptions(_N,_eps1,_eps2,_eps3);
}


void GoSUM::CContainer::computeSensitivities()
{
    if ( prjType==simpleanalysis ) {
        outputs.openEvaluation(inputs);
        sensitivity.prepare<GoSUM::COutputStates>(outputs);
        sensitivity.computeVarianceSensitivities1<GoSUM::COutputStates>(outputs);
        outputs.closeEvaluation();
    }
    else {
        sensitivity.prepare<GoSUM::CAnalyticalModel>(analytical);
        sensitivity.computeDerivativeSensitivities();
        sensitivity.computeVarianceSensitivities1<GoSUM::CAnalyticalModel>(analytical);
    }

    reduced.initialize();

    sensitivityComputingFinished();
}


void GoSUM::CContainer::reduce()
{
    clearForReducing();
    reduced.eraseNonSelectedVariables();
}


void GoSUM::CContainer::setObjective(const std::string &_fexpr)
{
    if (!pOP)  throw "GoSUM::CContainer::setObjective error: not an optimization project";
    pOP->setObjectiveExpression(_fexpr);
}


void GoSUM::CContainer::addOptimizationConstraint(const std::string &_gexpr)
{
    if (!pOP)  throw "GoSUM::CContainer::addConstraint error: not an optimization project";
    pOP->addConstraintExpression(_gexpr);
}


void GoSUM::CContainer::setLowerBound(const ArrayXd &_xL)
{
    if (!pOP)  throw "GoSUM::CContainer::setLowerBound error: not an optimization project";
    pOP->setLowerBound(_xL);
}


void GoSUM::CContainer::setUpperBound(const ArrayXd &_xU)
{
    if (!pOP)  throw "GoSUM::CContainer::setUpperBound error: not an optimization project";
    pOP->setUpperBound(_xU);
}


void GoSUM::CContainer::setInitialValue(const ArrayXd &_x0)
{
    if (!pOP)  throw "GoSUM::CContainer::setInitialValue error: not an optimization project";
    pOP->setInitialValue(_x0);
}


void GoSUM::CContainer::minimize()
{
    if (!pOP)  throw "GoSUM::CContainer::minimize error: not an optimization project";
    pOP->setMinimization();

    optimize();
}


void GoSUM::CContainer::maximize()
{
    if (!pOP)  throw "GoSUM::CContainer::maximize error: not an optimization project";
    pOP->setMaximization();

    optimize();
}


void GoSUM::CContainer::optimize()
{
    inconsts.parseExpressions();

    std::ostringstream oss;
    oss<<prjPath<<"\\"<<prjName<<"-log.txt";
    std::ofstream logfile(oss.str(),ios_base::out | ios_base::app);

    if ( CRandomGenerator::Type()==CRandomGenerator::dynamicsystem )  CRandomGenerator::SetSeed(inputs.size());

    ArrayXd xbest;
    if ( omType==mads )   xbest=MADS.optimize(pOP,logfile);
    else                  xbest=GAMO.optimize(pOP,logfile);

    if (logfile.is_open()) logfile.close();

    optimizingFinished();
}


void GoSUM::CContainer::selectSamples(CModelVariable *pmv,double _left,double _right)
{
    if ( !pmv ) return;
    selectedsamples=pmv->sample()->select(_left,_right);
}


bool GoSUM::CContainer::isSampleSelected(int _i) const
{
    return ( std::find(selectedsamples.begin(),selectedsamples.end(),_i) != selectedsamples.end() );
}


void GoSUM::CContainer::separateBySelection(const ArrayXd &X,std::vector<double> &x,std::vector<double> &selx) const
{
    if ( X.size()==0 ) return;

    for ( int i=0,n=X.size(); i<n; i++ )
    {
        if ( !isSampleSelected(i) )  {  x.push_back(X(i)); }
        else                         {  selx.push_back(X(i)); }
    }
}



void GoSUM::CContainer::eraseSelectedSamples()
{
    inputs.eraseSelectedSamples(selectedsamples);
    inputs.computeStatistics(computestatSize);
    inputs.setDistributions();

    outputs.eraseSelectedSamples(selectedsamples);
    outputs.computeStatistics(computestatSize);
    outputs.setDistributions();

    selectedsamples.clear();
}


void GoSUM::CContainer::save()
{
    ostringstream ofss;
    ofss<<prjPath<<"\\"<<prjName<<".gpf";
    ofstream ofs(ofss.str().c_str(),ios_base::out | ios_base::binary);
    if (!ofs.is_open()) { return; /*throw "CContainer::save error: unable to open gpf";*/ }
    boost::archive::binary_oarchive oa(ofs);
    oa<<boost::serialization::make_nvp("GoSUMcontainer",*this);
}


void GoSUM::CContainer::load()
{
    ostringstream ofss;
    ofss<<prjPath<<"\\"<<prjName<<".gpf";
    ifstream ifs(ofss.str().c_str(),ios_base::in | ios_base::binary);
    if (!ifs.is_open()) { return; /*throw "CContainer::load error: unable to open gpf";*/ }
    boost::archive::binary_iarchive ia(ifs);
    ia>>boost::serialization::make_nvp("GoSUMcontainer",*this);
}


void GoSUM::CContainer::saveXml()
{
    ostringstream ofss;
    ofss<<prjPath<<"\\"<<prjName<<".xml";
    ofstream ofs(ofss.str().c_str());
    if (!ofs.is_open()) throw "CContainer::saveXml error: unable to open xml";
    boost::archive::xml_oarchive oa(ofs);
    oa<<boost::serialization::make_nvp("GoSUMcontainer",*this);
}


void GoSUM::CContainer::loadXml()
{
    ostringstream ofss;
    ofss<<prjPath<<"\\"<<prjName<<".xml";
    ifstream ifs(ofss.str().c_str());
    if (!ifs.is_open()) throw "CContainer::loadXml error: unable to open xml";
    boost::archive::xml_iarchive ia(ifs);
    ia>>boost::serialization::make_nvp("GoSUMcontainer",*this);
}


void GoSUM::CContainer::saveTxt()
{
    ostringstream ofss;
    ofss<<prjPath<<"\\"<<prjName<<".txt";
    ofstream ofs(ofss.str().c_str());
    if (!ofs.is_open()) throw "CContainer::saveTxt error: unable to open txt";
    boost::archive::text_oarchive oa(ofs);
    oa<<boost::serialization::make_nvp("GoSUMcontainer",*this);
}


void GoSUM::CContainer::loadTxt()
{
    ostringstream ofss;
    ofss<<prjPath<<"\\"<<prjName<<".txt";
    ifstream ifs(ofss.str().c_str());
    if (!ifs.is_open()) throw "CContainer::loadTxt error: unable to open txt";
    boost::archive::text_iarchive ia(ifs);
    ia>>boost::serialization::make_nvp("GoSUMcontainer",*this);
}


void GoSUM::CContainer::exportDerivativeSensitivity(const std::string &_fname)
{
    ofstream ofs(_fname.c_str());
    if (!ofs.is_open()) throw "GoSUM::CContainer::exportDerivativeSensitivity error: unable to open export file";
    ofs<<sensitivity.derivativeSensitivity();
}


void GoSUM::CContainer::exportAverageDerivative(const std::string &_fname)
{
    ofstream ofs(_fname.c_str());
    if (!ofs.is_open()) throw "GoSUM::CContainer::exportAverageDerivative error: unable to open export file";
    ofs<<sensitivity.averageDerivative();
}


void GoSUM::CContainer::exportAbsoluteAverageDerivative(const std::string &_fname)
{
    ofstream ofs(_fname.c_str());
    if (!ofs.is_open()) throw "GoSUM::CContainer::exportAbsoluteAverageDerivative error: unable to open export file";
    ofs<<sensitivity.absoluteAverageDerivative()<<endl;
}


void GoSUM::CContainer::exportVarianceSensitivity(const std::string &_fname)
{
    ofstream ofs(_fname.c_str());
    if (!ofs.is_open()) throw "GoSUM::CContainer::exportVarianceSensitivity error: unable to open export file";
    ofs<<sensitivity.varianceSensitivity();
}


void GoSUM::CContainer::exportFirstOrderANOVA(const std::string &_fname)
{
    ofstream ofs(_fname.c_str());
    if (!ofs.is_open()) throw "GoSUM::CContainer::exportFirstOrderANOVA error: unable to open export file";
    ofs<<sensitivity.firstOrderANOVA();
}


void GoSUM::CContainer::exportOptimizationMethod(const std::string &_fname)
{
    ofstream ofs(_fname.c_str());
    if (!ofs.is_open()) throw "GoSUM::CContainer::exportOptimizationMethod error: unable to open export file";
    boost::archive::xml_oarchive oa(ofs);

    if ( omType==mads )   oa<<boost::serialization::make_nvp("MADS",MADS);
    else                  oa<<boost::serialization::make_nvp("GAMO",GAMO);
}


void GoSUM::CContainer::exportOptimizationHistory(const std::string &_fname)
{
    if ( !pOP || pOP->emptyHistory() ) throw "GoSUM::CContainer::exportOptimizationHistory error: no optimization history";

    ofstream ofs(_fname.c_str());
    if (!ofs.is_open()) throw "GoSUM::CContainer::exportOptimizationHistory error: unable to open export file";

    ofs<<"objective = "<<pOP->objectiveExpression();
    if ( pOP->isMinimization() ) ofs<<" to minimize"<<endl;
    else                         ofs<<" to maximize"<<endl;

    int i,n=pOP->constraintsSize(),dim=pOP->dimension();
    ofs<<"constraints: "<<endl;
    for ( i=0; i<n; i++ )
        ofs<<pOP->constraintExpression(i)<<endl;

    ofs<<endl<<"optimization history:"<<endl;
    ofs<<"objective";
    for ( i=0; i<dim; i++ )  ofs<<"\t"<<pOP->variableName(i);
    ofs<<endl;

    int row,rowcount=pOP->historySize();
    for ( row=0; row<rowcount; row++ )
    {
        ofs<<pOP->objectiveHistory(row);
        for ( i=0; i<dim; i++ )    ofs<<"\t"<<pOP->variableHistory(row,i);
        ofs<<endl;
    }
}


bool GoSUM::CContainer::readyToSampleInputs()
{
    if ( inputs.empty() )  return false;

    int i,Isize=inputs.size();
    for ( i=0; i<Isize; i++ )
    {
        if ( !inputs(i).isDistributionDefined() ) return false;
    }
    return inconsts.validateExpressions();
}


bool GoSUM::CContainer::readyToEvaluateOutputs()
{
    return (!inputs.emptySamples() && !outputs.empty() && !evaluator.ExternalEvaluator().empty() && inconsts.validateExpressions() );
}


bool GoSUM::CContainer::readyToLearnModel()
{

   return (!inputs.emptySamples() && inputs.sampleSize()==outputs.sampleSize());
}


bool GoSUM::CContainer::readyToComputeSensitivities()
{
  if ( prjType==simpleanalysis )
        return (!inputs.empty() && !outputs.empty() && !evaluator.ExternalEvaluator().empty() && inconsts.validateExpressions());
  else
        return (!analytical.empty()) ;
}


bool GoSUM::CContainer::readyToReduceModel()
{
   return !sensitivity.empty() && reduced.admissible();
}


bool GoSUM::CContainer::readyToSetOptimization()
{
    return !inputs.empty();
}


bool GoSUM::CContainer::readyToOptimize()
{
    if ( prjType==simpleoptimization )
        return (!inputs.empty() && pOP->validateExpressions());
    else if ( prjType==modeloptimization )
        return (!inputs.empty() && !outputs.empty() && !evaluator.ExternalEvaluator().empty() && pOP->validateExpressions());
    else if ( prjType==learnedmodeloptimization || prjType==learneddataoptimization )
        return (!analytical.empty() && pOP->validateExpressions());
    else
        return false;
}


void GoSUM::CContainer::startResampling()
{
    thrd=boost::thread(&CContainer::resampleInputs,this);
}


void GoSUM::CContainer::startEvaluating()
{
    thrd=boost::thread(&CContainer::evaluateOutputs,this);
}


void GoSUM::CContainer::startLearning()
{
    thrd=boost::thread(&CContainer::learnModel,this);
}


void GoSUM::CContainer::startSensitivityComputing()
{
    thrd=boost::thread(&CContainer::computeSensitivities,this);
}


void GoSUM::CContainer::startOptimizing()
{
    thrd=boost::thread(&CContainer::optimize,this);
}


void GoSUM::CContainer::join()
{
    if (thrd.joinable())  thrd.join();
}


