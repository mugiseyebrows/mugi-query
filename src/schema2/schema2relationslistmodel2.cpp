#include "schema2relationslistmodel2.h"

#include "schema2relation.h"

Schema2RelationsListModel2::Schema2RelationsListModel2(const QList<Schema2Relation*> relations, QObject *parent)
    : mRelations(relations), QAbstractTableModel{parent}
{

}

int Schema2RelationsListModel2::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return mRelations.size();
}

int Schema2RelationsListModel2::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return 1;
}

QVariant Schema2RelationsListModel2::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole) {

        if (index.column() == 0) {
            return mRelations[index.row()]->parentTable();
        }
    }
    return QVariant();
}
