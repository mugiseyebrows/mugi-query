#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <qwt_text.h>
#include "distributionplotitem.h"
#include <qwt_samples.h>

#include "distributiondataset.h"

class QAbstractItemModel;

class Histogram {
public:
    Histogram(int bins, const DistributionDataset& dataset, double min, double max);

    QList<QwtText> titles() const;

    QVector<QwtSetSample> samples() const;

protected:

    int mBins;
    double mMin;
    double mMax;

    QList<DistributionPlotItem> mItems;
    QList<QVector<double> > hist;
    QList<QList<double> > values;

};


#endif // HISTOGRAM_H
