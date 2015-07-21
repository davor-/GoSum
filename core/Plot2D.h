#ifndef PLOT2D_H
#define PLOT2D_H

#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

#include <QLayout>
#include "qwt_plot.h"
#include "qwt_plot_marker.h"
#include "qwt_plot_curve.h"
#include "qwt_plot_layout.h"
#include "qwt_legend.h"
//#include "qwt_legend_item.h"
#include "qwt_symbol.h"
#include "qwt_plot_grid.h"
#include "qwt_plot_histogram.h"
#include "qwt_plot_spectrogram.h"
#include "qwt_color_map.h"
#include "qwt_matrix_raster_data.h"
#include "qwt_scale_widget.h"
//#include "qwt_plot_magnifier.h"
//#include "qwt_plot_panner.h"
#include "qwt_plot_picker.h"
#include "qwt_picker_machine.h"
#include "qwt_plot_renderer.h"
#include "qwt_plot_canvas.h"
#include "qwt_point_data.h"




class CPlot2D {
private:
    QwtPlot *plot; //!< Holds pointer to the QwtPlot object.
public:
    CPlot2D() : plot(NULL) {}
    virtual ~CPlot2D() { if (plot) delete plot; }
//private:
public:
//    QwtLinearColorMap * newColorMap();
    static QwtLinearColorMap * NewColorMap();
    static QwtLinearColorMap * NewJetColorMap();
    static QwtLinearColorMap * NewHyperColorMap();
    static QwtLinearColorMap * NewBlueWhiteRedColorMap();
    static QwtLinearColorMap * NewBlueWhiteGreenColorMap();
public:
    QwtPlotCanvas *canvas(); //!< Returns pointer to plot canvas.
    void newPlot(QWidget *pwidget,double xmin,double xmax,double ymin,double ymax,double lymin,double lymax); //!< Creates new 2D plot with given min and max values on the axis.
//    void newColorBarPlot(QWidget *pwidget,double xmin,double xmax,double ymin,double ymax,double zmin,double zmax); //!< Creates new 2D plot with given min and max values on the axis.
    void newColorBarPlot(QWidget *pwidget,double xmin,double xmax,double ymin,double ymax,double zmin,double zmax,bool enableaxis,QwtLinearColorMap *_pmap=NULL); //!< Creates new 2D plot with given min and max values on the axis.
    void setTitles(string title,Qt::GlobalColor clr,
                   string btitle,Qt::GlobalColor bclr,string ttitle,Qt::GlobalColor tclr,
                   string ltitle,Qt::GlobalColor lclr,string rtitle,Qt::GlobalColor rclr); //! Sets titles (text and color) for all axis.
    void replot() { plot->replot(); } //!< Replots the 2D plot.
    void clear() {  if (plot) { delete plot;  plot=NULL; } } //!< Clears object.
    void plotWithPen(const ArrayXd &X,const ArrayXd &Y,string name,Qt::GlobalColor clr); //!< Plots data using pen.
    void plotWithSymbol(const ArrayXd &X,const ArrayXd &Y,string name,Qt::GlobalColor clr); //!< Plots data using symbols.
    void plotWithSymbol(const std::vector<double> &X,const std::vector<double> &Y,string name,Qt::GlobalColor clr); //!< Plots data using symbols.
    void plotWithHistogram(const ArrayXi &Xi,const ArrayXd &H,string name,Qt::GlobalColor clr); //!< Plots data using histogram.
    void plotWithHistogram(const ArrayXd &X,const ArrayXd &H,string name,Qt::GlobalColor clr); //!< Plots data using histogram.
    void plotWithSpectrogram(const ArrayXXd &A,const std::vector<std::string> &xlabels,const std::vector<std::string> &ylabels); //!< Plots data using spectrogram.
    void plotWithContourSpectrogram(const ArrayXd &X,const ArrayXd &Y,const ArrayXXd &A,int _contoursize=20);
    void plotWithSpectrogram(const ArrayXd &X,const ArrayXd &Y,const ArrayXXd &A,double _zmin=0,double _zmax=0,QwtLinearColorMap *_pmap=NULL);
    void plotWithContours(const ArrayXd &X,const ArrayXd &Y,const ArrayXXd &A,int _contoursize=20);
    void CPlot2D::plotWithVectors(const ArrayXd &X,const ArrayXd &Y,const ArrayXXd &U,const ArrayXXd &V,double _fac,int _every=10);
    QwtPlot *getPlot() { return plot; } //!< Returns the QwtPlot object.

};


#endif // PLOT2D_H
