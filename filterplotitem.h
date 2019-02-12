#ifndef FILTERPLOTITEM_H
#define FILTERPLOTITEM_H

#include <QList>
class QwtPlotMultiBarChart;
class QwtPlot;
class QwtPlotCurve;

QList<QwtPlotMultiBarChart*> filterMultiBarCharts(QwtPlot* plot);

QList<QwtPlotCurve*> filterCurves(QwtPlot* plot);

#endif // FILTERPLOTITEM_H
