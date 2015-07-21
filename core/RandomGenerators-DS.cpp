

#include "RandomGenerators.h"



//***********************************************************************

#define IA 16807L
#define IM 2147483647L
#define AM (1.0/IM)
#define IQ 127773L
#define IR 2836L
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)


long CSmallPeriodRNG::iy1=0;
long CSmallPeriodRNG::iv1[NTAB];
long CSmallPeriodRNG::idum1=0;


void CSmallPeriodRNG::setSeed(unsigned int seed)
{
    int j;
    long k;

    idum1 = seed;
    if (idum1 == 0) idum1=1;
    if (idum1 < 0) idum1 = -idum1;
    for (j=NTAB+7;j>=0;j--)
    {
        k=(idum1)/IQ;
        idum1=IA*(idum1-k*IQ)-IR*k;
        if (idum1 < 0) idum1 += IM;
        if (j < NTAB) iv1[j] = idum1;
    }
    iy1=iv1[0];
}


double CSmallPeriodRNG::rnd()
{
    int j;
    long k;
    double temp;

    k=(idum1)/IQ;
    idum1=IA*(idum1-k*IQ)-IR*k;
    if (idum1 < 0) idum1 += IM;
    j=iy1/NDIV;
    iy1=iv1[j];
    iv1[j] = idum1;
    if ((temp=AM*iy1) > RNMX) return RNMX;
    else return temp;
}


#undef IA
#undef IM
#undef AM
#undef IQ
#undef IR
#undef NTAB
#undef NDIV
#undef EPS
#undef RNMX



//***********************************************************************

#define IM1 2147483563L
#define IM2 2147483399L
#define AM (1.0/IM1)
#define IMM1 (IM1-1)
#define IA1 40014L
#define IA2 40692L
#define IQ1 53668L
#define IQ2 52774L
#define IR1 12211L
#define IR2 3791
#define NTAB 32
#define NDIV (1+IMM1/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)

long CMediumPeriodRNG::idum2=123456789;
long CMediumPeriodRNG::iy2=0;
long CMediumPeriodRNG::iv2[NTAB];
long CMediumPeriodRNG::idum=0;


void CMediumPeriodRNG::setSeed(unsigned int s)
{
    int j;
    long k;

    idum = long(s);
    if (idum == 0) idum=1;
    if (idum < 0) idum = -idum;
    idum2=(idum);
    for (j=NTAB+7;j>=0;j--) {
      k=(idum)/IQ1;
      idum=IA1*(idum-k*IQ1)-k*IR1;
      if (idum < 0) idum += IM1;
      if (j < NTAB) iv2[j] = idum;
    }
    iy2=iv2[0];
}


double CMediumPeriodRNG::rnd()
{
    int j;
    long k;
    float temp;

    k=(idum)/IQ1;
    idum=IA1*(idum-k*IQ1)-k*IR1;
    if (idum < 0) idum += IM1;
    k=idum2/IQ2;
    idum2=IA2*(idum2-k*IQ2)-k*IR2;
    if (idum2 < 0) idum2 += IM2;
    j=iy2/NDIV;
    iy2=iv2[j]-idum2;
    iv2[j] = idum;
    if (iy2 < 1) iy2 += IMM1;
    if ((temp=AM*iy2) > RNMX) return RNMX;
    else return temp;
}


#undef IM1
#undef IM2
#undef AM
#undef IMM1
#undef IA1
#undef IA2
#undef IQ1
#undef IQ2
#undef IR1
#undef IR2
#undef NTAB
#undef NDIV
#undef EPS
#undef RNMX



//***********************************************************************

#define MBIG 1000000000
#define MSEED 161803398
#define MZ 0
#define FAC (1.0/MBIG)


int CNLCRNG::inext,CNLCRNG::inextp;
long CNLCRNG::ma[56];


void CNLCRNG::setSeed(unsigned int s)
{
    long idum = s;
    long mj,mk;
    int i,ii,k;

    mj=labs(MSEED-labs(idum));
    mj %= MBIG;
    ma[55]=mj;
    mk=1;
    for (i=1;i<=54;i++) {
      ii=(21*i) % 55;
      ma[ii]=mk;
      mk=mj-mk;
      if (mk < MZ) mk += MBIG;
      mj=ma[ii];
    }
    for (k=1;k<=4;k++)
      for (i=1;i<=55;i++) {
        ma[i] -= ma[1+(i+30) % 55];
        if (ma[i] < MZ) ma[i] += MBIG;
      }
    inext=0;
    inextp=31;
}


double CNLCRNG::rnd()
{
    long mj;
    int i,ii,k;

    if (++inext == 56) inext=1;
    if (++inextp == 56) inextp=1;
    mj=ma[inext]-ma[inextp];
    if (mj < MZ) mj += MBIG;
    ma[inext]=mj;
    return mj*FAC;
}


#undef MBIG
#undef MSEED
#undef MZ
#undef FAC



//***********************************************************************

unsigned int CNomadRNG::x = 123456789;
unsigned int CNomadRNG::y = 362436069;
unsigned int CNomadRNG::z = 521288629;


unsigned int CNomadRNG::rndi()
{
    unsigned int t;
    x ^= x << 16;
    x ^= x >> 5;
    x ^= x << 1;

    t = x;
    x = y;
    y = z;
    z = t ^ x ^ y;

    return z;
}




//***********************************************************************

CSmallPeriodRNG CRandomGenerator::sp;
CMediumPeriodRNG CRandomGenerator::mp;
CNLCRNG CRandomGenerator::nlc;
CLargePeriodRNG CRandomGenerator::lp;
CNomadRNG CRandomGenerator::mad;
CBaseRNG *CRandomGenerator::p=&CRandomGenerator::mp;
CRandomGenerator::rngtype CRandomGenerator::type=CRandomGenerator::nomads;

CRandomGenerator::rngtype CRandomGenerator::Type(std::string _stype)
{
    if ( _stype=="smallperiod" )                 return CRandomGenerator::smallperiod;
    else if ( _stype=="mediumperiod" )           return CRandomGenerator::mediumperiod;
    else if ( _stype=="nonlinearcongruential" )  return CRandomGenerator::nonlinearcongruential;
    else if ( _stype=="largeperiod" )            return CRandomGenerator::largeperiod;
    else if ( _stype=="mads" )                   return CRandomGenerator::nomads;
    else  throw "CRandomGenerator::Type error: unrecognised type";
}


void CRandomGenerator::Set(rngtype _type,unsigned int s)
{
    if ( _type==smallperiod )                 SetSmallPeriod(s);
    else if ( _type==mediumperiod )           SetMediumPeriod(s);
    else if ( _type==nonlinearcongruential )  SetNonlinearCongruential(s);
    else if ( _type==largeperiod )            SetLargePeriod(s);
    else if ( _type==nomads )                 SetNomads(s);
    else  throw "CRandomGenerator::Set error: unrecognised type";

}
