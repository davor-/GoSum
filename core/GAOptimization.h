#ifndef GAOPTIMIZATION_H
#define GAOPTIMIZATION_H

#include "OptimizationProblem.h"
#include "ga.h"


//! \brief
//! Class for the genetic algorithm, i.e. interface for GALIB.
class CGAModelOptimization {
protected:
    int popsize;
    static COptimizationProblem *pOP; //!< Points to the optimization problem.
    static double C; //!< Constant added to get positive fitness.
    ArrayXd Xbest;
    double ybest;
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
public:
    CGAModelOptimization() : popsize(100) {}
    virtual ~CGAModelOptimization() {}
public:
    static ArrayXd GAGenome2ArrayXd(const GAGenome &g); //!< Converts GAGenome -> hpyercube point.
    static float Fitness(GAGenome &g); //!< Evalutes fitness of an individual.
    ArrayXd optimize(COptimizationProblem *_pSOP,std::ostream &_out=std::cout); //!< Solves the optimizations problem.
    int progressStepsSize() const { return popsize*1000; }  //!< Returns progress steps size.
    int populationSize() const { return popsize; } //!< Returns population size.
    void setPopulationSize(int _popsize) { popsize=_popsize; } //!< Sets population size.
};


BOOST_CLASS_EXPORT_KEY(CGAModelOptimization)


#endif // GAOPTIMIZATION_H
