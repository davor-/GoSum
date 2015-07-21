#ifndef SAMPLE_H
#define SAMPLE_H




#include "VariousMath.h"



//! \brief
//! Abstract class for all samples.
class CSample {
protected:
    static int maxdiscretesize;
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version) {}
public:
    CSample() {}
    virtual ~CSample() {}
    CSample(const CSample &O) {}
public:
    virtual void clear()=0; //!< Clears object.
    virtual void setSampleSize(int _n)=0;  //!< (Re)sizes the sample, all previous data is lost.
    virtual int sampleSize() const=0; //!< Returns sample size (number of data in the sample).
    virtual void setSampleValue(double _val,int _at)=0; //!< Sets particular sample data value.
    virtual void readSampleValue(std::ifstream &_ifs,int _at)=0; //!< Reads particular sample data from input file stream.
    virtual void writeSampleValue(std::ofstream &_ofs,int _at) const=0; //!< Writes particular sample data to output file stream.
    virtual void computeStatistics(int _n)=0;  //!< Computes sample statistics, i.e. normalized histogram etc.
    virtual double variance() const=0; //!< Returns sample variance (i.e.empirical).
    virtual std::vector<int> select(double _left,double _right) const=0; //!< Returns vector of indices of sample values selected if they are in the interval  [_left,_right].
    virtual void eraseSelected(const std::vector<int> &sel)=0; //!< Erases sample values on positions defiend by sel.
};


//! \brief
//! Template of an abstract class - covers two types of samples:
//! discrete (t=int) or continuous (t=double).
template <typename t>
class TSample : public CSample {
protected:
    Array<t,Dynamic,1> sample;  //!< Array that holds sample data.
    Array<t,Dynamic,1> x; //!< Array that holds x-coordinate of the normalized histogram.
    ArrayXd H,cH; //!< Array that holds normalized histogram.
    t range; //!< Range of the sample data.
    double mu,stdv; //!< Empirical mean (mu) and standard deviation (std).
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version)
    {   ar & boost::serialization::make_nvp("CSample",boost::serialization::base_object<CSample>(*this));
        ar & boost::serialization::make_nvp("sample",sample);
        ar & boost::serialization::make_nvp("x",x);
        ar & boost::serialization::make_nvp("H",H);
        ar & boost::serialization::make_nvp("cH",cH);
        ar & boost::serialization::make_nvp("range",range);
        ar & boost::serialization::make_nvp("mu",mu);
        ar & boost::serialization::make_nvp("stdv",stdv);
    }
public:
    TSample() : range(0), mu(0.), stdv(1.) {}
    virtual ~TSample() {}
    TSample(const TSample &O) : CSample(O), sample(O.sample), x(O.x), H(O.H), cH(O.cH), range(O.range), mu(O.mu), stdv(O.stdv) {}
public:
    virtual void computeNormalizedHistogram(int _n)=0; //!< Computes normalized histogram from sample data.
    virtual void computeNormalizedSubHistogram(ArrayXd &_subH,const std::vector<int> &subset) const=0; //!< Computes normalized histogram from a subset of sample data.
    void clear()  { range=0; H.resize(0); cH.resize(0); x.resize(0); sample.resize(0);  } //!< Clears object.
    virtual void setSampleSize(int _n) { sample.setZero(_n); } //!< (Re)sizes the sample, all previous data is lost.
    virtual int sampleSize() const { return int(sample.size()); } //!< Returns sample size (number of data in the sample).
    virtual void setSampleValue(double _val,int _at) { sample(_at)=t(_val); } //!< Sets particular sample data value.
    t sampleValue(int _at) const { return sample(_at); }  //!< Returns particular sample data value.
    virtual void readSampleValue(std::ifstream &_ifs,int _at)  //!< Reads particular sample data from input file stream.
    {  _ifs>>sample(_at); if (_ifs.fail()) throw "TSample::readSampleValue error: std::ifstream fail"; }
    virtual void writeSampleValue(std::ofstream &_ofs,int _at) const  //!< Writes particular sample data to output file stream.
    {  _ofs<<sample(_at)<<"\t"; if (_ofs.fail()) throw "TSample::writeSampleValue error: std::ofstream fail"; }
    bool isConstant() const { return (range==0.); } //!< Returns true if it is a constant sample, false otherwise.
    t minValue() const { return x(0); } //!< Returns minimal sample data value.
    t maxValue() const { return x(x.size()-1); } //!< Returns maximal sample data value.
    virtual double mean() const { return mu; } //!< Returns empirical mean.
    virtual double standardDeviation() const { return stdv; } //!< Returns empirical standard deviation.
    t normalizedHistogram(Array<t,Dynamic,1> &_x,ArrayXd &_H) const { _x=x; _H=H; return range; } //!< Returns normalized histogram (x,H).
    void cummulativeHistogram(Array<t,Dynamic,1> &_x,ArrayXd &_cH) const { _x=x; _cH=cH; } //!< Returns cummulative histogram (x,cH).
    virtual t exportHistogram(Array<t,Dynamic,1> &_x,ArrayXd &_H) const {
        if ( x.size()==0 )     { _x.resize(0); _H.resize(0); return 0.; }
        else if ( range==0. )  { _x.resize(1); _H.resize(1); _x(0)=x(0); _H(0)=1; }
        else                   { _x=x; _H=H*(double(H.size()-1)/range); }
        return range;
    } //!< Returns histogram (x,H).
    virtual void exportSubHistogram(ArrayXd &_subH,const std::vector<int> &subset) const=0; //!< Exports histogram relative to a subset of sample data.
    Array<t,Dynamic,1> exportSample() const { return sample; }  //!< Exports sample.
    virtual std::vector<int> select(double _left,double _right) const  //!< Returns indices of those sample values that are between _left and _right.
    {   std::vector<int> sel;
        for ( int i=0; i<sample.size(); i++ )
            if ( _left<=double(sample(i)) && double(sample(i))<=_right )  sel.push_back(i);
        return sel;  }
    virtual void eraseSelected(const std::vector<int> &sel) //!< Erases sample values on positions defiend by sel.
    {   std::vector<t> newS;
        for ( int i=0; i<sample.size(); i++ )
            if ( find(sel.begin(),sel.end(),i)==sel.end() )  newS.push_back(sample(i));
        sample.resize(newS.size());
        for ( int i=0; i<sample.size(); i++ )  sample(i)=newS[i]; }
};


//! \brief
//! Abstract class for all discrete samples
//! derived as specialization with t=int.
typedef  TSample<int> CTDiscreteSample;
class CDiscreteSample : public CTDiscreteSample {
protected:
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
public:
    CDiscreteSample()   {}
    virtual ~CDiscreteSample() {}
    CDiscreteSample(const CDiscreteSample &O) : CTDiscreteSample(O) {}
public:
    virtual void computeNormalizedHistogram(int _n); //!< C.omputes normalized histogram from sample data
    virtual void computeNormalizedSubHistogram(ArrayXd &_subH,const std::vector<int> &subset) const; //!< Computes normalized histogram from a subset of sample data.
    virtual void computeStatistics(int _n) { computeNormalizedHistogram(_n);  }  //!< Computes sample statistics, i.e. normalized histogram etc.
    virtual void exportSubHistogram(ArrayXd &_subH,const std::vector<int> &subset) const; //!< Exports histogram relative to a subset of sample data.
    virtual double variance() const { return 1.; } //!< Returns sample variance (i.e.empirical).
};


//! \brief
//! Abstract class for all continuous samples
//! derived as specialization with t=double.
typedef  TSample<double> CTContinuousSample;
class CContinuousSample : public CTContinuousSample {
protected:
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
public:
    CContinuousSample() {}
    virtual ~CContinuousSample() {}
    CContinuousSample(const CContinuousSample &O) : CTContinuousSample(O) {}
public:
    virtual void computeNormalizedHistogram(int _n); //!< Computes normalized histogram from sample data.
    virtual void computeNormalizedSubHistogram(ArrayXd &_subH,const std::vector<int> &subset) const; //!< Computes normalized histogram from a subset of sample data.
    virtual void computeMeanAndStandardDeviation(); //!< Computes empirical mean and standard deviation.
    virtual void computeStatistics(int _n) { computeNormalizedHistogram(_n); computeMeanAndStandardDeviation(); }  //!< Computes sample statistics, i.e. normalized histogram etc.
    virtual void exportSubHistogram(ArrayXd &_subH,const std::vector<int> &subset) const; //!< Exports histogram relative to a subset of sample data.
    virtual double variance() const { return stdv*stdv; } //!< Returns sample variance (i.e.empirical).
    bool hasDiscreteData();
};


//! \brief
//! Abstract class for numerical discrete samples.
class CNumericalSample : public CDiscreteSample {
private:
    std::vector<double> values; //!< Holds all numerical values.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
public:
    CNumericalSample() : values(0) {}
    CNumericalSample(const CNumericalSample &O) : CDiscreteSample(O), values(O.values) {}
    CNumericalSample(const CContinuousSample &O);
    virtual ~CNumericalSample() {}
public:
    virtual void computeMeanAndStandardDeviation(); //!< Computes empirical mean and standard deviation.
    virtual void computeStatistics(int _n) { computeNormalizedHistogram(_n); computeMeanAndStandardDeviation(); }  //!< Computes sample statistics, i.e. normalized histogram etc.
    virtual void readSampleValue(std::ifstream &_ifs,int _at);  //!< Reads particular sample data from input file stream.
    virtual void writeSampleValue(std::ofstream &_ofs,int _at) const //!< Writes particular sample data to output file stream.
    {  _ofs<<values[sample(_at)]<<"\t"; if (_ofs.fail()) throw "CNumericalSample::writeSampleValue error: std::ofstream fail"; }
    virtual void setSampleSize(int _n) { sample.setZero(_n); values.clear(); } //!< Sets particular sample data value.
    int minValue() const { return 0; } //!< Returns minimal sample data value.
    int maxValue() const { return 1; } //!< Returns maximal sample data value.
    virtual double variance() const { return 1.; } //!< Returns sample variance (i.e.empirical).
};


//! \brief
//! Abstract class for categorical discrete samples.
class CCategoricalSample : public CDiscreteSample {
private:
    std::vector<std::string> categories; //!< Holds all category values.
    //! Boost serialization.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
public:
    CCategoricalSample() : categories(0) {}
    CCategoricalSample(const CCategoricalSample &O) : CDiscreteSample(O), categories(O.categories) {}
    virtual ~CCategoricalSample() {}
public:
    virtual void readSampleValue(std::ifstream &_ifs,int _at);  //!< Reads particular sample data from input file stream.
    virtual void writeSampleValue(std::ofstream &_ofs,int _at) const //!< Writes particular sample data to output file stream.
    {  _ofs<<categories[sample(_at)]<<"\t"; if (_ofs.fail()) throw "CCategoricalSample::writeSampleValue error: std::ofstream fail"; }
    virtual void setSampleSize(int _n) { sample.setZero(_n); categories.clear(); } //!< Sets particular sample data value.
    int minValue() const { return 0; } //!< Returns minimal sample data value.
    int maxValue() const { return 1; } //!< Returns maximal sample data value.
    virtual double variance() const { return 1.; } //!< Returns sample variance (i.e.empirical).
};


BOOST_CLASS_EXPORT_KEY(CDiscreteSample)
BOOST_CLASS_EXPORT_KEY(CContinuousSample)
BOOST_CLASS_EXPORT_KEY(CNumericalSample)
BOOST_CLASS_EXPORT_KEY(CCategoricalSample)


#endif // SAMPLE_H
