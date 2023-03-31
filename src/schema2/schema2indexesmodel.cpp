#include "schema2indexesmodel.h"
#include "schema2index.h"

#include <QDebug>

Schema2IndexesModel::Schema2IndexesModel(QObject *parent)
    : QAbstractTableModel{parent}
{

}

int Schema2IndexesModel::find(const QString& name) const {
    for(int i=0;i<mIndexes.size();i++) {
        if (mIndexes[i]->name().toLower() == name.toLower()) {
            return i;
        }
    }
    return -1;
}

void Schema2IndexesModel::insertIndex(const QString &name, const QStringList &columns, bool primary, bool unique, Status status)
{

    int row = find(name);
    if (row > -1) {
        return;
    }

    row = rowCount();
    beginInsertRows(QModelIndex(), row, row);
    mIndexes.insert(row, new Schema2Index(name, columns, primary, unique, status));
    endInsertRows();
}

void Schema2IndexesModel::removeIndex(const QString &name)
{
    int row = find(name);
    if (row < 0) {
        return;
    }
    removeAt(row);
}

void Schema2IndexesModel::removeAt(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    mRemoveQueue.append(mIndexes[row]);
    mIndexes.removeAt(row);
    endRemoveRows();
}

Schema2Index *Schema2IndexesModel::index(const QString &name) const {
    int index = find(name);
    if (index > -1) {
        return mIndexes[index];
    }
    return 0;
}

QStringList Schema2IndexesModel::queries(const QString& tableName, const QString& driverName, QSqlDriver* driver) const
{
    QStringList res;
    for(int i=0;i<mIndexes.size();i++) {
        auto* index = mIndexes[i];
        if (index->status() != StatusExisting) {
            res.append(index->createQuery(tableName, driverName, driver));
        }
    }
    for(int i=0;i<mRemoveQueue.size();i++) {
        auto* index = mRemoveQueue[i];
        res.append(index->dropQuery(tableName, driverName, driver));
    }
    return res;
}

void Schema2IndexesModel::pushed()
{
    for(int i=0;i<mIndexes.size();i++) {
        auto* index = mIndexes[i];
        index->setStatus(StatusExisting);
    }
    mRemoveQueue.clear();
}

QStringList Schema2IndexesModel::primaryKey() const
{
    for(int i=0;i<mIndexes.size();i++) {
        auto* index = mIndexes[i];
        if (index->primary()) {
            return index->columns();
        }
    }
    return QStringList();
}

void Schema2IndexesModel::debugStatus() {
    for(int i=0;i<mIndexes.size();i++) {
        auto* index = mIndexes[i];
        qDebug() << index->name() << index->status();
    }
}

int Schema2IndexesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return mIndexes.size();
}

int Schema2IndexesModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return cols_count;
}

QVariant Schema2IndexesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    if (role == Qt::DisplayRole || role == Qt::EditRole) {

        switch(index.column()) {
        case col_name: return mIndexes[index.row()]->name();
        case col_columns: return mIndexes[index.row()]->columns().join(", ");
        }
    }
    if (role == Qt::CheckStateRole) {
        switch(index.column()) {
        case col_primary: return mIndexes[index.row()]->primary() ? Qt::Checked : Qt::Unchecked;
        case col_unique: return mIndexes[index.row()]->unique() ? Qt::Checked : Qt::Unchecked;
        }
    }

    return QVariant();
}

QVariant Schema2IndexesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    static QHash<int, QString> header = {
        {col_name, "Name"},
        {col_columns, "Columns"},
        {col_primary, "Primary"},
        {col_unique, "Unique"},
    };

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        if (orientation == Qt::Horizontal) {
            return header.value(section);
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}
