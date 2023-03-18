#include "schema2data.h"
#include <QSqlDatabase>

#include "drivernames.h"
#include <QSqlQuery>
#include "schema2tablemodel.h"
#include <QSqlRecord>
#include <QSqlDatabase>
#include "schema2view.h"
#include "schema2tableitem.h"
#include "schema2relationitem.h"
#include "schema2relationitem2.h"
#include "schema2relationmodel.h"
#include <QDebug>
#include "schema2arrange.h"
#include "schema2alterview.h"
#include "showandraise.h"
#include "datautils.h"
#include "schema2store.h"
#include "schema2relationdialog.h"
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


void Schema2Data::tablePulled(const QString& table, Status status) {
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
}


void Schema2Data::pullTablesMysql() {

    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    QStringList tables = db.tables();

    for(const QString& table: qAsConst(tables)) {

        tablePulled(table, StatusExisting);

        Schema2TableModel* model = mTableModels.get(table);

        // todo character_maximum_length, numeric_precision
        QSqlQuery q(db);
        q.prepare("SELECT COLUMN_NAME, DATA_TYPE FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_NAME = ?");
        q.addBindValue(table);
        q.exec();

        QString prev;
        while(q.next()) {
            QString name = q.value(0).toString();
            QString type = q.value(1).toString();
            model->insertColumnsIfNotContains(name, type, prev);
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

        tablePulled(tableName, StatusExisting);

        Schema2TableModel* model = mTableModels.get(tableName);

        QAxObject* tableDef = database->querySubObject("TableDefs(QString)", tableName);

        QAxObject* fields = tableDef->querySubObject("Fields");
        int fieldCount = fields->property("Count").toInt();
        QString prev;
        for(int i=0;i<fieldCount;i++) {
            QAxObject* field = tableDef->querySubObject("Fields(int)", i);
            QString name = field->property("Name").toString();
            int type = field->property("Type").toInt();
            int size = field->property("Size").toInt();

            //qDebug() << tableName << name << type;

            if (!mOdbcTypes.contains(type) || 0) {
                qDebug() << tableName << name << type << size;
            }

            QString typeName = mOdbcTypes.value(type, "UNKNOWN");
            if (type == dbText) {
                if (size != 255) {
                    typeName = QString("TEXT(%1)").arg(size);
                }
            }

            model->insertColumnsIfNotContains(name, typeName, prev);

            prev = name;
        }

    }


}


void Schema2Data::pullTablesOther() {

    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    QStringList tables = db.tables();

    for(const QString& table: qAsConst(tables)) {

        tablePulled(table, StatusExisting);

        Schema2TableModel* model = mTableModels.get(table);

        QSqlRecord r = db.record(table);
        QString prev;
        for(int i=0;i<r.count();i++) {
            QString name = r.fieldName(i);
            QString type;
            model->insertColumnsIfNotContains(name, type, prev);
            prev = name;
        }

    }


}

void Schema2Data::pullTables() {
    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    if (db.driverName() == DRIVER_MYSQL || db.driverName() == DRIVER_QMARIADB) {
        pullTablesMysql();
    } else if (db.driverName() == DRIVER_ODBC) {
        pullTablesOdbc();
    } else {
        pullTablesOther();
    }

}


void Schema2Data::pullRelationsMysql() {
#if 0


    //return;

    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    QStringList tables = db.tables();

    QSqlQuery q(db);
    q.prepare("SELECT TABLE_NAME, COLUMN_NAME, CONSTRAINT_NAME, REFERENCED_TABLE_NAME, REFERENCED_COLUMN_NAME FROM INFORMATION_SCHEMA.KEY_COLUMN_USAGE");
    q.exec();
    while(q.next()) {
        QString childTable = q.value(0).toString();
        QString childColumn = q.value(1).toString();
        QString constraintName = q.value(2).toString();
        QString parentTable = q.value(3).toString();
        QString parentColumn = q.value(4).toString();

        if (parentTable.isEmpty()) {
            continue;
        }
        if (tables.contains(childTable) && tables.contains(parentTable)) {

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

                Schema2RelationItem* item = new Schema2RelationItem(mRelationModels.get(key), childTableItem, parentTableItem);
                mRelationItems.set(key, item);

                parentTableItem->addRelation(item);
                childTableItem->addRelation(item);

                mScene->addItem(item);
            }


        } else {

#if 0
            // not an error: mariadb stores all databases data in one INFORMATION_SCHEMA
            qDebug() << "childTable" << childTable << "parentTable" << parentTable
                     << "tables.contains(childTable)" << tables.contains(childTable)
                     << "tables.contains(parentTable)" << tables.contains(parentTable)
                     << __FILE__ << __LINE__;
#endif
        }
    }
#endif
}

void Schema2Data::setTableItemsPos() {

    int w = 200;
    int s = 25;
    int spacing = 40;
    int h = 900;

    int x = 0;
    int y = 0;

    for(Schema2TableItem* item: mSetPosQueue) {
        int height = item->boundingRect().height();
        if ((y + height + spacing) > h) {
            x += w + spacing;
            y = 0;
            item->setPos(x, y);
        } else {
            item->setPos(x, y);
        }
        y += height + spacing;
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

void Schema2Data::indexPulled(const QString indexName, const QString& tableName, const QStringList& columns,
                              bool primary, Status status) {
    if (mTableModels.contains(tableName)) {
        mTableModels.get(tableName)->insertIndex(indexName, columns, primary, status);
    } else {
        qDebug() << "!mTableModels.contains(tableName)" << tableName;
    }
}

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

            QStringList columns;

            QAxObject* fields = index->querySubObject("Fields");
            int fieldCount = fields->property("Count").toInt();
            for(int k=0;k<fieldCount;k++) {
                QAxObject* field = index->querySubObject("Fields(int)", k);
                QString fieldName = field->property("Name").toString();
                columns.append(fieldName);
            }

            indexPulled(indexName, tableName, columns, primary, StatusExisting);

        }
    }


}

void Schema2Data::relationPulled(const QString& constraintName, const QString& childTable, const QStringList& childColumns,
                                 const QString& parentTable, const QStringList& parentColumns,
                                 bool constrained, Status status) {
    Schema2TableItem* childItem = mTableItems.get(childTable);
    Schema2TableItem* parentItem = mTableItems.get(parentTable);

    Schema2TableModel* childModel = mTableModels.get(childTable);
    Schema2TableModel* parentModel = mTableModels.get(parentTable);

    if (!childItem || !parentItem || !childModel || !parentModel) {
        qDebug() << childTable << childModel << childItem
                 << parentTable << parentModel << parentItem << __FILE__ << __LINE__;
        return;
    }

    Schema2Relation* newRelation = childModel->insertRelation(constraintName, childColumns, parentTable, parentColumns, constrained, status);
    if (newRelation) {
        Schema2RelationItem2* relationItem =
                new Schema2RelationItem2(childItem, parentItem);

        mRelationItems.append(relationItem);
        parentItem->addRelation(relationItem);
        childItem->addRelation(relationItem);

        mScene->addItem(relationItem);
    }
}


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

        relationPulled(constraintName, childTable, childColumns, parentTable, parentColumns, true, StatusExisting);

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
    if (db.driverName() == DRIVER_MYSQL || db.driverName() == DRIVER_QMARIADB) {
        pullRelationsMysql();
    } else if (db.driverName() == DRIVER_ODBC) {
        pullRelationsOdbc();
    } else {
        // todo implement pullRelations for QSQLITE QODBC QPSQL
    }

}

void Schema2Data::pullIndexesMysql() {

}

void Schema2Data::pullIndexes() {
    QSqlDatabase db = QSqlDatabase::database(mConnectionName);
    if (db.driverName() == DRIVER_MYSQL || db.driverName() == DRIVER_QMARIADB) {
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

    setTableItemsPos();

    mSelectModel->setList(mTableModels.keys());

}

#include "schema2pushview.h"

void Schema2Data::push(QWidget* widget)
{
    savePos();

    Schema2ChangeSet* changeSet = new Schema2ChangeSet();

    // tables
    QList<Schema2TableModel*> tables = mTableModels.values();
    for(Schema2TableModel* table: tables) {
        switch(table->status()) {
        case StatusNew:
            changeSet->append(table->createQueries(), [=](){
                table->altered();
            });
            break;
        case StatusModified:
            changeSet->append(table->alterQueries(), [=](){
                table->altered();
            });
            break;
        case StatusExisting:
            break;
        }
    }

    // indexes
    for(Schema2TableModel* table: tables) {
        Schema2IndexesModel* indexes = table->indexes();
        QStringList queries = indexes->queries(table->tableName());
        if (!queries.isEmpty()) {
            changeSet->append(queries, [=](){
                indexes->altered();
            });
        }
    }

    // relations
    for(Schema2TableModel* table: tables) {
        QList<Schema2Relation*> relations = table->getRelations().values();
        for(Schema2Relation* relation: relations) {
            switch(relation->status()) {
            case StatusExisting:
                break;
            case StatusModified:
                // todo implement recreate relation
                break;
            case StatusNew:
                changeSet->append(relation->createQueries(table->tableName()), [=](){
                    relation->created();
                });
                break;
            }
        }
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
    loadPos();
    pull();

}

bool Schema2Data::hasPendingChanges() const
{
    if (!mRemoveRelationsQueue.isEmpty()) {
        return true;
    }
    QList<Schema2TableModel*> models = mTableModels.values();
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

    mSelectModel->toggleChecked(tableName);

}

void Schema2Data::showRelationDialog(const QString &childTable, const QString& parentTable, QWidget *widget)
{
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

QStringList Schema2Data::dataTypes() const {

    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    if (db.driverName() == DRIVER_ODBC) {
        return mOdbcTypes.values();
    }

    // todo implement datatypes for

    return {};
}

QStringList Schema2Data::guessParentColumns(QString childTable, QStringList childColumns, QString parentTable) {

    QStringList res;
    auto* model = mTableModels.get(parentTable);
    for(int row=0;row<childColumns.size();row++) {
        res.append(model->newName(row));
    }
    return res;
}

#include "schema2relationdialog2.h"

void Schema2Data::onCreateRelation(QString childTable, QStringList childColumns, QString parentTable) {

    if (!mTableModels.get(parentTable)) {
        return;
    }

    auto* childTableModel = mTableModels.get(childTable);
    auto* parentTableModel = mTableModels.get(parentTable);
    auto* relation = childTableModel->getRelationTo(parentTable);

    QString constraintName;

    QStringList parentColumns;

    if (relation == 0) {
        parentColumns = guessParentColumns(childTable, childColumns, parentTable);
        constraintName = QString("FK_%1_%2").arg(parentTable).arg(childTable);
    } else {
        parentColumns = relation->parentColumns();
        constraintName = relation->name();
    }

    Schema2RelationDialog2 dialog;
    dialog.init(childTableModel, parentTableModel,
                constraintName,
                childColumns,
                parentColumns);

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    if (relation) {

    } else {

        QString constraintName = dialog.constraintName();
        QStringList childColumns = dialog.childColumns();
        QStringList parentColumns = dialog.parentColumns();
        bool constrained = dialog.constrained();

        relationPulled(constraintName, childTable, childColumns,
                       parentTable, parentColumns, constrained, StatusNew);

    }


}

void Schema2Data::showAlterView(const QString &tableName)
{

    if (!mAlterViews.contains(tableName)) {
        Schema2AlterView* view = new Schema2AlterView();
        Schema2TableModel* table = mTableModels.get(tableName);
        view->init(this, mTableModels, table, dataTypes());
        view->setWindowTitle(tableName);
        mAlterViews.set(tableName, view);
        connect(view,
                SIGNAL(createRelation(QString, QStringList, QString)),
                this,
                SLOT(onCreateRelation(QString, QStringList, QString)));
    }
    auto* view = mAlterViews.get(tableName);
    showAndRaise(view);
}

void Schema2Data::showInsertView(const QString &tableName)
{

}

void Schema2Data::loadPos()
{
    mTablePos = Schema2Store::instance(this)->loadPos(mConnectionName);
}

void Schema2Data::savePos()
{
    QHash<QString, QPointF> pos;
    QList<Schema2TableItem*> items = mTableItems.values();
    for(Schema2TableItem* item: qAsConst(items)) {
        pos[item->tableName()] = item->pos();
    }
    Schema2Store::instance(this)->savePos(mConnectionName, pos);
}

void Schema2Data::arrange()
{
    //squareArrange(mTableItems.keys(), mRelationModels, mTableItems);

#if 0

    arrangeTables(GridTriangle, mTableItems.keys(), mRelationModels, mTableItems);
#endif
}

QList<Schema2Join> Schema2Data::findJoin(const QStringList &join)
{
    return findJoinImpl(join, mTableModels);
}

QSortFilterProxyModel *Schema2Data::selectProxyModel() {
    return mSelectProxyModel;
}



void Schema2Data::showRelationsListDialog(QWidget* widget) {
#if 0
    Schema2RelationsListDialog dialog;
    dialog.init(mTableModels, mRelationModels);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }
    QString childTable = dialog.childTable();
    QString parentTable = dialog.parentTable();
    showRelationDialog(childTable, parentTable, widget);
#endif
}

void Schema2Data::createTable(const QString &name)
{
    tablePulled(name, StatusNew);
}


void Schema2Data::onSelectModelChanged(QModelIndex, QModelIndex) {
    QList<QPair<QString,bool>> data = mSelectModel->dataAsTupleList();
    for(int i=0;i<data.size();i++) {
        QPair<QString,bool> item = data[i];
        mTableItems.get(item.first)->setGrayed(!item.second);
    }
}

Schema2Data::Schema2Data(const QString &connectionName, QObject *parent)
    : mConnectionName(connectionName), mScene(new QGraphicsScene),
      mView(nullptr), mSelectModel(new CheckableStringListModel({}, this)),
      mSelectProxyModel(new QSortFilterProxyModel(this)), QObject{parent}
{
    mSelectProxyModel->setSourceModel(mSelectModel);
    mSelectProxyModel->sort(0);
    //mSelectProxyModel->setDynamicSortFilter(true);

    connect(mSelectModel,SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onSelectModelChanged(QModelIndex,QModelIndex)));
    load();
    mSelectModel->setAllChecked();
}
