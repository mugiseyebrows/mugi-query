#include "histogram.h"

#include "datautils.h"
#include <QAbstractItemModel>
#include "math.h"

using namespace DataUtils;

namespace  {

double round_(double sum, double prec) {
    return round((sum)*pow(10.0,prec))/pow(10.0,prec);
}

double round0(double sum) {
    return round_(sum,0.0);
}

double round2(double sum) {
    return round_(sum,2.0);
}

double round1(double sum) {
    return round_(sum,1.0);
}

bool equal(double v1, double v2, double eps) {
    return fabs(v1 - v2) < eps;
}

/*
int sampleStatPrec(int n) {
    double v =  100.0 / (double) n;
    double eps = 1e-6;
    if (equal(round0(v), v, eps)) {
        return 0;
    }
    if (equal(round1(v), v, eps)) {
        return 1;
    }
    return 2;
}*/

}

Histogram::Histogram(int bins, const DistributionDataset &dataset, double min, double max) {

    mBins = bins;
    mMin = min;
    mMax = max;

    if (bins < 1) {
        return;
    }

    while(mHist.size() < bins) {
        mHist.append(QVector<double>(dataset.size(), 0.0));
    }

    for(int i=0;i<dataset.size();i++) {
        const QList<double>& values = dataset.values(i);
        int filtered = 0;
        foreach(double v, values) {
            if (v < mMin || v > mMax) {
                continue;
            }
            int index = qMin((int)((v - mMin) * (double) bins / (mMax - mMin)), mHist.size() - 1);
            mHist[index][i] += 1.0;
            filtered += 1;
        }
        mTotal << values.size();
        mFiltered << filtered;
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
    for(int i=0;i<mHist.size();i++) {
        samples.append(QwtSetSample(mMin + ((mMax - mMin) * ((double)i + 0.5)) / (double) mBins, mHist[i]));
    }
    return samples;
}

int Histogram::prec() {
    int prec = (int) qMax(0.0, log10(500 * mBins/(mMax - mMin)));
    return prec;
}

int Histogram::total(int index) const
{
    return mTotal[index];
}

int Histogram::filtered(int index) const
{
    return mFiltered[index];
}

QList<QList<double> > Histogram::histRows(int index) const
{
    double d = (mMax - mMin) / mBins;
    QList<QList<double> > result;
    for(int i=0;i<mHist.size();i++) { // bins
        double n = mHist[i][index];
        double total = mTotal[index];
        double filtered = mFiltered[index];
        double vmin = mMin + d * i;
        double vmax = mMin + d * (i + 1);
        double vmid = (vmin + vmax) / 2;

        QList<double> row;
        row << vmin << vmax << vmid << n << n / filtered * 100 << n / total * 100;
        result << row;
    }

    return result;
}

int Histogram::size() const
{
    return mFiltered.size();
}
