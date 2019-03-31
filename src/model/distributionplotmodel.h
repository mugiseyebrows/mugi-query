#ifndef DISTRIBUTIONPLOTMODEL_H
#define DISTRIBUTIONPLOTMODEL_H

#include <QObject>
#include <QStandardItemModel>
#include "distributionplotitem.h"

class DistributionPlotModel : public QStandardItemModel
{
    Q_OBJECT
public:
    enum cols {
        col_v,
        col_color,
        cols_size,
    };
    DistributionPlotModel(QObject *parent = nullptr);

    QList<DistributionPlotItem> items(const QStringList &header) const;

};

#endif // DISTRIBUTIONPLOTMODEL_H
