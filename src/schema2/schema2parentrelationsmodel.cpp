#include "schema2parentrelationsmodel.h"

#include "schema2tablesmodel.h"
#include "schema2tablemodel.h"
#include "schema2relationsmodel.h"
#include "schema2relationmodel.h"


Schema2ParentRelationsModel::Schema2ParentRelationsModel(Schema2TableModel *table, QObject *parent)
: mTable(table), QAbstractTableModel{parent}
{

}

void Schema2ParentRelationsModel::update(Schema2TablesModel *tablesModel)
{
    auto tables = tablesModel->tables();
    QString tableName = mTable->tableName().toLower();
    QList<Schema2Relation*> res;
    for(auto* table: tables) {
        auto relations = table->relations()->values();
        for(auto* relation: relations) {
            if (relation->parentTable().toLower() == tableName) {
                res.append(relation);
            }
        }
    }
    setItems(res);
}

Schema2Relation *Schema2ParentRelationsModel::at(int index) const
{
    return mRelations[index];
}

QList<Schema2Relation *> Schema2ParentRelationsModel::values() const
{
    return mRelations;
}

void Schema2ParentRelationsModel::beginSetRowCount(int rows, int rowsBefore) {
    if (rows < rowsBefore) {
        int first = rows;
        int last = rowsBefore - 1;
        beginRemoveRows(QModelIndex(), first, last);
    } else if (rows > rowsBefore) {
        int first = rowsBefore;
        int last = rows - 1;
        beginInsertRows(QModelIndex(), first, last);
    }
}

void Schema2ParentRelationsModel::endSetRowCount(int rows, int rowsBefore) {
    if (rows < rowsBefore) {
        endRemoveRows();
    } else if (rows > rowsBefore) {
        endInsertRows();
    }
}

void Schema2ParentRelationsModel::setItems(const QList<Schema2Relation *> &relations)
{
    int rowsBefore = rowCount();
    beginSetRowCount(relations.size(), rowsBefore);
    mRelations = relations;
    endSetRowCount(relations.size(), rowsBefore);
}

int Schema2ParentRelationsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return mRelations.size();
}

int Schema2ParentRelationsModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return cols_count;
}

QVariant Schema2ParentRelationsModel::data(const QModelIndex &index, int role) const
{

    if (!index.isValid()) {
        return QVariant();
    }
#if 0
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        int row = index.row();
        switch(index.column()) {
        case col_name: return mRelations[row].second->name();
        case col_parent_table: return mRelations[row].second->parentTable();
        case col_child_columns: return mRelations[row].second->childColumns().join(", ");
        case col_parent_columns: return mRelations[row].second->parentColumns().join(", ");
        case col_child_table: return mRelations[row].first;
        }
    }
    return QVariant();
#endif
    return mRelations[index.row()]->data(index.column(), role);
}


QVariant Schema2ParentRelationsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
#if 0
    static QHash<int, QString> header = {
        {col_name, "Name"},
        {col_child_table, "Child table"},
        {col_child_columns, "Child column"},
        {col_parent_table, "Parent table"},
        {col_parent_columns, "Parent columns"},
        {col_constrained, "Constrained"}
    };

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        if (orientation == Qt::Horizontal) {
            return header.value(section);
        }
    }
    return QVariant();
#endif
    auto value = Schema2Relation::headerData(section, orientation, role);
    if (!value.isNull()) {
        return value;
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}
