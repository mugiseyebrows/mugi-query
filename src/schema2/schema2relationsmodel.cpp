#include "schema2relationsmodel.h"

#include "schema2relation.h"

Schema2RelationsModel::Schema2RelationsModel(QObject *parent)
    : QAbstractTableModel{parent}
{

}

Schema2Relation* Schema2RelationsModel::insert(const QString &name, const QStringList &childColumns,
                                                   const QString &parentTable, const QStringList &parentColumns,
                                                       bool constrained, Status status) {

    if (contains(name)) {
        return 0;
    }
    Schema2Relation* relation = new Schema2Relation(name, childColumns, parentTable,
                                                    parentColumns, constrained, status);

    int index = mRelations.size();
    beginInsertRows(QModelIndex(), index, index);
    mRelations.append(relation);
    endInsertRows();

    return relation;
}


int Schema2RelationsModel::indexOf(const QString& name) const {
    for(int i=0;i<mRelations.size();i++) {
        if (mRelations[i]->name().toLower() == name.toLower()) {
            return i;
        }
    }
    return -1;
}

Schema2Relation *Schema2RelationsModel::get(const QString &name) const
{
    int index = indexOf(name);
    if (index > -1) {
        return mRelations[index];
    }
    return 0;
}

bool Schema2RelationsModel::contains(const QString &name) const
{
    return indexOf(name) > -1;
}

Schema2Relation * Schema2RelationsModel::removeAt(int index) {

    beginRemoveRows(QModelIndex(), index, index);
    Schema2Relation* relation = mRelations[index];
    mRelations.removeAt(index);
    endRemoveRows();
    return relation;
}

Schema2Relation *Schema2RelationsModel::remove(const QString &name)
{
    int index = indexOf(name);
    if (index < 0) {
        return 0;
    }
    return removeAt(index);
}

Schema2Relation *Schema2RelationsModel::remove(Schema2Relation * relation) {
    int index = mRelations.indexOf(relation);
    if (index < 0) {
        return 0;
    }
    return removeAt(index);
}

Schema2Relation *Schema2RelationsModel::getRelationTo(const QString &tableName) const
{
    for(Schema2Relation* relation: mRelations) {
        if (relation->parentTable().toLower() == tableName.toLower()) {
            return relation;
        }
    }
    return 0;
}

QList<Schema2Relation*> Schema2RelationsModel::values() const
{
    return mRelations;
}

Schema2Relation *Schema2RelationsModel::at(int index) const
{
    return mRelations[index];
}

int Schema2RelationsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return mRelations.size();
}

int Schema2RelationsModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return cols_count;
}

QVariant Schema2RelationsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch(index.column()) {
        case col_name: return mRelations[index.row()]->name();
        case col_parent_table: return mRelations[index.row()]->parentTable();
        case col_child_columns: return mRelations[index.row()]->childColumns().join(", ");
        case col_parent_columns: return mRelations[index.row()]->parentColumns().join(", ");
        }
    }
    if (role == Qt::CheckStateRole) {
        switch(index.column()) {
        case col_constrained: return mRelations[index.row()]->constrained() ? Qt::Checked : Qt::Unchecked;
        }
    }
    return QVariant();
}


QVariant Schema2RelationsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    static QHash<int, QString> header = {
        {col_name,"Name"},
        {col_child_columns,"Child columns"},
        {col_parent_table,"Parent table"},
        {col_parent_columns,"Parent columns"},
        {col_constrained,"Constrained"}
    };

    if (role == Qt::EditRole || role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            return header.value(section);
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}
