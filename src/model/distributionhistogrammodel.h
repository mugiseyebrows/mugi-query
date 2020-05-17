#ifndef DISTRIBUTIONHISTOGRAMMODEL_H
#define DISTRIBUTIONHISTOGRAMMODEL_H

#include <QStandardItemModel>
class Histogram;

class DistributionHistogramModel : public QStandardItemModel
{
    Q_OBJECT
public:
    enum cols {
        col_min,
        col_max,
        col_mid,
        col_n,
        col_percent1,
        col_percent2,
        cols_size
    };

    DistributionHistogramModel(QObject *parent = nullptr);

    void setHistogram(const Histogram &hist);
    void setRowCount(int count_);
};

#endif // DISTRIBUTIONHISTOGRAMMODEL_H
