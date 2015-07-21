


#include "OriginalModel.h"
#include <QFileInfo>


int GoSUM::CEvaluator::trdN=1;
enum GoSUM::CEvaluator::evaluatortype GoSUM::CEvaluator::type;
std::string GoSUM::CEvaluator::path,GoSUM::CEvaluator::exe,GoSUM::CEvaluator::in,GoSUM::CEvaluator::out,GoSUM::CEvaluator::ext;


BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CEvaluator)
BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CModelEvaluator)
BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CExeEvaluator)
BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CExeAsciiEvaluator)
BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CExeMatEvaluator)
BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CMatlabShellEvaluator)
BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CMatlabEngineEvaluator)


template <class Archive>
void GoSUM::CEvaluator::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("trdN",trdN);
    ar & boost::serialization::make_nvp("type",type);
    ar & boost::serialization::make_nvp("path",path);
    ar & boost::serialization::make_nvp("exe",exe);
    ar & boost::serialization::make_nvp("in",in);
    ar & boost::serialization::make_nvp("out",out);
    ar & boost::serialization::make_nvp("ext",ext);
}


template <class Archive>
void GoSUM::CModelEvaluator::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CEvaluator",boost::serialization::base_object<CEvaluator>(*this));
    ar & boost::serialization::make_nvp("pIP",pIP);
    ar & boost::serialization::make_nvp("pOS",pOS);
    ar & boost::serialization::make_nvp("trdI",trdI);
    ar & boost::serialization::make_nvp("I",I);
}


template <class Archive>
void GoSUM::CExeEvaluator::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CModelEvaluator",boost::serialization::base_object<CModelEvaluator>(*this));
}


template <class Archive>
void GoSUM::CExeAsciiEvaluator::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CExeEvaluator",boost::serialization::base_object<CExeEvaluator>(*this));
}


template <class Archive>
void GoSUM::CExeMatEvaluator::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CExeEvaluator",boost::serialization::base_object<CExeEvaluator>(*this));
}


template <class Archive>
void GoSUM::CMatlabShellEvaluator::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CExeMatEvaluator",boost::serialization::base_object<CExeMatEvaluator>(*this));
}


template <class Archive>
void GoSUM::CMatlabEngineEvaluator::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("CModelEvaluator",boost::serialization::base_object<CModelEvaluator>(*this));
}


GoSUM::CEvaluator::evaluatortype GoSUM::CEvaluator::Type(const std::string &_stype)
{
    if ( _stype=="exeascii" )            return GoSUM::CEvaluator::exeascii;
    else if ( _stype=="exemat" )         return GoSUM::CEvaluator::exemat;
    else if ( _stype=="matlabshell" )    return GoSUM::CEvaluator::matlabshell;
    else if ( _stype=="matlabengine" )   return GoSUM::CEvaluator::matlabengine;
    else  throw "GoSUM::CModelEvaluator::Type error: unrecognised type";
}


GoSUM::CModelEvaluator *GoSUM::CEvaluator::New(const CInputParameters* _pIP,COutputStates* _pOS,int _trdI)
{
    if ( type==exeascii )            return new GoSUM::CExeAsciiEvaluator(_pIP,_pOS,_trdI);
    else if ( type==exemat )         return new GoSUM::CExeMatEvaluator(_pIP,_pOS,_trdI);
    else if ( type==matlabshell )    return new GoSUM::CMatlabShellEvaluator(_pIP,_pOS,_trdI);
    else if ( type==matlabengine )   return new GoSUM::CMatlabEngineEvaluator(_pIP,_pOS,_trdI);
    else  throw "GoSUM::CModelEvaluator::New error: unrecognised type";
}


void GoSUM::CEvaluator::SetType(GoSUM::CEvaluator::evaluatortype _type)
{
    type=_type;
    if ( type==GoSUM::CModelEvaluator::exeascii )             ext=".txt";
    else if ( type==GoSUM::CModelEvaluator::exemat ||
              type==GoSUM::CModelEvaluator::matlabshell ||
              type==GoSUM::CModelEvaluator::matlabengine )    ext=".mat";
    else throw "GoSUM::CModelEvaluator::Set error: bad model evaluator type";
}


void GoSUM::CEvaluator::SetExternalEvaluator(const std::string &_filename)
{
    QFileInfo finfo(_filename.c_str());

    path=finfo.path().replace('/','\\').toStdString()+"\\";
    exe=finfo.fileName().replace('/','\\').toStdString();
    std::string noextexe=finfo.baseName().replace('/','\\').toStdString();
    if ( type==GoSUM::CModelEvaluator::matlabengine ) exe=noextexe;
    in=noextexe+string("in");
    out=noextexe+string("out");
}


std::string GoSUM::CEvaluator::ExternalEvaluator()
{
    if ( type==GoSUM::CModelEvaluator::matlabengine ) return path+exe+".m";
    else return path+exe;
}


void GoSUM::CModelEvaluator::operator()()
{
    openEvaluation();

    int N=pIP->sampleSize();
    while ( I<N )
    {   if ( I%trdN==trdI )   pOS->setNTuple((*this)(pIP->nTuple(I)),I-1);
        else I++; }

    closeEvaluation();
}


void GoSUM::CExeEvaluator::openEvaluation()
{
    pP=new QProcess();
    if (!pP) throw "TExeEvaluator::openEvaluation error: unsuccessful QProcess start";
    I=0;
}


ArrayXd GoSUM::CExeEvaluator::operator()(const ArrayXd &X)
{
    std::ostringstream inname; inname<<path<<in<<I<<ext;
    std::ostringstream outname;  outname<<path<<out<<I<<ext;
    std::ostringstream syscmd; systemCommand(syscmd);
    writeTo(inname.str(),X,string("X"));
    pP->start(syscmd.str().c_str());
    pP->waitForFinished(-1);
    ArrayXd Y(pOS->size());
    readFrom(outname.str(),Y,string("Y"));
    I++;
//    QFile::remove(inname.str().c_str());
//    QFile::remove(outname.str().c_str());

    evaluatingProgressed();

    return Y;
}


int GoSUM::CExeEvaluator::evaluateOutputsSize()
{
    ArrayXd X=pIP->expectedValue();

    std::ostringstream inname; inname<<path<<in<<I<<ext;
    std::ostringstream outname;  outname<<path<<out<<I<<ext;
    std::ostringstream syscmd; systemCommand(syscmd);

    writeTo(inname.str(),X,string("X"));
    pP->start(syscmd.str().c_str());
    pP->waitForFinished(-1);
    int OSize=readSizeFrom(outname.str(),string("Y"));
    I++;
    QFile::remove(inname.str().c_str());
    QFile::remove(outname.str().c_str());

    return OSize;
}


void GoSUM::CExeEvaluator::closeEvaluation()
{
    if (pP) { delete pP; pP=NULL; }
    I=0;
}


void GoSUM::CExeAsciiEvaluator::writeTo(std::string _fname,const ArrayXd &_X,std::string _Xname)
{
    std::ofstream ofs(_fname.c_str());
    if ( ofs.fail() )  throw "GoSUM::CExeAsciiEvaluator::writeTo error: std::ofstream constructor fail";
    int i,n=_X.size();
    for ( i=0; i<n; i++ )
    {  ofs<<_X(i);  if (ofs.fail()) throw "GoSUM::CExeAsciiEvaluator::writeTo error: std::ofstream fail";
       ofs<<endl; }
    ofs.close();
}


int GoSUM::CExeAsciiEvaluator::readSizeFrom(std::string _fname,std::string _Yname)
{
    std::ifstream ifs(_fname.c_str());
    if ( !ifs.is_open() )   throw "GoSUM::CExeAsciiEvaluator::readFrom error: std::ifstream constructor fail";

    double d;
    int OSize=0;
    while ( ifs>>d, !ifs.fail() ) OSize++;
    ifs.close();

    return OSize;
}


void GoSUM::CExeAsciiEvaluator::readFrom(std::string _fname,ArrayXd &_Y,std::string _Yname)
{
    std::ifstream ifs(_fname.c_str());
    if ( !ifs.is_open() )  throw "GoSUM::CExeAsciiEvaluator::readFrom error: std::ifstream constructor fail";
    int i,n=_Y.size();
    for ( i=0; i<n; i++ )
    { ifs>>_Y(i);
        if (ifs.fail())  throw "GoSUM::CExeAsciiEvaluator::readFrom error: std::ifstream fail";  }
    ifs.close();
}


void GoSUM::CExeMatEvaluator::writeTo(std::string _fname,const ArrayXd &_X,std::string _Xname)
{
    matlab.matPut(_fname,_X,_Xname);
}


void GoSUM::CExeMatEvaluator::readFrom(std::string _fname,ArrayXd &_Y,std::string _Yname)
{
    matlab.matGet(_fname,_Y,_Yname);
}


int GoSUM::CExeMatEvaluator::readSizeFrom(std::string _fname,std::string _Yname)
{
    ArrayXd _Y;
    matlab.matGet(_fname,_Y,_Yname);
    return _Y.size();
}


void GoSUM::CMatlabEngineEvaluator::openEvaluation()
{
    ep=matlab.engOpenSingleUse("\0");
    if (!ep) throw "GoSUM::CMatlabEngineEvaluator::openEvaluation error: engine open single use fail";
    matlab.engSetVisible(ep,false);
    std::ostringstream cdpath; cdpath<<"cd "<<path;
    matlab.engEvalString(ep,cdpath.str());

    I=0;
}


ArrayXd GoSUM::CMatlabEngineEvaluator::operator()(const ArrayXd &X)
{
    ArrayXd Y(pOS->size());

    mxArray *pa=matlab.mxCreateDoubleMatrix(1,pIP->size());
    if(!pa)  throw "GoSUM::CMatlabEngineEvaluator error: pa is NULL";
    memcpy((void *)(matlab.mxGetPr(pa)),(void *)X.data(),pIP->size()*sizeof(double));
    matlab.engPutVariable(ep,string("p"),pa);
    matlab.mxDestroyArray(pa);

    std::string scmd;
    scmd="out="+exe+"(p)";
    int stat=matlab.engEvalString(ep,scmd.c_str());

    if(stat!=0)  throw "GoSUM::CMatlabEngineEvaluator error: engEvalStrig returns error";

    mxArray *pb=matlab.engGetVariable(ep,string("out"));
    if(!pb)  throw "GoSUM::CMatlabEngineEvaluator error: pb is NULL";
    if(!matlab.mxGetPr(pb))  throw "GoSUM::CMatlabEngineEvaluator error: mxGetPr on pb is NULL";
    memcpy((void *)Y.data(),(void *)(matlab.mxGetPr(pb)), pOS->size()*sizeof(double));
    matlab.mxDestroyArray(pb);

    I++;

    evaluatingProgressed();

    return Y;
}


int GoSUM::CMatlabEngineEvaluator::evaluateOutputsSize()
{
    int OSize=0;

    ArrayXd X=pIP->expectedValue();
    mxArray *pa=matlab.mxCreateDoubleMatrix(1,pIP->size());
    if(!pa)  throw "GoSUM::CMatlabEngineEvaluator error: pa is NULL";
    memcpy((void *)(matlab.mxGetPr(pa)),(void *)X.data(),pIP->size()*sizeof(double));
    matlab.engPutVariable(ep,string("p"),pa);
    matlab.mxDestroyArray(pa);
    std::string scmd;
    scmd="out="+exe+"(p)";
    int stat=matlab.engEvalString(ep,scmd.c_str());

    if(stat!=0)  throw "GoSUM::CMatlabEngineEvaluator error: engEvalStrig returns error";

    mxArray *pb=matlab.engGetVariable(ep,string("out"));
    if(!pb)  throw "GoSUM::CMatlabEngineEvaluator error: pb is NULL";
    OSize=matlab.mxGetNumberOfElements(pb);
    matlab.mxDestroyArray(pb);

    I++;
    return OSize;
}


void GoSUM::CMatlabEngineEvaluator::closeEvaluation()
{
    if (ep) { matlab.engClose(ep); ep=NULL; }
    I=0;
}
