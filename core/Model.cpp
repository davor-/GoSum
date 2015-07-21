





#include "Model.h"
#include "Hypercube.h"
#include "OriginalModel.h"


std::string GoSUM::CModelVariables::nameToFind="";


BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CModelVariables)
BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CInputParameters)
BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::COutputStates)


template <class Archive>
void GoSUM::CModelVariables::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("nameToFind",nameToFind);
    ar & boost::serialization::make_nvp("basename",basename);
    ar & boost::serialization::make_nvp("mvs",mvs);
    ar & boost::serialization::make_nvp("trn",trn);
    ar & boost::serialization::make_nvp("scl",scl);
}

template <class Archive>
void GoSUM::CInputParameters::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CModelVariables",boost::serialization::base_object<CModelVariables>(*this));
    ar & boost::serialization::make_nvp("pIC",pIC);
    ar & boost::serialization::make_nvp("RSsize",RSsize);
}


template <class Archive>
void GoSUM::COutputStates::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CModelVariables",boost::serialization::base_object<CModelVariables>(*this));
    ar & boost::serialization::make_nvp("pE",pE);
}


std::string GoSUM::CModelVariables::nextName() const
{
    std::string newname;
    int i=1;
    do
    {
        ostringstream oss;
        oss<<baseName()<<mvs.size()+i;
        newname=oss.str();
        i++;
    }
    while ( find(newname)<mvs.size() && i<100000 );

    if (i==100000) newname.clear();

    return newname;
}


std::vector<ArrayXd> GoSUM::CModelVariables::samples()
{
    if ( mvs.size()<=0 )  throw "GoSUM::CModelVariables::samples error: no model variables";
    if ( mvs[0].sampleSize()<=0 )  throw "GoSUM::CModelVariables::samples error: no samples";

    int Ssize=mvs[0].sampleSize(),Vsize=mvs.size(),i,j;
    std::vector<ArrayXd> S(Ssize,ArrayXd(Vsize));

    for ( j=0; j<Ssize; j++ )
        for ( i=0; i<Vsize; i++ )
            S[j](i)=mvs[i].sampleValue(j);

    return S;
}


void GoSUM::CModelVariables::setSamples(const std::vector<ArrayXd> &_samples)
{
    int i,j,Nrows=int(_samples.size()),Ncols=int(_samples[0].size());

    if ( Nrows<=0 || Ncols!=mvs.size() )  throw "GoSUM::CModelVariables::setSamples error: bad _samples size";
    for ( j=0; j<Ncols; j++ ) mvs[j].setSampleSize(Nrows);
    for ( i=0; i<Nrows; i++ )
        for ( j=0; j<Ncols; j++ )
            mvs[j].setSampleValue(_samples[i](j),i);
}


void GoSUM::CModelVariables::setDistributions(const std::vector<int> &_selected)
{
    int j,M=int(_selected.size());
    for ( j=0; j<M; j++ ) mvs[_selected[j]].setDistribution();
}


void GoSUM::CModelVariables::setDistributions()
{
    int j,M=int(mvs.size());
    for ( j=0; j<M; j++ ) mvs[j].setDistribution();
}


int GoSUM::CModelVariables::expandedSize() const
{
    int i,N=int(mvs.size()),eN=0;
    for ( i=0; i<N; i++ ) eN+=mvs[i].expandedSize();
    return eN;
}


std::vector<std::string>  GoSUM::CModelVariables::expandedNames() const
{
    std::vector<std::string> expnames;
    expnames.resize(0);

    int i,j,N=int(mvs.size()),eN=0;
    for ( i=0; i<N; i++ )
    {
        eN=mvs[i].expandedSize();
        if ( eN>1 )
        {
            for ( j=1; j<=eN; j++ )
            {
                std::ostringstream oss;
                oss<<mvs[i].name()<<"-"<<j;
                expnames.push_back(oss.str());
            }
        }
        else    expnames.push_back(mvs[i].name());
    }
    return expnames;
}


void GoSUM::CModelVariables::setNTuple(const ArrayXd &X,int _at)
{
    if ( X.size()!=mvs.size() ) throw "GoSUM::CModelVariables::setNTuple error: bad nTupe size";

    int i,N=int(mvs.size());
    for ( i=0; i<N; i++ ) mvs[i].setSampleValue(X(i),_at);
}


ArrayXd GoSUM::CModelVariables::nTuple(int _at) const
{
    int i,N=int(mvs.size());
    ArrayXd X(N);
    for ( i=0; i<N; i++ ) X(i)=mvs[i].sampleValue(_at);
    return X;
}


ArrayXd GoSUM::CModelVariables::expectedValue() const
{
    int i,N=int(mvs.size());
    ArrayXd X(N);
    for ( i=0; i<N; i++ ) X(i)=mvs[i].expectedValue();
    return X;
}


ArrayXd GoSUM::CModelVariables::expandedNTuple(int _at) const
{
    int i,j,k,N=int(mvs.size()),eN=expandedSize(),exsize;
    ArrayXd eX=ArrayXd::Zero(eN);
    for ( i=j=0; i<N; i++,j+=exsize )
    {   exsize=mvs[i].expandedSize();
        if ( exsize==1 )  {  eX(j)=mvs[i].sampleValue(_at);  }
        else              {  for ( k=0; k<exsize; k++ )  if (k==mvs[i].sampleValue(_at)) { eX(j+k) = 1.; break; }  }  }
    return eX;
}


ArrayXd GoSUM::CModelVariables::expandNTuple(const ArrayXd &X) const
{
    if ( X.size()!=mvs.size() )  throw "GoSUM::CModelVariables::expand error: wrong X size";
    int i,j,k,N=int(mvs.size()),eN=expandedSize(),exsize;
    ArrayXd eX=ArrayXd::Zero(eN);
    for ( i=j=0; i<N; i++,j+=exsize )
    {   exsize=mvs[i].expandedSize();
        if ( exsize==1 )  {  eX(j)=X(i);  }
        else              {  for ( k=0; k<exsize; k++ )  if (k==X(i)) { eX(j+k) = 1.; break; } } }
    return eX;
}


void GoSUM::CModelVariables::setNormalization()
{
    int i,j,N=size(),eN=expandedSize(),exsize;
    trn.setZero(eN);
    scl.setOnes(eN);

    for ( i=0,j=0; i<N; i++ ) {
        const CModelVariable &aMV=mvs[i];
        exsize=aMV.expandedSize();
        if ( exsize==1 ) {
//           trn(j)=aMV.minValue();
//           scl(j)=aMV.maxValue()-aMV.minValue();
           trn(j)=aMV.expectedValue();
           scl(j)=sqrt(aMV.variance());
           if ( fabs(scl(j))<TINY ) { trn(j)+=1.;   scl(j)=1.; }
        }
        j+=exsize;
    }
}


ArrayXd GoSUM::CModelVariables::hcPoint2ModelPoint(const ArrayXd &x)
{
    if ( x.size()!=mvs.size() )   throw "GoSUM::CModelVariables::hcPoint2ModelPoint error: wrong dimension";
    int j,dim=int(x.size());
    ArrayXd X(dim);

    for ( j=0; j<dim; j++ )
    {   X(j)=mvs[j].generateSampleValue(x(j));  }

    return X;
}


void GoSUM::CInputParameters::generateSamples(std::vector<ArrayXd> &_samples)
{
    GoSUM::CModelHypercube *pHC=CHypercube::New(this);
    if (!pHC)   throw "GoSUM::CInputParameters::generateSamples error: no pHC";
//    if (progressSlot)   {   pHC->generatingProgressed.connect(progressSlot);   }


    int dim=int(mvs.size());
    _samples.clear();
    pHC->generate(RSsize,dim,_samples);

    if (pHC)  delete pHC;
}


void GoSUM::CInputParameters::generateSamples()
{
    GoSUM::CModelHypercube *pHC=CHypercube::New(this);
    if (!pHC)   throw "GoSUM::CInputParameters::generateSamples error: no pHC";
    if (progressSlot)   {   pHC->generatingProgressed.connect(progressSlot);   }

    int dim=int(mvs.size());
    std::vector<ArrayXd> _samples;
    pHC->generate(RSsize,dim,_samples);

    if ( !_samples.empty() )
    {
        for ( int j=0; j<dim; j++ )    {   mvs[j].setSampleSize(RSsize);  }
        for ( int i=0; i<RSsize; i++ )    {   setNTuple(_samples[i],i);  }
    }

    if (pHC)  delete pHC;
}


int GoSUM::CInputParameters::resampleStepsSize() const
{
    return CHypercube::ProgressSize(RSsize,mvs.size());
}


bool GoSUM::COutputStates::setEvaluatorCompatible(const CInputParameters &inputs)
{
    bool done=false;
    if (pE) throw "GoSUM::COutputStates::openEvaluation error: evaluator running";
    pE=GoSUM::CModelEvaluator::New(&inputs,this);

    pE->openEvaluation();
    int k,K=pE->evaluateOutputsSize();

    if ( K>0 && K!=mvs.size() )
    {
        clear();
        for ( k=0; k<K; k++ )
            add(new CContinuousMV(nextName(),CRandomVariable::empiricalcontinuous));
        done=true;
    }
    else if ( K>0 && K==mvs.size() )
    {
        done=true;
    }

    pE->closeEvaluation();
    if (pE) { delete pE; pE=NULL; }

    return done;
}


void GoSUM::COutputStates::evaluate(const CInputParameters &inputs)
{
    int N=inputs.sampleSize();
    int k,K=int(mvs.size());
    for ( k=0; k<K; k++ )  mvs[k].setSampleSize(N);

    int trdN,trdI;
//#ifdef DEBUG
//    trdN = 1;
//#else
    trdN = GoSUM::CEvaluator::ThreadSize();
//#endif

    boost::ptr_vector<GoSUM::CModelEvaluator> evaluators;
    boost::ptr_vector<boost::thread> threads;

    for( trdI=0; trdI<trdN; trdI++ )
    {
        evaluators.push_back(GoSUM::CModelEvaluator::New(&inputs,this,trdI));
        threads.push_back(new boost::thread(boost::ref(evaluators[trdI])));
        if (progressSlot) evaluators[trdI].evaluatingProgressed.connect(progressSlot);
    }

    for( trdI=0; trdI<trdN; trdI++ ) threads[trdI].join();

}


void GoSUM::COutputStates::openEvaluation(const CInputParameters &inputs)
{
    if (pE) throw "GoSUM::COutputStates::openEvaluation error: evaluator running";
    pE=GoSUM::CModelEvaluator::New(&inputs,this);
    pE->openEvaluation();
}


ArrayXd GoSUM::COutputStates::evaluate(const ArrayXd &X)
{
    if (!pE) throw "GoSUM::COutputStates::evaluate error: no evaluator running";
    return (*pE)(X);
}


void GoSUM::COutputStates::closeEvaluation()
{
    if (!pE) throw "GoSUM::COutputStates::closeEvaluation error: no evaluator running";
    pE->closeEvaluation();
    if (pE) { delete pE; pE=NULL; }
}
