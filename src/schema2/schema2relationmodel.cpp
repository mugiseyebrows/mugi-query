#include "schema2relationmodel.h"

Schema2RelationModel::Schema2RelationModel(const QString &childTable, const QString &childColumn,
                                           const QString &parentTable, const QString &parentColumn,
                                           bool constrained, bool existing, QObject *parent)
    : mChildTable(childTable), mChildColumn(childColumn),
      mParentTable(parentTable), mParentColumn(parentColumn),
      mConstrained(constrained), mExisting(existing),
      QAbstractTableModel(parent)
{

}

int Schema2RelationModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return rows_count;
}

int Schema2RelationModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return 1;
}

QVariant Schema2RelationModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.row()) {
        case row_parent_table: return mParentTable;
        case row_parent_column: return mParentColumn;
        case row_child_table: return mChildTable;
        case row_child_column: return mChildColumn;
        }
    }
    return QVariant();
}
