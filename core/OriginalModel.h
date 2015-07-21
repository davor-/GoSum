#ifndef GOSUMORIGINALMODEL_H
#define GOSUMORIGINALMODEL_H

#include <QProcess>
#include <QFile>

#include "Model.h"
#include "MatlabLibrary.h"


//! \brief
//! Namespace for GoSUM model.
namespace GoSUM {


class GoSUM::CModelEvaluator;
//! \brief
//! Class for GoSUM model evaluator.
class CEvaluator  {
public:
    enum evaluatortype {exeascii,exemat,matlabshell,matlabengine};
protected:
    static int trdN; //!< Total number of threads and id of the particular thread this class runs in.
    static enum evaluatortype type; //!< Holds type of the evalautor.
    static std::string path,exe,in,out,ext; //!< Names of the external executable, and its input and output files.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
public:
    CEvaluator() {}
    virtual ~CEvaluator() {}
public:
    static void clear() { trdN=1; type=exeascii; path.clear(); exe.clear(); in.clear(); out.clear(); ext.clear(); } //!< Clears data.
    static int ThreadSize() { return trdN; } //!< Returns thread size.
    static void SetThreadSize(int _trdN) { trdN=_trdN; } //!< Sets thread size.
    static void SetType(GoSUM::CEvaluator::evaluatortype _type); //!< Sets evaluator type.
    static void SetExternalEvaluator(const std::string &_filename); //!< Sets external evaluator.
    static evaluatortype Type(const std::string &_stype); //!< Converts evaluator type name to evaluator type enumerator.
    static evaluatortype Type() { return type; } //!< Returns evalutor type.
    static GoSUM::CModelEvaluator *New(const CInputParameters* _pIP,COutputStates* _pOS,int _trdI=0); //!< Returns new model evalutor.
    static std::string ExternalEvaluator(); //!< Returns full path of the external evalautor.
    static std::string ExternalEvaluatorFolder() { return path; } //!< Returns folder of the external evalautor.
public:
    boost::signal<void()> evaluatingProgressed; //!< Signal for evaluation progress, emitted after single evaluation.
};


//! \brief
//! Class for GoSUM model evaluator.
class CModelEvaluator : public CEvaluator {
protected:
    const CInputParameters* pIP; //!< Pointer to GoSUM input parameters.
    COutputStates* pOS; //!< Pointer to GoSUM output states.
    int trdI,I; //!< Total number of threads and id of the particular thread this class runs in.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
public:
    CModelEvaluator() : pIP(NULL), pOS(NULL), trdI(0), I(0) {}
    CModelEvaluator(const CInputParameters* _pIP,COutputStates* _pOS,int _trdI) : pIP(_pIP), pOS(_pOS), trdI(_trdI), I(0) {}
    virtual ~CModelEvaluator() {}
public:
    virtual void openEvaluation()=0; //!< Opens, i.e. prepares evaluation process.
    virtual ArrayXd operator()(const ArrayXd &X)=0; //!< Returns output state evaluated on a single input parameter n-tuple.
    virtual void operator()(); //!< Thread calls this operator which then evalutes output states from every trdI-th input parameter n-tuple.
    virtual int evaluateOutputsSize() { return 0; } //!< Evaluates outputs size by running model evaluator.
    virtual void closeEvaluation()=0; //!< Closes evaluation process.
};


//! \brief
//! Template for the evaluator with some file i/o and .exe
//! derived from CModelEvaluator.
class CExeEvaluator : public CModelEvaluator  {
protected:
    QProcess *pP; //!< Points to the qprocess.
protected:
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
protected:
    virtual void systemCommand(std::ostringstream &cmd)
    {   cmd<<path<<exe<<" "<<path<<in<<I<<ext<<" "<<path<<out<<I<<ext;  }
    virtual void writeTo(std::string _fname,const ArrayXd &_X,std::string _Xname)=0;
    virtual void readFrom(std::string _fname,ArrayXd &_Y,std::string _Yname)=0;
    virtual int readSizeFrom(std::string _fname,std::string _Yname)=0;
public:
    CExeEvaluator() : CModelEvaluator(), pP(NULL) {}
    CExeEvaluator(const CInputParameters* _pIP,COutputStates* _pOS,int _trdI) : CModelEvaluator(_pIP,_pOS,_trdI), pP(NULL) {}
    virtual ~CExeEvaluator() {}
public:
    virtual void openEvaluation(); //!< Opens, i.e. prepares evaluation process.
    virtual ArrayXd operator()(const ArrayXd &X); //!< Returns output state evaluated on for a single input parameter n-tuple.
    virtual int evaluateOutputsSize(); //!< Evaluates outputs size by running model evaluator.
    virtual void closeEvaluation(); //!< Closes evaluation process.
};


//! \brief
//! Class for the evaluator with ascii i/o and .exe.
class CExeAsciiEvaluator : public CExeEvaluator  {
protected:
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
protected:
    virtual void writeTo(std::string _fname,const ArrayXd &_X,std::string _Xname);
    virtual void readFrom(std::string _fname,ArrayXd &_Y,std::string _Yname);
    virtual int readSizeFrom(std::string _fname,std::string _Yname);
public:
    CExeAsciiEvaluator() : CExeEvaluator() {}
    CExeAsciiEvaluator(const CInputParameters* _pIP,COutputStates* _pOS,int _trdI) : CExeEvaluator(_pIP,_pOS,_trdI) {}
    virtual ~CExeAsciiEvaluator() {}
};


//! \brief
//! Class for the evaluator with Matlab .mat i/o and .exe.
class CExeMatEvaluator : public CExeEvaluator  {
protected:
    CMATLAB matlab; //!< Object for matlab library functions.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
protected:
    virtual void writeTo(std::string _fname,const ArrayXd &_X,std::string _Xname);
    virtual void readFrom(std::string _fname,ArrayXd &_Y,std::string _Yname);
    virtual int readSizeFrom(std::string _fname,std::string _Yname);
public:
    CExeMatEvaluator() : CExeEvaluator() {}
    CExeMatEvaluator(const CInputParameters* _pIP,COutputStates* _pOS,int _trdI) : CExeEvaluator(_pIP,_pOS,_trdI) {}
    virtual ~CExeMatEvaluator() {}
};


//! \brief
//! Class for the evaluator through Matlab engine and .mat i/o.
class CMatlabShellEvaluator : public CExeMatEvaluator  {
protected:
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
protected:
    virtual void systemCommand(std::ostringstream &cmd)
    {   cmd<<"matlab -wait -nosplash -nodesktop -singleCompThread -sd "<<path;
        cmd<<" -r "<<exe<<" -p "<<in<<I<<ext<<" -o "<<out<<I<<ext; }
public:
    CMatlabShellEvaluator() : CExeMatEvaluator() {}
    CMatlabShellEvaluator(const CInputParameters* _pIP,COutputStates* _pOS,int _trdI) : CExeMatEvaluator(_pIP,_pOS,_trdI) {}
    virtual ~CMatlabShellEvaluator() {}
};


//! \brief
//! Class for the evaluator through Matlab engine
//! derived from CModelEvaluator.
class CMatlabEngineEvaluator : public CModelEvaluator  {
protected:
    Engine *ep;  //!< Points to the matlab engine.
    CMATLAB matlab;  //!< Object for matlab library functions.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
public:
    CMatlabEngineEvaluator() : CModelEvaluator(), ep(NULL) {}
    CMatlabEngineEvaluator(const CInputParameters* _pIP,COutputStates* _pOS,int _trdI) : CModelEvaluator(_pIP,_pOS,_trdI), ep(NULL) {}
    virtual ~CMatlabEngineEvaluator() {}
public:
    virtual void openEvaluation(); //!< Opens, i.e. prepares evaluation process.
    virtual ArrayXd operator()(const ArrayXd &X); //!< Returns output state evaluated on a single input parameter n-tuple.
    virtual int evaluateOutputsSize(); //!< Evaluates outputs size by running model evaluator.
    virtual void closeEvaluation(); //!< Closes evaluation process.
};


}


BOOST_CLASS_EXPORT_KEY(GoSUM::CEvaluator)
BOOST_CLASS_EXPORT_KEY(GoSUM::CModelEvaluator)
BOOST_CLASS_EXPORT_KEY(GoSUM::CExeEvaluator)
BOOST_CLASS_EXPORT_KEY(GoSUM::CExeAsciiEvaluator)
BOOST_CLASS_EXPORT_KEY(GoSUM::CExeMatEvaluator)
BOOST_CLASS_EXPORT_KEY(GoSUM::CMatlabShellEvaluator)
BOOST_CLASS_EXPORT_KEY(GoSUM::CMatlabEngineEvaluator)


#endif // GOSUMORIGINALMODEL_H




