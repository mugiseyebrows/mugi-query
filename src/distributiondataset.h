#ifndef DISTRIBUTIONDATASET_H
#define DISTRIBUTIONDATASET_H

#include "distributionplotitem.h"
#include <QList>

class QAbstractItemModel;

class DistributionDataset {
public:
    void update(const QList<DistributionPlotItem> &items, QAbstractItemModel *model);
    bool isEmpty() const;
    double min() const;
    double max() const;
    QList<double> values(int index) const;
    int size() const;
protected:
    double mMin;
    double mMax;
    QList<QList<double> > mValues;
};


#endif // DISTRIBUTIONDATASET_H
