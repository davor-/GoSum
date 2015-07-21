#ifndef MATLABLIBRARY_H
#define MATLABLIBRARY_H



#include "Utilities.h"
#include <QLibrary>
#include <mat.h>
#include <engine.h>



//! Typedefs for functions in Matlab's libmat dynamic library.
typedef MATFile *(*matftype1)(const char *,const char *);
typedef  int (*matftype2)(MATFile *);
typedef int (*matftype3)(MATFile *,const char *,const mxArray *);
typedef mxArray *(*matftype4)(MATFile *, const char *);

//! Typedefs for functions in Matlab's libmx dynamic library.
typedef mxArray *(*mxftype1)(mwSize ,mwSize ,mxComplexity);
typedef void (*mxftype2)(mxArray *);
typedef double *(*mxftype3)(const mxArray *);
typedef void (*mxftype4)(mxArray *pa,double *pr);
typedef size_t (*mxftype5)(const mxArray *);

//! Typedefs for functions in Matlab's libeng dynamic library.
typedef Engine *(*engftype1)(const char *,void *,int *);
typedef Engine *(*engftype2)(const char *);
typedef int (*engftype3)(Engine	*);
typedef int (*engftype4)(Engine *,bool *);
typedef int (*engftype5)(Engine *,bool );
typedef int (*engftype6)(Engine	*,const char *);
typedef mxArray *(*engftype7)(Engine *,const char *);
typedef int (*engftype8)(Engine	*,const char *,const mxArray *);
typedef int (*engftype9)(Engine *,char *,int );


//! \brief
//! Class interface for Matlab's libmat and libmx dynamic libraries.
class CMATLAB {
public:
    static std::string path; //!< Path to the matlab lib.
    static void SetPath(const std::string &_path); //!< Sets path to the matlab lib.
    static std::string &Path(); //!< Returns path to the matlab lib.
private:
    QLibrary libmat; //!< QLibrary for libmat.
    QLibrary libmx; //!< QLibrary for libmx.
    QLibrary libeng; //!< QLibrary for libeng.
    matftype1 pMatOpen; //!< Reference to matOpen function from libmat.
    matftype2 pMatClose; //!< Reference to matClose function from libmat.
    matftype3 pMatPutVariable; //!< Reference to matPutVariable function from libmat.
    matftype4 pMatGetVariable; //!< Reference to matGetVariable function from libmat.
    mxftype1 pMxCreateDoubleMatrix; //!< Reference to mxCreateDoubleMatrix function from libmx.
    mxftype2 pMxDestroyArray;  //!< Reference to mxDestroyArray function from libmx.
    mxftype3 pMxGetPr;  //!< Reference to mxGetPr function from libmx.
    mxftype4 pMxSetPr; //!< Reference to mxSetPr function from libmx.
    mxftype5 pMxGetNumberOfElements; //!< Reference to mxSetPr function from libmx.
    engftype1 pEngOpenSingleUse;  //!< Reference to engOpenSingleUse function from libeng.
    engftype2 pEngOpen; //!< Reference to engOpen function from libeng.
    engftype3 pEngClose; //!< Reference to engClose function from libeng.
    engftype4 pEngGetVisible; //!< Reference to engGetVisible function from libeng.
    engftype5 pEngSetVisible; //!< Reference to engSetVisible function from libeng.
    engftype6 pEngEvalString; //!< Reference to engEvalString function from libeng.
    engftype7 pEngGetVariable; //!< Reference to engGetVariable function from libeng.
    engftype8 pEngPutVariable; //!< Reference to engPutVariable function from libeng.
    engftype9 pEngOutputBuffer; //!< Reference to engOutputBuffer function from libeng.
public:
    CMATLAB();
    virtual ~CMATLAB() { libmat.unload(); libmx.unload(); libeng.unload(); }
public:
    MATFile *matOpen(const string &filename,const string &mode) { return pMatOpen(filename.data(),mode.data()); } //!< Opens mat file using libmat.
    bool matClose(MATFile *pmat) { return (pMatClose(pmat)==0); } //!< Closes mat file using libmat.
    bool matPutVariable(MATFile *pmat,const string &name, const mxArray *pa) { return (pMatPutVariable(pmat,name.data(),pa)==0); } //!< Puts variable in mat file using libmat.
    mxArray *matGetVariable(MATFile *pmat,const string &name) { return pMatGetVariable(pmat,name.data());} //!< Gets variable from mat file using libmat.
    mxArray *mxCreateDoubleMatrix(int N,int M) { return pMxCreateDoubleMatrix((mwSize)N,(mwSize)M,mxREAL); } //!< Creats double matrix using libmx.
    void mxDestroyArray(mxArray *pa) { pMxDestroyArray(pa); } //!< Destroys array using libmx.
    int mxGetNumberOfElements(mxArray *pa)  {  return int(pMxGetNumberOfElements(pa)); } //!< Returns number of ements in mxArray using libmx.
    double *mxGetPr(mxArray *pa) { return pMxGetPr(pa);  } //!< Sets data in double matrix using libmx.
    Engine *engOpenSingleUse(const char *startcmd) { return pEngOpenSingleUse(startcmd,NULL,NULL); } //!< Opens engine for single use.
    Engine *engOpen(const string &startcmd) { return pEngOpen(startcmd.data()); } //!< Opens engine.
    int engClose(Engine	*ep) { return pEngClose(ep); } //!< Closes engine.
    int engGetVisible(Engine *ep,bool* bVal) { return pEngGetVisible(ep,bVal); }  //!< Gets if engine is (in)visible.
    int engSetVisible(Engine *ep,bool newVal) { return pEngSetVisible(ep,newVal); }  //!< Sets engine to (in)visible.
    int engEvalString(Engine	*ep,const string &str) { return pEngEvalString(ep,str.data()); } //!< Gives to engine a string to evaluate.
    mxArray *engGetVariable(Engine	*ep,const string &name) { return pEngGetVariable(ep,name.data()); } //!< Gets variable from engine.
    int engPutVariable(Engine	*ep,const string &var_name,const mxArray *ap) { return pEngPutVariable(ep,var_name.data(),ap); } //!< Puts variable into engine.
    int engOutputBuffer(Engine *ep,char *buffer,int buflen) { return pEngOutputBuffer(ep,buffer,buflen); } //!< Sets engines output buffer.
public:
    void matPut(string filename,const ArrayXd &X,string Xname); //!< Puts 1 array to .mat file.
    void matGet(string filename,ArrayXd &X,string Xname); //!< gets 1 array from .mat file.
};




#endif // MATLABLIBRARY_H
