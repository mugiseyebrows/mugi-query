#include "histogram.h"

#include "datautils.h"
#include <QAbstractItemModel>

using namespace DataUtils;

Histogram::Histogram(int bins, const QList<DistributionPlotItem> &items, const QStringList &header, QAbstractItemModel *mModel, double vmin, double vmax) {

    mItems = items;
    mBins = bins;

    int numBars = mItems.size();
    if (bins < 1 || numBars < 1) {
        return;
    }

    while(mModel->canFetchMore(QModelIndex())) {
        mModel->fetchMore(QModelIndex());
    }

    for(int i=0;i<numBars;i++) {
        const DistributionPlotItem& item = mItems[i];
        QList<double> values_ = toDouble(filterNumeric(columnData(mModel,header.indexOf(item.v()))));
        values.append(values_);
    }

#if 0
    bool ok = false;
    for(int i=0;i<values.size();i++) {
        if (!values[i].isEmpty()) {
            mMin = values[i][0];
            mMax = values[i][0];
            ok = true;
            break;
        }
    }
    if (!ok) {
        return;
    }

    for(int i=0;i<numBars;i++) {
        const QList<double> values_ = values[i];
        if (values_.isEmpty()) {
            continue;
        }
        double min_ = *std::min_element(values_.begin(),values_.end());
        double max_ = *std::max_element(values_.begin(),values_.end());
        mMin = qMin(min_, mMin);
        mMax = qMax(max_, mMax);
    }

    double w = mMax - mMin;
    if (mMin == mMax) {
        return;
    }
#endif

    mMin = vmin;
    mMax = vmax;
    double w = mMax - mMin;

    while(hist.size() < bins) {
        hist.append(QVector<double>(numBars, 0.0));
    }

    for(int i=0;i<numBars;i++) {
        const QList<double> values_ = values[i];
        foreach(double v, values_) {
            if (v < mMin || v > mMax) {
                continue;
            }
            int index = qMin((int)((v - mMin) * (double) bins / w), hist.size() - 1);
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
    double w = mMax - mMin;
    QVector<QwtSetSample> samples;
    for(int i=0;i<hist.size();i++) {
        samples.append(QwtSetSample(mMin + (w * ((double)i + 0.5)) / mBins, hist[i]));
    }
    return samples;
}

QPair<double, double> Histogram::range(const QList<DistributionPlotItem> &items,
                                       const QStringList &header, QAbstractItemModel *mModel, bool* ok)
{

    *ok = true;

    int numBars = items.size();
    if (numBars < 1) {
        *ok = false;
        return QPair<double,double>(0,0);
    }

    while(mModel->canFetchMore(QModelIndex())) {
        mModel->fetchMore(QModelIndex());
    }

    QList<QList<double> > values;
    for(int i=0;i<numBars;i++) {
        const DistributionPlotItem& item = items[i];
        QList<double> values_ = toDouble(filterNumeric(columnData(mModel,header.indexOf(item.v()))));
        values.append(values_);
    }

    double mMin;
    double mMax;

    bool ok_ = false;
    for(int i=0;i<values.size();i++) {
        if (!values[i].isEmpty()) {
            mMin = values[i][0];
            mMax = values[i][0];
            ok_ = true;
            break;
        }
    }
    if (!ok_) {
        *ok = false;
        return QPair<double,double>(0,0);
    }

    for(int i=0;i<numBars;i++) {
        const QList<double> values_ = values[i];
        if (values_.isEmpty()) {
            continue;
        }
        double min_ = *std::min_element(values_.begin(),values_.end());
        double max_ = *std::max_element(values_.begin(),values_.end());
        mMin = qMin(min_, mMin);
        mMax = qMax(max_, mMax);
    }

    return QPair<double,double>(mMin,mMax);
}
