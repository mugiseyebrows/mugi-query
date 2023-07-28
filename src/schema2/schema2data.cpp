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
#include "schema2relationmodel.h"
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
#include "schema2relationslistdialog.h"
#include <QAxObject>
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
    for(Schema2TableModel* table: qAsConst(mDropTableQueue)) {
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

    QStringList tables = db.tables();

    for(const QString& table: qAsConst(tables)) {

        mTables->tablePulled(table, StatusExisting);

        Schema2TableModel* model = mTables->table(table);

        // todo character_maximum_length, numeric_precision
        QSqlQuery q(db);
        q.prepare("SELECT column_name, column_type, is_nullable FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_NAME = ?");
        q.addBindValue(table);
        q.exec();

        QString prev;
        while(q.next()) {
            QString name = q.value(0).toString();
            QString type = q.value(1).toString();
            bool notNull = q.value(2).toString() == "NO";
            model->insertColumnsIfNotContains(name, type, notNull, prev);
            prev = name;
        }

    }
}


void Schema2Data::pullTablesOdbc() {

    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    QStringList tables = db.tables();

    QString filePath = accessFilePath(db);

    // todo pullRelationsOdbc for sql server
    if (filePath.isEmpty()) {
        return;
    }

    QAxObject engine("DAO.DBEngine.120");
    QAxObject* database = engine.querySubObject("OpenDatabase(QString, bool)", filePath, false);



    for(const QString& tableName: tables) {

        mTables->tablePulled(tableName, StatusExisting);

        Schema2TableModel* model = mTables->table(tableName);

        QAxObject* tableDef = database->querySubObject("TableDefs(QString)", tableName);

        QAxObject* fields = tableDef->querySubObject("Fields");
        int fieldCount = fields->property("Count").toInt();
        QString prev;
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

            model->insertColumnsIfNotContains(name, typeName, notNull, prev);

            prev = name;
        }

    }


}


void Schema2Data::pullTablesOther() {

    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    QStringList tables = db.tables();

    for(const QString& table: qAsConst(tables)) {

        mTables->tablePulled(table, StatusExisting);

        Schema2TableModel* model = mTables->table(table);

        QSqlRecord r = db.record(table);
        QString prev;
        for(int i=0;i<r.count();i++) {
            QString name = r.fieldName(i);
            QString type;
            bool notNull = false;
            model->insertColumnsIfNotContains(name, type, notNull, prev);
            prev = name;
        }

    }


}

void Schema2Data::pullTables() {
    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    if (db.driverName() == DRIVER_MYSQL || db.driverName() == DRIVER_MARIADB) {
        pullTablesMysql();
    } else if (db.driverName() == DRIVER_ODBC) {
        pullTablesOdbc();
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

void Schema2Data::pullRelationsMysql() {

    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    QStringList tables = db.tables();

    QStringList tablesLower = toLower(tables);

    QSqlQuery q(db);
    q.prepare("SELECT TABLE_NAME, COLUMN_NAME, CONSTRAINT_NAME, REFERENCED_TABLE_NAME, REFERENCED_COLUMN_NAME "
              "FROM INFORMATION_SCHEMA.KEY_COLUMN_USAGE "
              "WHERE CONSTRAINT_SCHEMA=?");
    q.addBindValue(db.databaseName());
    q.exec();

    QList<RelationItem> relations;

    while(q.next()) {
        QString childTable = q.value(0).toString();
        QString childColumn = q.value(1).toString();
        QString constraintName = q.value(2).toString();
        QString parentTable = q.value(3).toString();
        QString parentColumn = q.value(4).toString();

        if (parentTable.isEmpty()) {
            continue;
        }

        if (!tables.contains(childTable) && !tablesLower.contains(childTable.toLower())) {
            qDebug() << childTable << __FILE__ << __LINE__;
            continue;
        }
        if (!tables.contains(parentTable) && !tablesLower.contains(parentTable.toLower())) {
            qDebug() << childTable << __FILE__ << __LINE__;
            continue;
        }

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

    }

    for(const RelationItem& relation: qAsConst(relations)) {
        mTables->relationPulled(relation.constraintName, relation.childTable,
                                relation.childColumns, relation.parentTable,
                                relation.parentColumns, true, StatusExisting);
        //qDebug() << relation;
    }


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

void Schema2Data::pullIndexesOdbc() {

    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    QString filePath = accessFilePath(db);

    // todo pullIndexesOdbc for sql server
    if (filePath.isEmpty()) {
        return;
    }

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


}
#if 0
void Schema2Data::relationPulled(const QString& constraintName, const QString& childTable,
                                 const QStringList& childColumns,
                                 const QString& parentTable, const QStringList& parentColumns,
                                 bool constrained, Status status) {



    mTables->relationPulled(constraintName, childTable, childColumns, parentTable, parentColumns, constrained, status);
}
#endif

void Schema2Data::pullRelationsOdbc() {
    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    QString filePath = accessFilePath(db);

    // todo pullRelationsOdbc for sql server
    if (filePath.isEmpty()) {
        return;
    }

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

        mTables->relationPulled(constraintName, childTable, childColumns, parentTable, parentColumns, true, StatusExisting);

#if 0

        QStringList key = {childTable, parentTable};

        if (!mRelationModels.contains(key)) {
            Schema2RelationModel* relationModel = new Schema2RelationModel(childTable, childColumn,
                                                                           parentTable, parentColumn, constraintName,
                                                                           true, true);
            mRelationModels.set(key, relationModel);
        }

        if (!mRelationItems.contains(key)) {

            Schema2TableItem* childTableItem = mTableItems.get(childTable);
            Schema2TableItem* parentTableItem = mTableItems.get(parentTable);

            if (childTableItem && parentTableItem) {

                Schema2RelationItem* item = new Schema2RelationItem(mRelationModels.get(key), childTableItem, parentTableItem);
                mRelationItems.set(key, item);

                parentTableItem->addRelation(item);
                childTableItem->addRelation(item);

                mScene->addItem(item);
            } else {
                if (!childTableItem) {
                    qDebug() << "!mTableItems.contains(childTable)" << childTable;
                }
                if (!parentTableItem) {
                    qDebug() << "!mTableItems.contains(parentTable)" << parentTable;
                }
            }


        }

#endif


    }


}

void Schema2Data::pullRelations() {
    QSqlDatabase db = QSqlDatabase::database(mConnectionName);
    if (db.driverName() == DRIVER_MYSQL || db.driverName() == DRIVER_MARIADB) {
        pullRelationsMysql();
    } else if (db.driverName() == DRIVER_ODBC) {
        pullRelationsOdbc();
    } else {
        // todo implement pullRelations for QSQLITE QODBC QPSQL
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
    if (db.driverName() == DRIVER_MYSQL || db.driverName() == DRIVER_MARIADB) {
        pullIndexesMysql();
    } else if (db.driverName() == DRIVER_ODBC) {
        pullIndexesOdbc();
    } else {
        // todo implement pullRelations for QSQLITE QODBC QPSQL
    }
}

void Schema2Data::pull()
{
    pullTables();
    pullIndexes();
    pullRelations();
    mTables->setTableItemsPos();
}



void Schema2Data::push(QWidget* widget)
{
    mTables->savePos();

    QString driverName = QSqlDatabase::database(mConnectionName).driverName();
    auto* driver = this->driver();

    Schema2ChangeSet* changeSet = new Schema2ChangeSet();

    // create and alter tables
    QList<Schema2TableModel*> tables = mTables->tables();
    for(Schema2TableModel* table: tables) {
        switch(table->status()) {
        case StatusNew:
            changeSet->append(table->createQueries(driverName, driver), [=](){
                table->pushed();
            });
            break;
        case StatusModified:
            changeSet->append(table->alterQueries(driverName, driver), [=](){
                table->pushed();
            });
            break;
        case StatusExisting:
            break;
        }
    }

    // create and alter indexes
    for(Schema2TableModel* table: tables) {
        Schema2IndexesModel* indexes = table->indexes();
        changeSet->append(indexes->queries(table->tableName(), driverName, driver), [=](){
            indexes->pushed();
        });
    }

    // create and alter relations
    for(Schema2TableModel* table: tables) {
        QList<Schema2Relation*> relations = table->relations()->values();
        for(Schema2Relation* relation: relations) {
            switch(relation->status()) {
            case StatusExisting:
                break;
            case StatusModified:
                changeSet->append(relation->modifyQueries(table->tableName(), driverName, driver), [=](){
                    relation->pushed();
                });
                break;
            case StatusNew:
                changeSet->append(relation->createQueries(table->tableName(), driverName, driver), [=](){
                    relation->pushed();
                });
                break;
            }
        }
    }

    auto queue = mTables->dropRelationQueue();

    // drop relations
    for(auto item: queue) {
        QString name = item.first;
        auto* relation = item.second;
        changeSet->append(relation->dropQueries(name, driverName, driver),[=](){
            //mDropRelationsQueue.removeOne(item);
            mTables->relationDropped(name, relation);
        });
    }

    // drop tables
    for(auto* item: mDropTableQueue) {
        changeSet->append(item->dropQueries(driverName, driver),[=](){
            mDropTableQueue.removeOne(item);
        });
    }


#if 0

    for(Schema2RelationModel* relation: mRemoveRelationsQueue) {
        changeSet->append(relation->removeQuery(), [=](){
            mRemoveRelationsQueue.removeOne(relation);
        });
    }

    QList<Schema2RelationModel*> relations = mRelationModels.values();
    for(Schema2RelationModel* relation: relations) {
        if (relation->constrained() && !relation->existing()) {
            changeSet->append(relation->createQuery(),[=](){
                relation->setExisting(true);
            });
        }
    }
#endif

    if (changeSet->isEmpty()) {
        delete changeSet;
        return;
    }

#if 0
    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    QStandardItemModel* model = new QStandardItemModel(queries.size(), 2);

    for(int i=0;i<queries.size();i++) {
        QSqlQuery q(db);
        QString query = queries[i];
        QString error;
        if (!q.exec(query)) {
            error = q.lastError().text();
        } else {
            history->addQuery(mConnectionName, query);
        }
        model->setData(model->index(i, 0), query);
        model->setData(model->index(i, 1), error);
    }
#endif

    changeSet->execute(mConnectionName);

    /*QTableView* view = new QTableView();
    view->setWindowTitle("Push Result");
    view->setModel(changeSet);*/

    Schema2PushView* view = new Schema2PushView();
    view->setModel(changeSet);

    showAndRaise(view);



    /*QList<Schema2TableModel*> models = mTableModels.values();
    for(Schema2TableModel* model: models) {
        if (model->hasPendingChanges()) {

        }
    }
    QList<Schema2RelationModel*> relations = mRelationModels.values();
    for(Schema2RelationModel* relation: qAsConst(relations)) {
        if (relation->hasPendingChanges()) {

        }
    }*/


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
#if 0
    QList<Schema2RelationModel*> relations = mRelationModels.values();
    for(Schema2RelationModel* relation: qAsConst(relations)) {
        if (relation->hasPendingChanges()) {
            return true;
        }
    }
#endif
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

QGraphicsScene *Schema2Data::scene()
{
    return mScene;
}

void Schema2Data::selectOrDeselect(const QString& tableName) {

    //mSelectModel->toggleChecked(tableName);

}



void Schema2Data::createRelationDialog(const QString &childTable, const QString& parentTable, QWidget *widget)
{

    auto* childTableModel = mTables->table(childTable);
    auto* parentTableModel = mTables->table(parentTable);

    auto* relation = childTableModel->relationTo(parentTable);

    if (!childTableModel) {
        qDebug() << "!childTableModel" << __FILE__ << __LINE__;
        return;
    }
    if (!parentTableModel) {
        qDebug() << "!parentTableModel" << __FILE__ << __LINE__;
        return;
    }

    // todo implement multiple relations between same pair of tables
    if (relation) {
        editRelationDialog(relation, widget);
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

    mTables->relationPulled(dialog.relationName(),
                   childTable, dialog.childColumns(),
                   parentTable, dialog.parentColumns(), dialog.constrained(), StatusNew);


#if 0
    if (!mTableModels.contains(childTable)
            || !mTableModels.contains(parentTable)
            || !mTableItems.contains(childTable)
            || !mTableItems.contains(parentTable)
            ) {
        report_error("!mTableModels.contains() || !mTableItems.contains()", __FILE__, __LINE__, widget);
        return;
    }

    QStringList key = {childTable, parentTable};

    Schema2RelationModel* model = 0;
    bool newRelation = true;

    if (mRelationModels.contains(key)) {
        model = mRelationModels.get(key);
        newRelation = false;
    }

    Schema2TableModel* childModel = mTableModels.get(childTable);
    Schema2TableModel* parentModel = mTableModels.get(parentTable);

    Schema2TableItem* childItem = mTableItems.get(childTable);
    Schema2TableItem* parentItem = mTableItems.get(parentTable);

    Schema2RelationDialog dialog(widget);
    dialog.init(childModel, parentModel, model);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    if (dialog.remove()) {
        if (model) {
            mRemoveRelationsQueue.append(model);
        }
    } else {

        if (newRelation) {

            Schema2RelationModel* model = new Schema2RelationModel(childTable, dialog.childColumn(),
                                                                   parentTable, dialog.parentColumn(),
                                                                   dialog.constraintName(),
                                                                   dialog.constrained(), false);
            mRelationModels.set(key,model);
            Schema2RelationItem* item = new Schema2RelationItem(model, childItem, parentItem);
            mRelationItems.set(key, item);
            mScene->addItem(item);

            childItem->addRelation(item);
            parentItem->addRelation(item);

        } else {


        }

    }

#endif
}

void Schema2Data::dropRelationDialog(Schema2Relation *relation, QWidget *widget) {
    QString message = QString("Are you sure to drop relation %1?").arg(relation->name());
    if (!ConfirmationDialog::question(widget, message, &mDontAskOnDropRelation)) {
        return;
    }
    tables()->relationRemoved(relation);
}

void Schema2Data::dropRelationDialog(const QString &childTable, const QString &parentTable, QWidget *widget)
{
    auto* table1 = mTables->table(childTable);
    auto* table2 = mTables->table(parentTable);
    if (!table1 || !table2) {
        return;
    }
    auto* relation = table1->relationTo(table2->tableName());
    if (relation) {
        return dropRelationDialog(relation, widget);
    }
    relation = table2->relationTo(table1->tableName());
    if (relation) {
        return dropRelationDialog(relation, widget);
    }
}

void Schema2Data::dropTableDialog(const QString &tableName, QWidget *widget) {

    QString message = QString("Are you sure to drop table %1?").arg(tableName);

    if (!ConfirmationDialog::question(widget, message, &mDontAskOnDropTable)) {
        return;
    }

    auto* table = mTables->table(tableName);
    if (!table) {
        qDebug() << __FILE__ << __LINE__;
        return;
    }
    auto relations = (mTables->relationsTo(tableName) + mTables->relationsFrom(tableName)).toSet();

    qDebug() << relations.size() << "relations with table" << tableName;

    for(auto* relation: relations) {
        mTables->relationRemoved(relation);
    }

    qDebug() << "dropRelationQueue.size()" << mTables->dropRelationQueue();

    mTables->tableRemoved(tableName);
    mDropTableQueue.append(table);

    Schema2AlterView* view = mAlterViews.get(tableName);
    if (view) {
        mAlterViews.remove(tableName);
        view->hide();
        view->deleteLater();
    }

}

QString Schema2Data::driverName() const {
    return QSqlDatabase::database(mConnectionName).driverName();
}

Schema2AlterView *Schema2Data::alterView(const QString &table) {
    return mAlterViews.get(table);
}

Schema2TablesModel *Schema2Data::tables() const {
    return mTables;
}


QSqlDriver* Schema2Data::driver() const {
    return QSqlDatabase::database(mConnectionName).driver();
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
        model->setQuery(q);

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
                    .arg(toPythonStr(relation->childTable()))
                    .arg(toPythonStr(relation->parentTable()))
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
        items[table->tableName()] = table->indexes()->primaryKey();
    }
    QString text = toPythonDict(items);
    qApp->clipboard()->setText(text);
    QMessageBox::information(widget, "", "Copied to clipboard");
}

#include <QSvgGenerator>
#include <QFileInfo>

static QString dotQuoted(const QString& name) {
    if (name.contains(" ") || name.contains("-")) {
        return "\"" + name + "\"";
    }
    return name;
}

static QString dotTableRow(const QString& cell, bool bold = false) {
    return QString("<tr><td>%1</td></tr>").arg(bold ? "<b>" + cell + "</b>" : cell);
}

static QString dotTable(Schema2TableItem * table, double posk) {

    QStringList columns;
    auto names = table->model()->columnNames();
    for(const QString& name: names) {
        columns.append(dotTableRow(name));
    }

    QString label = QString("<<table border=\"0\" cellborder=\"1\" cellspacing=\"0\" cellpadding=\"4\">\n"
                    "%1\n"
                    "<tr><td>\n"
                    "<table border=\"0\" cellborder=\"0\" cellspacing=\"0\" >\n"
                    "%2\n"
                    "</table>\n"
                    "</td></tr>\n"
                    "</table>>")
            .arg(dotTableRow(table->tableName(), true))
            .arg(columns.join("\n"));

    QPointF pos = table->centerPos() * posk;

    return QString("%1 [label=%2\npos=\"%3,%4!\"]\n").arg(dotQuoted(table->tableName())).arg(label).arg(pos.x()).arg(-pos.y());
}

static QString dotRelation(Schema2Relation* relation) {
    auto childTable = relation->childTable();
    auto parentTable = relation->parentTable();
    return QString("%1 -> %2\n")
            .arg(dotQuoted(childTable))
            .arg(dotQuoted(parentTable));
}

void Schema2Data::saveAs(const QString &path, OutputFormat format, QWidget *widget)
{

    if (format == PngFormat) {



    } else if (format == SvgFormat) {

        QSvgGenerator generator;

        auto rect = mView->sceneRect();

        generator.setFileName(path);
        generator.setSize(rect.size().toSize());
        generator.setViewBox(rect);
        QPainter painter;
        painter.begin(&generator);
        mScene->render(&painter, rect);
        painter.end();

    } else if (format == DotFormat) {

        QStringList res;
        double posk = 0.01;

        auto tables = mTables->tableItems();
        for(auto* table: tables) {
            if (table->grayed()) {
                continue;
            }
            res.append(dotTable(table, posk));
        }

        for(auto* table: tables) {
            auto relations = table->model()->relations()->values();
            for(auto* relation: relations) {
                auto* childTable = mTables->tableItem(relation->parentTable());
                auto* parentTable = mTables->tableItem(relation->childTable());
                if (childTable->grayed()) {
                    continue;
                }
                if (parentTable->grayed()) {
                    continue;
                }
                res.append(dotRelation(relation));
            }
        }

        QFile file(path);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::critical(widget, "Error", QString("Cannot open %1 for writing").arg(path));
            return;
        }

        QString text = QString("digraph G {\nnode [shape=plain]\n%1\n}").arg(res.join("\n"));
        file.write(text.toUtf8());

    } else if (format == DbioFormat) {

        QStringList tablesText;
        QStringList relationsText;

        auto tables = mTables->tableItems();

        for(auto* table: tables) {

            if (table->grayed()) {
                continue;
            }

            auto relations = table->model()->relations()->values();

            for(Schema2Relation* relation: relations) {
                auto* childTable = mTables->tableItem(relation->parentTable());
                auto* parentTable = mTables->tableItem(relation->childTable());
                if (childTable->grayed()) {
                    continue;
                }
                if (parentTable->grayed()) {
                    continue;
                }
                QString item = QString("ref {\n    %1.%2 > %3.%4\n}")
                        .arg(relation->childTable())
                        .arg(relation->childColumns().join(""))
                        .arg(relation->parentTable())
                        .arg(relation->parentColumns().join(""));
                relationsText.append(item);
            }

            Schema2TableModel* tableModel = table->model();

            QStringList fields;
            for(int row=0;row<tableModel->rowCount();row++) {
                QString type = tableModel->type(row);
                type.replace(" unsigned", "");
                QString field = QString("    %1 %2")
                        .arg(tableModel->name(row))
                        .arg(type);
                fields.append(field);
            }
            QString item = QString("table %1\n{\n%2\n}")
                    .arg(table->tableName())
                    .arg(fields.join("\n"));
            tablesText.append(item);
        }

        QFile file(path);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::critical(widget, "Error", QString("Cannot open %1 for writing").arg(path));
            return;
        }

        file.write((tablesText.join("\n") + "\n").toUtf8());
        file.write((relationsText.join("\n") + "\n").toUtf8());

    }

}

void Schema2Data::scriptDialog(QWidget *widget)
{
    auto driverName = this->driverName();
    auto* driver = this->driver();

    QStringList queries =
            mTables->createTablesQueries(driverName, driver)
            + mTables->createIndexesQueries(driverName, driver)
            + mTables->createRelationsQueries(driverName, driver);

    qApp->clipboard()->setText(queries.join(";\n") + "\n");
    QMessageBox::information(widget, "", "Copied to clipboard");
}

static QStringList mysqlTypes() {
    // https://dev.mysql.com/doc/refman/8.0/en/data-types.html

    // numeric
    QStringList exact = {"INTEGER", "SMALLINT", "DECIMAL"};
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

    auto* relation = childTable->relationTo(parentTable);

    QString relationName;

    QStringList parentColumns;

    Schema2RelationGuesser guesser(childTable, parentTableModel);

    if (relation == 0) {
        parentColumns = guesser.parentColumns();
        relationName = guesser.relationName();
    } else {
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

    if (relation) {

        relation->setName(dialog.relationName());
        relation->setChildColumns(dialog.childColumns());
        relation->setParentColumns(dialog.parentColumns());

    } else {

        QString constraintName = dialog.relationName();
        QStringList childColumns = dialog.childColumns();
        QStringList parentColumns = dialog.parentColumns();
        bool constrained = dialog.constrained();

        mTables->relationPulled(constraintName, childTable->tableName(), childColumns,
                                parentTable, parentColumns, constrained, StatusNew);

    }


}

void Schema2Data::showAlterView(const QString &tableName)
{
    if (!mAlterViews.contains(tableName)) {
        Schema2AlterView* view = new Schema2AlterView();
        Schema2TableModel* table = mTables->table(tableName);

        if (!table) {
            qDebug() << "!table" << __FILE__ << __LINE__;
            return;
        }

        view->init(this, mTables, table, dataTypes());
        mAlterViews.set(tableName, view);
    }
    auto* view = mAlterViews.get(tableName);
    showAndRaise(view);
}

void Schema2Data::showInsertView(const QString &tableName)
{

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


#if 0
void Schema2Data::showRelationsListDialog(QWidget* widget) {

    Schema2RelationsListDialog dialog;
    dialog.init(mTableModels, mRelationModels);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }
    QString childTable = dialog.childTable();
    QString parentTable = dialog.parentTable();
    showRelationDialog(childTable, parentTable, widget);

}
#endif

Schema2TableModel* Schema2Data::createTable(const QString &name)
{
    return mTables->tablePulled(name, StatusNew);
}

Schema2TableItem *Schema2Data::tableItem(const QString &name) const {
    return mTables->tableItem(name);
}


void Schema2Data::onSelectModelChanged(QModelIndex, QModelIndex) {
    /*QList<QPair<QString,bool>> data = mSelectModel->dataAsTupleList();
    for(int i=0;i<data.size();i++) {
        QPair<QString,bool> item = data[i];
        mTables->setGrayed(item.first, !item.second);
    }*/
}

Schema2Data::Schema2Data(const QString &connectionName, QObject *parent)
    : mConnectionName(connectionName), mScene(new QGraphicsScene),
      mView(nullptr), /*mSelectModel(new CheckableStringListModel({}, this)),*/
      mSelectProxyModel(new QSortFilterProxyModel(this)), mTables(new Schema2TablesModel(connectionName, mScene, this)),
      QObject{parent}
{


    mSelectProxyModel->setSourceModel(mTables);
    mSelectProxyModel->sort(0);
    //mSelectProxyModel->setDynamicSortFilter(true);

    connect(mTables,SIGNAL(tableClicked(QString)),this,SIGNAL(tableClicked(QString)));

    //connect(mSelectModel,SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onSelectModelChanged(QModelIndex,QModelIndex)));
    load();
    //mSelectModel->setAllChecked();
}
