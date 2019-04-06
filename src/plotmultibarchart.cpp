#include "plotmultibarchart.h"

#include <QDebug>
#include <qwt_column_symbol.h>
#include <qwt_samples.h>

PlotMultiBarChart::PlotMultiBarChart(const QString &title) : QwtPlotMultiBarChart(title)
{

}

void PlotMultiBarChart::drawBar(QPainter *painter, int sampleIndex,
                                int valueIndex, const QwtColumnRect &rect) const
{
    QwtPlotMultiBarChart::drawBar(painter, sampleIndex, valueIndex, rect);
}

int PlotMultiBarChart::closestSample(double value)
{
    int n = dataSize();
    if (n > 1) {
        QwtSetSample p1 = sample(0);
        QwtSetSample p2 = sample(n-1);
        double sampleWidth = (p2.value - p1.value) / (n-1);
        double index = (value - p1.value) / sampleWidth;
        return index;
    }
    return -1;
}
