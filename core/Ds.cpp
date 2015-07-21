

#include "Ds.h"


Array<long double,Dynamic,1> prepareDSample(unsigned int M)
{
    ArrayXi prime(M);
    Array<long double,Dynamic,1> w(M);
    bool isPrime=true;

    unsigned k=0,i=2,j;
    while ( k<M )
    {
        for ( j=2; j<i; j++ )
        {
            if ( i % j == 0 )
            {
              isPrime=false;
              break;
            }
        }
        if (isPrime)
        {
          prime(k)=i;
          k++;
        }
        isPrime=true;
        i++;
    }

    for ( j=0; j<M; j++ ) 	w(j)=sqrt((long double)(prime(j)));

    return w;
}

