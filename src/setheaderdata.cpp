#include "setheaderdata.h"

#include <QAbstractItemModel>

void setHeaderData(QAbstractItemModel *model, Qt::Orientation orientation, const QStringList &values) {
    for(int section=0;section<values.size();section++) {
        model->setHeaderData(section,orientation,values[section]);
    }
}
