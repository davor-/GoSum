#ifndef EIGENADDITIONS_H
#define EIGENADDITIONS_H

#include "Utilities.h"

#include <Eigen/Dense>
#include <Eigen/Sparse>



ArrayXXd norm(ArrayXXd X);
ArrayXXd firstDerivative(ArrayXXd X,double hs,bool closed=false);
ArrayXXd secondDerivative(ArrayXXd X,double hs,bool closed=false);
ArrayXXd fourthDerivative(ArrayXXd X,double hs,bool closed=false);
ArrayXXd unitTangent(ArrayXXd X,double hs,bool closed=false);
ArrayXXd curvature(ArrayXXd X,double hs,bool closed=false);

ArrayXd average(const ArrayXd &A);
ArrayXd difference(const ArrayXd &A);
ArrayXXd xAverage(const ArrayXXd &A);
ArrayXXd xDifference(const ArrayXXd &A);
ArrayXXd xxDifference(const ArrayXXd &A);
ArrayXXd yAverage(const ArrayXXd &A);
ArrayXXd yDifference(const ArrayXXd &A);
ArrayXXd yyDifference(const ArrayXXd &A);

SparseMatrix<double> KroneckerProduct(const SparseMatrix<double> &A,const SparseMatrix<double> &B,int _nzs=0);
SparseMatrix<double> identitySparseMatrix(int n);
SparseMatrix<double> Laplace1dMatrix(int _n,double _r,double _a11,double _ann);
SparseMatrix<double> HelmholtzMatrix(int _n,ArrayXd _de,bool _periodic);


#endif // EIGENADDITIONS_H
