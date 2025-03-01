#include "schema2data.h"
#include <QSqlDatabase>

#include "drivernames.h"
#include <QSqlQuery>
#include "schema2tablemodel.h"
#include <QSqlRecord>
#include <QSqlDatabase>
#include "schema2view.h"
#include "schema2tableitem.h"
#include "schema2relationitem2.h"

#include <QDebug>
#include "schema2arrange.h"
#include "schema2alterview.h"
#include "showandraise.h"
#include "datautils.h"
#include "schema2store.h"
#include "reporterror.h"
#include "schema2arrange.h"
#include <QSortFilterProxyModel>
#include "checkablestringlistmodel.h"
#include "codewidget.h"

#ifdef Q_OS_WIN
#include <QAxObject>
#endif

#include <QStandardItemModel>
#include "history.h"
#include <QSqlError>
#include <QTableView>
#include "showandraise.h"
#include <QRegularExpression>

#include "schema2changeset.h"
#include "schema2indexesmodel.h"
#include "schema2relationsmodel.h"
#include "schema2relationdialog2.h"
#include "schema2tablesmodel.h"
#include "schema2pushview.h"
#include "tolower.h"
#include "schema2relationguesser.h"
#include "confirmationdialog.h"
#include "schema2export.h"
#include "odbcuri.h"

#include <QApplication>
#include <QClipboard>
#include <QMessageBox>
#include "sqlescaper.h"
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include "copyeventfilter.h"
#include "tablestretcher.h"
#include "ones.h"
#include <QSqlQueryModel>
#include <QFileInfo>
#include "sdata.h"
#include "dataimportwidget2.h"
#include "schema2/schema2storedmodel.h"

/*static*/ bool Schema2Data::mDontAskOnDropTable = false;

/*static*/ bool Schema2Data::mDontAskOnDropRelation = false;

/*static*/ QHash<QString, Schema2Data*> Schema2Data::mData = {};

enum OdbcType {
    dbBoolean = 1,
    dbByte = 2,
    dbInteger = 3,
    dbLong = 4,
    dbCurrency = 5,
    dbSingle = 6,
    dbDouble = 7,
    dbDate = 8,
    dbBinary = 9,
    dbText = 10,
    dbLongBinary = 11,
    dbMemo = 12,
    dbGUID = 15,
    dbBigInt = 16,
    dbVarBinary = 17,
    dbChar = 18,
    dbNumeric = 19,
    dbDecimal = 20,
    dbFloat = 21,
    dbTime = 22,
    dbTimeStamp = 23,
    dbAttachment = 101,
    dbComplexByte = 102,
    dbComplexInteger = 103,
    dbComplexLong = 104,
    dbComplexSingle = 105,
    dbComplexDouble = 106,
    dbComplexGUID = 107,
    dbComplexDecimal = 108,
    dbComplexText = 109
};

/*static*/
QHash<int, QString> Schema2Data::mOdbcTypes = {
    {dbByte, "BYTE"},
    {dbBinary, "BINARY"},
    {dbBoolean, "BIT"},
    {dbCurrency, "CURRENCY"}, // MONEY
    {dbDate, "DATETIME"},
    {dbSingle, "SINGLE"}, // REAL
    {dbDouble, "DOUBLE"},  // FLOAT
    {dbInteger, "SMALLINT"},
    {dbLong, "INTEGER"},
    {dbText, "TEXT"},
    {dbLongBinary, "LONGBINARY"},
    {dbMemo, "MEMO"},
    {dbGUID, "GUID"},
};

Schema2Data *Schema2Data::instance(const QString &connectionName, QObject *parent)
{
    if (!mData.contains(connectionName)) {
        Schema2Data* data = new Schema2Data(connectionName, parent);
        mData[connectionName] = data;
    }
    return mData[connectionName];
}


static QString unquoted(const QString& path) {
    if (path.startsWith("\"") && path.endsWith("\"")) {
        return path.mid(1, path.size()-2);
    }
    return path;
}

static QString unbraced(const QString& path) {
    if (path.startsWith("{") && path.endsWith("}")) {
        return path.mid(1, path.size()-2);
    }
    return path;
}

static bool isAccessDriver(const QString& driver) {
    return driver.contains("Microsoft Access Driver", Qt::CaseInsensitive);
}

#if 0
// todo write actual parser
static QString accessFilePath(QSqlDatabase db) {
    QString databaseName = db.databaseName();
    QStringList props = databaseName.split(";");
    QRegularExpression rx("DBQ\\s*=\\s*(.*)");
    QString filePath;
    for(const QString& prop: props) {
        QRegularExpressionMatch m = rx.match(prop);
        if (m.hasMatch()) {
            filePath = unquoted(m.captured(1).trimmed());
        }
    }
    return filePath;
}
#endif

#if 0
void Schema2Data::unoverlapTables() {

    int w = 200;
    arrangeInGrid(mTableItems.values(), w, 700, 40);
}
#endif

#if 0
void Schema2Data::tablePulled(const QString& tableName, Status status) {
#if 0
    if (!mTableModels.contains(table)) {
        Schema2TableModel* model = new Schema2TableModel(table, status);
        mTableModels.set(table, model);
        connect(model, SIGNAL(tableClicked(QString)), this, SIGNAL(tableClicked(QString)));
    }
    Schema2TableModel* model = mTableModels.get(table);
    if (!mTableItems.contains(table)) {
        Schema2TableItem* item = new Schema2TableItem(model);
        if (mTablePos.contains(table)) {
            item->setPos(mTablePos.get(table));
        } else {
            mSetPosQueue.append(item);
        }
        mTableItems.set(table, item);
        mScene->addItem(item);
    }
#endif

#if 0
    // todo move me to pullTables
    QStringList dropQueue;
    for(Schema2TableModel* table: std::as_const(mDropTableQueue)) {
        dropQueue.append(table->tableName().toLower());
    }
    if (dropQueue.contains(tableName.toLower())) {
        return;
    }
#endif

    return mTables->tablePulled(tableName, status);
}
#endif

void Schema2Data::pullTablesMysql() {

    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    QList<STable> state = mTables->tablesState();
    //QList<STable> newState;

    QHash<SName, STable> newState;

    //QList<STable> tables;

    QSqlQuery q(db);
    q.prepare("select table_schema, table_name, table_type, engine from information_schema.tables "
              "where table_type in ('BASE TABLE', 'VIEW') and table_schema not in ('mysql','performance_schema','sys','information_schema')");
    q.exec();
    while (q.next()) {
        QString table_schema = q.value(0).toString();
        QString table_name = q.value(1).toString();
        QString table_type = q.value(2).toString();
        QString engine = q.value(3).toString();

        TableType type = TableType::Undefined;
        if (table_type == "BASE TABLE") {
            type = TableType::Table;
        } else if (table_type == "VIEW") {
            type = TableType::View;
        }

        SName sname(table_schema, table_name);
        newState[sname] = STable(sname, {}, engine, type);
    }


#if 0
    for(const STable& table: std::as_const(tables)) {

        QList<SColumn> columns;

        QSqlQuery q(db);
        q.prepare("select column_name, column_type, is_nullable, column_default, extra from information_schema.columns where table_schema=? and table_name=?");
        q.addBindValue(table.name.schema);
        q.addBindValue(table.name.name);
        q.exec();
        //QString prev;
        while(q.next()) {
            QString name = q.value(0).toString();
            QString type = q.value(1).toString();
            bool notNull = q.value(2).toString() == "NO";
            QString default_ = q.value(3).toString();
            bool autoincrement = q.value(4).toString() == "auto_increment";
            if (default_ == "NULL") {
                default_ = QString();
            }
            columns.append(SColumn(name, type, notNull, default_, autoincrement));
            //mTables->updateColumn(table, name, type, notNull, default_, autoincrement, prev);
            //prev = name;
        }

        STable table_ = table;
        table_.columns = columns;
        newState.append(table_);
    }
#endif

    q.prepare("select column_name, column_type, is_nullable, column_default, extra, table_schema, table_name "
              "from information_schema.columns "
              "where table_schema not in ('mysql','performance_schema','sys','information_schema')");
    q.exec();
    while(q.next()) {
        QString name = q.value(0).toString();
        QString type = q.value(1).toString();
        bool notNull = q.value(2).toString() == "NO";
        QString default_ = q.value(3).toString();
        bool autoincrement = q.value(4).toString() == "auto_increment";
        QString table_schema = q.value(5).toString();
        QString table_name = q.value(6).toString();

        if (default_ == "NULL") {
            default_ = QString();
        }
        SColumn column(name, type, notNull, default_, autoincrement);

        SName sname(table_schema, table_name);
        if (newState.contains(sname)) {
            newState[sname].columns.append(column);
        } else {
            qDebug() << "!newState.contains(sname)" << table_schema << table_name;
        }
    }

    STablesDiff diff = getDiff(state, newState.values());
    mTables->merge(diff);

}

#if 0
void Schema2Data::pullTablesMysql_() {

    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    QStringList tables = db.tables();

    QList<STable> state = mTables->tablesState();
    QList<STable> newState;



    for(const QString& table: std::as_const(tables)) {
        //mTables->updateTable(table, StatusExisting);
        // todo character_maximum_length, numeric_precision

        QList<SColumn> columns;

        QSqlQuery q(db);
        q.prepare("SELECT column_name, column_type, is_nullable, column_default, extra FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_NAME=? AND TABLE_SCHEMA=?");
        q.addBindValue(table);
        q.addBindValue(db.databaseName());
        q.exec();
        QString prev;
        while(q.next()) {
            QString name = q.value(0).toString();
            QString type = q.value(1).toString();
            bool notNull = q.value(2).toString() == "NO";
            QString default_ = q.value(3).toString();
            bool autoincrement = q.value(4).toString() == "auto_increment";
            if (default_ == "NULL") {
                default_ = QString();
            }
            columns.append(SColumn(name, type, notNull, default_, autoincrement));
            //mTables->updateColumn(table, name, type, notNull, default_, autoincrement, prev);
            prev = name;
        }

        newState.append(STable(table, columns));
        //mTables->updateColumns(table);

    }

    STablesDiff diff = getDiff(state, newState);
    mTables->merge(diff);

}
#endif

void Schema2Data::pullTablesOdbcAccess(const OdbcUri& uri) {

    QSqlDatabase db = this->database();

    QStringList tables = db.tables();

    QString filePath = unquoted(uri.dbq());

    if (filePath.isEmpty()) {
        qDebug() << "pullTablesOdbc filePath.isEmpty() error" << db.databaseName();
        return;
    }
#ifdef Q_OS_WIN

    QAxObject engine("DAO.DBEngine.120");
    QAxObject* database = engine.querySubObject("OpenDatabase(QString, bool)", filePath, false);

    QList<STable> state = mTables->tablesState();
    QList<STable> newState;

    for(const QString& tableName: tables) {

        //mTables->tableCreated(tableName, StatusExisting);

        QList<SColumn> columns;

        QAxObject* tableDef = database->querySubObject("TableDefs(QString)", tableName);

        QAxObject* fields = tableDef->querySubObject("Fields");
        int fieldCount = fields->property("Count").toInt();
        //QString prev;
        for(int i=0;i<fieldCount;i++) {
            QAxObject* field = tableDef->querySubObject("Fields(int)", i);
            QString name = field->property("Name").toString();
            int type = field->property("Type").toInt();
            int size = field->property("Size").toInt();
            bool notNull = field->property("Required").toBool();

            //qDebug() << tableName << name << type;

            if (!mOdbcTypes.contains(type) || 0) {
                qDebug() << tableName << name << type << size;
            }

            QString typeName = mOdbcTypes.value(type, "UNKNOWN").toLower();
            if (type == dbText) {
                if (size != 255) {
                    typeName = QString("TEXT(%1)").arg(size).toLower();
                }
            }

            // todo odbc autoincrement
            bool autoincrement = false;
            // todo odbc default
            QString default_;

            columns.append(SColumn(name, typeName, notNull, default_, autoincrement));

            //mTables->updateColumn(tableName, name, typeName, notNull, default_, autoincrement, prev);

            //prev = name;
        }

        //mTables->updateColumns(tableName);
        newState.append(STable(tableName, columns));

    }

    STablesDiff diff = getDiff(state, newState);
    mTables->merge(diff);

#else
    // todo
#endif
}


void Schema2Data::pullTablesOther() {

    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    QStringList tables = db.tables();

    QList<STable> state = mTables->tablesState();
    QList<STable> newState;

    for(const QString& table: std::as_const(tables)) {

        //mTables->tableCreated(table, StatusExisting);

        QList<SColumn> columns;

        QSqlRecord r = db.record(table);
        QString prev;
        for(int i=0;i<r.count();i++) {
            QString name = r.fieldName(i);
            QString type;
            bool notNull = false;
            bool autoincrement = false;
            QString default_;
            //mTables->updateColumn(table, name, type, notNull, default_, autoincrement, prev);
            columns.append(SColumn(name, type, notNull, default_, autoincrement));
            prev = name;
        }

        //mTables->updateColumns(table);
        newState.append(STable(table, columns));

    }

    STablesDiff diff = getDiff(state, newState);
    mTables->merge(diff);


}

void Schema2Data::pullTables() {
    QSqlDatabase db = QSqlDatabase::database(mConnectionName);
    OdbcUri uri(db.databaseName());
    if (db.driverName() == DRIVER_MYSQL || db.driverName() == DRIVER_MARIADB) {
        pullTablesMysql();
    } else if (db.driverName() == DRIVER_ODBC && isAccessDriver(uri.driver())) {
        pullTablesOdbcAccess(uri);
    } else {
        pullTablesOther();
    }
}

class RelationItem {
public:
    QString constraintName;
    QString childTable;
    QStringList childColumns;
    QString parentTable;
    QStringList parentColumns;
};

QDebug&	operator<<(QDebug debug, const RelationItem& relation) {
    debug.nospace() << "RelationItem";
    debug.space() << relation.constraintName << relation.childTable << relation.childColumns
                  << relation.parentTable << relation.parentColumns;
    return debug.maybeSpace();
}

static int indexOf(const QList<RelationItem>& relations, const QString& childTable, const QString& constraintName) {
    for(int i=0;i<relations.size();i++) {
        if (relations[i].childTable.toLower() == childTable.toLower() && relations[i].constraintName == constraintName) {
            return i;
        }
    }
    return -1;
}

static int indexOf(const QList<SRelation>& relations, const QString& name, const SName& childTable, const SName& parentTable) {
    for(int i=0;i<relations.size();i++) {
        if (relations[i].name == name
            && relations[i].childTable == childTable
            && relations[i].parentTable == parentTable) {
            return i;
        }
    }
    return -1;
}


void Schema2Data::pullRelationsMysql() {

    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    QStringList tables = db.tables();

    QStringList tablesLower = toLower(tables);

    QSqlQuery q(db);
    q.prepare("SELECT CONSTRAINT_SCHEMA, TABLE_NAME, COLUMN_NAME, CONSTRAINT_NAME, REFERENCED_TABLE_NAME, REFERENCED_COLUMN_NAME "
              "FROM INFORMATION_SCHEMA.KEY_COLUMN_USAGE ");
    if (!q.exec()) {
        qDebug() << "error" << __FILE__ << __LINE__;
    }

    //QList<RelationItem> relations;

    QList<SRelation> state = mTables->relationsState();
    QList<SRelation> newState;

    while(q.next()) {

        QString schema = q.value(0).toString();
        QString childTable = q.value(1).toString();
        QString childColumn = q.value(2).toString();
        QString constraintName = q.value(3).toString();
        QString parentTable = q.value(4).toString();
        QString parentColumn = q.value(5).toString();

        if (parentTable.isEmpty()) {
            continue;
        }

#if 0
        if (!tables.contains(childTable) && !tablesLower.contains(childTable.toLower())) {
            qDebug() << childTable << __FILE__ << __LINE__;
            continue;
        }
        if (!tables.contains(parentTable) && !tablesLower.contains(parentTable.toLower())) {
            qDebug() << childTable << __FILE__ << __LINE__;
            continue;
        }
#endif

        SName childTable_(schema, childTable);
        SName parentTable_(schema, parentTable);

        int index_ = indexOf(newState, constraintName, childTable_, parentTable_);
        if (index_ > -1) {
            newState[index_].childColumns.append(childColumn);
            newState[index_].parentColumns.append(parentColumn);
        } else {
            newState.append(SRelation(constraintName, childTable_, {childColumn}, parentTable_, {parentColumn}));
        }

#if 0
        int index = indexOf(relations, childTable, constraintName);
        if (index < 0) {
            RelationItem relation;
            relation.constraintName = constraintName;
            relation.childTable = childTable;
            relation.childColumns = QStringList {childColumn};
            relation.parentColumns = QStringList {parentColumn};
            relation.parentTable = parentTable;
            relations.append(relation);
        } else {
            relations[index].childColumns.append(childColumn);
            relations[index].parentColumns.append(parentColumn);
        }
#endif

    }

    SRelationsDiff diff = getDiff(state, newState);
    mTables->merge(diff);

#if 0
    for(const RelationItem& relation: std::as_const(relations)) {
        mTables->relationCreated(relation.constraintName, relation.childTable,
                                relation.childColumns, relation.parentTable,
                                relation.parentColumns, true, StatusExisting);
        //qDebug() << relation;
    }
#endif

}

#if 0
template <class T>
static T* hash_find(const QHash<QString, T*>& hash, const QString& key) {
    if (hash.contains(key)) {
        return hash[key];
    }
    QStringList keys = hash.keys();
    for(const QString& other: keys) {
        if (other.toLower() == key.toLower()) {
            return hash[other];
        }
    }
    return 0;
}
#endif

#if 0
void Schema2Data::indexPulled(const QString indexName, const QString& tableName, const QStringList& columns,
                              bool primary, bool unique, Status status) {
    if (mTables->contains(tableName)) {
        mTables->table(tableName)->insertIndex(indexName, columns, primary, unique, status);
    } else {
        qDebug() << "!mTableModels.contains(tableName)" << tableName;
    }
}
#endif

void Schema2Data::pullIndexesOdbcAccess(const OdbcUri& uri) {

    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    QString filePath = unquoted(uri.dbq());

    if (filePath.isEmpty()) {
        qDebug() << "pullIndexesOdbc filePath.isEmpty() error" << db.databaseName();
        return;
    }

#ifdef Q_OS_WIN

    QAxObject engine("DAO.DBEngine.120");

    QAxObject* database = engine.querySubObject("OpenDatabase(QString)", filePath);

    QAxObject* tableDefs = database->querySubObject("TableDefs");

    int tableDefsCount = tableDefs->property("Count").toInt();

#if 0
    QSet<QString> systemTables = {
        "MSysNavPaneGroupCategories",
        "MSysNavPaneGroups",
        "MSysNavPaneGroupToObjects",
        "MSysNavPaneObjectIDs",
        "MSysObjects",
        "MSysQueries",
        "MSysRelationships",
        "MSysACEs"
    };
#endif

    QStringList tables = db.tables();

    for(const QString& tableName: tables) {

        QAxObject* tableDef = database->querySubObject("TableDefs(QString)", tableName);

        //QString tableName = tableDef->property("Name").toString();

        /*if (systemTables.contains(tableName)) {
            continue;
        }*/

        //qDebug() << "tableName" << tableName;

        QAxObject* indexes = tableDef->querySubObject("Indexes");
        int indexesCount = indexes->property("Count").toInt();

        for(int j=0;j<indexesCount;j++) {
            QAxObject* index = tableDef->querySubObject("Indexes(int)", j);

            QString indexName = index->property("Name").toString();
            bool primary = index->property("Primary").toBool();
            bool unique = index->property("Unique").toBool();

            QStringList columns;

            QAxObject* fields = index->querySubObject("Fields");
            int fieldCount = fields->property("Count").toInt();
            for(int k=0;k<fieldCount;k++) {
                QAxObject* field = index->querySubObject("Fields(int)", k);
                QString fieldName = field->property("Name").toString();
                columns.append(fieldName);
            }

            mTables->table(tableName)->insertIndex(indexName, columns, primary, unique, StatusExisting);

        }
    }
#else

#endif


}
#if 0
void Schema2Data::relationPulled(const QString& constraintName, const QString& childTable,
                                 const QStringList& childColumns,
                                 const QString& parentTable, const QStringList& parentColumns,
                                 bool constrained, Status status) {



    mTables->relationPulled(constraintName, childTable, childColumns, parentTable, parentColumns, constrained, status);
}
#endif

void Schema2Data::pullRelationsOdbcAccess(const OdbcUri& uri) {
    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    QString filePath = unquoted(uri.dbq());
    if (filePath.isEmpty()) {
        qDebug() << "pullRelationsOdbc filePath.isEmpty() error" << db.databaseName();
        return;
    }

#ifdef Q_OS_WIN

    QAxObject engine("DAO.DBEngine.120");
    QAxObject* database = engine.querySubObject("OpenDatabase(QString, bool)", filePath, false);
    QAxObject* relations = database->querySubObject("Relations");
    int count = relations->property("Count").toInt();

    for(int i=0;i<count;i++) {
        QAxObject* relation = database->querySubObject("Relations(int)", i);
        QString constraintName = relation->property("Name").toString();
        QString parentTable = relation->property("Table").toString();
        QString childTable = relation->property("ForeignTable").toString();
        //qDebug() << name << table << foreignTable;

        QAxObject* fields = relation->querySubObject("Fields");
        int fieldCount = fields->property("Count").toInt();

        QStringList childColumns;
        QStringList parentColumns;
        for(int j=0;j<fieldCount;j++) {
            QAxObject* field = relation->querySubObject("Fields(int)", j);
            QString parentColumn = field->property("Name").toString();
            QString childColumn = field->property("ForeignName").toString();

            parentColumns.append(parentColumn);
            childColumns.append(childColumn);
        }

        mTables->relationCreated(constraintName, childTable, childColumns, parentTable, parentColumns, true, StatusExisting);


    }
#else
    // todo
#endif

}

void Schema2Data::pullRelations() {
    QSqlDatabase db = QSqlDatabase::database(mConnectionName);
    OdbcUri uri(db.databaseName());
    if (db.driverName() == DRIVER_MYSQL || db.driverName() == DRIVER_MARIADB) {
        pullRelationsMysql();
    } else if (db.driverName() == DRIVER_ODBC && isAccessDriver(uri.driver())) {
        pullRelationsOdbcAccess(uri);
    } else {
        // todo implement pullRelations for QSQLITE QPSQL
    }

}

class MysqlIndex {
public:
    QString indexName;
    QString tableName;
    QStringList columns;
    bool unique;
    bool primary;
};

QDebug operator << (QDebug& debug, const MysqlIndex& index) {
    debug.nospace() << "MysqlIndex";
    debug.space() << index.indexName << index.tableName << index.columns << index.unique;
    return debug.maybeSpace();
}

int indexOf(const QList<MysqlIndex>& indexes, const QString& tableName, const QString indexName) {
    for(int i=0;i<indexes.size();i++) {
        if (indexes[i].tableName == tableName && indexes[i].indexName == indexName) {
            return i;
        }
    }
    return -1;
}

void Schema2Data::pullIndexesMysql() {

    QSqlDatabase db = QSqlDatabase::database(mConnectionName);
    QSqlQuery q(db);
    q.prepare("SELECT TABLE_NAME, NON_UNIQUE, INDEX_NAME, COLUMN_NAME "
              "FROM INFORMATION_SCHEMA.STATISTICS WHERE TABLE_SCHEMA=? "
              "ORDER BY INDEX_NAME, SEQ_IN_INDEX");
    q.addBindValue(db.databaseName());

    if (!q.exec()) {
        qDebug() << q.lastError().text() << __FILE__ << __LINE__;
        return;
    }

    QList<MysqlIndex> indexes;

    while (q.next()) {

        QString tableName = q.value(0).toString();
        bool unique = q.value(1).toInt() == 0;
        QString indexName = q.value(2).toString();
        QString column = q.value(3).toString();
        bool primary = indexName == "PRIMARY";

        int index = indexOf(indexes, tableName, indexName);
        if (index > -1) {
            indexes[index].columns.append(column);
        } else {
            MysqlIndex index;
            index.tableName = tableName;
            index.indexName = indexName;
            index.columns = QStringList {column};
            index.unique = unique;
            index.primary = primary;
            indexes.append(index);
        }

    }

    for(const MysqlIndex& index: indexes) {
        mTables->table(index.tableName)->insertIndex(index.indexName, index.columns, index.primary, index.unique, StatusExisting);
    }

}

void Schema2Data::pullIndexes() {
    QSqlDatabase db = QSqlDatabase::database(mConnectionName);
    OdbcUri uri(db.databaseName());
    if (db.driverName() == DRIVER_MYSQL || db.driverName() == DRIVER_MARIADB) {
        pullIndexesMysql();
    } else if (db.driverName() == DRIVER_ODBC && isAccessDriver(uri.driver())) {
        pullIndexesOdbcAccess(uri);
    } else {
        // todo implement pullRelations for QSQLITE QODBC QPSQL
    }
}

void Schema2Data::pullStored() {
    QSqlDatabase db = QSqlDatabase::database(mConnectionName);
    if (db.driverName() == DRIVER_MYSQL || db.driverName() == DRIVER_MARIADB) {
        pullStoredMysql();
    }
}

void Schema2Data::pullStoredMysql() {
    QSqlDatabase db = QSqlDatabase::database(mConnectionName);
    QSqlQuery q(db);
    q.prepare("select ROUTINE_SCHEMA, ROUTINE_NAME, ROUTINE_TYPE from INFORMATION_SCHEMA.routines where ROUTINE_SCHEMA != 'sys'");
    q.exec();

    QList<SStored> state = mStored->state();
    QList<SStored> newState;

    while (q.next()) {
        QString schema = q.value(0).toString();
        QString name = q.value(1).toString();
        QString type = q.value(2).toString();

        SStored item;
        item.name = SName(schema, name);
        item.type = type == "PROCEDURE" ? SStored::Procedure : SStored::Function;
        newState.append(item);
    }

    auto diff = getDiff(state, newState);
    mStored->merge(diff);
}

void Schema2Data::pull()
{
    QElapsedTimer timer;
    timer.start();

    pullTables();
    qDebug() << "pullTables" << timer.restart();

    pullIndexes();
    qDebug() << "pullIndexes" << timer.restart();

    pullRelations();
    qDebug() << "pullRelations" << timer.restart();

    pullStored();
    qDebug() << "pullStored" << timer.restart();

    mTables->setTableItemsPos();
}



void Schema2Data::push(QWidget* widget)
{
    mTables->savePos();

    //QString driverName = QSqlDatabase::database(mConnectionName).driverName();
    //auto* driver = this->driver();

    QSqlDatabase db = database();


    Schema2ChangeSet* changeSet = new Schema2ChangeSet();

    // create and alter tables
    QList<Schema2TableModel*> tables = mTables->tables();
    for(Schema2TableModel* table: tables) {
        switch(table->status()) {
        case StatusNew:
            changeSet->append(table->createQueries(db), [=](){
                table->columnsCreated(true);
                table->primaryKeysCreated();
            });
            break;
        case StatusModified:
        {
            SName tableName = table->tableName();
            SName tableNamePrev = table->tableNamePrev();
            QStringList renameQueries = table->renameQueries(db);
            if (!renameQueries.isEmpty()) {
                changeSet->append(renameQueries, [=](){
                    table->setTableNamePrev(tableName);
                    tableRenamed(tableName, tableNamePrev);
                });
            }
            changeSet->append(table->alterQueries(db), [=](){
                table->columnsCreated(false);
            });
            changeSet->append(table->alterEngineQueries(db), [=](){
                table->engineChanged();
            });
        }
            break;
        case StatusExisting:
            break;
        }
    }

    // create and alter indexes
    for(Schema2TableModel* table: tables) {
        Schema2IndexesModel* indexes = table->indexes();

        changeSet->append(indexes->queries(table, db), [=](){
            indexes->pushed();
        });
    }

    // update autoincrement
#if 0
    for(Schema2TableModel* table: tables) {
        if (table->status() == StatusNew) {
            changeSet->append(table->autoincrementQueries(driverName, driver), [=](){
                table->pushed(true);
            });
        }
    }
#endif

    // create and alter relations
    for(Schema2TableModel* table: tables) {
        QList<Schema2Relation*> relations = table->relations()->values();
        for(Schema2Relation* relation: relations) {
            switch(relation->status()) {
            case StatusExisting:
                break;
            case StatusModified:
                changeSet->append(relation->modifyQueries(table->tableName(), db), [=](){
                    relation->pushed();
                });
                break;
            case StatusNew:
                changeSet->append(relation->createQueries(table->tableName(), db), [=](){
                    relation->pushed();
                });
                break;
            }
        }
    }

    QList<QPair<SName, Schema2Relation*>> queue = mTables->dropRelationQueue();

    // drop relations
    for(auto item: queue) {
        SName name = item.first;
        Schema2Relation* relation = item.second;
        changeSet->append(relation->dropQueries(name, db),[=](){
            //mDropRelationsQueue.removeOne(item);
            mTables->relationDropped(name, relation);
        });
    }

    // drop tables
    for(auto* item: mDropTableQueue) {
        changeSet->append(item->dropQueries(db),[=](){
            mDropTableQueue.removeOne(item);
        });
    }


    if (changeSet->isEmpty()) {
        delete changeSet;
        return;
    }

    changeSet->execute(mConnectionName);

    Schema2PushView* view = new Schema2PushView();
    view->setModel(changeSet);

    showAndRaise(view);

}

void Schema2Data::save()
{

}

void Schema2Data::load()
{
    mTables->loadPos();
    pull();

}

// not used
bool Schema2Data::hasPendingChanges() const
{
    /*if (!mDropRelationsQueue.isEmpty()) {
        return true;
    }*/
    QList<Schema2TableModel*> models = mTables->tables();
    for(Schema2TableModel* model: models) {
        if (model->hasPendingChanges()) {
            return true;
        }
    }
    return false;
}

Schema2View *Schema2Data::view()
{
    if (!mView) {
        mView = new Schema2View();
        mView->setData(this);
        QSqlDatabase db = QSqlDatabase::database(mConnectionName);
        mView->setWindowTitle(DataUtils::windowTitle(QString{}, db, QString()));
    }
    return mView;
}

QGraphicsScene *Schema2Data::scene() const
{
    return mScene;
}

void Schema2Data::selectOrDeselect(const SName& tableName) {

    //mSelectModel->toggleChecked(tableName);

}



void Schema2Data::createRelationDialog(const SName &childTable, const SName& parentTable, QWidget *widget)
{

    auto* childTableModel = mTables->table(childTable);
    auto* parentTableModel = mTables->table(parentTable);

    if (!childTableModel) {
        qDebug() << "!childTableModel" << __FILE__ << __LINE__;
        return;
    }
    if (!parentTableModel) {
        qDebug() << "!parentTableModel" << __FILE__ << __LINE__;
        return;
    }

    auto relations = childTableModel->relationsTo(parentTable);

    // todo implement multiple relations between same pair of tables
    if (relations.size() > 0) {
        editRelationDialog(relations[0], widget);
        return;
    }

    if (!childTableModel) {
        qDebug() << "!childTableModel" << __FILE__ << __LINE__;
        return;
    }
    if (!parentTableModel) {
        qDebug() << "!parentTableModel" << __FILE__ << __LINE__;
        return;
    }

    Schema2RelationGuesser guesser(childTableModel, parentTableModel);

    QString relationName = guesser.relationName();

    QStringList parentColumns = guesser.parentColumns();

    QStringList childColumns = guesser.childColumns(parentColumns);

    Schema2RelationDialog2 dialog(widget);
    dialog.init(childTableModel,
                parentTableModel,
                relationName,
                childColumns,
                parentColumns);

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    mTables->relationCreated(dialog.relationName(),
                   childTable, dialog.childColumns(),
                   parentTable, dialog.parentColumns(), dialog.constrained(), StatusNew);

}

void Schema2Data::dropRelationDialog(Schema2Relation *relation, QWidget *widget) {
    QString message = QString("Are you sure to drop relation %1?").arg(relation->name());
    if (!ConfirmationDialog::question(widget, message, &mDontAskOnDropRelation)) {
        return;
    }
    tables()->relationRemoved(relation);
}

void Schema2Data::dropRelationDialog(const SName &childTable, const SName &parentTable, QWidget *widget)
{
    auto* table1 = mTables->table(childTable);
    auto* table2 = mTables->table(parentTable);
    if (!table1 || !table2) {
        return;
    }
    auto relations = table1->relationsTo(table2->tableName());
    if (relations.size() > 0) {
        return dropRelationDialog(relations[0], widget);
    }
    relations = table2->relationsTo(table1->tableName());
    if (relations.size() > 0) {
        return dropRelationDialog(relations[0], widget);
    }
}

template <typename T>
static QSet<T> toSet(const QList<T>& qlist)
{
    return QSet<T> (qlist.constBegin(), qlist.constEnd());
}

void Schema2Data::dropTableDialog(const SName &tableName, QWidget *widget) {

    QString message = QString("Are you sure to drop table %1?").arg(tableName.name);

    if (!ConfirmationDialog::question(widget, message, &mDontAskOnDropTable)) {
        return;
    }

    auto* table = mTables->table(tableName);
    if (!table) {
        qDebug() << __FILE__ << __LINE__;
        return;
    }
    auto relations = toSet(mTables->relationsTo(tableName) + mTables->relationsFrom(tableName));

    qDebug() << relations.size() << "relations with table" << tableName;

    for(auto* relation: relations) {
        mTables->relationRemoved(relation);
    }

    qDebug() << "dropRelationQueue.size()" << mTables->dropRelationQueue();

    mTables->tableRemoved(tableName);
    mDropTableQueue.append(table);

    // todo renamed and removed
    Schema2AlterView* view = mAlterViews.value(tableName);
    if (view) {
        mAlterViews.remove(tableName);
        view->hide();
        view->deleteLater();
    }

}

QSqlDatabase Schema2Data::database() const {
    return QSqlDatabase::database(mConnectionName);
}

QString Schema2Data::driverName() const {
    return database().driverName();
}

Schema2AlterView *Schema2Data::alterView(const QString &table) {
    return mAlterViews.value(table);
}

Schema2TablesModel *Schema2Data::tables() const {
    return mTables;
}

Schema2StoredModel* Schema2Data::stored() const {
    return mStored;
}

QSqlDriver* Schema2Data::driver() const {
    return database().driver();
}



void Schema2Data::showDataStatistics(QWidget *widget)
{
    QSqlDatabase db = QSqlDatabase::database(mConnectionName);
    auto tables = db.tables();

    SqlEscaper es(db.driver());

    auto driverName = this->driverName();

    QAbstractItemModel* statisticsModel = 0;

    if (driverName == DRIVER_MYSQL || driverName == DRIVER_MARIADB) {

        QSqlQuery q(db);
        q.prepare("select table_name, table_rows, (data_length + index_length) from information_schema.tables where table_schema=?");
        q.addBindValue(db.databaseName());
        if (!q.exec()) {
            qDebug() << q.lastError().text() << __FILE__ << __LINE__;
            return;
        }
        QSqlQueryModel* model = new QSqlQueryModel();
        model->setQuery(std::move(q));

        model->setHeaderData(0, Qt::Horizontal, "Name");
        model->setHeaderData(1, Qt::Horizontal, "Rows");
        model->setHeaderData(2, Qt::Horizontal, "Size");

        statisticsModel = model;

    } else {

        QStandardItemModel* model = new QStandardItemModel(tables.size(), 2);
        model->setHorizontalHeaderLabels({"Name", "Rows"});
        for(int row=0;row<tables.size();row++) {
            QString table = tables[row];
            QSqlQuery q(db);
            model->setData(model->index(row, 0), table);
            if (q.exec(QString("select count(*) from %1").arg(es.table(table)))) {
                if (q.next()) {
                    model->setData(model->index(row, 1), q.value(0));
                } else {
                    qDebug() << "q.next() == false" << q.lastError().text() << __FILE__ << __LINE__;
                }
            } else {
                qDebug() << "q.exec() == false" << q.lastError().text() << __FILE__ << __LINE__;
            }
        }
        statisticsModel = model;
    }

    QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel();
    proxyModel->setSourceModel(statisticsModel);

    QTableView* view = new QTableView();
    view->horizontalHeader()->setStretchLastSection(true);
    view->setSortingEnabled(true);

    TableStretcher::stretch(view, ones(statisticsModel->columnCount()));

    view->setModel(proxyModel);
    showAndRaise(view);

    CopyEventFilter::copyTsv(view);

}

static QString toPythonStr(const QString& value) {
    return "\"" + value + "\"";
}

static QString toPythonList(const QStringList& values) {
    QStringList res;
    for(const QString& value: values) {
        res.append(toPythonStr(value));
    }
    return "[" + res.join(", ") + "]";
}

static QString toPythonDict(const QHash<QString, QStringList>& items) {
    auto keys = items.keys();
    QStringList res;
    for(const QString& key: keys) {
        QString item = QString("%1:%2").arg(toPythonStr(key)).arg(toPythonList(items[key]));
        res.append(item);
    }
    return "{\n" + res.join(",\n") + "\n}";
}

void Schema2Data::copyRelationsToClipboard(QWidget *widget) {
    QStringList items;
    auto tables = mTables->tables();
    for(auto* table: tables) {
        auto relations = table->relations()->values();
        for(auto* relation: relations) {
            QString item = QString("(%1, %2, %3, %4)")
                    .arg(toPythonStr(relation->childTable().name))
                    .arg(toPythonStr(relation->parentTable().name))
                    .arg(toPythonList(relation->childColumns()))
                    .arg(toPythonList(relation->parentColumns()));
            items.append(item);
        }
    }
    QString text = "[" + items.join(",\n") + "\n]\n";
    qApp->clipboard()->setText(text);
    QMessageBox::information(widget, "", "Copied to clipboard");
}

void Schema2Data::copyPrimaryKeysToClipboard(QWidget *widget) {
    QHash<QString, QStringList> items;
    auto tables = mTables->tables();
    for(auto* table: tables) {
        items[table->tableName().name] = table->indexes()->primaryKey();
    }
    QString text = toPythonDict(items);
    qApp->clipboard()->setText(text);
    QMessageBox::information(widget, "", "Copied to clipboard");
}

void Schema2Data::saveAs(bool clipboard, const QString &path,
                         const QRectF& rect, bool onlySelected,
                         Schema2Export::ExportFormat format, QWidget *widget)
{
    Schema2Export exp(mView);
    exp.saveAs(clipboard, path, rect, onlySelected, format, widget);
}

void Schema2Data::tableRenamed(const SName &tableName, const SName &tableNamePrev)
{
    if (mAlterViews.contains(tableNamePrev)) {
        mAlterViews[tableName] = mAlterViews[tableNamePrev];
        mAlterViews.remove(tableNamePrev);
    }
    if (mDataImportWidgets.contains(tableNamePrev)) {
        mDataImportWidgets[tableName] = mDataImportWidgets[tableNamePrev];
        mDataImportWidgets.remove(tableNamePrev);
    }
}

static bool containsAll(const SNames& ordered, const SNames& related, const SName& name) {
    for(const SName& item: related.names) {
        if (item == name) {
            continue;
        }
        if (!ordered.contains(item)) {
            return false;
        }
    }
    return true;
}


static SNames filtered(const SNames& names1, const SNames& names2) {
    SNames res;
    for(const SName& name: names1.names) {
        if (names2.indexOf(name) > -1) {
            res.append(name);
        }
    }
    return res;
}

SNames Schema2Data::sortedInInsertOrder(const SNames &names_)
{
    QHash<SName, SNames> relations;
    auto* tables = mTables;
    SNames names = tables->tableNames();

    for(const SName& name: names.names) {
        relations[name] = tables->table(name)->relatedTables();
    }
    SNames ordered;

    int i = 0;
    while(i < names.size()) {
        auto name = names[i];
        if (relations[name].isEmpty()) {
            ordered.append(name);
            names.removeAt(i);
            i--;
        }
        i++;
    }
    while(!names.isEmpty()) {
        int count1 = names.size();
        i = 0;
        while(i < names.size()) {
            auto name = names[i];
            auto related = relations[name];
            if (containsAll(ordered, related, name)) {
                ordered.append(name);
                names.removeAt(i);
                i--;
            }
            i++;
        }
        int count2 = names.size();
        if (count1 == count2) {
            qDebug() << "count1 == count2" << __FILE__ << __LINE__;
            return {};
        }
    }

    return filtered(ordered, names_);
}

#include "highlighter.h"
#include "tokens.h"

void Schema2Data::scriptDialog(QWidget *parent)
{
    //auto driverName = this->driverName();
    //auto* driver = this->driver();
    auto db = database();

    QStringList queries =
            mTables->createTablesQueries(db)
            + mTables->createIndexesQueries(db)
            + mTables->createRelationsQueries(db);

    Tokens tokens = Tokens(database(), this);
    auto* highligher = new Highlighter(tokens, 0);

    CodeWidget* widget = new CodeWidget();
    widget->setText(queries.join(";\n") + "\n");
    widget->setHighlighter(highligher);
    widget->show();

    /*qApp->clipboard()->setText();
    QMessageBox::information(widget, "", "Copied to clipboard");*/
}

static QStringList mysqlTypes() {
    // https://dev.mysql.com/doc/refman/8.0/en/data-types.html

    // numeric
    QStringList exact = {"INTEGER", "SMALLINT", "DECIMAL", "BIGINT"};
    QStringList approx = {"FLOAT", "DOUBLE"};
    QStringList bit = {"BIT"};
    // other
    QStringList date = {"DATE", "TIME", "DATETIME", "TIMESTAMP"};
    QStringList string = {"CHAR", "VARCHAR", "BINARY", "VARBINARY", "BLOB", "TEXT", "ENUM", "SET"};
    QStringList spatial = {}; // todo spatial mysql types
    QStringList json = {"JSON"};

    return exact + approx + bit + date + string + spatial + json;
}

QStringList Schema2Data::dataTypes() const {

    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    if (db.driverName() == DRIVER_ODBC) {
        return toLower(mOdbcTypes.values());
    } else if (db.driverName() == DRIVER_MYSQL || db.driverName() == DRIVER_MARIADB) {
        return toLower(mysqlTypes());
    }

    // todo implement datatypes for DRIVER_PSQL

    return {};
}

#if 0
QStringList Schema2Data::guessParentColumns(QString childTable, QStringList childColumns, QString parentTable) {

    QStringList res;
    auto* model = mTables->table(parentTable);
    for(int row=0;row<childColumns.size();row++) {
        res.append(model->newName(row));
    }
    return res;
}
#endif

void Schema2Data::editRelationDialog(
        Schema2Relation* relation,
        QWidget* widget) {

    auto* childTableModel = relation->childTableModel();

    auto* parentTableModel = mTables->table(relation->parentTable());

    if (!childTableModel) {
        qDebug() << "!childTableModel" << __FILE__ << __LINE__;
        return;
    }
    if (!parentTableModel) {
        qDebug() << "!parentTableModel" << __FILE__ << __LINE__;
        return;
    }

    Schema2RelationDialog2 dialog(widget);
    dialog.init(childTableModel,
                parentTableModel,
                relation->name(),
                relation->childColumns(),
                relation->parentColumns());

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    relation->setName(dialog.relationName());

    relation->setChildColumns(dialog.childColumns());
    relation->setParentColumns(dialog.parentColumns());

}

void Schema2Data::createRelationDialog(Schema2TableModel* childTable,
                                   QStringList childColumns,
                                   QString parentTable) {


    // todo implement multiple relations between same pair of tables

    auto* parentTableModel = mTables->table(parentTable);

    if (!childTable) {
        qDebug() << "!childTableModel" << __FILE__ << __LINE__;
        return;
    }
    if (!parentTableModel) {
        qDebug() << "!parentTableModel" << __FILE__ << __LINE__;
        return;
    }

    auto relations = childTable->relationsTo(parentTable);

    QString relationName;

    QStringList parentColumns;

    Schema2RelationGuesser guesser(childTable, parentTableModel);

    if (relations.size() == 0) {
        parentColumns = guesser.parentColumns();
        relationName = guesser.relationName();
    } else {
        auto* relation = relations[0];
        parentColumns = relation->parentColumns();
        relationName = relation->name();
    }

    Schema2RelationDialog2 dialog;
    dialog.init(childTable, parentTableModel,
                relationName,
                childColumns,
                parentColumns);

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    if (relations.size() > 0) {
        auto* relation = relations[0];
        relation->setName(dialog.relationName());
        relation->setChildColumns(dialog.childColumns());
        relation->setParentColumns(dialog.parentColumns());

    } else {

        QString constraintName = dialog.relationName();
        QStringList childColumns = dialog.childColumns();
        QStringList parentColumns = dialog.parentColumns();
        bool constrained = dialog.constrained();

        mTables->relationCreated(constraintName, childTable->tableName(), childColumns,
                                 parentTable, parentColumns, constrained, StatusNew);

    }


}

void Schema2Data::showAlterView(const SName &tableName)
{
    qDebug() << "showAlterView" << tableName.fullname();

    if (!mAlterViews.contains(tableName)) {

        qDebug() << "create new view for" << tableName.fullname();

        Schema2AlterView* view = new Schema2AlterView();
        Schema2TableModel* table = mTables->table(tableName);

        if (!table) {
            qDebug() << "!table" << __FILE__ << __LINE__;
            return;
        }

        view->init(this, table, dataTypes());
        mAlterViews[tableName] = view;
    } else {
        qDebug() << "show existing view for" << tableName.fullname();
    }
    auto* view = mAlterViews[tableName];
    showAndRaise(view);
}



void Schema2Data::showDataImportWidget(const SName &tableName)
{
    if (!mDataImportWidgets.contains(tableName)) {
        DataImportWidget2* widget = new DataImportWidget2();
        widget->init(this, mTables->table(tableName));
        mDataImportWidgets[tableName] = widget;
    }
    auto* widget = mDataImportWidgets[tableName];
    showAndRaise(widget);
}

void Schema2Data::arrange(bool all)
{
    //squareArrange(mTableItems.keys(), mRelationModels, mTableItems);
    arrangeTables(GridTriangle, mTables, all);
}

QList<Schema2Join> Schema2Data::findJoin(const QStringList &join)
{
    return findJoinImpl(join, mTables);
}

QSortFilterProxyModel *Schema2Data::selectProxyModel() {
    return mSelectProxyModel;
}

Schema2TableModel* Schema2Data::createTable(const SName& name)
{
    return mTables->tableCreated(name, StatusNew);
}

Schema2TableItem *Schema2Data::tableItem(const QString &name) const {
    return mTables->tableItem(name);
}

Schema2TableItem *Schema2Data::tableItem(const SName &name) const {
    return mTables->tableItem(name);
}


void Schema2Data::onSelectModelChanged(QModelIndex, QModelIndex) {
    /*QList<QPair<QString,bool>> data = mSelectModel->dataAsTupleList();
    for(int i=0;i<data.size();i++) {
        QPair<QString,bool> item = data[i];
        mTables->setChecked(item.first, !item.second);
    }*/
}


Schema2Data::Schema2Data(const QString &connectionName, QObject *parent)
    : mConnectionName(connectionName), mScene(new QGraphicsScene),
      mView(nullptr), /*mSelectModel(new CheckableStringListModel({}, this)),*/
      mSelectProxyModel(new QSortFilterProxyModel(this)), mTables(new Schema2TablesModel(connectionName, mScene, this)),
        mStored(new Schema2StoredModel(connectionName)),
      QObject{parent}
{

    mSelectProxyModel->setSourceModel(mTables);
    mSelectProxyModel->sort(0);
    //mSelectProxyModel->setDynamicSortFilter(true);

    connect(mTables,SIGNAL(tableClicked(SName, QPointF)),this,SIGNAL(tableClicked(SName, QPointF)));

    //connect(mSelectModel,SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onSelectModelChanged(QModelIndex,QModelIndex)));
    load();
    //mSelectModel->setAllChecked();


#if 0
        QTableView* view = new QTableView();
        view->setModel(mStored);
        view->show();
#endif

}
