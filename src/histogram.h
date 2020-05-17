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

    QList<QList<double> > histRows(int index) const;

    int size() const;

    double min() const;

    double max() const;

    int total(int index) const;
    int filtered(int index) const;

    int prec();
protected:

    int mBins;
    double mMin;
    double mMax;

    QList<int> mTotal;
    QList<int> mFiltered;

    QList<DistributionPlotItem> mItems;
    QList<QVector<double> > mHist; // [bins][]
    //QList<QList<double> > mValues;

};


#endif // HISTOGRAM_H
