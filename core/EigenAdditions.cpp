

#include "EigenAdditions.h"


ArrayXXd norm(ArrayXXd X) {
    return sqrt(X.row(0)*X.row(0)+X.row(1)*X.row(1));
}

ArrayXXd unitTangent(ArrayXXd X,double hs,bool closed) {
    int ns=int(X.cols())-1;
    ArrayXXd dXds=firstDerivative(X,hs,closed);
    ArrayXXd dXdsNorm=ArrayXXd::Zero(2,ns+1);
    dXdsNorm.row(0)=dXdsNorm.row(1)=sqrt(dXds.row(0)*dXds.row(0)+dXds.row(1)*dXds.row(1));
    return dXds/dXdsNorm;
}

ArrayXXd firstDerivative(ArrayXXd X,double hs,bool closed) {
    int ns=int(X.cols())-1,dim=X.rows();
    ArrayXXd dXds=ArrayXXd::Zero(dim,ns+1);

    for ( int i=1; i<ns; i++ ) {
        dXds.col(i)=(X.col(i+1)-X.col(i-1))/(2.*hs);
    }
    if ( closed ) {
        dXds.col(0)=dXds.col(ns)=(X.col(1)-X.col(ns-1))/(2.*hs);
    }
    else  {
        dXds.col(0)=(X.col(1)-X.col(0))/hs;
        dXds.col(ns)=(X.col(ns)-X.col(ns-1))/hs;
    }
    return dXds;
}

ArrayXXd secondDerivative(ArrayXXd X,double hs,bool closed) {
    int ns=int(X.cols())-1,dim=X.rows();
    ArrayXXd d2Xds2=ArrayXXd::Zero(dim,ns+1);
    double hs2=hs*hs;

    for ( int i=1; i<ns; i++ ) {
        d2Xds2.col(i)=(X.col(i+1)-2.*X.col(i)+X.col(i-1))/hs2;
    }
    if ( closed ) {
        d2Xds2.col(0)=d2Xds2.col(ns)=(X.col(1)-2.*X.col(0)+X.col(ns-1))/hs2;
    }
    else  {
        d2Xds2.col(0).setConstant(0.);
        d2Xds2.col(ns).setConstant(0.);
    }

    return d2Xds2;
}

ArrayXXd fourthDerivative(ArrayXXd X,double hs,bool closed) {
    int ns=int(X.cols())-1,dim=X.rows();
    ArrayXXd d4Xds4=ArrayXXd::Zero(dim,ns+1);
    double hs4=pow(hs,4.);

    for ( int i=2; i<=ns-2; i++ ) {
        d4Xds4.col(i)=(X.col(i+2)-4.*X.col(i+1)+6.*X.col(i)-4.*X.col(i-1)+X.col(i-2))/hs4;
    }
    if ( closed ) {
        d4Xds4.col(0)=(X.col(2)-4.*X.col(1)+6.*X.col(0)-4.*X.col(ns-1)+X.col(ns-2))/hs4;
        d4Xds4.col(1)=(X.col(3)-4.*X.col(2)+6.*X.col(1)-4.*X.col(0)+X.col(ns-1))/hs4;
        d4Xds4.col(ns-1)=(X.col(1)-4.*X.col(ns)+6.*X.col(ns-1)-4.*X.col(ns-2)+X.col(ns-3))/hs4;
        d4Xds4.col(ns)=(X.col(2)-4.*X.col(1)+6.*X.col(ns)-4.*X.col(ns-1)+X.col(ns-2))/hs4;
    }
    else  {
        d4Xds4.col(0).setConstant(0.); //=(X.col(2)-4.*X.col(1)+6.*X.col(0)-4.*(2.*X.col(0)-X.col(1))+(2.*X.col(0)-X.col(2)))/hs4;
        d4Xds4.col(1).setConstant(0.); //=(X.col(3)-4.*X.col(2)+6.*X.col(1)-4.*X.col(0)+(2.*X.col(0)-X.col(1)))/hs4;
        d4Xds4.col(ns-1).setConstant(0.); //=((2.*X.col(ns)-X.col(ns-1))-4.*X.col(ns)+6.*X.col(ns-1)-4.*X.col(ns-2)+X.col(ns-3))/hs4;
        d4Xds4.col(ns).setConstant(0.); //=((2.*X.col(ns)-X.col(ns-2))-4.*(2.*X.col(ns)-X.col(ns-1))+6.*X.col(ns)-4.*X.col(ns-1)+X.col(ns-2))/hs4;
    }
    return d4Xds4;
}

ArrayXXd curvature(ArrayXXd X,double hs,bool closed) {
    ArrayXXd dXds=firstDerivative(X,hs,closed);
    ArrayXXd d2Xds2=secondDerivative(X,hs,closed);
    ArrayXXd kappa=(dXds.row(0)*d2Xds2.row(1)-dXds.row(1)*d2Xds2.row(0))/pow(dXds.row(0)*dXds.row(0)+dXds.row(1)*dXds.row(1),3./2.);
    return kappa;
}





ArrayXd average(const ArrayXd &A) {
    ArrayXd Aa(A.size()-1);
    for ( int i=0; i<Aa.size(); i++ )
        Aa(i)=0.5*(A(i)+A(i+1));
    return Aa;
}

ArrayXd difference(const ArrayXd &A) {
    ArrayXd Ad(A.size()-1);
    for ( int i=0; i<Ad.size(); i++ )
        Ad(i)=(A(i+1)-A(i));
    return Ad;
}

ArrayXXd xAverage(const ArrayXXd &A) {
    ArrayXXd Aa(A.rows()-1,A.cols());
    for ( int i=0; i<Aa.rows(); i++ )
        Aa.row(i)=0.5*(A.row(i)+A.row(i+1));
    return Aa;
}

ArrayXXd xDifference(const ArrayXXd &A) {
    ArrayXXd Ad(A.rows()-1,A.cols());
    for ( int i=0; i<Ad.rows(); i++ )
        Ad.row(i)=(A.row(i+1)-A.row(i));
    return Ad;
}

ArrayXXd xxDifference(const ArrayXXd &A) {
    ArrayXXd Ad(A.rows(),A.cols());
    for ( int i=1; i<Ad.rows()-1; i++ )
        Ad.row(i)=(A.row(i-1)-2.*A.row(i)+A.row(i+1));
    return Ad;
}

ArrayXXd yAverage(const ArrayXXd &A) {
    ArrayXXd Aa(A.rows(),A.cols()-1);
    for ( int j=0; j<Aa.cols(); j++ )
        Aa.col(j)=0.5*(A.col(j)+A.col(j+1));
    return Aa;
}

ArrayXXd yDifference(const ArrayXXd &A) {
    ArrayXXd Ad(A.rows(),A.cols()-1);
    for ( int j=0; j<Ad.cols(); j++ )
        Ad.col(j)=(A.col(j+1)-A.col(j));
    return Ad;
}

ArrayXXd yyDifference(const ArrayXXd &A) {
    ArrayXXd Ad(A.rows(),A.cols());
    for ( int j=1; j<Ad.cols()-1; j++ )
        Ad.col(j)=(A.col(j-1)-2.*A.col(j)+A.col(j+1));
    return Ad;
}

SparseMatrix<double> KroneckerProduct(const SparseMatrix<double> &A,const SparseMatrix<double> &B,int _nzs) {
    int n=A.rows(),m=A.cols(),p=B.rows(),q=B.cols();
    SparseMatrix<double> C(n*p,m*q);

    if ( _nzs>0 ) {
        VectorXi nzsI(m*q);
        nzsI.setConstant(_nzs);
        C.makeCompressed();
        C.reserve(nzsI);
    }
    for ( int k=0; k<A.outerSize(); ++k ) {
      for ( SparseMatrix<double>::InnerIterator ait(A,k); ait; ++ait ) {
          for ( int l=0; l<B.outerSize(); ++l ) {
            for ( SparseMatrix<double>::InnerIterator bit(B,l); bit; ++bit ) {
                C.insert(ait.row()*p+bit.row(),ait.col()*q+bit.col())=ait.value()*bit.value();
            }
          }
      }
    }
    return C;
}

SparseMatrix<double> identitySparseMatrix(int n) {
    SparseMatrix<double> I(n,n);
    I.setIdentity();
    return I;
}

SparseMatrix<double> Laplace1dMatrix(int _n,double _r, double _a11,double _ann) {
    SparseMatrix<double> L1(_n,_n);

    VectorXi nzsI(_n);
    nzsI.setConstant(3);
    L1.makeCompressed();
    L1.reserve(nzsI);

    L1.insert(0,0)=_a11; L1.insert(0,1)=-1;
    for ( int i=1; i<_n-1; i++ ) {
        L1.insert(i,i-1)=-1.;
        L1.insert(i,i)=2.;
        L1.insert(i,i+1)=-1.;
    }
    L1.insert(_n-1,_n-2)=-1.; L1.insert(_n-1,_n-1)=_ann;

    L1*=_r;

    return L1;
}

SparseMatrix<double> HelmholtzMatrix(int _n,ArrayXd _de,bool _periodic) {
    SparseMatrix<double> H1(_n,_n);

    VectorXi nzsI(_n);
    nzsI.setConstant(3);
    H1.makeCompressed();
    H1.reserve(nzsI);

    if (_periodic) {
        H1.insert(0,_n-1)=-1.;
        H1.insert(0,0)=2.+_de(0);
        H1.insert(0,1)=-1.;
    }
    else {
        H1.insert(0,0)=1.+_de(0);
        H1.insert(0,1)=-1.;
    }
    for ( int i=1; i<_n-1; i++ ) {
        H1.insert(i,i-1)=-1.;
        H1.insert(i,i)=2.+_de(i);
        H1.insert(i,i+1)=-1.;
    }
    if (_periodic) {
        H1.insert(_n-1,_n-2)=-1.;
        H1.insert(_n-1,_n-1)=2.+_de(_n-1);
        H1.insert(_n-1,0)=-1.;
    }
    else {
        H1.insert(_n-1,_n-2)=-1.;
        H1.insert(_n-1,_n-1)=1.+_de(_n-1);
    }

    return H1;

}





