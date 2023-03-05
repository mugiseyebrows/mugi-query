#include "schema2tablemodel.h"

Schema2TableModel::Schema2TableModel(const QString &name, bool existing, QObject *parent)
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

bool Schema2TableModel::hasPendingChanges() const
{
    return false;
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

Qt::ItemFlags Schema2TableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (index.column() == col_newname || index.column() == col_newtype) {
        flags |= Qt::ItemIsEditable;
    }
    return flags;
}

bool Schema2TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole) {
        mColumns[index.row()][index.column()] = value.toString();
        emit dataChanged(index, index);
    }
    return false;
}

QVariant Schema2TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    static QHash<int, QString> header = {
        {col_name, "Name"},
        {col_newname, "Name"},
        {col_type, "Type"},
        {col_newtype, "Type"},
    };

    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            return header.value(section);
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}
