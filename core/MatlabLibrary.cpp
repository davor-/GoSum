


#include "MatlabLibrary.h"
#include <windows.h>


std::string CMATLAB::path="C:\\Program Files\\MATLAB\\R2010b\\bin\\win64";
void CMATLAB::SetPath(const std::string &_path)   {  path=_path; }
std::string &CMATLAB::Path() {  return path; }


CMATLAB::CMATLAB()
{
    std::wstring wpath=string2WideString(path);
    SetDllDirectory(LPCWSTR(wpath.data()));

    libmat.setFileName("libmat");
    if (!libmat.load()) throw "CMATLAB::CMATLAB error: unable to load libmat";

    libmx.setFileName("libmx");
    if(!libmx.load()) throw "CMATLAB::CMATLAB error: unable to load libmx";

    libeng.setFileName("libeng");
    if(!libeng.load()) throw "CMATLAB::CMATLAB error: unable to load libeng";

    pMatOpen = (matftype1) libmat.resolve("matOpen");
    if (!pMatOpen)  throw "CMATLAB::CMATLAB error: unable to resolve libmat for matOpen";
    pMatClose = (matftype2) libmat.resolve("matClose");
    if (!pMatClose) throw "CMATLAB::CMATLAB error: unable to resolve libmat for matClose";
    pMatPutVariable = (matftype3) libmat.resolve("matPutVariable");
    if (!pMatPutVariable) throw "CMATLAB::CMATLAB error: unable to resolve libmat for matPutVariable";
    pMatGetVariable = (matftype4) libmat.resolve("matGetVariable");
    if (!pMatGetVariable) throw "CMATLAB::CMATLAB error: unable to resolve libmat for matGetVariable";


    pMxCreateDoubleMatrix = (mxftype1) libmx.resolve("mxCreateDoubleMatrix");
    if (!pMxCreateDoubleMatrix) throw "CMATLAB::CMATLAB error: unable to resolve libmx for mxCreateDoubleMatrix";
    pMxDestroyArray = (mxftype2) libmx.resolve("mxDestroyArray");
    if (!pMxDestroyArray) throw "CMATLAB::CMATLAB error: unable to resolve libmx for mxDestroyArray";
    pMxGetPr = (mxftype3) libmx.resolve("mxGetPr");
    if (!pMxGetPr) throw "CMATLAB::CMATLAB error: unable to resolve libmx for mxGetPr";
    pMxSetPr = (mxftype4) libmx.resolve("mxSetPr");
    if (!pMxSetPr) throw "CMATLAB::CMATLAB error: unable to resolve libmx for mxSetPr";
    pMxGetNumberOfElements = (mxftype5) libmx.resolve("mxGetNumberOfElements");
    if (!pMxGetNumberOfElements) throw "CMATLAB::CMATLAB error: unable to resolve libmx for mxGetNumberOfElements";


    pEngOpenSingleUse = (engftype1) libeng.resolve("engOpenSingleUse");
    if (!pEngOpenSingleUse) throw "CMATLAB::CMATLAB error: unable to resolve libeng for engOpenSingleUse";
    pEngOpen = (engftype2) libeng.resolve("engOpen");
    if (!pEngOpen) throw "CMATLAB::CMATLAB error: unable to resolve libeng for engOpen";
    pEngClose = (engftype3) libeng.resolve("engClose");
    if (!pEngClose) throw "CMATLAB::CMATLAB error: unable to resolve libeng for engClose";
    pEngGetVisible = (engftype4) libeng.resolve("engGetVisible");
    if (!pEngGetVisible) throw "CMATLAB::CMATLAB error: unable to resolve libeng for engGetVisible";
    pEngSetVisible = (engftype5) libeng.resolve("engSetVisible");
    if (!pEngSetVisible) throw "CMATLAB::CMATLAB error: unable to resolve libeng for engSetVisible";
    pEngEvalString = (engftype6) libeng.resolve("engEvalString");
    if (!pEngEvalString) throw "CMATLAB::CMATLAB error: unable to resolve libeng for engEvalString";
    pEngGetVariable = (engftype7) libeng.resolve("engGetVariable");
    if (!pEngGetVariable) throw "CMATLAB::CMATLAB error: unable to resolve libeng for engGetVariable";
    pEngPutVariable = (engftype8) libeng.resolve("engPutVariable");
    if (!pEngPutVariable) throw "CMATLAB::CMATLAB error: unable to resolve libeng for engPutVariable";
    pEngOutputBuffer = (engftype9) libeng.resolve("engOutputBuffer");
    if (!pEngOutputBuffer) throw "CMATLAB::CMATLAB error: unable to resolve libeng for engOutputBuffer";
}


void CMATLAB::matPut(string filename,const ArrayXd &X,string Xname)
{
    MATFile *pmat=matOpen(filename,string("w"));
    if (!pmat) throw "CMATLAB::exportTo error: matOpen failed";
    mxArray *pa=mxCreateDoubleMatrix((int)X.size(),1);
    if (!pa) throw "CMATLAB::exportTo error: mxCreateDoubleMatrix failed";
    memcpy((void *)(mxGetPr(pa)), (void *)X.data(), X.size()*sizeof(double));
    if (!matPutVariable(pmat,string("X"),pa)) throw "CMATLAB::exportTo error: matlab.matPutVariable failed";
    mxDestroyArray(pa);
    if (!matClose(pmat)) throw "CMATLAB::exportTo error: matlab.matClose failed";

}


void CMATLAB::matGet(string filename,ArrayXd &X,string Xname)
{
    MATFile *pmat=matOpen(filename,string("r"));
    if (!pmat) throw "CMATLAB::importFrom error: matlab.matOpen failed";
    mxArray *pa=matGetVariable(pmat,Xname);
    if (!pa) throw "CMATLAB::importFrom error: matlab.matGetVariable failed";
    int N=mxGetNumberOfElements(pa);
    if (N<=0) throw "CMATLAB::importFrom error: matlab.mxGetNumberOfElements failed";
    X.resize(N);
    memcpy((void *)(mxGetPr(pa)), (void *)X.data(), sizeof((double *)X.data()));
    mxDestroyArray(pa);
    if (!matClose(pmat)) throw "CMATLAB::importFrom error: matlab.matClose failed";
}
