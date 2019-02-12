#include "distributionplotmodel.h"
#include "rowvaluegetter.h"
#include "setheaderdata.h"
#include "lit.h"

DistributionPlotModel::DistributionPlotModel(QObject *parent) : QStandardItemModel(1,cols_size,parent)
{
    ::setHeaderData(this,Qt::Horizontal,Lit::sl("V","Color"));
}

QList<DistributionPlotItem> DistributionPlotModel::items(const QStringList& header) const
{
    QList<DistributionPlotItem> result;
    for(int row=0; row < rowCount(); row++) {
        RowValueGetter g(this,row);
        QString color = g(col_color).toString();
        int column = header.indexOf(g(col_x).toString());
        if (column > -1 && !color.isEmpty()) {
            result.append(DistributionPlotItem(column, color));
        }
    }
    return result;
}
