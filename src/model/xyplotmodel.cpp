#include "xyplotmodel.h"
#include "rowvaluegetter.h"
#include "lit.h"

#include "setheaderdata.h"

XYPlotModel::XYPlotModel(QObject *parent) : QStandardItemModel(1,cols_size,parent)
{
    ::setHeaderData(this,Qt::Horizontal,Lit::sl("X","Y","Line","Symbol"));
}

QList<XYPlotModelItem> XYPlotModel::items(const QStringList& header) const
{
    QList<XYPlotModelItem> result;
    for(int row=0; row<rowCount();row++) {
        RowValueGetter g(this,row);
        QString x = g(col_x).toString();
        QString y = g(col_y).toString();
        QString line = g(col_line).toString();
        QString marker = g(col_marker).toString();
        if (y.isEmpty() || (!x.isEmpty() && y.isEmpty()) || !header.contains(y)) {
            continue;
        }
        result.append(XYPlotModelItem(x,y,line,marker));
    }
    return result;
}
