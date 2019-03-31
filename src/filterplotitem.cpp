#include "filterplotitem.h"

#include <qwt_plot.h>
#include <qwt_plot_multi_barchart.h>
#include <qwt_plot_curve.h>

template <class T>
QList<T*> filterItems(QwtPlot* plot, int rtti) {
    QList<T*> result;
    QwtPlotItemList items = plot->itemList();
    foreach (QwtPlotItem* item, items) {
        if (item->rtti() == rtti) {
            T* curve = static_cast<T*>(item);
            result.append(curve);
        }
    }
    return result;
}

QList<QwtPlotMultiBarChart*> filterMultiBarCharts(QwtPlot* plot) {
    return filterItems<QwtPlotMultiBarChart>(plot, QwtPlotItem::Rtti_PlotMultiBarChart);
}

QList<QwtPlotCurve*> filterCurves(QwtPlot* plot) {
    return filterItems<QwtPlotCurve>(plot, QwtPlotItem::Rtti_PlotCurve);
}
