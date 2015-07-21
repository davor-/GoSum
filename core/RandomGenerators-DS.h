#ifndef CRANDOMGENERATORS_H
#define CRANDOMGENERATORS_H


#include <Eigen/Dense>
using namespace Eigen;

#if !defined(UINT32_MAX)
#define UINT32_MAX	0xffffffff
#endif


//! \brief
//! Abstract base class for all uniform random number generators (RNG).
class CBaseRNG {
public:
    virtual void setSeed(unsigned int s)=0; //!< Sets seed of the RNG.
    virtual unsigned int rndi()=0; //!< Returns randomly generated unsigned int.
    virtual double rnd()=0; //!< Returns randomly generated double between 0 and 1.
    virtual double rnd(double a, double b) { return a+((b-a)*rnd()); } //!< Returns randomly generated double between a and b.
};


//! \brief
//! Class for uniform RNG with large period = 3.138*10^57.
class CLargePeriodRNG : public CBaseRNG {
private:
#ifdef _MSC_VER
    typedef unsigned __int64 Ullong;
#else
    typedef unsigned long long int Ullong;
#endif
    Ullong u,v,w; //!< Parameters of the large period RNG.
private:
    inline Ullong int64() //!< Private function needed in setSeed() and rnd().
    {   u = u * 2862933555777941757LL + 7046029254386353087LL;
        v ^= v >> 17; v ^= v << 31; v ^= v >> 8;
        w = 4294957665U * (w & 0xffffffff) + (w >> 32);
        Ullong x = u ^ (u << 21); x ^= x >> 35; x ^= x << 4;
        return (x + v) ^ w;   }
    //inline unsigned int int32(){return (unsigned int)int64();} //!< private function
public:
    CLargePeriodRNG() {  setSeed(111);  }
    CLargePeriodRNG(unsigned int s) {  setSeed(s);  }
    virtual ~CLargePeriodRNG() {}
public:
    virtual void setSeed(unsigned int s) //!< Sets seed of the RNG.
    {   v=4101842887655102017LL; w=1;
        u = s ^ v; int64();
        v = u;     int64();
        w = v;     int64();   }
    virtual unsigned int rndi() { return (unsigned int)int64(); } //!< Returns randomly generated unsigned int.
    virtual double rnd() { return 5.42101086242752217E-20*int64(); } //!< Returns randomly generated double between 0 and 1.
};


//! \brief
//! Class for dynamic system uniform RNG.
class CDynamicSystemRNG : public CBaseRNG  {
private:
#ifdef _MSC_VER
    typedef __int64 Long;
#else
    typedef long long int Long;
#endif
    unsigned int k; //!< Paramters of the DS RNG.
    unsigned int i, j; //!< Paramters of the DS RNG.
    Long n; //!< Paramters of the DS RNG.
    Array<long double,Dynamic,1> x; //!< Paramters of the DS RNG.
    Array<long double,Dynamic,1> w; //!< Paramters of the DS RNG.
private:
    Array<long double,Dynamic,1> prepare(unsigned int M); //!< Private function.
public:
    CDynamicSystemRNG() {  setSeed(1); }
    CDynamicSystemRNG(unsigned int s) {  setSeed(s); }
    virtual ~CDynamicSystemRNG() {}
public:
    virtual void setSeed(unsigned int s) //!< Sets seed of the RNG.
    {   n=0;
        const Array<long double,Dynamic,1> ir=prepare(s);
        k=s;
        x.resize(k);
        w.resize(k);
        for (i=0; i<k; i++)
        {   w(i) = ir(i);
            x(i) = 0.5L - w(i);  }   }
    virtual unsigned int rndi() { return rnd()*UINT32_MAX; } //!< Returns randomly generated unsigned int.
    virtual double rnd() //!< Returns randomly generated double between 0 and 1.
    {   j = (n++)%k;
        x(j) = x(j) + w(j);
        x(j) = x(j) - (int)x(j);
        return x(j);   }
};


//! \brief
//! Class for uniform RNG with small period = 10^8.
#define NTAB 32
class CSmallPeriodRNG : public CBaseRNG {
private:
    static long iy1; //!< Paramters of the small period RNG.
    static long iv1[NTAB]; //!< Paramters of the small period RNG.
    static long idum1; //!< Paramters of the small period RNG.
public:
    CSmallPeriodRNG() {  setSeed(111); }
    CSmallPeriodRNG(unsigned int s) {  setSeed(s); }
    virtual ~CSmallPeriodRNG() {}
public:
    virtual void setSeed(unsigned int s); //!< Sets seed of the RNG.
    virtual unsigned int rndi() { return rnd()*UINT32_MAX; } //!< Returns randomly generated unsigned int.
    virtual double rnd(); //!< Returns randomly generated double between 0 and 1.
};
#undef NTAB


//! \brief
//! Class for uniform RNG with medium period = 2 * 10^18.
#define NTAB 32
class CMediumPeriodRNG : public CBaseRNG {
private:
    static long idum2; //!< Paramters of the medium period RNG.
    static long iy2; //!< Paramters of the medium period RNG.
    static long iv2[NTAB]; //!< Paramters of the medium period RNG.
    static long idum; //!< Paramters of the medium period RNG.
public:
    CMediumPeriodRNG() {  setSeed(111); }
    CMediumPeriodRNG(unsigned int s) {  setSeed(s); }
    virtual ~CMediumPeriodRNG() {}
public:
    virtual void setSeed(unsigned int s); //!< Sets seed of the RNG.
    virtual unsigned int rndi() { return rnd()*UINT32_MAX; } //!< Returns randomly generated unsigned int.
    virtual double rnd(); //!< Returns randomly generated double between 0 and 1.
};
#undef NTAB


//! \brief
//! Class for uniform RNG which is not linearily congruential.
#define NTAB 32
class CNLCRNG : public CBaseRNG {
private:
    static int inext,inextp; //!< Paramters of the nlc RNG.
    static long ma[56]; //!< Paramters of the nlc RNG.
public:
    CNLCRNG() {  setSeed(111); }
    CNLCRNG(unsigned int s) {  setSeed(s); }
    virtual ~CNLCRNG() {}
public:
    virtual void setSeed(unsigned int s); //!< Sets seed of the RNG.
    virtual unsigned int rndi() { return rnd()*UINT32_MAX; } //!< Returns randomly generated unsigned int.
    virtual double rnd(); //!< Returns randomly generated double between 0 and 1.
};
#undef NTAB


//! \brief
//! Class for NOMAD RNG with period = 2^96-1.
class CNomadRNG : public CBaseRNG {
private:
    static unsigned int x,y,z;
public:
public:
    CNomadRNG() {  setSeed(111);  }
    CNomadRNG(unsigned int s) {  setSeed(s);  }
    virtual ~CNomadRNG() {}
public:
    virtual void setSeed(unsigned int s) { x=s; } //!< Sets seed of the RNG.
    virtual unsigned int rndi();
    virtual double rnd() { return rndi()/UINT32_MAX; } //!< Returns randomly generated double between 0 and 1.
};



//! \brief
//! Class that holds all implemented uniform RNGs as static.
//! and allows switching between them.
class CRandomGenerator {
public:
    enum rngtype {smallperiod, mediumperiod, nonlinearcongruential, largeperiod, nomads }; //!< Implemented RNG types.
    static rngtype Type(std::string _stype); //!< Converts rng type name (string) to rngtype enumerator.
private:
    static CSmallPeriodRNG sp; //!< Small period RNG, static.
    static CMediumPeriodRNG mp; //!< Medium period RNG, static.
    static CNLCRNG nlc; //!< Non linearily congruential RNG, static.
    static CLargePeriodRNG lp; //!< Large period RNG, static.
    static CNomadRNG mad; //!< Dynamic system RNG, static.
    static CBaseRNG *p; //!< Poitner to the active RNG, static.
    static enum rngtype type; //!< Type of the active RNG.
public:
    static void Set(rngtype _type,unsigned int s=111); //!< Sets RNG of rngtype _type as active.
    static void SetSmallPeriod(unsigned int s=111) { type=smallperiod; p=&sp; p->setSeed(s); } //!< Sets small period RNG as active.
    static void SetMediumPeriod(unsigned int s=111) { type=mediumperiod; p=&mp; p->setSeed(s); } //!< Sets medium period RNG as active.
    static void SetNonlinearCongruential(unsigned int s=111) { type=nonlinearcongruential; p=&nlc; p->setSeed(s); } //!< Sets non lienarily congruential RNG as active.
    static void SetLargePeriod(unsigned int s=111) { type=largeperiod; p=&lp; p->setSeed(s); } //!< Sets large period RNG as active.
    static void SetNomads(unsigned int s=111) { type=nomads; p=&mad; p->setSeed(s); } //!< Sets dynamic system RNG as active.
    static rngtype &Type() { return type; } //!< Returns type of the active RNG.
    static void SetSeed(unsigned int s) { p->setSeed(s); }  //!< Sets seed of the active RNG.
    static unsigned int RndInt() { return p->rndi(); } //!< Returns an unsigned int randomly generated by the active RNG.
    static double Rnd() { return p->rnd(); } //!< Returns a double between 0 and 1 randomly generated by the active RNG.
    static double Rnd(double a,double b) { return p->rnd(a,b); } //!< Returns a double between a and b randomly generated by the active RNG.
};



#endif // CRANDOMGENERATORS_H
