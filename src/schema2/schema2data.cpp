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

/*static*/ QHash<QString, Schema2Data*> Schema2Data::mData = {};

Schema2Data *Schema2Data::instance(const QString &connectionName, QObject *parent)
{
    if (!mData.contains(connectionName)) {
        Schema2Data* data = new Schema2Data(connectionName, parent);
        mData[connectionName] = data;
    }
    return mData[connectionName];
}

#if 0
void Schema2Data::unoverlapTables() {

    int w = 200;
    arrangeInGrid(mTableItems.values(), w, 700, 40);
}
#endif

void Schema2Data::pullTables() {
    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    QStringList tables = db.tables();

    for(const QString& table: qAsConst(tables)) {
        if (!mTableModels.contains(table)) {
            Schema2TableModel* model = new Schema2TableModel(table, true);
            mTableModels[table] = model;
            connect(model, SIGNAL(tableClicked(QString)), this, SIGNAL(tableClicked(QString)));
        }
        Schema2TableModel* model = mTableModels[table];

        if (!mTableItems.contains(table)) {
            Schema2TableItem* item = new Schema2TableItem(model);
            if (mTablePos.contains(table)) {
                item->setPos(mTablePos[table]);
            } else {
                mSetPosQueue.append(item);
            }
            mTableItems[table] = item;
            mScene->addItem(item);
        }

        if (db.driverName() == DRIVER_MYSQL || db.driverName() == DRIVER_QMARIADB) {
            // todo character_maximum_length, numeric_precision
            QSqlQuery q(db);
            q.prepare("SELECT COLUMN_NAME, DATA_TYPE FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_NAME = ?");
            q.addBindValue(table);
            q.exec();

            QString prev;
            while(q.next()) {
                QString name = q.value(0).toString();
                QString type = q.value(1).toString();
                model->insertIfNotContains(name, type, prev);
                prev = name;
            }

        } else {
            // todo read types for ODBC PSQL

            QSqlRecord r = db.record(table);
            QString prev;
            for(int i=0;i<r.count();i++) {
                QString name = r.fieldName(i);
                QString type;
                model->insertIfNotContains(name, type, prev);
                prev = name;
            }

        }

    }

}

void Schema2Data::pullRelationsMysql() {

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

        Q_UNUSED(constraintName);

        if (parentTable.isEmpty()) {
            continue;
        }
        if (tables.contains(childTable) && tables.contains(parentTable)) {

            QStringList key = {childTable, parentTable};

            if (!mRelationModels.contains(key)) {
                Schema2RelationModel* relationModel = new Schema2RelationModel(childTable, childColumn, parentTable, parentColumn, true, true);
                mRelationModels[key] = relationModel;
            }

            if (!mRelationItems.contains(key)) {

                Schema2TableItem* childTableItem = mTableItems[childTable];
                Schema2TableItem* parentTableItem = mTableItems[parentTable];

                Schema2RelationItem* item = new Schema2RelationItem(mRelationModels[key], childTableItem, parentTableItem);
                mRelationItems[key] = item;

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

#include <QRegularExpression>

static QString unquoted(const QString& path) {
    if (path.startsWith("\"") && path.endsWith("\"")) {
        return path.mid(1, path.size()-2);
    }
    return path;
}

#include <QAxObject>

template <class T>
T* hash_find(const QHash<QString, T*>& hash, const QString& key) {
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

void Schema2Data::pullRelationsOdbc() {
    QSqlDatabase db = QSqlDatabase::database(mConnectionName);
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

        if (fieldCount != 1) {
            qDebug() << "pullRelationsOdbc fieldCount != 1" << childTable << parentTable;
        }

        fieldCount = 1;

        QString childColumn;
        QString parentColumn;
        for(int j=0;j<fieldCount;j++) {
            QAxObject* field = relation->querySubObject("Fields(int)", j);
            parentColumn = field->property("Name").toString();
            childColumn = field->property("ForeignName").toString();
        }

        QStringList key = {childTable, parentTable};

        if (!mRelationModels.contains(key)) {
            Schema2RelationModel* relationModel = new Schema2RelationModel(childTable, childColumn, parentTable, parentColumn, true, true);
            mRelationModels[key] = relationModel;
        }

        if (!mRelationItems.contains(key)) {

            Schema2TableItem* childTableItem = hash_find(mTableItems, childTable);
            Schema2TableItem* parentTableItem = hash_find(mTableItems, parentTable);

            if (childTableItem && parentTableItem) {

                Schema2RelationItem* item = new Schema2RelationItem(mRelationModels[key], childTableItem, parentTableItem);
                mRelationItems[key] = item;

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

void Schema2Data::pull()
{
    pullTables();
    pullRelations();
    setTableItemsPos();

    mSelectModel->setList(mTableModels.keys());

}

void Schema2Data::push()
{
    savePos();

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
    QList<Schema2RelationModel*> relations = mRelationModels.values();
    for(Schema2RelationModel* relation: qAsConst(relations)) {
        if (relation->hasPendingChanges()) {
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

QGraphicsScene *Schema2Data::scene()
{
    return mScene;
}

void Schema2Data::selectOrDeselect(const QString& tableName) {

    mSelectModel->toggleChecked(tableName);

}

void Schema2Data::showRelationDialog(const QString &childTable, const QString& parentTable, QWidget *parent)
{

    if (!mTableModels.contains(childTable)
            || !mTableModels.contains(parentTable)
            || !mTableItems.contains(childTable)
            || !mTableItems.contains(parentTable)
            ) {
        report_error("!mTableModels.contains() || !mTableItems.contains()", __FILE__, __LINE__, parent);
        return;
    }

    QStringList key = {childTable, parentTable};

    Schema2RelationModel* model = 0;
    bool newRelation = true;

    if (mRelationModels.contains(key)) {
        model = mRelationModels[key];
        newRelation = false;
    }

    Schema2TableModel* childModel = mTableModels[childTable];
    Schema2TableModel* parentModel = mTableModels[parentTable];

    Schema2TableItem* childItem = mTableItems[childTable];
    Schema2TableItem* parentItem = mTableItems[parentTable];

    Schema2RelationDialog dialog(parent);
    dialog.init(childModel, parentModel, model);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    if (dialog.remove()) {
        // todo implement relation removal

    } else {

        if (newRelation) {

            Schema2RelationModel* model = new Schema2RelationModel(childTable, dialog.childColumn(), parentTable, dialog.parentColumn(), dialog.constrained(), false);
            mRelationModels[key] = model;
            Schema2RelationItem* item = new Schema2RelationItem(model, childItem, parentItem);
            mRelationItems[key] = item;
            mScene->addItem(item);

            childItem->addRelation(item);
            parentItem->addRelation(item);

        } else {


        }

    }


}

void Schema2Data::showAlterView(const QString &tableName)
{
    if (!mAlterViews.contains(tableName)) {
        Schema2AlterView* view = new Schema2AlterView();
        view->setModel(mTableModels[tableName]);
        view->setWindowTitle(tableName);
        mAlterViews[tableName] = view;
    }
    auto* view = mAlterViews[tableName];
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

    arrangeTables(GridTriangle, mTableItems.keys(), mRelationModels, mTableItems);
}

QList<Schema2Join> Schema2Data::findJoin(const QStringList &join)
{
    return findJoinImpl(join, mTableModels, mRelationModels);
}



void Schema2Data::onSelectModelChanged(QModelIndex, QModelIndex) {
    QList<QPair<QString,bool>> data = mSelectModel->dataAsTupleList();
    for(int i=0;i<data.size();i++) {
        QPair<QString,bool> item = data[i];
        mTableItems[item.first]->setGrayed(!item.second);
    }
}

Schema2Data::Schema2Data(const QString &connectionName, QObject *parent)
    : mConnectionName(connectionName), mScene(new QGraphicsScene),
      mView(nullptr), mSelectModel(new CheckableStringListModel({}, this)),
      mSelectProxyModel(new QSortFilterProxyModel(this)), QObject{parent}
{
    mSelectProxyModel->setSourceModel(mSelectModel);
    connect(mSelectModel,SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onSelectModelChanged(QModelIndex,QModelIndex)));
    load();
    mSelectModel->setAllChecked();
}
