#ifndef GOSUMSENSITIVITYANALYSIS_H
#define GOSUMSENSITIVITYANALYSIS_H



#include "AnalyticalModel.h"



//! \brief
//! Namespace for GoSUM model.
namespace GoSUM {


//! \brief
//! Class for sensitivity analyis of the model.
class CSensitivityAnalysis {
private:
    CInputParameters *pIP; //!< Points to input parameters.
    COutputStates *pOS; //!< Points to output states.
    CAnalyticalModel *pAM; //!< Points to the analytical model.
    int RSsize; //!< Resized sample size.
    double eps1; //!< Sensitivity error.
    double eps2; //!< Separability error.
    double eps3; //!< Coupling error.
    std::vector<ArrayXd> sample1;
    std::vector<ArrayXd> sample2;
    ArrayXXd y;
    ArrayXd yvar, ymu;
    ArrayXXd si1der; //!< Holds derivative sensitivity (L2) for all output states (rows) over all input parameters (columns).
    ArrayXXd ader; //!< Holds average derivative for all output states (rows) over all input parameters (columns).
    ArrayXXd aader; //!< Holds absolute average derivative (L1) for all output states (rows) over all input parameters (columns).
    ArrayXXd siT; //!< Holds variance sensitivity (L2) for all output states (rows) over all input parameters (columns).
    ArrayXXd si1; //!< Holds first order ANOVA sensitivity for all output states (rows) over all input parameters (columns).
    std::vector<std::vector<std::pair<std::pair<int,int>,double> > > si2; //!< Holds second order ANOVA sensitivity for all output states over all input parameter pairs.
    friend class boost::serialization::access;
    template <class Archive>
    void serialize (Archive &ar,const unsigned int version);
private:
    CSensitivityAnalysis() : pIP(NULL), pOS(NULL), pAM(NULL), sample1(0), sample2(0)  {}
public:
    CSensitivityAnalysis(CInputParameters *_pIP,COutputStates *_pOS,CAnalyticalModel *_pAM) : pIP(_pIP), pOS(_pOS), pAM(_pAM),
        RSsize(10000), eps1(0.005), eps2(0.01), eps3(0.01), sample1(0), sample2(0)  { }
    virtual ~CSensitivityAnalysis() {}
public:
    void clear() //!< Clears data.
    { sample1.resize(0);  sample2.resize(0); y.resize(0,0); yvar.resize(0); ymu.resize(0);
        ader.resize(0,0); aader.resize(0,0); si1der.resize(0,0); si1.resize(0,0);  siT.resize(0,0); si2.clear(); }
    void setSensitivityOptions(int _RSsize, double _eps1, double _eps2, double _eps3) //!< Sets options for sensitivity analysis.
    { RSsize=_RSsize; eps1=_eps1; eps2=_eps2; eps3=_eps3; }
    int resampleSize() const { return RSsize; } //!< Returns resample size.
    void setResampleSize(int _RSsize) { RSsize=_RSsize; } //!< Sets resample size.
    double sensitivityError() const { return  eps1; } //!< Returns sensitivity error.
    void setSensitivityError(double _eps1) { eps1=_eps1; } //!< Sets sensitivity error.
    double separabilityError() const { return  eps2; } //!< Returns separability error.
    void setSeparabilityError(double _eps2) { eps2=_eps2; } //!< Sets separability error.
    double couplingError() const { return  eps3; } //!< Returns coupling error.
    void setCouplingError(double _eps3) { eps3=_eps3; } //!< Sets coupling error.
    template <class E> //!< Prepares sensitivity analysis computation.
    void prepare(E &_evaluator) {
        int i,Osize=pOS->size();

        y.setZero(Osize,RSsize);
        ymu.setZero(Osize);
        yvar.setZero(Osize);

        CHypercube::hctype oldhctype=CHypercube::Type();
        CHypercube::SetType(CHypercube::dsample);
        int oldrssize=pIP->resampleSize();
        pIP->setResampleSize(RSsize);
        pIP->generateSamples(sample1);
        pIP->setResampleSize(oldrssize);
        CHypercube::SetType(oldhctype);

        computingProgressed();

        for ( i=0; i<RSsize; i++) {
            y.col(i)=_evaluator.evaluate(sample1[i]);
            computingProgressed();
        }


        ArrayXXd y2(Osize,RSsize);
        y2=y.square();
        ymu=y.rowwise().sum();
        yvar=y2.rowwise().sum();
        ymu/=RSsize; yvar/=RSsize;
        yvar-=ymu.square();

        computingProgressed();
    }

    void computeDerivativeSensitivities();  //!< Computes derivative sensitivities.

    template <class E>
    void computeVarianceSensitivities(E &_evaluator) {  //!< Computes variance sensitivities.
        int i,j,k,Isize=pIP->size(),EIsize=pIP->expandedSize(),Osize=pOS->size();

        si1.setZero(Osize,EIsize);
        siT.setZero(Osize,EIsize);

        ArrayXd x1(Isize), x2(Isize);
        ArrayXd y1(Osize), y2(Osize);
        ArrayXXd ymu1, ymu2, yvar1, yvar2;
        ymu1.setZero(Osize,EIsize);
        ymu2.setZero(Osize,EIsize);
        yvar1.setZero(Osize,EIsize);
        yvar2.setZero(Osize,EIsize);

        ArrayXXi perm = permutations(Isize,RSsize);

        for ( i=0; i<RSsize; i++)
        {
            x2=sample1[perm(0,i)];
            x1=sample1[i];
            double temp;
            ArrayXd diff;
            diff.setZero(Osize);

            for ( j=0; j<Isize; j++ )
            {
                temp=x2(j);
                x2(j)=x1(j); // resampled point with all parameter changed except xj (xj fixed)
                y1=_evaluator.evaluate(x2);
                ymu1.col(j)+=y1;
                yvar1.col(j)+=y1.square();
                x2(j)=temp;
                diff=y.col(i)-y1;
                si1.col(j)+=diff.square();

                temp=x1(j);
                x1(j)=x2(j); // resampled point with changed parameter xj
                y2=_evaluator.evaluate(x1);
                ymu2.col(j)+=y2;
                yvar2.col(j)+=y2.square();
                x1(j)=temp;
                diff=y.col(i)-y2;
                siT.col(j)+=diff.square();
            }

            computingProgressed();
        }
        ymu1/=RSsize;  ymu2/=RSsize;
        yvar1/=RSsize; yvar2/=RSsize;
        yvar1-=ymu1.square();
        yvar2-=ymu2.square();

        si1/=RSsize; siT/=RSsize;

        for (k=0; k<Osize; k++)
        {   if (sqrt(yvar(k))<TINY)
            { si1.row(k).setZero(); siT.row(k).setZero();}
            else
            {   for (j=0; j<EIsize; j++)
                { si1(k,j)=std::max(0.,1.-si1(k,j)/(yvar(k)+yvar1(k,j)));
                  siT(k,j)=siT(k,j)/(yvar(k)+yvar2(k,j)); }
            }

            computingProgressed();
        }

        // 1# look at the outputs which are not separable with respect to parameters
        std::vector<int> sout; // separabile outputs
        std::vector<int> nsout; // nonseparabile outputs
        ArrayXd ssum=siT.rowwise().sum();
        for (k=0; k<Osize; k++)
        {
            if (ssum(k)<1.+eps2) sout.push_back(k);
            else nsout.push_back(k);
            computingProgressed();
        }

        // 2# for the set of nonseparabile (ino) outputs look for the parameters whick can give 2nd order effects
        int Onsize=int(nsout.size());
        std::vector<std::vector<int> > inputs2(Onsize);

        si2.clear();
        si2.resize(Osize);
        std::pair<std::pair<int,int>, double> sind2; // 2nd order sensitivity indexes <int,int> belongs to indexes and double to Sind value


        for (k=0; k<Onsize; k++)
        {
            int k2=nsout[k];
            for (j=0; j<EIsize; j++)
            {
                if ((siT(k2,j)>eps1) && siT(k2,j)-si1(k2,j)>eps3)  //ind which can give 2nd order effects in output k2
                { inputs2[k].push_back(j); }
            }

            int I2size=int(inputs2[k].size());
            for (i=0; i<I2size; i++)
            {   for (j=i+1; j<I2size; j++)
                {
                    int j1=inputs2[k][i], j2=inputs2[k][j];

                    double y1d, ymu12=0., yvar12=0., diff=0.;
                    double s12;

                    for (int m=0; m<RSsize; m++)
                    {   x2=sample1[perm(0,m)]; // resampled point with all parameters changed except xi1 and xi2
                        x2(j1)=sample1[m](j1);
                        x2(j2)=sample1[m](j2);
                        y1d=E.evaluate(x2)(k2);

                        ymu12+=y1d;
                        yvar12+=y1d*y1d;
                        diff+=(y(k2,m)-y1d)*(y(k2,m)-y1d);
                     }
                     ymu12/=RSsize;
                     yvar12/=RSsize;
                     yvar12-=ymu12*ymu12;

                     s12=(sqrt(yvar(k2))<TINY)? 0.: std::max(0.,1.-diff/(RSsize*(yvar(k2)+yvar12))-si1(k2,j1)-si1(k2,j2));

                     if (s12>eps1)
                     {
                        sind2.first.first=j1;
                        sind2.first.second=j2;
                        sind2.second=s12;
                        si2[k2].push_back(sind2);
                     }
                }
            }
            computingProgressed();
        }

        for (k=0; k<int(sout.size()); k++)
        {
            int k2=sout[k];
            si1.row(k2)=siT.row(k2);
            computingProgressed();
        }
    }

    template <class E>
    void computeVarianceSensitivities1(E &_evaluator) {  //!< Computes variance sensitivities (Homma & Saltelli approach).
        int i,j,k,Isize=pIP->size(),EIsize=pIP->expandedSize(),Osize=pOS->size();

        si1.setZero(Osize,EIsize);
        siT.setZero(Osize,EIsize);

        ArrayXd x1(Isize), x2(Isize);
        ArrayXd y1(Osize), y2(Osize);
        ArrayXd diff;
        diff.setZero(Osize);
        ArrayXXd ymu1, ymu2,yvar1, yvar2;
        ymu1.setZero(Osize,EIsize);
        ymu2.setZero(Osize,EIsize);
        yvar1.setZero(Osize,EIsize);
        yvar2.setZero(Osize,EIsize);

        ArrayXXi perm = permutations(1,RSsize);

        for ( i=0; i<RSsize; i++)
        {
            x1=sample1[i];
            x2=sample1[perm(0,i)];
            double temp;

            for ( j=0; j<Isize; j++ )
            {
                temp=x2(j);
                x2(j)=x1(j);  // resampled point with all parameter changed except xj (xj fixed)
                y1=_evaluator.evaluate(x2);
                x2(j)=temp;
                y2=_evaluator.evaluate(x2);
                si1.col(j)+=(y.col(i)*(y1-y2));

                temp=x1(j);
                x1(j)=x2(j); // resampled point with changed parameter xj
                y2=_evaluator.evaluate(x1);
                x1(j)=temp;
                diff=y.col(i)-y2;
                siT.col(j)+=diff.square();
            }
            computingProgressed();
        }
        si1/=RSsize; siT/=RSsize;

        for (k=0; k<Osize; k++)
        {
            if (sqrt(yvar(k))<TINY)
            {  si1.row(k).setZero(); siT.row(k).setZero();}
            else
            {    for (j=0; j<EIsize; j++)
                 {  si1(k,j)=std::max(0.,si1(k,j)/yvar(k)); }
                 siT.row(k)/=(2.*yvar(k));
            }
            computingProgressed();
        }

        // 1# look at the outputs which are not separable with respect to parameters
        std::vector<int> sout; // separabile outputs
        std::vector<int> nsout; // nonseparabile outputs
        ArrayXd ssum=siT.rowwise().sum();
        for (k=0; k<Osize; k++)
        {
            if (ssum(k)<1.+eps2) sout.push_back(k);
            else nsout.push_back(k);
        }

        // 2# for the set of nonseparabile (ino) outputs look for the parameters whick can give 2nd order effects
        int Onsize=int(nsout.size());
        std::vector<std::vector<int> > inputs2(Onsize);

        si2.clear();
        si2.resize(Osize);
        std::pair<std::pair<int,int>, double> sind2; // 2nd order sensitivity indexes <int,int> belongs to indexes and double to Sind value

        for (k=0; k<Onsize; k++)
        {
            int k2=nsout[k];
            for (j=0; j<EIsize; j++)
            {
                if ((siT(k2,j)>eps1) && (siT(k2,j)-si1(k2,j))>eps3)  //ind which can give 2nd order effects in output k2
                { inputs2[k].push_back(j); }
            }

            int I2size=int(inputs2[k].size());
            for (i=0; i<I2size; i++)
            {   for (j=i+1; j<I2size; j++)
                {
                    int j1=inputs2[k][i], j2=inputs2[k][j];
                    double y1d, y2d,sum=0.;
                    double s12;
                    double temp1, temp2;

                    for (int m=0; m<RSsize; m++)
                    {
                        x2=sample1[perm(0,m)];
                        temp1=x2(j1); temp2=x2(j2);
                        x2(j1)=sample1[m](j1);
                        x2(j2)=sample1[m](j2);
                        y1d=_evaluator.evaluate(x2)(k2);

                        x2(j1)=temp1; x2(j2)=temp2;
                        y2d=_evaluator.evaluate(x2)(k2);
                        sum+=y(k2,m)*(y1d-y2d);
                     }
                     sum/=RSsize;
                     s12=(sqrt(yvar(k2))<TINY)? 0.:std::max(0.,sum/yvar(k2)-si1(k2,j1)-si1(k2,j2));
                     if (s12>eps1)
                     {
                        sind2.first.first=j1;
                        sind2.first.second=j2;
                        sind2.second=s12;
                        si2[k2].push_back(sind2);
                     }
                }
                computingProgressed();
            }
        }
        for (k=0; k<int(sout.size()); k++)
        {
            int k2=sout[k];
            si1.row(k2)=siT.row(k2);
            computingProgressed();
        }
    }
    const ArrayXXd & derivativeSensitivity() { return si1der; } //!< Returns derivative sensitivity (L2) for all output states (rows) over all input parameters (columns).
    const ArrayXXd & averageDerivative() { return ader; } //!< Returns average derivative for all output states (rows) over all input parameters (columns).
    const ArrayXXd & absoluteAverageDerivative() { return aader; } //!< Returns absolute average derivative (L1) for all output states (rows) over all input parameters (columns).
    const ArrayXXd & varianceSensitivity() { return siT; } //!< Returns variance sensitivity (L2) for all output states (rows) over all input parameters (columns).
    const ArrayXXd & firstOrderANOVA() { return si1; } //!< Returns first order ANOVA for all output states (rows) over all input parameters (columns).
    std::vector< std::pair<int,int> > secondOrderANOVAPairs(); //!< Returns second order ANOVA pairs.
    ArrayXXd secondOrderANOVA(const std::vector< std::pair<int,int> > &a2L); //! Returns second order ANOVA indices for given pairs.
    bool emptyDerivativeSensitivity() const { return si1der.size()==0; } //!< Returns true if derivative sentitivity (L2) is empty, false otherwise.
    bool emptyAverageDerivativeSensitivity() const  { return ader.size()==0; } //!< Returns true if average derivative is empty, false otherwise.
    bool emptyAbsoluteAverageDerivativeSensitivity() const  { return aader.size()==0; } //!< Returns true if absolute average derivative (L1) is empty, false otherwise.
    bool emptyVarianceSensitivity() const { return siT.size()==0; } //!< Returns true if variance sensitivity (L2) is empty, false otherwise.
    bool emptyFirstOrderANOVA() const { return si1.size()==0; } //!< Returns true if 1st order ANOVA is empty, false otherwise.
    bool empty() const { return (si1der.size()==0 && ader.size()==0 && aader.size()==0 && siT.size()==0); } //!< Returns true if all sentitivity indices are empty, false otherwise.
    int progressStepsSize() const; //!< Returns progress steps size.
public:
    boost::signal<void()> computingProgressed; //!< Signal for computing progress, emitted on each sensitivity index computed.
};

} // end of namespace GoSUM

BOOST_CLASS_EXPORT_KEY(GoSUM::CSensitivityAnalysis)



#endif // SENSITIVITYANALYSIS_H
