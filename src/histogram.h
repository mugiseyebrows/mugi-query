#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <qwt_text.h>
#include "distributionplotitem.h"
#include <qwt_samples.h>

class QAbstractItemModel;

class Histogram {
public:
    Histogram(int bins, const QList<DistributionPlotItem>& items, const QStringList& header,
              QAbstractItemModel* mModel, double vmin, double vmax);

    QList<QwtText> titles() const;

    QVector<QwtSetSample> samples() const;

    static QPair<double,double> range(const QList<DistributionPlotItem>& items, const QStringList& header, QAbstractItemModel* mModel, bool *ok);

protected:

    double mMin;
    double mMax;
    int mBins;

    QList<DistributionPlotItem> mItems;
    QList<QVector<double> > hist;
    QList<QList<double> > values;

};


#endif // HISTOGRAM_H
