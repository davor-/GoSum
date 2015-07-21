

#include "Plot2D.h"


QwtPlotCanvas *CPlot2D::canvas()  {
    return (plot) ? (QwtPlotCanvas *)plot->canvas() : NULL;
}

void CPlot2D::newPlot(QWidget *pwidget,double xmin,double xmax,double ymin,double ymax,double lymin,double lymax)  {
    if (plot)  delete plot;
    plot=new QwtPlot(pwidget);
    pwidget->layout()->addWidget(plot);
    plot->setStyleSheet("border-style: none;border-width: 0px;");

    plot->setAxisScale(QwtPlot::xBottom, xmin,xmax);
    plot->setAxisScale(QwtPlot::yLeft,ymin,ymax);
    plot->enableAxis(QwtPlot::yRight);
    //plot->enableAxis(QwtPlot::xTop);
    //plot->setAxisScale(QwtPlot::xTop,xmin,xmax);
    plot->setAxisScale(QwtPlot::yRight,lymin,lymax);

    QwtPlotGrid *grid=new QwtPlotGrid;
    grid->setPen(QPen(Qt::DotLine));
    grid->attach(plot);
    plot->setCanvasBackground(QColor(255,255,255));

    plot->setAxisLabelAlignment(QwtPlot::yLeft,Qt::AlignTop|Qt::AlignLeft);
    plot->setAxisLabelAlignment(QwtPlot::yRight,Qt::AlignTop|Qt::AlignRight);

    plot->insertLegend(new QwtLegend());
}

void CPlot2D::newColorBarPlot(QWidget *pwidget,double xmin,double xmax,double ymin,double ymax,double zmin,double zmax,bool enableaxis,QwtLinearColorMap *_pmap)  {
    if (plot)  delete plot;
    plot=new QwtPlot(pwidget);
    pwidget->layout()->addWidget(plot);
    plot->setStyleSheet("border-style: none;border-width: 0px;");

    plot->plotLayout()->setAlignCanvasToScales(false);
    plot->plotLayout()->setCanvasMargin(2);

    plot->setAxisScale(QwtPlot::xBottom,xmin,xmax);
    plot->setAxisScale(QwtPlot::yLeft,ymin,ymax);
    plot->setAxisScale(QwtPlot::yRight,zmin,zmax);

    plot->enableAxis(QwtPlot::xBottom,enableaxis);
    plot->enableAxis(QwtPlot::yLeft,enableaxis);
    plot->enableAxis(QwtPlot::yRight,enableaxis);

    const QwtInterval zInterval = QwtInterval(zmin, zmax);

    QwtScaleWidget *rightAxis = plot->axisWidget(QwtPlot::yRight);
    rightAxis->setColorBarEnabled(true);
    rightAxis->setColorBarWidth(20);
    if (!_pmap) _pmap=CPlot2D::NewColorMap();
    rightAxis->setColorMap(zInterval,_pmap);

//    plot->insertLegend(new QwtLegend());
}

//void CPlot2D::newColorBarPlot(QWidget *pwidget,double xmin,double xmax,double ymin,double ymax,double zmin,double zmax)  {
//    if (plot)  delete plot;
//    plot=new QwtPlot(pwidget);
//    pwidget->layout()->addWidget(plot);
//    plot->setStyleSheet("border-style: none;border-width: 0px;");

//    plot->plotLayout()->setAlignCanvasToScales(false);
//    plot->plotLayout()->setCanvasMargin(30);

//    plot->setAxisScale(QwtPlot::xBottom,xmin,xmax);
//    plot->setAxisScale(QwtPlot::yLeft,ymin,ymax);
//    plot->setAxisScale(QwtPlot::yRight,zmin,zmax);

//    plot->enableAxis(QwtPlot::xBottom,false);
//    plot->enableAxis(QwtPlot::xTop,false);
//    plot->enableAxis(QwtPlot::yLeft,false);
//    plot->enableAxis(QwtPlot::yRight,true);

//    plot->setAxisLabelAlignment(QwtPlot::yRight,Qt::AlignTop|Qt::AlignRight);

//    const QwtInterval zInterval = QwtInterval(zmin, zmax);

//    QwtScaleWidget *rightAxis = plot->axisWidget(QwtPlot::yRight);
//    rightAxis->setColorBarEnabled(true);
//    rightAxis->setColorBarWidth(40);
//    rightAxis->setColorMap(zInterval, newColorMap() );

//}

void CPlot2D::setTitles(string title,Qt::GlobalColor clr,
                        string btitle,Qt::GlobalColor bclr,string ttitle,Qt::GlobalColor tclr,
                        string ltitle,Qt::GlobalColor lclr,string rtitle,Qt::GlobalColor rclr)  {
    QwtText txt;
    txt.setColor(QColor(clr));
    txt.setText(title.c_str());
    plot->setTitle(txt);

    QwtText btxt;
    btxt.setColor(QColor(bclr));
    btxt.setText(btitle.c_str());
    plot->setAxisTitle(QwtPlot::xBottom,btxt);

    QwtText ttxt;
    ttxt.setColor(QColor(tclr));
    ttxt.setText(ttitle.c_str());
    plot->setAxisTitle(QwtPlot::xTop,ttxt);

    QwtText ltxt;
    ltxt.setColor(QColor(lclr));
    ltxt.setText(ltitle.c_str());
    plot->setAxisTitle(QwtPlot::yLeft,ltxt);

    QwtText rtxt;
    rtxt.setColor(QColor(rclr));
    rtxt.setText(rtitle.c_str());
    plot->setAxisTitle(QwtPlot::yRight,rtxt);
}

//QwtLinearColorMap * CPlot2D::newColorMap() {
//    QwtLinearColorMap *colormap=new QwtLinearColorMap(QColor(0,0,int(0.5*255)),QColor(int(0.5*255),0,0));
//    colormap->setMode(QwtLinearColorMap::ScaledColors);
//    colormap->addColorStop(0.1111,QColor(0,0,int(0.94444*255)));
//    colormap->addColorStop(0.2222,QColor(0,int(0.38889*255),255));
//    colormap->addColorStop(0.3333,QColor(0,int(0.83333*255),255));
//    colormap->addColorStop(0.4444,QColor(int(0.27778*255),255,int(0.72222*255)));
//    colormap->addColorStop(0.5555,QColor(int(0.72222*255),255,int(0.27778*255)));
//    colormap->addColorStop(0.6666,QColor(255,int(0.83333*255),0));
//    colormap->addColorStop(0.7777,QColor(255,int(0.38889*255),0));
//    colormap->addColorStop(0.8888,QColor(int(0.94444*255),0,0));

//    return colormap;
//}


QwtLinearColorMap * CPlot2D::NewColorMap() {
    QwtLinearColorMap *colormap=new QwtLinearColorMap(QColor(0,0,int(0.5*255)),QColor(int(0.5*255),0,0));
    colormap->setMode(QwtLinearColorMap::FixedColors);
    colormap->addColorStop(0.1111,QColor(0,0,int(0.94444*255)));
    colormap->addColorStop(0.2222,QColor(0,int(0.38889*255),255));
    colormap->addColorStop(0.3333,QColor(0,int(0.83333*255),255));
    colormap->addColorStop(0.4444,QColor(int(0.27778*255),255,int(0.72222*255)));
    colormap->addColorStop(0.5555,QColor(int(0.72222*255),255,int(0.27778*255)));
    colormap->addColorStop(0.6666,QColor(255,int(0.83333*255),0));
    colormap->addColorStop(0.7777,QColor(255,int(0.38889*255),0));
    colormap->addColorStop(0.8888,QColor(int(0.94444*255),0,0));

    return colormap;
}

QwtLinearColorMap * CPlot2D::NewHyperColorMap() {
    QwtLinearColorMap *colormap=new QwtLinearColorMap(QColor(0,0,int(255)),QColor(int(255),0,0));
    colormap->setMode(QwtLinearColorMap::FixedColors);
    int ind,i;
    ind=0;
    for ( i=0; i<42; i++ ) {
        colormap->addColorStop(double(++ind)/126.,QColor(int((double(i)/41.)*255),int((double(i)/41.)*255),255));
    }
    for ( i=0; i<42; i++ ) {
        colormap->addColorStop(double(++ind)/126.,QColor(int((pow(cos(M_PI*double(i)/41.),2.)*255)),255,int(pow(cos(M_PI*double(i)/41.),2.)*255)));
    }
    for ( i=0; i<41; i++ ) {
        colormap->addColorStop(double(++ind)/126.,QColor(255,int((double(41.-i)/41.)*255),int((double(41.-i)/41.)*255)));
    }

    return colormap;
}

QwtLinearColorMap * CPlot2D::NewBlueWhiteGreenColorMap() {
    QwtLinearColorMap *colormap=new QwtLinearColorMap(QColor(0,0,int(255)),QColor(0,int(255),0));
    colormap->setMode(QwtLinearColorMap::FixedColors);
    int ind,i;
    ind=0;
    for ( i=0; i<42; i++ ) {
        colormap->addColorStop(double(++ind)/126.,QColor(int((double(i)/41.)*255),int((double(i)/41.)*255),255));
    }
    for ( i=0; i<42; i++ ) {
        colormap->addColorStop(double(++ind)/126.,QColor(255,255,255));
    }
    for ( i=0; i<41; i++ ) {
        colormap->addColorStop(double(++ind)/126.,QColor(int((double(41.-i)/41.)*255),255,int((double(41.-i)/41.)*255)));
    }

    return colormap;
}

QwtLinearColorMap * CPlot2D::NewBlueWhiteRedColorMap() {
    QwtLinearColorMap *colormap=new QwtLinearColorMap(QColor(0,0,int(255)),QColor(int(255),0,0));
    colormap->setMode(QwtLinearColorMap::FixedColors);
    int ind,i;
    ind=0;
    for ( i=0; i<42; i++ ) {
        colormap->addColorStop(double(++ind)/126.,QColor(int((double(i)/41.)*255),int((double(i)/41.)*255),255));
    }
    for ( i=0; i<42; i++ ) {
        colormap->addColorStop(double(++ind)/126.,QColor(255,255,255));
    }
    for ( i=0; i<41; i++ ) {
        colormap->addColorStop(double(++ind)/126.,QColor(255,int((double(41.-i)/41.)*255),int((double(41.-i)/41.)*255)));
    }

    return colormap;
}

QwtLinearColorMap * CPlot2D::NewJetColorMap() {
    QwtLinearColorMap *colormap=new QwtLinearColorMap(QColor(0,0,int(0.5*255)),QColor(int(0.5*255),0,0));
    colormap->setMode(QwtLinearColorMap::FixedColors);

    colormap->addColorStop(1./64.,QColor(0,0,143));
    colormap->addColorStop(2./64.,QColor(0,0,159));
    colormap->addColorStop(3./64.,QColor(0,0,175));
    colormap->addColorStop(4./64.,QColor(0,0,191));
    colormap->addColorStop(5./64.,QColor(0,0,207));
    colormap->addColorStop(6./64.,QColor(0,0,223));
    colormap->addColorStop(7./64.,QColor(0,0,239));
    colormap->addColorStop(8./64.,QColor(0,0,255));
    colormap->addColorStop(9./64.,QColor(0,16,255));
    colormap->addColorStop(10./64.,QColor(0,32,255));
    colormap->addColorStop(11./64.,QColor(0,48,255));
    colormap->addColorStop(12./64.,QColor(0,64,255));
    colormap->addColorStop(13./64.,QColor(0,80,255));
    colormap->addColorStop(14./64.,QColor(0,96,255));
    colormap->addColorStop(15./64.,QColor(0,111,255));
    colormap->addColorStop(16./64.,QColor(0,128,255));
    colormap->addColorStop(17./64.,QColor(0,143,255));
    colormap->addColorStop(18./64.,QColor(0,159,255));
    colormap->addColorStop(19./64.,QColor(0,175,255));
    colormap->addColorStop(20./64.,QColor(0,191,255));
    colormap->addColorStop(21./64.,QColor(0,207,255));
    colormap->addColorStop(22./64.,QColor(0,203,255));
    colormap->addColorStop(23./64.,QColor(0,239,255));
    colormap->addColorStop(24./64.,QColor(0,255,255));
    colormap->addColorStop(25./64.,QColor(16,255,239));
    colormap->addColorStop(26./64.,QColor(32,255,223));
    colormap->addColorStop(27./64.,QColor(48,255,207));
    colormap->addColorStop(28./64.,QColor(64,255,191));
    colormap->addColorStop(29./64.,QColor(80,255,175));
    colormap->addColorStop(30./64.,QColor(96,255,159));
    colormap->addColorStop(31./64.,QColor(111,255,143));
    colormap->addColorStop(32./64.,QColor(128,255,128));
    colormap->addColorStop(33./64.,QColor(143,255,111));
    colormap->addColorStop(34./64.,QColor(159,255,96));
    colormap->addColorStop(35./64.,QColor(175,255,80));
    colormap->addColorStop(36./64.,QColor(191,255,64));
    colormap->addColorStop(37./64.,QColor(207,255,48));
    colormap->addColorStop(38./64.,QColor(223,255,32));
    colormap->addColorStop(39./64.,QColor(239,255,16));
    colormap->addColorStop(40./64.,QColor(255,255,0));
    colormap->addColorStop(41./64.,QColor(255,239,0));
    colormap->addColorStop(42./64.,QColor(255,223,0));
    colormap->addColorStop(43./64.,QColor(255,207,0));
    colormap->addColorStop(44./64.,QColor(255,191,0));
    colormap->addColorStop(45./64.,QColor(255,175,0));
    colormap->addColorStop(46./64.,QColor(255,159,0));
    colormap->addColorStop(47./64.,QColor(255,143,0));
    colormap->addColorStop(48./64.,QColor(255,128,0));
    colormap->addColorStop(49./64.,QColor(255,111,0));
    colormap->addColorStop(50./64.,QColor(255,96,0));
    colormap->addColorStop(51./64.,QColor(255,80,0));
    colormap->addColorStop(52./64.,QColor(255,64,0));
    colormap->addColorStop(53./64.,QColor(255,48,0));
    colormap->addColorStop(54./64.,QColor(255,32,0));
    colormap->addColorStop(55./64.,QColor(255,16,0));
    colormap->addColorStop(56./64.,QColor(255,0,0));
    colormap->addColorStop(57./64.,QColor(239,0,0));
    colormap->addColorStop(58./64.,QColor(223,0,0));
    colormap->addColorStop(59./64.,QColor(207,0,0));
    colormap->addColorStop(60./64.,QColor(191,0,0));
    colormap->addColorStop(61./64.,QColor(175,0,0));
    colormap->addColorStop(62./64.,QColor(159,0,0));
    colormap->addColorStop(63./64.,QColor(143,0,0));

    return colormap;
}


void CPlot2D::plotWithPen(const ArrayXd &X,const ArrayXd &Y,string name,Qt::GlobalColor clr)  {
    if ( X.size()==0 || Y.size()==0 ) return;

    QwtPointArrayData * const pdata = new QwtPointArrayData(X.data(),Y.data(),X.size());
    QwtPlotCurve *pcurve=new QwtPlotCurve(name.c_str());
    pcurve->setData(pdata);

    QPen pen;
    pen.setColor(QColor(clr));
    pen.setWidth(2);
    pcurve->setPen(pen);
    pcurve->setLegendAttribute(QwtPlotCurve::LegendShowLine);

    pcurve->attach(plot);
}

void CPlot2D::plotWithSymbol(const ArrayXd &X,const ArrayXd &Y,string name,Qt::GlobalColor clr) {
    if ( X.size()==0 || Y.size()==0 ) return;

    QwtPointArrayData * const pdata = new QwtPointArrayData(X.data(),Y.data(),X.size());
    QwtPlotCurve *pcurve=new QwtPlotCurve(name.c_str());
    pcurve->setData(pdata);

    pcurve->setPen(QPen(Qt::NoPen));
    pcurve->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,QBrush(clr),QPen(clr),QSize(6,6)));
    pcurve->setLegendAttribute(QwtPlotCurve::LegendShowSymbol);

    pcurve->attach(plot);
}

void CPlot2D::plotWithSymbol(const std::vector<double> &X,const std::vector<double> &Y,string name,Qt::GlobalColor clr) {
    if ( X.size()==0 || Y.size()==0 ) return;

//    ArrayXd Xa,Ya;
//    Xa=ArrayXd::Map(&X.front(),X.size());
//    Ya=ArrayXd::Map(&Y.front(),Y.size());

    plotWithSymbol(ArrayXd::Map(&X.front(),X.size()),ArrayXd::Map(&Y.front(),Y.size()),name,clr);
}

void CPlot2D::plotWithHistogram(const ArrayXd &X,const ArrayXd &Y,string name,Qt::GlobalColor clr) {
    if ( X.size()==0 || Y.size()==0 ) return;

    QVector<QwtIntervalSample> bars;
    bars.resize(X.size());

    if ( X.size()==1 )
    {
        double dbar=0.01;
        QwtInterval interval(X(0)-dbar,X(0)+dbar);
        interval.setBorderFlags(QwtInterval::ExcludeMaximum);
        bars[0] = QwtIntervalSample(Y(0), interval);
    }
    else
    {
        int i;
        QwtInterval interval0(X(0)-(0.5*(X(0)+X(1))-X(0)),0.5*(X(0)+X(1)));
        interval0.setBorderFlags(QwtInterval::ExcludeMaximum);
        bars[0] = QwtIntervalSample(Y(0), interval0);
        for ( i=1; i<X.size()-1; i++ )
        {   QwtInterval interval(0.5*(X(i-1)+X(i)),0.5*(X(i)+X(i+1)));
            interval.setBorderFlags(QwtInterval::ExcludeMaximum);
            bars[i] = QwtIntervalSample(Y(i), interval); }
        QwtInterval interval1(0.5*(X(i-1)+X(i)),X(i)+(X(i)-0.5*(X(i-1)+X(i))));
        interval1.setBorderFlags(QwtInterval::ExcludeMaximum);
        bars[i] = QwtIntervalSample(Y(i), interval1);
    }


    QwtPlotHistogram *histogram=new QwtPlotHistogram(name.c_str());
    histogram->setStyle(QwtPlotHistogram::Columns);
    histogram->setPen(QPen(Qt::black));
    histogram->setBrush(QBrush(QColor(clr)));
    QwtColumnSymbol *symbol = new QwtColumnSymbol(QwtColumnSymbol::Box);
    symbol->setFrameStyle(QwtColumnSymbol::Raised);
    symbol->setLineWidth(2);
    symbol->setPalette(QPalette(QColor(clr)));
    histogram->setSymbol(symbol);
    histogram->setData(new QwtIntervalSeriesData(bars));
    histogram->attach(plot);
}

void CPlot2D::plotWithHistogram(const ArrayXi &Xi,const ArrayXd &Y,string name,Qt::GlobalColor clr) {
    if ( Xi.size()==0 || Y.size()==0 ) return;

    double dbar=(Xi.size()==1)? 0.005:0.025;
    int i;
    QVector<QwtIntervalSample> bars;
    bars.resize(Xi.size());
    for ( i=0; i<Xi.size(); i++ )
    {   QwtInterval interval(double(Xi(i))-dbar,double(Xi(i))+dbar);
        interval.setBorderFlags(QwtInterval::ExcludeMaximum);
        bars[i] = QwtIntervalSample(Y(i), interval); }

    QwtPlotHistogram *histogram=new QwtPlotHistogram(name.c_str());
    histogram->setStyle(QwtPlotHistogram::Columns);
    histogram->setPen(QPen(Qt::black));
    histogram->setBrush(QBrush(QColor(clr)));
    QwtColumnSymbol *symbol = new QwtColumnSymbol(QwtColumnSymbol::Box);
    symbol->setFrameStyle(QwtColumnSymbol::Raised);
    symbol->setLineWidth(2);
    symbol->setPalette(QPalette(QColor(clr)));
    histogram->setSymbol(symbol);
    histogram->setData(new QwtIntervalSeriesData(bars));
    histogram->attach(plot);
}

void CPlot2D::plotWithSpectrogram(const ArrayXXd &A,const std::vector<std::string> &xlabels,const std::vector<std::string> &ylabels) {
    QwtPlotSpectrogram *spectrogram = new QwtPlotSpectrogram();
    spectrogram->setRenderThreadCount(0);

    spectrogram->setColorMap(NewColorMap());

    QwtMatrixRasterData *rdata=new QwtMatrixRasterData;
    QVector<double> values;
    int i,o;

    for ( o=A.rows()-1; o>=0; o-- )
    {   for ( i=0; i<A.cols(); i++ )
        {   values += A(o,i);  }  }
    rdata->setValueMatrix(values,A.cols());

    rdata->setInterval(Qt::XAxis,QwtInterval(0,double(A.cols()),QwtInterval::ExcludeMaximum));
    rdata->setInterval(Qt::YAxis,QwtInterval(0,double(A.rows()),QwtInterval::ExcludeMaximum));
    rdata->setInterval(Qt::ZAxis,QwtInterval(A.minCoeff(),A.maxCoeff()));

    for ( i=0; i<xlabels.size(); i++ )
    {
        QwtPlotMarker *mark=new QwtPlotMarker();
        mark->setLabel(QwtText(xlabels[i].c_str()));
        mark->setValue(0.5+double(i),-0.1);
        mark->setLabelOrientation(Qt::Vertical);
        mark->attach(plot);
    }

    for ( o=0; o<ylabels.size(); o++ )
    {
        QwtPlotMarker *mark=new QwtPlotMarker();
        mark->setLabel(QwtText(ylabels[o].c_str()));
        mark->setValue(-1.,double(ylabels.size()-o)-0.5);
        mark->attach(plot);
    }

    spectrogram->setData(rdata);
    spectrogram->attach(plot);
}


void CPlot2D::plotWithContourSpectrogram(const ArrayXd &X,const ArrayXd &Y,const ArrayXXd &A,int _contoursize) {
    QwtPlotSpectrogram *spectrogram = new QwtPlotSpectrogram();
    spectrogram->setRenderThreadCount(0);

    spectrogram->setColorMap(NewColorMap());

    QwtMatrixRasterData *rdata=new QwtMatrixRasterData;
    QVector<double> values;
    int i,j;

    for ( j=0; j<A.rows(); j++ )
    {   for ( i=0; i<A.cols(); i++ )
        {   values += A(j,i);  }  }
    rdata->setValueMatrix(values,A.cols());

    rdata->setInterval(Qt::XAxis,QwtInterval(X(0),X(X.size()-1),QwtInterval::IncludeBorders));
    rdata->setInterval(Qt::YAxis,QwtInterval(Y(0),Y(Y.size()-1),QwtInterval::IncludeBorders));
    rdata->setInterval(Qt::ZAxis,QwtInterval(A.minCoeff(),A.maxCoeff(),QwtInterval::IncludeBorders));

    spectrogram->setDisplayMode(QwtPlotSpectrogram::ImageMode,true);
    spectrogram->setDisplayMode(QwtPlotSpectrogram::ContourMode,true);
    spectrogram->setData(rdata);

    ArrayXd ca=ArrayXd::LinSpaced(_contoursize,A.minCoeff(),A.maxCoeff());
    QList<double> cl;
    for ( i=0; i<ca.size(); i++ )  cl.push_back(ca(i));
    spectrogram->setContourLevels(cl);

    spectrogram->setDefaultContourPen(QPen(Qt::white));

    spectrogram->attach(plot);
}

void CPlot2D::plotWithSpectrogram(const ArrayXd &X,const ArrayXd &Y,const ArrayXXd &A,double _zmin,double _zmax,QwtLinearColorMap *_pmap) {
    QwtPlotSpectrogram *spectrogram = new QwtPlotSpectrogram();
    spectrogram->setRenderThreadCount(0);

    if (!_pmap) _pmap=NewColorMap();
    spectrogram->setColorMap(_pmap);

    QwtMatrixRasterData *rdata=new QwtMatrixRasterData;
    QVector<double> values;
    int i,j;

    for ( j=0; j<A.rows(); j++ )
    {   for ( i=0; i<A.cols(); i++ )
        {   values += A(j,i);  }  }
    rdata->setValueMatrix(values,A.cols());

    rdata->setInterval(Qt::XAxis,QwtInterval(X(0),X(X.size()-1),QwtInterval::IncludeBorders));
    rdata->setInterval(Qt::YAxis,QwtInterval(Y(0),Y(Y.size()-1),QwtInterval::IncludeBorders));
//    rdata->setInterval(Qt::ZAxis,QwtInterval(A.minCoeff(),A.maxCoeff(),QwtInterval::IncludeBorders));
    if (_zmin==_zmax) {
        _zmin=A.minCoeff();
        _zmax=A.maxCoeff();
    }
    rdata->setInterval(Qt::ZAxis,QwtInterval(_zmin,_zmax,QwtInterval::IncludeBorders));

    spectrogram->setData(rdata);

    spectrogram->setDisplayMode(QwtPlotSpectrogram::ImageMode,true);
    spectrogram->setDisplayMode(QwtPlotSpectrogram::ContourMode,false);
    spectrogram->attach(plot);
}

void CPlot2D::plotWithContours(const ArrayXd &X,const ArrayXd &Y,const ArrayXXd &A,int _contoursize) {
    QwtPlotSpectrogram *spectrogram = new QwtPlotSpectrogram();
    spectrogram->setRenderThreadCount(0);

//    spectrogram->setColorMap(newColorMap());

    QwtMatrixRasterData *rdata=new QwtMatrixRasterData;
    QVector<double> values;
    int i,j;

    for ( j=0; j<A.rows(); j++ )
    {   for ( i=0; i<A.cols(); i++ )
        {   values += A(j,i);  }  }
    rdata->setValueMatrix(values,A.cols());

    rdata->setInterval(Qt::XAxis,QwtInterval(X(0),X(X.size()-1),QwtInterval::IncludeBorders));
    rdata->setInterval(Qt::YAxis,QwtInterval(Y(0),Y(Y.size()-1),QwtInterval::IncludeBorders));
    rdata->setInterval(Qt::ZAxis,QwtInterval(A.minCoeff(),A.maxCoeff(),QwtInterval::IncludeBorders));

    spectrogram->setData(rdata);

    spectrogram->setDisplayMode(QwtPlotSpectrogram::ImageMode,false);
    spectrogram->setDisplayMode(QwtPlotSpectrogram::ContourMode,true);
    ArrayXd ca=ArrayXd::LinSpaced(_contoursize,A.minCoeff(),A.maxCoeff());
    QList<double> cl;
    for ( i=0; i<ca.size(); i++ )  cl.push_back(ca(i));
    spectrogram->setContourLevels(cl);

    spectrogram->attach(plot);
}


void CPlot2D::plotWithVectors(const ArrayXd &X,const ArrayXd &Y,const ArrayXXd &U,const ArrayXXd &V,double _fac,int _every)  {
    if ( X.size()==0 || Y.size()==0 ) return;

    ArrayXd cx(5),cy(5);
    double sx,sy;
    for ( int i=0; i<X.size(); i+=_every ) {
        for ( int j=0; j<Y.size(); j+=_every  ) {
            cx(0)=X(i);                 cy(0)=Y(j);
            cx(1)=X(i)+U(i,j)*_fac;     cy(1)=Y(j)+V(i,j)*_fac;
            sx=cx(0)+0.8*(cx(1)-cx(0)); sy=cy(0)+0.8*(cy(1)-cy(0));
            cx(2)=sx+0.1*(cy(1)-cy(0)); cy(2)=sy-0.1*(cx(1)-cx(0));
            cx(3)=cx(1);                cy(3)=cy(1);
            cx(4)=sx-0.1*(cy(1)-cy(0)); cy(4)=sy+0.1*(cx(1)-cx(0));
            QwtPointArrayData * const pdata = new QwtPointArrayData(cx.data(),cy.data(),5);
            QwtPlotCurve *pcurve=new QwtPlotCurve();
            pcurve->setData(pdata);
            QPen pen;
            pen.setColor(Qt::black);
            pen.setWidth(2);
            pcurve->setPen(pen);
            pcurve->attach(plot);
         }
    }
}
