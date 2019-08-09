#include "distributionplotmodel.h"
#include "rowvaluegetter.h"
#include "setheaderdata.h"

DistributionPlotModel::DistributionPlotModel(QObject *parent) : QStandardItemModel(1,cols_size,parent)
{
    ::setHeaderData(this,Qt::Horizontal,{"V","Color"});
}

QList<DistributionPlotItem> DistributionPlotModel::items(const QStringList& header) const
{
    QList<DistributionPlotItem> result;
    for(int row=0; row < rowCount(); row++) {
        RowValueGetter g(this,row);
        QString v = g(col_v).toString();
        QString color = g(col_color).toString();
        int column = header.indexOf(v);
        if (column > -1 && !color.isEmpty()) {
            result.append(DistributionPlotItem(v, color));
        }
    }
    return result;
}
