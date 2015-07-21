
// Uniform random number generator with period = 3.138*10^57

#pragma once

#ifdef _MSC_VER
typedef unsigned __int64 Ullong;
#else
typedef unsigned long long int Ullong;
#endif


class Ran  {
private:
  Ullong u,v,w;
public:
  Ran(Ullong j) : v(4101842887655102017LL), w(1)
  {
    u = j ^ v; int64();
    v = u;     int64();
    w = v;     int64();
  }
  inline Ullong int64()
  {
    u = u * 2862933555777941757LL + 7046029254386353087LL;
    v ^= v >> 17; v ^= v << 31; v ^= v >> 8;
	w = 4294957665U * (w & 0xffffffff) + (w >> 32);
    Ullong x = u ^ (u << 21); x ^= x >> 35; x ^= x << 4;
    return (x + v) ^ w;
  }
  inline double doub(){return 5.42101086242752217E-20 * int64();}
  inline unsigned int int32(){return (unsigned int)int64();}
};
