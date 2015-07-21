#ifndef GOSUMMODELVARIABLE_H
#define GOSUMMODELVARIABLE_H





#include "Probability.h"


//! \brief
//! Namespace for GoSUM model.
namespace GoSUM {

class CContinuousMV;
class CDiscreteMV;

//! \brief
//! Class for any variable in the GoSUM model (input or output).
class CModelVariable {
protected:
    std::string uname; //!< Unique name of the model variable.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version)
    {   ar & boost::serialization::make_nvp("uname",uname);  }
public:
    CModelVariable() : uname("") {}
    virtual ~CModelVariable() {}
public:
    CModelVariable *clone(std::string _uname); //!< Clones itself to a new model variable with different name.
public:
    void setName(const std::string &_uname) { uname=_uname; }  //!< Set name of the model variable.
    std::string name() const { return uname; } //!< Returns name of the model variable.
    virtual void clearSample()=0; //!< Clears variable's sample.
    virtual int expandedSize() const=0; //! Returns number of variables after expansion for analytical model.
    virtual void setSampleSize(int _n)=0;  //!< Sets sample size.
    virtual int sampleSize() const=0; //!< Returns sample size.
    virtual void setSampleValue(double _val,int _at)=0;  //!< Sets particular sample value.
    virtual double sampleValue(int _at) const=0;  //!< Returns particular sample value.
    virtual void readSampleValue(std::ifstream &_ifs,int _at)=0; //!< Reads particular sample value from input file stream.
    virtual void writeSampleValue(std::ofstream &_ofs,int _at) const=0; //!< Writes particular sample value to output file stream.
    virtual void generateSampleValue(double _ranval,int _at)=0;  //!< Generates particular sample value using actual random variable model.
    virtual double generateSampleValue(double _p)=0;  //!< Generates sample value using actual random variable model.
    virtual void computeStatistics(int _n)=0; //!< Computes statistics from sample (histogram, empirical mean and standard deviation etc.).
    virtual double minValue() const=0; //!< Returns minimal variable value.
    virtual double maxValue() const=0; //!< Returns maximal variable value.
    virtual double expectedValue() const=0; //!< Returns expected variable value.
    virtual double variance() const=0; //!< Returns variance.
    virtual void setEmpiricalDistribution() {}  //!< Computes distribution of the actual random variable from empirical sample data.
    virtual void setTheoreticalDistribution() {} //!< Tests goodness-of-fit for theoretical random variables and replaces with the best that satisfies Kolomogorov-Smirnov.
    virtual void setDistribution() { setTheoreticalDistribution(); setEmpiricalDistribution(); }  //!< Computes distribution of the actual random variable from available data.
    virtual CRandomVariable::distributiontype distributionType() const=0; //!< Returns type of the random variable distribution.
    virtual std::string distributionName() const=0; //!< Returns name of the random variable distribution.
    virtual bool isContinuous() const=0; //!< Returns true if model variable is continuous, false otherwise.
    virtual bool isDiscrete() const=0; //!< Returns true if model variable is discrete, false otherwise.
    virtual bool isDistributionDefined() const=0; //!< Returns true if distribution is defined, false otherwise.
    CContinuousMV *cast2ContinuousMV(); //!< Safe cast of model variable to CContinuousMV type.
    CDiscreteMV *cast2DiscreteMV(); //!< Safe cast of model variable to CDiscreteMV type.
    virtual CRandomVariable *randomVariable() const=0;  //!< Returns random variable.
    virtual CSample *sample() const=0;  //!< Returns sample.
    ArrayXd castExportSample(); //!< Exports model variables sample and casts it to array of doubles.
};


template <class R,class S,typename t>
class TModelVariable : public CModelVariable {
protected:
    R *pRV; //!< Pointer to a random variable that models behaviour of model variable.
    S *pS; //!< Pointer to the sample data of the model variable.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version)
    {   ar & boost::serialization::make_nvp("CModelVariable",boost::serialization::base_object<CModelVariable>(*this));
        ar & boost::serialization::make_nvp("pRandomVariable",pRV);
        ar & boost::serialization::make_nvp("pSample",pS);   }
public:
    TModelVariable() : CModelVariable(), pRV(NULL), pS(NULL) {}
    virtual ~TModelVariable() { if (pS) delete pS; if (pRV) delete pRV; }
public:
    virtual void clearSample() { pS->clear(); } //!< Clears variable's sample.
    virtual int expandedSize() const { return pRV->expandedSize(); } //! Returns number of variables after expansion for analyitical model.
    virtual void setSampleSize(int _n) { pS->setSampleSize(_n); }  //!< Sets sample size.
    virtual int sampleSize() const { return pS->sampleSize(); } //!< Returns sample size.
    virtual void setSampleValue(double _val,int _at) { pS->setSampleValue(_val,_at); }  //!< Sets particular sample value.
    virtual double sampleValue(int _at) const { return pS->sampleValue(_at); } //!< Returns particular sample value.
    virtual void readSampleValue(std::ifstream &_ifs,int _at) { pS->readSampleValue(_ifs,_at); } //!< Reads particular sample value from input file stream.
    virtual void writeSampleValue(std::ofstream &_ofs,int _at) const { pS->writeSampleValue(_ofs,_at); } //!< Writes particular sample value to output file stream.
    virtual void generateSampleValue(double _p,int _at) { pS->setSampleValue(pRV->quantile(_p),_at); } //!< generates particular sample value using actual random variable model.
    virtual double generateSampleValue(double _p) { return pRV->quantile(_p); }  //!< Generates sample value using actual random variable model.
    virtual void computeStatistics(int _n)  {  pS->computeStatistics(_n); } //!< Computes histogram and statistical indicators.
    virtual double minValue() const { return double(pRV->minValue()); } //!< Returns minimal variable value.
    virtual double maxValue() const { return double(pRV->maxValue()); } //!< Returns maximal variable value.
    virtual double expectedValue() const { return double(pRV->expectedValue()); } //!< Returns expected variable value.
    virtual double variance() const { return double(pRV->variance()); } //!< Returns expected variable value.
    virtual CRandomVariable *randomVariable() const { return (CRandomVariable *)pRV; }  //!< Returns random variable.
    virtual CSample *sample() const { return (CSample *)pS; }  //!< Returns sample.
    void setDistributionParameters(t _dp1,t _dp2=0) { pRV->setDistribution(_dp1,_dp2); } //!< Set parameters of the theoretical distribution.
    virtual void setTheoreticalDistribution()=0;  //!< Detects distribution and turns random variable to appropirate type.
    virtual void setEmpiricalDistribution()=0;  //!< Compute distribution of the actual random variable from empirical sample data.
    Array<t,Dynamic,1> exportDomain() const { return pRV->exportDomain(); } //!< Exports probability values for given _x values.
    ArrayXd exportProbability(const Array<t,Dynamic,1> &_x) const { return pRV->exportProbability(_x); } //!< Exports probability values for given _x values.
    ArrayXd exportCumulative(const Array<t,Dynamic,1> &_x) const { return pRV->exportCumulative(_x); } //!< Exports cumulative values for given _x values.
    ArrayXd exportQuantile(const ArrayXd &_p) const { return pRV->exportQuantile(_p); } //!< Exports quantile values for given _x values.
    Array<t,Dynamic,1> exportSample() const { return pS->exportSample(); } //!< Exports sample values.
    virtual CRandomVariable::distributiontype distributionType() const { return pRV->distributionType(); } //!< Returns type of the random variable distribution.
    virtual std::string distributionName() const { return pRV->distributionName(); } //!< Returns name of the random variable distribution.
    t normalizedHistogram(Array<t,Dynamic,1> &_x,ArrayXd &_H) const { return pS->normalizedHistogram(_x,_H); } //!< Returns normalized histogram (x,H).
    t exportHistogram(Array<t,Dynamic,1> &_x,ArrayXd &_H) const { return pS->exportHistogram(_x,_H); } //!< Returns histogram (x,H).
    void exportSubHistogram(ArrayXd &_subH,const std::vector<int> &subset) const { pS->exportSubHistogram(_subH,subset); } //!< Returns subhistogram.
    virtual bool isDistributionDefined() const { return pRV->isDistributionDefined(); } //!< Returns true if distribution is defined, false otherwise.
};


typedef TModelVariable<CDiscreteRV,CDiscreteSample,int> CTDiscreteMV;
class CDiscreteMV : public CTDiscreteMV {
private:
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version)
    {   ar & boost::serialization::make_nvp("CTDiscreteMV",boost::serialization::base_object<CTDiscreteMV>(*this));  }
    CDiscreteMV() : CTDiscreteMV() {}
public:
    CDiscreteMV(const std::string &_uname,CRandomVariable::distributiontype _type); //!< Constructs new model variable by name and type.
    CDiscreteMV(const CDiscreteMV &O);
    CDiscreteMV(const CContinuousMV &O);
public:
    virtual void setTheoreticalDistribution();  //!< Detects constant sample and turns random variable to appropirate type.
    virtual void setEmpiricalDistribution()   //!< Compute distribution of the actual random variable from empirical sample data.
    {  if ( typeid(*pRV)==typeid(CCategoricalDRV) || typeid(*pRV)==typeid(CEmpiricalDRV) ) pRV->setDistribution(*pS);    }
    virtual bool isContinuous() const { return false; } //!< Returns true if model variable is continuous, false otherwise.
    virtual bool isDiscrete() const { return true; } //!< Returns true if model variable is discrete, false otherwise.
};


typedef TModelVariable<CContinuousRV,CContinuousSample,double> CTContinuousMV;
class CContinuousMV : public CTContinuousMV {
private:
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version)
    {   ar & boost::serialization::make_nvp("CTContinuousMV",boost::serialization::base_object<CTContinuousMV>(*this));  }
    CContinuousMV() : CTContinuousMV() {}  //!< Private constructor.
public:
    CContinuousMV(const std::string &_uname,CRandomVariable::distributiontype _type); //!< Constructs new model variable by name and type.
    CContinuousMV(const CContinuousMV &O);
public:
    virtual void setTheoreticalDistribution(); //!< Tests goodness-of-fit for theoretical random variables and replaces with the best that satisfies Kolomogorov-Smirnov.
    virtual void setEmpiricalDistribution()  //!< Compute distribution of the actual random variable from empirical sample data.
    { if ( typeid(*pRV)==typeid(CEmpiricalCRV) )  pRV->setDistribution(*pS); }
    virtual bool isContinuous() const { return true; } //!< Returns true if model variable is continuous, false otherwise.
    virtual bool isDiscrete() const { return false; } //!< Returns true if model variable is discrete, false otherwise.
    bool hasDiscreteData() { return pS->hasDiscreteData(); }
};


} //end of namespace GoSUM

BOOST_CLASS_EXPORT_KEY(GoSUM::CDiscreteMV)
BOOST_CLASS_EXPORT_KEY(GoSUM::CContinuousMV)



#endif // GOSUMMODELVARIABLE_H

