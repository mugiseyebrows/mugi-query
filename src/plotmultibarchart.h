#ifndef PLOTMULTIBARCHART_H
#define PLOTMULTIBARCHART_H

#include <qwt_plot_multi_barchart.h>

class PlotMultiBarChart : public QwtPlotMultiBarChart
{
public:
    PlotMultiBarChart(const QString &title = QString::null);

    void drawBar( QPainter *painter, int sampleIndex, int valueIndex, const QwtColumnRect &rect ) const override;

    int closestSample(double value);
};

#endif // PLOTMULTIBARCHART_H
