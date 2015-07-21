

#include "GAOptimization.h"

#define INSTANTIATE_REAL_GENOME
#include "GARealGenome.h"
#undef INSTANTIATE_REAL_GENOME


BOOST_CLASS_EXPORT_IMPLEMENT(CGAModelOptimization)


COptimizationProblem *CGAModelOptimization::pOP=NULL;
double CGAModelOptimization::C=1.e+2;


template <class Archive>
void CGAModelOptimization::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("popsize",popsize);
    ar & boost::serialization::make_nvp("pOP",pOP);
    ar & boost::serialization::make_nvp("C",C);
    ar & boost::serialization::make_nvp("Xbest",Xbest);
    ar & boost::serialization::make_nvp("ybest",ybest);
}


ArrayXd CGAModelOptimization::GAGenome2ArrayXd(const GAGenome &g)
{
    GARealGenome &genome=(GARealGenome &)g;
    int i,L=genome.length();
    ArrayXd hp(L);
    for( i=0; i<L; i++ ) hp(i)=double(genome.gene(i));
    return hp;
}


float CGAModelOptimization::Fitness(GAGenome &g)
{
    ArrayXd ep;
    ArrayXd gp=GAGenome2ArrayXd(g);
    ArrayXd op=pOP->lowerBound()+gp*(pOP->upperBound()-pOP->lowerBound());
    if ( !pOP->evaluate(op,ep) )  return 0.;

    int i,M=pOP->constraintsSize();
    for ( i=1; i<M+1; i++ )  if ( ep(i)<=0. ) return 0.;

    pOP->writeHistory(op,ep(0));

    ep(0)=(pOP->isMaximization())?ep(0):-ep(0);

    return float(max(0.,C+ep(0)));
}


ArrayXd CGAModelOptimization::optimize(COptimizationProblem *_pOP,std::ostream &_out)
{
    pOP=_pOP;
    pOP->openOptimization();

    int dim=pOP->dimension();
    GARealGenome genome(dim,GARealAlleleSet(0.000001,0.999999,GAAllele::EXCLUSIVE,GAAllele::EXCLUSIVE),CGAModelOptimization::Fitness);
    GAPopulation population(genome,popsize);
    GASimpleGA ga(population);
    ga.evolve(dim);

    ArrayXd gp=GAGenome2ArrayXd(ga.statistics().bestIndividual());
    Xbest=pOP->lowerBound()+gp*(pOP->upperBound()-pOP->lowerBound());
    ArrayXd epbest;
    pOP->evaluate(Xbest,epbest);
    ybest=epbest(0);

    _out<<ga.statistics()<<endl;
    pOP->closeOptimization();

    pOP=NULL;

    return Xbest;
}
