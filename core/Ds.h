
#pragma once

#include <Eigen/Dense>

using namespace Eigen;




Array<long double,Dynamic,1> prepareDSample(unsigned int M);


// DSample points generator
class DSn
{
    private:
#ifdef _MSC_VER
	typedef __int64 Long;
#else
    typedef long long int Long;
#endif
        unsigned int k;
        unsigned int i, j;
        Long n;
        Array<long double,Dynamic,1> x;
        Array<long double,Dynamic,1> w;

/// constructor
    public:
        DSn(unsigned int var) : n(0)
        {
            const Array<long double,Dynamic,1> ir=prepareDSample(var);

            k=var;
            x.resize(k);
            w.resize(k);
            for (i=0; i<k; i++)
            {
                w(i) = ir(i);
                x(i) = 0.5L - w(i);
            }
        }

/// destructor
    public:
        ~DSn() {}

/// generator
    public:
        inline long double doub()
        {
            j = (n++)%k;
            x(j) = x(j) + w(j);
            x(j) = x(j) - (int)x(j);
            return x(j);
        }
};

