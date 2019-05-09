#include "distributiondataset.h"

#include "datautils.h"

#include <QAbstractItemModel>

using namespace DataUtils;

void DistributionDataset::update(const QList<DistributionPlotItem> &items, QAbstractItemModel *model) {
    mValues.clear();
    while(model->canFetchMore(QModelIndex())) {
        model->fetchMore(QModelIndex());
    }

    QStringList header = toLower(headerData(model,Qt::Horizontal));

    for(int i=0;i<items.size();i++) {
        const DistributionPlotItem& item = items[i];
        QList<double> values = toDouble(filterNumeric(columnData(model,header.indexOf(item.v()))));
        mValues.append(values);
    }

    if (isEmpty()) {
        return;
    }

    for(int i=0;i<mValues.size();i++) {
        if (!mValues[i].isEmpty()) {
            mMin = mValues[i][0];
            mMax = mValues[i][0];
            break;
        }
    }

    for(int i=0;i<mValues.size();i++) {
        const QList<double>& values = mValues[i];
        if (values.isEmpty()) {
            continue;
        }
        double min = *std::min_element(values.begin(),values.end());
        double max = *std::max_element(values.begin(),values.end());
        mMin = qMin(min, mMin);
        mMax = qMax(max, mMax);
    }

}

bool DistributionDataset::isEmpty() const {
    for(int i=0;i<mValues.size();i++) {
        if (!mValues[i].isEmpty()) {
            return false;
        }
    }
    return true;
}

double DistributionDataset::min() const {
    return mMin;
}

double DistributionDataset::max() const {
    return mMax;
}

QList<double> DistributionDataset::values(int index) const {
    return mValues[index];
}

int DistributionDataset::size() const {
    return mValues.size();
}
