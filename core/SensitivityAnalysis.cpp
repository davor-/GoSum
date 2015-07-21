


#include "SensitivityAnalysis.h"
#include "Hypercube.h"


BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CSensitivityAnalysis)


template <class Archive>
void GoSUM::CSensitivityAnalysis::serialize (Archive &ar,const unsigned int version)
{
    ar & boost::serialization::make_nvp("pIP",pIP);
    ar & boost::serialization::make_nvp("pOS",pOS);
    ar & boost::serialization::make_nvp("pAM",pAM);
    ar & boost::serialization::make_nvp("RSsize",RSsize);
    ar & boost::serialization::make_nvp("eps1",eps1);
    ar & boost::serialization::make_nvp("eps2",eps2);
    ar & boost::serialization::make_nvp("eps3",eps3);
    ar & boost::serialization::make_nvp("sample1",sample1);
    ar & boost::serialization::make_nvp("sample2",sample2);
    ar & boost::serialization::make_nvp("y",y);
    ar & boost::serialization::make_nvp("yvar",yvar);
    ar & boost::serialization::make_nvp("ymu",ymu);
    ar & boost::serialization::make_nvp("ader",ader);
    ar & boost::serialization::make_nvp("aader",aader);
    ar & boost::serialization::make_nvp("si1der",si1der);
    ar & boost::serialization::make_nvp("siT",siT);
    ar & boost::serialization::make_nvp("si1",si1);
    ar & boost::serialization::make_nvp("si2",si2);
}


void GoSUM::CSensitivityAnalysis::computeDerivativeSensitivities()
{

    int i,j,k,l,Isize=pIP->size(),EIsize=pIP->expandedSize(),Osize=pOS->size();

    ader.setZero(Osize,EIsize);
    aader.setZero(Osize,EIsize);
    si1der.setZero(Osize,EIsize);


    ArrayXXd dy=ArrayXXd::Zero(Osize,EIsize);
    ArrayXd ysdev=yvar.sqrt();
    double sdevi,sdevo;

    for ( i=0; i<RSsize; i++)
    {
        dy=pAM->predictDerivatives(sample1[i]);
        ader+=dy;
        aader+=dy.abs();
        si1der+=dy.square();

        computingProgressed();
    }
    ader/=RSsize; aader/=RSsize; si1der/=RSsize;

    for ( i=0, l=0; i<Isize; i++ )
    {
        sdevi=sqrt((*pIP)(i).sample()->variance());
        for ( k=0; k<(*pIP)(i).expandedSize(); k++,l++ )
        {
            ader.col(l)*=sdevi;
            aader.col(l)*=sdevi;
            si1der.col(l)*=sdevi*sdevi;
        }

        computingProgressed();
    }

    for ( j=0; j<Osize; j++ )
    {
        sdevo=sqrt((*pOS)(j).sample()->variance());
        if ( sdevo<TINY )
        { ader.row(j).setZero(); aader.row(j).setZero(); si1der.row(j).setZero();  }
        else
        {
            ader.row(j)/=sdevo;
            aader.row(j)/=sdevo;
            si1der.row(j)/=sdevo*sdevo;
        }
        computingProgressed();
    }


}


std::vector< std::pair<int,int> > GoSUM::CSensitivityAnalysis::secondOrderANOVAPairs()
{
    std::vector< std::pair<int,int> > a2p;
    int i,k,Osize=pOS->size(),K;

    for ( k=0; k<Osize; k++ )
    {
        K=si2[k].size();
        for ( i=0; i<K; i++ )
        {
            if ( find(a2p.begin(),a2p.end(),si2[k][i].first)==a2p.end() )   {  a2p.push_back(si2[k][i].first);  }
        }
    }
    return a2p;
}


ArrayXXd GoSUM::CSensitivityAnalysis::secondOrderANOVA(const std::vector< std::pair<int,int> > &a2L)
{

    int row,col,i,K,Osize=pOS->size(),Psize=a2L.size();
    ArrayXXd anova2;
    anova2.setZero(Osize,Psize);

    for ( row=0; row<Osize; row++ )
    {
        K=si2[row].size();
        for ( i=0; i<K; i++ )
        {
            col=find(a2L.begin(),a2L.end(),si2[row][i].first)-a2L.begin();
            anova2(row,col)=si2[row][i].second;
        }
    }

    return anova2;
}


int GoSUM::CSensitivityAnalysis::progressStepsSize() const
{
   int Isize=(pIP)?pIP->size():0,Osize=(pOS)?pOS->size():0;
   return 2+3*RSsize+Isize+5*Osize;
}
