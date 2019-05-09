#include "histogram.h"

#include "datautils.h"
#include <QAbstractItemModel>

using namespace DataUtils;

Histogram::Histogram(int bins, const DistributionDataset &dataset, double min, double max) {

    mBins = bins;
    mMin = min;
    mMax = max;

    if (bins < 1) {
        return;
    }

    while(hist.size() < bins) {
        hist.append(QVector<double>(dataset.size(), 0.0));
    }

    for(int i=0;i<dataset.size();i++) {
        const QList<double>& values = dataset.values(i);
        foreach(double v, values) {
            if (v < mMin || v > mMax) {
                continue;
            }
            int index = qMin((int)((v - mMin) * (double) bins / (mMax - mMin)), hist.size() - 1);
            hist[index][i] += 1.0;
        }
    }

}

QList<QwtText> Histogram::titles() const {
    int numBars = mItems.size();
    QList<QwtText> titles;
    for ( int i = 0; i < numBars; i++ )
    {
        const DistributionPlotItem& item = mItems[i];
        titles.append(item.v());
    }
    return titles;
}

QVector<QwtSetSample> Histogram::samples() const {
    QVector<QwtSetSample> samples;
    for(int i=0;i<hist.size();i++) {
        samples.append(QwtSetSample(mMin + ((mMax - mMin) * ((double)i + 0.5)) / (double) mBins, hist[i]));
    }
    return samples;
}
