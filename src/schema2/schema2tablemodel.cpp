#include "schema2tablemodel.h"

Schema2TableModel::Schema2TableModel(const QString &name, QObject *parent)
    : mName(name), QAbstractTableModel{parent}
{

}

void Schema2TableModel::insertIfNotContains(const QString &name, const QString &type, const QString& prev)
{
    int insertRow = rowCount();

    for(int row=0;row<mColumns.size();row++) {
        if (this->name(row) == name) {
            return;
        }
        if (this->name(row) == prev) {
            insertRow = row + 1;
        }
    }

    beginInsertRows(QModelIndex(), insertRow, insertRow);
    mColumns.insert(insertRow, {name, name, type, type});
    endInsertRows();
}

QString Schema2TableModel::tableName() const
{
    return mName;
}

QString Schema2TableModel::name(int row) const
{
    return mColumns[row][col_name];
}

QString Schema2TableModel::newName(int row) const
{
    return mColumns[row][col_newname];
}

QString Schema2TableModel::type(int row) const
{
    return mColumns[row][col_type];
}

QString Schema2TableModel::newType(int row) const
{
    return mColumns[row][col_newtype];
}

int Schema2TableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return mColumns.size();
}

int Schema2TableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return cols_count;
}

QVariant Schema2TableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        return mColumns[index.row()][index.column()];
    }
    return QVariant();
}
