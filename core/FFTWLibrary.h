#ifndef FFTWLIBRARY_H
#define FFTWLIBRARY_H


#include "Utilities.h"
#include <windows.h>
#include <QLibrary>
#include <fftw3.h>


//! \brief
//! Class interface for FFTW's libfftw library.
class CFFTW {
public:
    CFFTW() {}
    virtual ~CFFTW() {}
public:
    ArrayXd discreteCosineTransform(const ArrayXd& in); //!< Computes discrete cosine transform using fftw.
    ArrayXd inverseDiscreteCosineTransform(const ArrayXd& in); //!< Computes inverse discrete cosine transform using fftw.
};



#endif // FFTWLIBRARY_H
