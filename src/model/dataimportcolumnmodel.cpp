#include "dataimportcolumnmodel.h"

DataImportColumnModel::DataImportColumnModel(const QStringList &names, const QList<QVariant::Type> &types, QObject *parent)
    : CheckableStringListModel(names, parent), mTypes(types)
{

}

void DataImportColumnModel::checked(QStringList& names, QList<QVariant::Type>& types) const {
    checkState(Qt::Checked, names, types);
}

void DataImportColumnModel::checkState(Qt::CheckState state, QStringList& names, QList<QVariant::Type>& types) const
{
    for(int r=0;r<rowCount();r++) {
        if (data(index(r,0),Qt::CheckStateRole).toInt() == state) {
            names << data(index(r,0)).toString();
            types << mTypes.value(r);
        }
    }
}
