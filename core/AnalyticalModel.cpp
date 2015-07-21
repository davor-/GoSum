


#include "AnalyticalModel.h"


GoSUM::CInputParameters *GoSUM::CSingleAM::pIP=NULL;
GoSUM::COutputStates *GoSUM::CSingleAM::pOS=NULL;
struct svm_problem GoSUM::CSingleAM::prob={0,NULL,NULL};
std::vector<struct svm_node> GoSUM::CSingleAM::xspace;
ArrayXi GoSUM::CSingleAM::perm;
int GoSUM::CSingleAM::maxlearnsize=1000;
double GoSUM::CSingleAM::learnfrac=0.05;


BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CSingleAM)
BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CCSvcSAM)
BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CNuSvcSAM)
BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::COneClassSAM)
BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CEpsSvrSAM)
BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CNuSvrSAM)
BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CAnalyticalModel)


template <class Archive>
void GoSUM::CSingleAM::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("COptimizationProblem",boost::serialization::base_object<COptimizationProblem>(*this));
    ar & boost::serialization::make_nvp("maxlearnsize",maxlearnsize);
    ar & boost::serialization::make_nvp("learnfrac",learnfrac);
    ar & boost::serialization::make_nvp("pIP",pIP);
    ar & boost::serialization::make_nvp("pOS",pOS);
    ar & boost::serialization::make_nvp("osi",osi);
    ar & boost::serialization::make_nvp("gamma",gamma);
    ar & boost::serialization::make_nvp("C",C);
    ar & boost::serialization::make_nvp("p",p);
    ar & boost::serialization::make_nvp("nu",nu);
    ar & boost::serialization::make_nvp("coef0",coef0);
    ar & boost::serialization::make_nvp("degree",degree);
    ar & boost::serialization::make_nvp("Nsvm",Nsvm);
    ar & boost::serialization::make_nvp("coeff",coeff);
    ar & boost::serialization::make_nvp("Xsv",Xsv);
}


template <class Archive>
void GoSUM::CCSvcSAM::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CSingleAM",boost::serialization::base_object<CSingleAM>(*this));
}


template <class Archive>
void GoSUM::CNuSvcSAM::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CSingleAM",boost::serialization::base_object<CSingleAM>(*this));
}


template <class Archive>
void GoSUM::COneClassSAM::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CSingleAM",boost::serialization::base_object<CSingleAM>(*this));
}


template <class Archive>
void GoSUM::CEpsSvrSAM::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CSingleAM",boost::serialization::base_object<CSingleAM>(*this));
}


template <class Archive>
void GoSUM::CNuSvrSAM::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CSingleAM",boost::serialization::base_object<CSingleAM>(*this));
}


template <class Archive>
void GoSUM::CAnalyticalModel::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("etype",etype);
    ar & boost::serialization::make_nvp("pIP",pIP);
    ar & boost::serialization::make_nvp("pOS",pOS);
    ar & boost::serialization::make_nvp("sam",sam);
    ar & boost::serialization::make_nvp("X",X);
    ar & boost::serialization::make_nvp("Y",Y);
}


void GoSUM::CSingleAM::OpenSVM(CInputParameters *_pIP,COutputStates *_pOS)
{
    pIP=_pIP; pOS=_pOS;
    int i,j,k,Ssize=pIP->sampleSize(),EIsize=pIP->expandedSize(),XSsize;

    perm=permutation(Ssize);

    prob.l=std::min(int(double(Ssize)*2./3.+0.5),std::max(int(learnfrac*double(Ssize)+0.5),maxlearnsize)); //Ssize

    for ( j=0; j<prob.l; j++ ) //Ssize; j++ )
    {   ArrayXd eX=pIP->normalize(pIP->expandedNTuple(perm(j))); //expandedNTuple(j));
        for ( i=0; i<EIsize; i++ )   if ( fabs(eX(i))>TINY ) xspace.push_back(svm_node(i+1,eX(i)));
        xspace.push_back(svm_node(-1,0.));
    }

    prob.y=new double [prob.l];
    prob.x=new struct svm_node * [prob.l];
    i=0; prob.x[i++]=&(xspace[0]);
    for ( k=0, XSsize=int(xspace.size()); k<XSsize; k++ )
    {   if ( xspace[k].index==-1 && k+1<XSsize )  prob.x[i++]=&(xspace[k+1]);  }

}


void GoSUM::CSingleAM::CloseSVM()
{
    if (prob.y) delete prob.y;
    prob.y=NULL;
    if (prob.x) delete prob.x;
    prob.y=NULL;
    xspace.clear();

    prob.l=0;
    perm.resize(0);
}


GoSUM::CSingleAM *GoSUM::CSingleAM::New(const CAnalyticalModel *_pAM)
{
    if ( _pAM->svmType()==CAnalyticalModel::epsilonsvr )    return new CEpsSvrSAM;
    else if ( _pAM->svmType()==CAnalyticalModel::nusvr )    return new CNuSvrSAM;
    else throw "GoSUM::CSingleAM::New error: unrecognized svm type";
}


void GoSUM::CSingleAM::learn(int _osi,CMADS &_mads,std::ostream &_out)
{
    osi=_osi;

    optimizationPoint2SVMParam(_mads.optimize(this,_out));

    model = svm_train(&prob,&param);

    int j,k,EIsize=pIP->expandedSize();
    gamma=param.gamma;
    C=param.C;
    p=param.p;
    nu=param.nu;
    coef0=model->param.coef0;
    degree=model->param.degree;
    Nsvm=model->l;

    coeff.resize(Nsvm+1);
    double *sv_coef = model->sv_coef[0];
    for ( j=0; j<Nsvm; j++ ) coeff(j)=sv_coef[j];
    coeff(Nsvm)=-model->rho[0];

    Xsv.resize(Nsvm);
    for ( j=0; j<Nsvm; j++ )
    {  Xsv[j].setZero(EIsize);
       for ( k=0; k<EIsize; k++ ) Xsv[j](k)=model->SV[j][k].value;
    }

    svm_free_and_destroy_model(&model);
}


void GoSUM::CSingleAM::openOptimization()
{
    int j;//,Ssize=pIP->sampleSize();

    for ( j=0; j<prob.l/*Ssize*/; j++ ) prob.y[j]=pOS->normalize(((*pOS)(osi)).sampleValue(perm(j)),osi); //sampleValue(j),osi);
}


void GoSUM::CEpsSvrSAM::openOptimization()
{
    CSingleAM::openOptimization();

    param.svm_type=EPSILON_SVR;
    param.kernel_type=RBF;
    param.degree=3;
    param.gamma=1.;
    param.coef0=0.;
    param.cache_size=100;
    param.eps=0.001;
    param.C=10.;
    param.nr_weight=0;
    param.weight_label=NULL;
    param.weight=NULL;
    param.nu=0.5;
    param.p=0.4;
    param.shrinking=1;
    param.probability=0;


    ovs.resize(3);
    ovs[0].set(log(param.gamma/1000.),log(param.gamma*10.),log(param.gamma));
    ovs[1].set(log(param.C/1000.),log(param.C*10.),log(param.C));
    ovs[2].set(log(param.p/100.),log(param.p*100.),log(param.p));
}


void GoSUM::CNuSvrSAM::openOptimization()
{
    CSingleAM::openOptimization();

    param.svm_type=NU_SVR;
    param.kernel_type=RBF;
    param.degree=3;
    param.gamma=1.;
    param.coef0=0.;
    param.cache_size=100;
    param.eps=0.01;
    param.C=10.;
    param.nr_weight=0;
    param.weight_label=NULL;
    param.weight=NULL;
    param.nu=0.4;
    param.p=0.4;
    param.shrinking=1;
    param.probability=0;

    ovs.resize(3);
    ovs[0].set(log(param.gamma/1000.),log(param.gamma*10.),log(param.gamma));
    ovs[1].set(log(param.C/1000.),log(param.C*10.),log(param.C));
    ovs[2].set(log(param.nu/100.),log(param.nu*100.),log(param.nu));
}


double GoSUM::CSingleAM::objective(const ArrayXd &ov)
{
    double *target = new double [prob.l];
    int nr_fold=4;
    double mserror=100.,y,v;

    optimizationPoint2SVMParam(ov);
    svm_cross_validation(&prob,&param,nr_fold,target);

    double total_error=0.;
    for( int k=0; k<prob.l; k++ )
    {
       y = prob.y[k];
       v = target[k];
       total_error += (v-y)*(v-y);
    }
    mserror=std::sqrt(total_error/prob.l);

    delete target;

    learningProgressed();
    return mserror;
}


void GoSUM::CEpsSvrSAM::optimizationPoint2SVMParam(const ArrayXd &ov)
{
    param.gamma=exp(ov(0));
    param.C=exp(ov(1));
    param.p=exp(ov(2));
}


void GoSUM::CNuSvrSAM::optimizationPoint2SVMParam(const ArrayXd &ov)
{
    param.gamma=exp(ov(0));
    param.C=exp(ov(1));
    param.nu=exp(ov(2));
}


double GoSUM::CSingleAM::f(const ArrayXd& x) const
{
    double y=coeff(Nsvm);
    for ( int j=0; j<Nsvm; j++ ) y += coeff(j)*rbfKernel(x,j);
    return y;
}


ArrayXd GoSUM::CSingleAM::df(const ArrayXd& x) const
{
    int j,k,xsize=int(x.size());
    ArrayXd dy=ArrayXd::Zero(xsize);
    for ( k=0; k<xsize; k++ )
       for ( j=0; j<Nsvm; j++ )  dy(k)+= (x(k)-Xsv[j](k))*coeff(j)*rbfKernel(x,j);
	dy*=(-2.*gamma); 
    return dy;
}


void GoSUM::CAnalyticalModel::learn(CMADS &_mads,std::ostream &_out)
{
    pIP->setNormalization();
    pOS->setNormalization();

    CSingleAM::OpenSVM(pIP,pOS);

    int i,Osize=pOS->size();
    for ( i=0; i<Osize; i++ )
    {
        sam.push_back(CSingleAM::New(this));
        if (progressSlot)   {   sam.back().learningProgressed.connect(progressSlot);   }
        sam.back().learn(i,_mads,_out);
    }

    X=pIP->samples();
    predict();

    CSingleAM::CloseSVM();
}


void GoSUM::CAnalyticalModel::predict()
{
    int Ssize=int(X.size()),Osize=int(sam.size()),s,o;

    Y.resize(Ssize);

    for ( s=0; s<Ssize; s++ )
    {
        Y[s].resize(Osize);
        ArrayXd x=pIP->normalize(pIP->expandNTuple(X[s]));
        for ( o=0; o<Osize; o++ )  Y[s](o)=pOS->denormalize(sam[o].f(x),o);
    }
}


double GoSUM::CAnalyticalModel::predictValue(const ArrayXd& _X, int i) const
{
    return pOS->denormalize(sam[i].f(pIP->normalize(pIP->expandNTuple(_X))),i);
//        ArrayXd _Y=fEvaluate(_X);
//        return _Y(i);
}


ArrayXd GoSUM::CAnalyticalModel::predictValues(const ArrayXd& _X) const
{
    ArrayXd x=pIP->normalize(pIP->expandNTuple(_X));
    int i,Osize=int(sam.size());
    ArrayXd _Y=ArrayXd::Zero(Osize);
    for ( i=0; i<Osize; i++ )  _Y(i)=pOS->denormalize(sam[i].f(x),i);
    return _Y;
//      ArrayXd _Y=fEvaluate(_X);
//      return _Y;
}


ArrayXd GoSUM::CAnalyticalModel::evaluate(const ArrayXd& _X)
{
    ArrayXd x=pIP->normalize(pIP->expandNTuple(_X));
    int i,Osize=int(sam.size());
    ArrayXd _Y=ArrayXd::Zero(Osize);
    for ( i=0; i<Osize; i++ )  _Y(i)=pOS->denormalize(sam[i].f(x),i);
    return _Y;
//      ArrayXd _Y=fEvaluate(_X);
//      return _Y;
}


ArrayXd GoSUM::CAnalyticalModel::predictDerivative(const ArrayXd& _X,int i)  const
{
    return pOS->dDenormalize(i)*sam[i].df(pIP->normalize(pIP->expandNTuple(_X)))*pIP->dNormalize();
}


ArrayXXd GoSUM::CAnalyticalModel::predictDerivatives(const ArrayXd& _X) const
{
    ArrayXd x=pIP->normalize(pIP->expandNTuple(_X));
    int i,Osize=int(sam.size()),EIsize=int(x.size());
    ArrayXXd _dY=ArrayXXd::Zero(Osize,EIsize);
    for ( i=0; i<Osize; i++ )  _dY.row(i)=pOS->dDenormalize(i)*sam[i].df(x)*pIP->dNormalize();
    return _dY;
}


ArrayXd GoSUM::CAnalyticalModel::exportExpectedCurve(const ArrayXd &_x,int _i,int _o) const
{
    int i,N=_x.size(),ISize=pIP->size();

    ArrayXd xexp(ISize);
    for ( i=0; i<ISize; i++ ) xexp(i)=(*pIP)(i).expectedValue();

    ArrayXd yexp(N);
    for ( i=0; i<N; i++ )
    {
        xexp(_i)=_x(i);
        yexp(i)=predictValue(xexp,_o);
    }

    return yexp;

}


ArrayXd GoSUM::CAnalyticalModel::exportIntersectionCurve(int _s,const ArrayXd &_x,int _i,int _o) const
{
    int i,N=_x.size();

    ArrayXd xsect=X[_s];

    ArrayXd ysect(N);
    for ( i=0; i<N; i++ )
    {
        xsect(_i)=_x(i);
        ysect(i)=predictValue(xsect,_o);
    }

    return ysect;
}


//ArrayXd GoSUM::CAnalyticalModel::fEvaluate(const ArrayXd& x) const //Output for example1
//{
//    int i, Isize;

//    ArrayXd y(3);
//    Isize=x.size();
//    double sum=1.;
//    y(2)=0.1;
//    for ( i=0; i<Isize; i++ )
//    {
//        sum=sum+(x(i)+0.25)*(x(i)+0.25);
//        y(2)=y(2)+0.1*(i+1)*x(i);
//    }
//    y(0)=1./sum;
//    y(1)=1.;

//    return y;
//}


//ArrayXd GoSUM::CAnalyticalModel::fEvaluate(const ArrayXd& x) const //Output for example11
//{
//    int Isize;

//    ArrayXd y(1);
//    Isize=x.size();
//    double pi=2.*asin(1.);
//    y(0)=sin(pi*(2.*x(0)-1.))+7.*pow(sin(pi*(2.*x(1)-1.)),2)+0.1*pow(pi*(2.*x(2)-1.),4)*sin(pi*(2.*x(0)-1.));

//    return y;
//}


//ArrayXd GoSUM::CAnalyticalModel::fEvaluate(const ArrayXd& x) const //Output for example2A
//{
//    int Isize;

//    ArrayXd y(1);
//    Isize=x.size();
//    double a = 6.52;
//    double a1 = a+1.;

//    y(0)=(std::abs(4.*x(0)-2.))*(std::abs(4.*x(1)-2.));
//    for (int i=2; i<Isize; i++)
//    {
//        y(0)*=((std::abs(4.*x(i)-2.)+a)/a1);
//    }

//    return y;
//}

//ArrayXd GoSUM::CAnalyticalModel::fEvaluate(const ArrayXd& x) const //Output for example3B
//{
//    int Isize;

//    ArrayXd y(1);
//    Isize=x.size();
//    double a = 6.52;
//    double a1 = a+1.;

//    y(0)=1.;
//    for (int i=0; i<Isize; i++)
//    {
//        y(0)*=((std::abs(4.*x(i)-2.)+a)/a1);
//    }

//    return y;
//}

//ArrayXd GoSUM::CAnalyticalModel::fEvaluate(const ArrayXd& x) const //Output for example2C
//{
//    int Isize;

//    ArrayXd y(1);
//    Isize=x.size();

//    y(0)=1.;
//    for (int i=0; i<Isize; i++)
//    {
//        y(0)=y(0)*2.*x(i);
//    }
//    //y(0)=pow(2.,Isize)*y(0);
//    return y;
//}



//ArrayXd GoSUM::CAnalyticalModel::svm_predictValues(const ArrayXd& X)
//{
//    ArrayXd x=pIP->normalize(pIP->expandNTuple(X));
//    int i,Osize=int(sam.size()),n=int(x.size());
//    struct svm_node *xsvm;
//    xsvm  = new struct svm_node [n+1];
//    for (i=0; i<n; i++)
//    {  xsvm[i].index=i+1; xsvm[i].value=x(i); }
//    xsvm[n].index=-1;

//    ArrayXd Y=ArrayXd::Zero(Osize);
//    double t;

//    for ( i=0; i<Osize; i++ )
//    {
//        t = svm_predict(model[i],xsvm);
//        Y(i)=pOS->denormalize(t,i);
//    }
//    return Y;
//}
