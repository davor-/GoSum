


#include "MADSOptimization.h"


BOOST_CLASS_EXPORT_IMPLEMENT(CMADS)


template <class Archive>
void CMADS::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("pOP",pOP);
    ar & boost::serialization::make_nvp("maxeval",maxeval);
    ar & boost::serialization::make_nvp("lhinit",lh0);
    ar & boost::serialization::make_nvp("lhiter",lhi);
    ar & boost::serialization::make_nvp("ims",ims);
    ar & boost::serialization::make_nvp("mps",mps);
    ar & boost::serialization::make_nvp("Xbest",Xbest);
    ar & boost::serialization::make_nvp("ybest",ybest);
}


NOMAD::Point CMADS::ArrayXd2NOMADPoint(const ArrayXd &x)
{
    int i,n=int(x.size());
    NOMAD::Point p(n);
    for ( i=0; i<n; i++ ) p[i]=x(i);
    return p;
}


ArrayXd CMADS::NOMADPoint2ArrayXd(const NOMAD::Point &p)
{
    int i,n=p.size();
    ArrayXd x(n);
    for ( i=0; i<n; i++ ) x(i)=p[i].value();
    return x;
}


ArrayXd CMADS::optimize(COptimizationProblem *_pOP,std::ostream &_out)
{
    pOP=_pOP;
    pOP->openOptimization();
    int j,M=pOP->constraintsSize();

    NOMAD::Display out; //(_out);
    NOMAD::begin(0,NULL);

    NOMAD::Parameters p(out);
    p.set_DISPLAY_DEGREE(NOMAD::NO_DISPLAY);//   (NOMAD::NORMAL_DISPLAY);
    p.set_DIMENSION(pOP->dimension());

    p.set_X0(ArrayXd2NOMADPoint(pOP->initialValue()));
    p.set_LOWER_BOUND(ArrayXd2NOMADPoint(pOP->lowerBound()));
    p.set_UPPER_BOUND(ArrayXd2NOMADPoint(pOP->upperBound()));

    p.set_MAX_BB_EVAL(maxeval);
    p.set_LH_SEARCH(lh0,lhi);
    p.set_INITIAL_MESH_SIZE(ArrayXd2NOMADPoint((pOP->upperBound()-pOP->lowerBound())*ims));
    p.set_MIN_POLL_SIZE(ArrayXd2NOMADPoint((pOP->upperBound()-pOP->lowerBound())*mps));

    std::vector <NOMAD::bb_output_type> bbot(M+2);
    bbot[0]=NOMAD::OBJ;
    bbot[1]=NOMAD::PB;
    for ( j=2; j<M+2; j++ ) bbot[j]=NOMAD::PB;
    p.set_BB_OUTPUT_TYPE(bbot);

    p.set_DIRECTION_TYPE(NOMAD::GPS_NP1_RAND_UNIFORM);

    p.check();

    CMADSEvaluator ev(p,pOP);
    NOMAD::Mads mads(p,&ev);
    mads.run();

    const NOMAD::Eval_Point *best=mads.get_best_feasible();
    if (best)
        Xbest=NOMADPoint2ArrayXd(NOMAD::Point(*best));
    else
        Xbest=pOP->initialValue();
    if (!best) cout<<"no best"<<endl;

    ArrayXd epbest;
    pOP->evaluate(Xbest,epbest);
    ybest=epbest(0);

    NOMAD::end();
    pOP->closeOptimization();

    pOP=NULL;

    return Xbest;
}


bool CMADSEvaluator::eval_x(NOMAD::Eval_Point &_x, const NOMAD::Double &_h_max, bool &_count_eval) const
{
    int i,M=pOP->constraintsSize();
    NOMAD::Point y(M+2);

    ArrayXd op,ep;
    op=CMADS::NOMADPoint2ArrayXd(_x);
    y[1]=(pOP->evaluate(op,ep))?0.:1.;
    y[0]=(pOP->isMinimization())?ep(0):-ep(0);
    for ( i=2; i<M+2; i++ ) { y[i]=1.-ep(i-1); }

    _x.set_bb_output(y);

    bool tohist=true;
    for ( i=1; tohist && i<M+2; i++ )  { if ( y[i]>0. ) tohist=false; }
    if ( tohist ) {
        pOP->writeHistory(op,ep(0));
    }

    _count_eval=true;

    return true;
}

