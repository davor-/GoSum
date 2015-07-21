#ifndef PROBABILITY_H
#define PROBABILITY_H


#include "RandomVariable.h"


//! brief
//! Function that implements chi-sqaure test
//! to determine if a continuous random variable is a good fit to a continuous sample.
bool chiSquareTest(const CContinuousRV &_aRV,const CContinuousSample &_aS,const double chi_alpha);


//! brief
//! Function that implements Kolmogorov-Smirnov test
//! to determine if a continuous random variable is a good fit to a continuous sample.
double KolmogorovSmirnovTest(const CContinuousRV &_aRV,const CContinuousSample &_aS);



#endif // PROBABILITY_H
