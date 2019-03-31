#ifndef XYPLOTMODEL_H
#define XYPLOTMODEL_H

#include <QObject>
#include <QStandardItemModel>

#include "xyplotmodelitem.h"

class XYPlotModel : public QStandardItemModel
{
    Q_OBJECT
public:
    enum cols {
        col_x,
        col_y,
        col_line,
        col_marker,
        cols_size
    };
    XYPlotModel(QObject* parent = 0);

    QList<XYPlotModelItem> items(const QStringList& header) const;

};

#endif // XYPLOTMODEL_H
