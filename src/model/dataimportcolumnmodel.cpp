#include "dataimportcolumnmodel.h"

DataImportColumnModel::DataImportColumnModel(const QStringList &names, const QList<QMetaType::Type> &types, QObject *parent)
    : CheckableStringListModel(names, parent), mTypes(types)
{

}

void DataImportColumnModel::checked(QStringList& names, QList<QMetaType::Type>& types) const {
    checkState(Qt::Checked, names, types);
}

void DataImportColumnModel::checkState(Qt::CheckState state, QStringList& names, QList<QMetaType::Type>& types) const
{
    for(int r=0;r<rowCount();r++) {
        if (data(index(r,0),Qt::CheckStateRole).toInt() == state) {
            names << data(index(r,0)).toString();
            types << mTypes.value(r);
        }
    }
}

QList<bool> DataImportColumnModel::checkList() const {
    QList<bool> res;
    for(int r=0;r<rowCount();r++) {
        res << (data(index(r,0),Qt::CheckStateRole).toInt() == Qt::Checked);
    }
    return res;
}
