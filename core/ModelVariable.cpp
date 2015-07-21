


#include "ModelVariable.h"



BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CDiscreteMV)
BOOST_CLASS_EXPORT_IMPLEMENT(GoSUM::CContinuousMV)


GoSUM::CModelVariable *GoSUM::CModelVariable::clone(std::string _uname)
{
    CModelVariable *pC=NULL;

    if ( typeid(*this)==typeid(GoSUM::CDiscreteMV) )
    {
        pC=new GoSUM::CDiscreteMV(*((GoSUM::CDiscreteMV *)this));
    }
    else if ( typeid(*this)==typeid(GoSUM::CContinuousMV) )
    {
        pC=new GoSUM::CContinuousMV(*((GoSUM::CContinuousMV *)this));
    }
    if (pC) pC->uname=_uname;

    return pC;
}


GoSUM::CContinuousMV *GoSUM::CModelVariable::cast2ContinuousMV()
{
    if ( typeid(*this)==typeid(GoSUM::CContinuousMV) )
        return (GoSUM::CContinuousMV *)(this);
    else
        return NULL;
}


GoSUM::CDiscreteMV *GoSUM::CModelVariable::cast2DiscreteMV()
{
    if ( typeid(*this)==typeid(GoSUM::CDiscreteMV) )
        return (GoSUM::CDiscreteMV *)(this);
    else
        return NULL;
}

ArrayXd GoSUM::CModelVariable::castExportSample()
{
    return (this->isContinuous()) ? this->cast2ContinuousMV()->exportSample():
                                    this->cast2DiscreteMV()->exportSample().cast<double>();

}


GoSUM::CDiscreteMV::CDiscreteMV(const std::string &_uname,CRandomVariable::distributiontype _type)
{
    uname=_uname;
    switch(_type)
    {
        case CRandomVariable::constantdiscrete:
            pRV = new CConstantDRV;
            pS = new CDiscreteSample;
            break;
        case CRandomVariable::uniformdiscrete:
            pRV = new CUniformDRV;
            pS = new CDiscreteSample;
            break;
        case CRandomVariable::empiricaldiscrete:
            pRV = new CEmpiricalDRV;
            pS = new CNumericalSample;
            break;
        case CRandomVariable::categorical:
            pRV = new CCategoricalDRV;
            pS = new CCategoricalSample;
            break;
        default:
            throw "CDisreteMV::CDiscreteMV error: unrecognised type";
    }
}


GoSUM::CDiscreteMV::CDiscreteMV(const CDiscreteMV &O)
{
    uname=O.uname;
    switch(O.distributionType())
    {
        case CRandomVariable::constantdiscrete:
            pRV = new CConstantDRV(*((CConstantDRV *)O.pRV));
            pS = new CDiscreteSample(*((CDiscreteSample *)O.pS));
            break;
        case CRandomVariable::uniformdiscrete:
            pRV = new CUniformDRV(*((CUniformDRV *)O.pRV));
            pS = new CDiscreteSample(*((CDiscreteSample *)O.pS));
            break;
        case CRandomVariable::empiricaldiscrete:
            pRV = new CEmpiricalDRV(*((CEmpiricalDRV *)O.pRV));
            pS = new CNumericalSample(*((CNumericalSample *)O.pS));
            break;
        case CRandomVariable::categorical:
            pRV = new CCategoricalDRV(*((CCategoricalDRV *)O.pRV));
            pS = new CCategoricalSample(*((CCategoricalSample *)O.pS));
            break;
        default:
            throw "CDisreteMV::CDiscreteMV error: unrecognised type";
    }
}


GoSUM::CDiscreteMV::CDiscreteMV(const GoSUM::CContinuousMV &O)
{
    uname=O.name();
    switch(O.distributionType())
    {
        case CRandomVariable::empiricalcontinuous:
            pRV = new CEmpiricalDRV;
            pS = new CNumericalSample(*((CContinuousSample *)O.sample()));
            break;
        default:
            throw "CDisreteMV::CDiscreteMV(const CContinuousMV &) error: unrecognised type";
    }
}


GoSUM::CContinuousMV::CContinuousMV(const std::string &_uname,CRandomVariable::distributiontype _type)
{
    uname=_uname;
    switch(_type)
    {
        case CRandomVariable::constantcontinuous:
            pRV = new CConstantCRV;
            pS = new CContinuousSample;
            break;
        case CRandomVariable::uniformcontinuous:
            pRV = new CUniformCRV;
            pS = new CContinuousSample;
            break;
        case CRandomVariable::gaussian:
            pRV = new CGaussianCRV;
            pS = new CContinuousSample;
            break;
        case CRandomVariable::exponential:
            pRV = new CExponentialCRV;
            pS = new CContinuousSample;
            break;
        case CRandomVariable::empiricalcontinuous:
            pRV = new CEmpiricalCRV;
            pS = new CContinuousSample;
            break;
        default:
            throw "CContinuousMV::CContinuousMV error: unrecognised type";
    }
}


GoSUM::CContinuousMV::CContinuousMV(const CContinuousMV &O)
{
    uname==O.uname;
    switch(O.distributionType())
    {
        case CRandomVariable::constantcontinuous:
            pRV = new CConstantCRV(*((CConstantCRV *)O.pRV));
            pS = new CContinuousSample(*((CContinuousSample *)O.pS));
            break;
        case CRandomVariable::uniformcontinuous:
            pRV = new CUniformCRV(*((CUniformCRV *)O.pRV));
            pS = new CContinuousSample(*((CContinuousSample *)O.pS));
            break;
        case CRandomVariable::gaussian:
            pRV = new CGaussianCRV(*((CGaussianCRV *)O.pRV));
            pS = new CContinuousSample(*((CContinuousSample *)O.pS));
            break;
        case CRandomVariable::exponential:
            pRV = new CExponentialCRV(*((CExponentialCRV *)O.pRV));
            pS = new CContinuousSample(*((CContinuousSample *)O.pS));
            break;
        case CRandomVariable::empiricalcontinuous:
            pRV = new CEmpiricalCRV(*((CEmpiricalCRV *)O.pRV));
            pS = new CContinuousSample(*((CContinuousSample *)O.pS));
            break;
        default:
            throw "CContinuousMV::CContinuousMV error: unrecognised type";
    }
}


void GoSUM::CDiscreteMV::setTheoreticalDistribution()
{
    if ( pS->isConstant() )
    {
        delete pRV;
        pRV=new CConstantDRV;
        pRV->setDistribution(*pS);
    }
}


void GoSUM::CContinuousMV::setTheoreticalDistribution()
{
    if ( typeid(*pRV)==typeid(CEmpiricalCRV) )
    {
        if ( pS->isConstant() )
        {
            delete pRV;
            pRV=new CConstantCRV;
            pRV->setDistribution(*pS);
        }
        /*else
        {
            CUniformCRV *pURV=new CUniformCRV;
            CGaussianCRV *pGRV=new CGaussianCRV;
            CExponentialCRV *pERV=new CExponentialCRV;
            pURV->setDistribution(*pS);
            pGRV->setDistribution(*pS);
            pERV->setDistribution(*pS);
            double Uerr=KolmogorovSmirnovTest(*pURV,*pS);
            double Gerr=KolmogorovSmirnovTest(*pGRV,*pS);
            double Eerr=KolmogorovSmirnovTest(*pERV,*pS);
            double err=std::min(std::min(Uerr,Gerr),Eerr),KSerr=0.01;
            if ( err<KSerr )
            {
                delete pRV;
                if ( Uerr==err )
                {  pRV=pURV; delete pGRV; delete pERV;  }
                else if ( Gerr==err )
                {  pRV=pGRV; delete pERV; delete pURV;  }
                else //if ( Eerr==err )
                {  pRV=pERV; delete pURV; delete pGRV;  }
            }
        }*/
    }
}



