



#include "FFTWLibrary.h"


ArrayXd CFFTW::discreteCosineTransform(const ArrayXd& in)
{
    int i,k,N=in.size();
    if ( N==0 ) throw "CFFTW::discreteCosineTransform error: empty in array";

    fftw_complex *fftwin  = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*N);
    if (!fftwin) throw "CFFTW::discreteCosineTransform error: fftwin is NULL";
    fftw_complex *fftwout = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*N);
    if (!fftwout) throw "CFFTW::discreteCosineTransform error: fftwin is NULL";
    fftw_plan p = fftw_plan_dft_1d(N,fftwin,fftwout,FFTW_FORWARD,FFTW_ESTIMATE);

    for ( i=0, k=0; k<N; k=k+2, i++ )
    {   fftwin[i][0] = in(k);
        fftwin[i][1] = 0.;  }
    for ( k=N-1; k>0; k=k-2, i++ )
    {   fftwin[i][0] = in(k);
        fftwin[i][1] = 0.;  }

    fftw_execute(p);

    ArrayXcd weight=ArrayXcd::Constant(N,complex<double>(0.,-M_PI/double(2*N)));
    weight=exp(weight*ArrayXd::LinSpaced(N,0.,double(N-1)))*2.; weight(0)=1.;
    ArrayXcd outc(N);
    for ( k=0; k<N; k++ )  outc(k)=complex<double>(fftwout[k][0],fftwout[k][1]);
    outc*=weight;

    fftw_destroy_plan(p);
    fftw_free(fftwin);
    fftw_free(fftwout);

    return outc.real();
}


ArrayXd CFFTW::inverseDiscreteCosineTransform(const ArrayXd& in)
{
    int j,k,N=in.size();
    if ( N==0 ) throw "CFFTW::inverseDiscreteCosineTransform error: empty in array";

    fftw_complex *fftwin  = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*N);
    if (!fftwin) throw "CFFTW::discreteCosineTransform error: fftwin is NULL";
    fftw_complex *fftwout = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*N);
    if (!fftwout) throw "CFFTW::discreteCosineTransform error: fftwin is NULL";
    fftw_plan p = fftw_plan_dft_1d(N,fftwin,fftwout,FFTW_BACKWARD,FFTW_ESTIMATE);

    ArrayXcd weight=ArrayXcd::Constant(N,complex<double>(0.,M_PI/double(2*N)));
    weight=in*N*exp(weight*ArrayXd::LinSpaced(N,0.,double(N-1)));

    for ( k=0; k<N; k++ )
    {   fftwin[k][0]=weight(k).real();
        fftwin[k][1]=weight(k).imag(); }

    fftw_execute(p);

    ArrayXd out=ArrayXd::Zero(N);
    double iN=1./double(N);
    for ( j=0, k=0; j<N; j=j+2, ++k )  out(j)=fftwout[k][0]*iN;
    for ( j=1, k=N-1; j<N; j=j+2, --k )  out(j)=fftwout[k][0]*iN;

    fftw_destroy_plan(p);
    fftw_free(fftwin);
    fftw_free(fftwout);

    return out;
}
