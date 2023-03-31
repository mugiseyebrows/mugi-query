#include "schema2tablemodel.h"

#include "schema2index.h"
#include "schema2relation.h"
#include "schema2indexesmodel.h"
#include "schema2relationsmodel.h"
#include <QSqlDriver>
#include "sqlescaper.h"
#include "filterempty.h"
#include "drivernames.h"

// todo table renames

Schema2TableModel::Schema2TableModel(const QString &name, Status status, QObject *parent)
    : mTableName(name), mStatus(status), mRelations(new Schema2RelationsModel(this)), mIndexes(new Schema2IndexesModel(this)), QAbstractTableModel{parent}
{

}

void Schema2TableModel::insertColumnsIfNotContains(const QString &name, const QString &type, bool notNull,
                                                   const QString& prev)
{
    int insertRow = rowCount();

    for(int row=0;row<mColumns.size();row++) {
        if (this->namePrev(row) == name) {
            return;
        }
        if (this->namePrev(row) == prev) {
            insertRow = row + 1;
        }
    }

    beginInsertRows(QModelIndex(), insertRow, insertRow);
    //mColumns.insert(insertRow, {name, name, type, type});
    mColumns.insert(insertRow, Schema2TableColumn(name, type, notNull));
    mColumnsPrev.insert(insertRow, Schema2TableColumn(name, type, notNull));
    endInsertRows();
}

bool Schema2TableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(QModelIndex(), row, row + count - 1);
    for(int i=0;i<count;i++) {
        mColumns.insert(row, Schema2TableColumn());
        mColumnsPrev.insert(row, Schema2TableColumn());
    }
    endInsertRows();
    return true;
}

QString Schema2TableModel::tableName() const
{
    return mTableName;
}

QString Schema2TableModel::namePrev(int row) const
{
    return mColumnsPrev[row].name;
}

QString Schema2TableModel::name(int row) const
{
    return mColumns[row].name;
}

QString Schema2TableModel::typePrev(int row) const
{
    return mColumnsPrev[row].type;
}

QString Schema2TableModel::type(int row) const
{
    return mColumns[row].type;
}

bool Schema2TableModel::notNull(int row) const
{
    return mColumns[row].notNull;
}

bool Schema2TableModel::notNullPrev(int row) const
{
    return mColumnsPrev[row].notNull;
}

bool Schema2TableModel::hasPendingChanges() const
{
    return false;
}

QStringList Schema2TableModel::columnNames() const
{
    QStringList res;
    for(int row=0;row<mColumns.size();row++) {
        res.append(mColumns[row].name);
    }
    return res;
}

void Schema2TableModel::setName(int row, const QString &value)
{
    mColumns[row].name = value;
}

void Schema2TableModel::setType(int row, const QString &value)
{
    mColumns[row].type = value;
}

#if 0
Schema2Index *Schema2TableModel::getIndex(const QString &name) const
{
    return mIndexes.get(name);
}

QList<Schema2Index *> Schema2TableModel::getIndexes() const {
    return mIndexes.values();
}
#endif

void Schema2TableModel::insertIndex(const QString &name, const QStringList &columns, bool primary, bool unique, Status status)
{
    //mIndexes.set(name, new Schema2Index(name, columns, primary, status));

    mIndexes->insertIndex(name, columns, primary, unique, status);

}

void Schema2TableModel::removeIndex(const QString &name)
{
    //mIndexes.remove(name);
    mIndexes->removeIndex(name);
}

#if 0
bool Schema2TableModel::isIndexColumn(const QString &column) const
{
    QList<Schema2Index*> indexes = mIndexes.values();
    for(Schema2Index* index: indexes) {
        if (index->isIndexColumn(column)) {
            return true;
        }
    }
    return false;
}
#endif

bool Schema2TableModel::containsRelation(const QString &name) const
{
    return mRelations->contains(name);
}

Schema2Relation* Schema2TableModel::insertRelation(const QString &name, const QStringList &childColumns,
                                                   const QString &parentTable, const QStringList &parentColumns,
                                                   bool constrained, Status status)
{
    return mRelations->insert(name, childColumns, parentTable, parentColumns, constrained, status);
}

Schema2Relation* Schema2TableModel::removeRelation(const QString &name)
{
    return mRelations->remove(name);
}

Schema2Relation *Schema2TableModel::removeRelation(Schema2Relation *relation)
{
    return mRelations->remove(relation);
}

Schema2Relation *Schema2TableModel::relation(const QString &name) const
{
    return mRelations->relation(name);
}

Schema2RelationsModel *Schema2TableModel::relations() const
{
    return mRelations;
}

Schema2Relation *Schema2TableModel::relationTo(const QString &tableName) const
{
    return mRelations->getRelationTo(tableName);
}

void Schema2TableModel::pushed() {
    mStatus = StatusExisting;
    for(int row=0;row<rowCount();row++) {
# if 0
        QString newName = this->name(row);
        QString newType = this->type(row);
        setData(index(row, col_name), newName);
        setData(index(row, col_type), newType);
#endif
        mColumnsPrev[row] = mColumns[row];
    }
    mDropColumnsQueue.clear();
}

Status Schema2TableModel::status() const {
    if (mStatus == StatusNew) {
        return StatusNew;
    }
    if (mStatus == StatusExisting) {
        if (!mDropColumnsQueue.isEmpty()) {
            return StatusModified;
        }
        for(int row=0;row<rowCount();row++) {
            if (name(row).isEmpty()) {
                continue;
            }
            if (namePrev(row) != name(row) || typePrev(row) != type(row)) {
                return StatusModified;
            }
        }
        return StatusExisting;
    }
    return mStatus;
}

QStringList Schema2TableModel::createQueries(const QString &driverName, QSqlDriver *driver) const {
    SqlEscaper es(driver);
    QStringList fields;
    for(int row=0;row<rowCount();row++) {
        QString name = this->name(row);
        QString type = this->type(row);
        bool notNull = this->notNull(row);
        if (name.isEmpty() || type.isEmpty()) {
            continue;
        }
        QString field = filterEmpty({es.field(name), type, notNull ? "NOT NULL" : ""}).join(" ");
        fields.append(field);
    }
    QString expr = QString("CREATE TABLE %1 (%2)")
            .arg(es.table(mTableName))
            .arg(fields.join(", "));
    return {expr};
}

QStringList Schema2TableModel::alterQueries(const QString &driverName, QSqlDriver *driver) const {

    QStringList res;
    SqlEscaper es(driver);

    for(const QString& colName: mDropColumnsQueue) {
        // todo check for removed and readded
        QString expr = QString("ALTER TABLE %1 DROP COLUMN %2")
                .arg(es.table(mTableName))
                .arg(es.field(colName));
        res.append(expr);
    }

    for(int row=0;row<rowCount();row++) {

        QString namePrev = this->namePrev(row);
        QString name = this->name(row);
        QString typePrev = this->typePrev(row);
        QString type = this->type(row);
        bool notNull = this->notNull(row);

        bool nameChanged = name != namePrev;
        bool typeChanged = type != typePrev;
        bool notNullChanged = notNull != notNull;

        if (name.isEmpty()) {

        } else {
            if (namePrev.isEmpty()) {

                QString expr = filterEmpty({"ALTER TABLE", es.table(mTableName),
                                            "ADD COLUMN",
                                            es.field(name), type, notNull ? "NOT NULL" : ""}).join(" ");

                res.append(expr);

            } else if (nameChanged || typeChanged || notNullChanged) {

                if (driverName == DRIVER_ODBC) {

                    if (name == namePrev) {

                        QString expr = filterEmpty({"ALTER TABLE", es.table(mTableName),
                                                    "ALTER COLUMN",
                                                    es.field(name), type, notNull ? "NOT NULL" : ""}).join(" ");

                        res.append(expr);

                    } else {

                        // todo forbid renaming existing fields for DRIVER_ODBC

                    }

                } else if (driverName == DRIVER_MYSQL || driverName == DRIVER_MARIADB) {

                    QString expr = filterEmpty({"ALTER TABLE", es.table(mTableName),
                                                "CHANGE COLUMN", es.field(namePrev), es.field(name), type, notNull ? "NOT NULL" : ""}).join(" ");

                    res.append(expr);
                } else {

                    // todo implement column modifying for DRIVER_PSQL

                }

            } else if (namePrev != name) {
                // todo implement column renaming for supported drivers
            }
        }
    }

    return res;
}

QStringList Schema2TableModel::dropQueries(const QString &driverName, QSqlDriver *driver) const {
    SqlEscaper es(driver);
    QString expr = QString("DROP TABLE %1").arg(es.table(mTableName));
    return {expr};
}

void Schema2TableModel::setStatus(Status status) {
    mStatus = status;
}

Schema2IndexesModel *Schema2TableModel::indexes() const {
    return mIndexes;
}

bool Schema2TableModel::contains(Schema2Relation *relation)
{
    return mRelations->values().contains(relation);
}

QStringList Schema2TableModel::relatedTables() const
{
    QStringList res;
    auto relations = mRelations->values();
    for(auto* relation: relations) {
        res.append(relation->parentTable());
    }
    return res;
}

QStringList Schema2TableModel::relationNames() const
{
    QStringList res;
    auto relations = mRelations->values();
    for(auto* relation: relations) {
        res.append(relation->name());
    }
    return res;
}

QStringList Schema2TableModel::indexNames() const
{
    QStringList res;
    auto indexes = mIndexes->values();
    for(auto* index: indexes) {
        res.append(index->name());
    }
    return res;
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
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        int row = index.row();
        switch(index.column()) {
        case col_name: return mColumns[row].name;
        case col_type: return mColumns[row].type;

        case col_name_prev: return mColumnsPrev[row].name;
        case col_type_prev: return mColumnsPrev[row].type;
        }
    }
    if (role == Qt::CheckStateRole) {
        int row = index.row();
        switch(index.column()) {
        case col_notnull: return mColumns[row].notNull ? Qt::Checked : Qt::Unchecked;
        case col_notnull_prev: return mColumnsPrev[row].notNull ? Qt::Checked : Qt::Unchecked;
        }
    }

    return QVariant();
}

Qt::ItemFlags Schema2TableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (index.column() == col_name || index.column() == col_type) {
        flags |= Qt::ItemIsEditable;
    }
    if (index.column() == col_notnull) {
        flags |= Qt::ItemIsUserCheckable;
    }
    return flags;
}

bool Schema2TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole) {

        int row = index.row();
        switch(index.column()) {
        case col_name: mColumns[row].name = value.toString(); break;
        case col_type: mColumns[row].type = value.toString(); break;
        case col_name_prev: mColumnsPrev[row].name = value.toString(); break;
        case col_type_prev: mColumnsPrev[row].type = value.toString(); break;
        }

        emit dataChanged(index, index);
    } else if (role == Qt::CheckStateRole) {

        int row = index.row();
        switch(index.column()) {
        case col_notnull: mColumns[row].notNull = value.toInt() == Qt::Checked; break;
        case col_notnull_prev: mColumnsPrev[row].notNull = value.toInt() == Qt::Checked; break;
        }
        emit dataChanged(index, index);
    }

    return false;
}

QVariant Schema2TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    static QHash<int, QString> header = {
        {col_name, "Name"},
        {col_notnull, "Non null"},
        {col_type, "Type"},

        {col_name_prev, "Name"},
        {col_notnull_prev, "Non null"},
        {col_type_prev, "Type"},
    };

    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            return header.value(section);
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

bool Schema2TableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(QModelIndex(), row, row+count-1);
    for(int i=0;i<count;i++) {
        QString colName = mColumnsPrev[row].name;
        if (!colName.isEmpty()) {
            mDropColumnsQueue.append(colName);
        }
        mColumns.removeAt(row);
        mColumnsPrev.removeAt(row);
    }
    endRemoveRows();
}
