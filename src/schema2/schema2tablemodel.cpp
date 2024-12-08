#include "schema2tablemodel.h"

#include "schema2index.h"
#include "schema2relation.h"
#include "schema2indexesmodel.h"
#include "schema2relationsmodel.h"
#include <QSqlDriver>
#include "sqlescaper.h"
#include "filterempty.h"
#include "drivernames.h"
#include "schema2parentrelationsmodel.h"
#include "sdata.h"
#include "columnposition.h"

// todo table renames

Schema2TableModel::Schema2TableModel(const QString &name, Status status, QObject *parent)
    : mTableName(name), mTableNamePrev(name), mStatus(status), mRelations(new Schema2RelationsModel(this, this)),
      mParentRelations(new Schema2ParentRelationsModel(this)),
      mIndexes(new Schema2IndexesModel(this)), QAbstractTableModel{parent}
{

}

void Schema2TableModel::tableAltered(const STable& table) {

    QStringList state = this->columnNames();
    QStringList newState = table.columnNames();

    //qDebug() << "Schema2TableModel::tableAltered";

    // removed
    for(auto it=state.rbegin();it!=state.rend();it++) {
        QString name = *it;
        int index = newState.indexOf(name);
        if (index < 0) {
            index = state.indexOf(name);
            //qDebug() << "remove" << name;
            beginRemoveRows(QModelIndex(), index, index);
            mColumns.removeAt(index);
            mColumnsPrev.removeAt(index);
            endRemoveRows();
        }
    }

    // inserted and changed
    for(int i=0;i<table.columns.size();i++) {
        auto column = table.columns[i];
        int index = indexOf(column.name);
        if (index > -1) {
            // todo keep user changes
            mColumns[index] = column;
            mColumnsPrev[index] = column;
            //qDebug() << "replace" << column.name << index;
        } else {
            int index = mColumns.size();
            if (i > 0) {
                int indexPrev = indexOf(table.columns[i-1].name);
                if (indexPrev > -1) {
                    index = indexPrev + 1;
                }
            }
            //qDebug() << "insert" << column.name << index;
            beginInsertRows(QModelIndex(), index, index);
            mColumns.insert(index, column);
            mColumnsPrev.insert(index, column);
            endInsertRows();
        }
    }

}

int Schema2TableModel::indexOf(const QString& name) {
    for(int row=0;row<mColumns.size();row++) {
        if (mColumns[row].name == name) {
            return row;
        }
    }
    return -1;
}

bool Schema2TableModel::updateColumn(const SColumn& column, const QString& prev)
{
    int insertRow = rowCount();

    QString name_ = column.name.toLower();
    QString prev_ = prev.toLower();

    for(int row=0;row<mColumns.size();row++) {
        if (this->namePrev(row).toLower() == name_) {
            return false;
        }
        if (this->namePrev(row).toLower() == prev_) {
            insertRow = row + 1;
        }
    }

    beginInsertRows(QModelIndex(), insertRow, insertRow);
    //mColumns.insert(insertRow, {name, name, type, type});
    mColumns.insert(insertRow, column);
    mColumnsPrev.insert(insertRow, column);
    endInsertRows();
    return true;
}

bool Schema2TableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(QModelIndex(), row, row + count - 1);
    for(int i=0;i<count;i++) {
        mColumns.insert(row, SColumn());
        mColumnsPrev.insert(row, SColumn());
    }
    endInsertRows();
    return true;
}

QString Schema2TableModel::tableName() const
{
    return mTableName;
}

QString Schema2TableModel::tableNamePrev() const
{
    return mTableNamePrev;
}

void Schema2TableModel::setTableName(const QString &value)
{
    mTableName = value;
}

void Schema2TableModel::setTableNamePrev(const QString &value)
{
    mTableNamePrev = value;
}

QString Schema2TableModel::namePrev(int row) const
{
    return mColumnsPrev[row].name;
}

QString Schema2TableModel::name(int row) const
{
    if (row < 0 || row >= mColumns.size()) {
        return QString();
    }
    return mColumns[row].name;
}

QString Schema2TableModel::typePrev(int row) const
{
    return mColumnsPrev[row].type;
}

QString Schema2TableModel::type(int row) const
{
    if (row < 0 || row >= mColumns.size()) {
        return QString();
    }
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

bool Schema2TableModel::autoincrement(int row) const {
    return mColumns[row].autoIncrement;
}

bool Schema2TableModel::autoincrementPrev(int row) const {
    return mColumnsPrev[row].autoIncrement;
}

QString Schema2TableModel::default_(int row) const {
    return mColumns[row].default_;
}

QString Schema2TableModel::defaultPrev(int row) const {
    return mColumnsPrev[row].default_;
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

void Schema2TableModel::updateParentRelations(Schema2TablesModel* tables) {
    mParentRelations->update(tables);
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

QList<Schema2Relation*> Schema2TableModel::relationsTo(const QString &tableName) const
{
    return mRelations->getRelationsTo(tableName);
}

void Schema2TableModel::pushed(bool created) {
    mStatus = StatusExisting;
    for(int row=0;row<rowCount();row++) {
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
        if (mTableName != mTableNamePrev) {
            return StatusModified;
        }
        for(int row=0;row<rowCount();row++) {
            if (name(row).isEmpty()) {
                continue;
            }
            if (namePrev(row) != name(row)
                    || typePrev(row) != type(row)
                    || notNullPrev(row) != notNull(row)
                    || defaultPrev(row) != default_(row)
                    || autoincrementPrev(row) != autoincrement(row)) {
                return StatusModified;
            }
        }
        return StatusExisting;
    }
    return mStatus;
}

QStringList Schema2TableModel::createQueries(const QString &driverName, QSqlDriver *driver) const {
    SqlEscaper es(driver);
    QStringList columns;

    QStringList primaryKey = this->indexes()->primaryKey();

    for(int row=0;row<rowCount();row++) {
        QString name = this->name(row);
        QString type = this->type(row);
        if (name.isEmpty() || type.isEmpty()) {
            continue;
        }
        QString columnDefinition = getColumnDefinition(driverName, driver, row, false, primaryKey, {});
        columns.append(columnDefinition);
    }
    if (primaryKey.size() > 1) {
        columns.append(QString("CONSTRAINT PK_%1 PRIMARY KEY (%2)").arg(mTableName).arg(primaryKey.join(", ")));
    }

    QString expr = QString("CREATE TABLE %1 (%2)")
            .arg(es.table(mTableName))
            .arg(columns.join(", "));
    return {expr};
}

QStringList Schema2TableModel::renameQueries(const QString &driverName, QSqlDriver *driver) const {
    QStringList res;
    SqlEscaper es(driver);
    if (mTableName != mTableNamePrev) {
        QString expr = QString("RENAME TABLE %1 TO %2").arg(es.table(mTableNamePrev)).arg(es.table(mTableName));
        res.append(expr);
    }
    return res;
}

QString Schema2TableModel::alterTableAddColumnsQuery(int row, const QString &driverName, QSqlDriver *driver) const {

    QStringList primaryKey = indexes()->primaryKey();
    SqlEscaper es(driver);

    ColumnPosition position;
    if (row == 0) {
        position = ColumnPosition(ColumnPosition::First);
    } else {
        position = ColumnPosition(ColumnPosition::After, this->name(row-1));
    }

    QString columnDefinition = getColumnDefinition(driverName, driver, row, false, primaryKey, position);
    QString expr = QStringList{"ALTER TABLE", es.table(mTableName),
                                "ADD COLUMN", columnDefinition}.join(" ");
    return expr;
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

    QStringList primaryKey = indexes()->primaryKey();

    for(int row=0;row<rowCount();row++) {

        QString namePrev = this->namePrev(row);
        QString name = this->name(row);
        QString typePrev = this->typePrev(row);
        QString type = this->type(row);
        bool notNull = this->notNull(row);
        bool notNullPrev = this->notNullPrev(row);
        bool autoincrement = this->autoincrement(row);
        bool autoincrementPrev = this->autoincrementPrev(row);
        QString default_ = this->default_(row);
        QString defaultPrev = this->defaultPrev(row);

        bool nameChanged = name != namePrev;
        bool typeChanged = type != typePrev;
        bool notNullChanged = notNull != notNullPrev;
        bool autoincrementChanged = autoincrement != autoincrementPrev;
        bool defaultChanged = default_ != defaultPrev;

        if (name.isEmpty()) {

        } else {
            if (namePrev.isEmpty()) {

#if 0
                QString after = row > 0 ? this->name(row-1) : QString();

                QString column_definition = columnDefinition(driverName, driver, row, false, primaryKey, after);

                QString expr = QStringList{"ALTER TABLE", es.table(mTableName),
                                            "ADD COLUMN", column_definition}.join(" ");
#endif
                QString expr = alterTableAddColumnsQuery(row, driverName, driver);
                res.append(expr);

            } else if (nameChanged || typeChanged || notNullChanged || autoincrementChanged || defaultChanged) {

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

                    QString columnDefinition = getColumnDefinition(driverName, driver, row, false, primaryKey, {});

                    QString expr = filterEmpty({"ALTER TABLE", es.table(mTableName),
                                                "CHANGE COLUMN", es.field(namePrev), columnDefinition}).join(" ");

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

QStringList Schema2TableModel::autoincrementQueries(const QString &driverName, QSqlDriver *driver) const {
    SqlEscaper es(driver);
    QStringList res;
    auto primaryKey = indexes()->primaryKey();
    for(int row=0;row<rowCount();row++) {
        QString namePrev = this->namePrev(row);
        QString name = this->name(row);
        QString type = this->type(row);
        bool autoincrement = this->autoincrement(row);
        if (namePrev.isEmpty() && !name.isEmpty() && !type.isEmpty() && autoincrement) {
            QString columnDefinition = getColumnDefinition(driverName, driver, row, false, primaryKey, {});
            QString expr = QStringList {"ALTER TABLE", es.table(mTableName),
                                        "MODIFY COLUMN", columnDefinition}.join(" ");
            res.append(expr);
        }
    }
    return res;
}

static bool is_text_type(const QString& driverName, const QString& type) {
    return type.toLower().startsWith("varchar") || type.toLower() == "text";
}

QString Schema2TableModel::getColumnDefinition(const QString &driverName, QSqlDriver *driver, int row,
                                               bool skipAutoIncrement, const QStringList& primaryKey,
                                               const ColumnPosition &position) const {

    //Q_ASSERT(driverName == DRIVER_MYSQL || driverName == DRIVER_MARIADB);

    SqlEscaper es(driver);

    QString name = this->name(row);
    QString type = this->type(row);
    bool notNull = this->notNull(row);
    QString default_ = this->default_(row);
    bool autoincrement = this->autoincrement(row);

    QString defaultExpr;

    if (!default_.isEmpty()) {
        if (driverName == DRIVER_MYSQL || driverName == DRIVER_MARIADB) {
            QString value = default_;
            if (default_.startsWith("'")) {

            } else if (default_.startsWith("(")) {

            } else {
                if (is_text_type(driverName, type)) {
                    value = "'" + value + "'";
                } else {
                    value = "(" + value + ")";
                }
            }
            defaultExpr = QString("DEFAULT %1").arg(value);
        } else {
            defaultExpr = QString("DEFAULT %1").arg(default_);
        }
    }
    QString afterExpr;
    if (driverName != DRIVER_SQLITE) {
        afterExpr = position.toString(es);
    }

    QString notNullExpr = notNull ? "NOT NULL" : "";

    QString primaryKeyExpr = (primaryKey.size() == 1 && primaryKey[0] == name) ? "PRIMARY KEY" : "";

    QString autoincrementExpr = (autoincrement && !skipAutoIncrement) ? "AUTO_INCREMENT" : "";

    QString res = filterEmpty({es.field(name), type, notNullExpr,
                               defaultExpr,
                               primaryKeyExpr,
                               autoincrementExpr,
                               afterExpr}).join(" ");
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

QStringList Schema2TableModel::relationsChildColumns() const
{
    QStringList res;
    auto relations = mRelations->values();
    for(auto* relation: relations) {
        res.append(relation->childColumns());
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

Schema2ParentRelationsModel *Schema2TableModel::parentRelations() const {
    return mParentRelations;
}

QStringList Schema2TableModel::names() const
{
    QStringList res;
    for(int row=0;row<rowCount();row++) {
        res.append(mColumns[row].name);
    }
    return res;
}

SColumn Schema2TableModel::at(int row) const
{
    return mColumns[row];
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

static int bool_to_checked(bool value) {
    return value ? Qt::Checked : Qt::Unchecked;
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
        case col_default: return mColumns[row].default_;
        case col_name_prev: return mColumnsPrev[row].name;
        case col_type_prev: return mColumnsPrev[row].type;
        case col_default_prev: return mColumnsPrev[row].default_;
        }
    }
    if (role == Qt::CheckStateRole) {
        int row = index.row();
        switch(index.column()) {
        case col_notnull: return bool_to_checked(mColumns[row].notNull);
        case col_notnull_prev: return bool_to_checked(mColumnsPrev[row].notNull);
        case col_autoincrement: return bool_to_checked(mColumns[row].autoIncrement);
        case col_autoincrement_prev: return bool_to_checked(mColumnsPrev[row].autoIncrement);
        }
    }

    return QVariant();
}

Qt::ItemFlags Schema2TableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (index.column() == col_name || index.column() == col_type || index.column() == col_default) {
        flags |= Qt::ItemIsEditable;
    }
    if (index.column() == col_notnull || index.column() == col_autoincrement) {
        flags |= Qt::ItemIsUserCheckable;
    }
    return flags;
}

static bool checked_to_bool(const QVariant& value) {
    return value.toInt() == Qt::Checked;
}

bool Schema2TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole) {

        int row = index.row();
        switch(index.column()) {
        case col_name: mColumns[row].name = value.toString(); break;
        case col_type: mColumns[row].type = value.toString(); break;
        case col_default: mColumns[row].default_ = value.toString(); break;
        case col_name_prev: mColumnsPrev[row].name = value.toString(); break;
        case col_type_prev: mColumnsPrev[row].type = value.toString(); break;
        case col_default_prev: mColumnsPrev[row].default_ = value.toString(); break;
        }

        emit dataChanged(index, index);
    } else if (role == Qt::CheckStateRole) {        
        int row = index.row();
        switch(index.column()) {
        case col_notnull: mColumns[row].notNull = checked_to_bool(value); break;
        case col_notnull_prev: mColumnsPrev[row].notNull = checked_to_bool(value); break;
        case col_autoincrement: mColumns[row].autoIncrement = checked_to_bool(value); break;
        case col_autoincrement_prev: mColumnsPrev[row].autoIncrement = checked_to_bool(value); break;
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
        {col_autoincrement, "Autoinc"},
        {col_default, "Default"},

        {col_name_prev, "Name"},
        {col_notnull_prev, "Non null"},
        {col_type_prev, "Type"},
        {col_autoincrement_prev, "Autoinc"},
        {col_default_prev, "Default"},
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
    return true;
}
