#include "schema2storedmodel.h"

Schema2StoredModel::Schema2StoredModel(const QString &connectionName, QObject *parent)
    : QAbstractTableModel{parent}, mConnectionName(connectionName)
{}

int Schema2StoredModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return mData.size();
}

int Schema2StoredModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return ColumnCount;
}

QVariant Schema2StoredModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch(index.column()) {
        case ColumnSchema:
            return mData[index.row()].name.schema;
        case ColumnName:
            return mData[index.row()].name.name;
        case ColumnType:
            return mData[index.row()].type;
        }
    }
    return {};
}

QList<SStored> Schema2StoredModel::state() const
{
    return mData;
}

void Schema2StoredModel::merge(const SStoredDiff &diff)
{
    auto removed = diff.removed;
    for(const SName& name: removed.names) {
        int index = indexOf(mData, name);
        if (index > -1) {
            beginRemoveRows({}, index, index);
            mData.removeAt(index);
            endRemoveRows();
        }
    }
    auto created = diff.created;
    if (created.isEmpty()) {
        return;
    }
    beginInsertRows({}, mData.size(), mData.size() + created.size() - 1);
    mData.append(created);
    endInsertRows();
}
