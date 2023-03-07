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

#if 0
    int w = 200;
    int s = 25;

    int gw = w + 20;
    int gh = s * 7;

    int gcc = (1920 - 200) / gw ;
    int grc = (1080 - 200) / gh ;

    qDebug() << "cc rc" << gcc << grc;

    QSet<QPair<int,int> > grid;
#endif

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

#if 0
                int c;
                int r;
                for(int i=0;i<100;i++) {
                    c = rand() % gcc;
                    r = rand() % grc;
                    if (!grid.contains({r, c})) {
                        grid.insert({r, c});
                        break;
                    }
                    //qDebug() << "rc" << r << c;
                }
                int x = c * gw;
                int y = r * gh;
                item->setPos(x, y);
#endif
                mSetPosQueue.append(item);

                //item->setPos(qreal(rand() % 800), qreal(rand() % 600));
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

void Schema2Data::pullRelations() {
    QSqlDatabase db = QSqlDatabase::database(mConnectionName);
    if (db.driverName() == DRIVER_MYSQL || db.driverName() == DRIVER_QMARIADB) {
        pullRelationsMysql();
    } else {
        // todo implement pullRelations for QSQLITE QODBC QPSQL
    }

}

void Schema2Data::pull()
{
    pullTables();
    pullRelations();
    setTableItemsPos();
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

Schema2Data::Schema2Data(const QString &connectionName, QObject *parent)
    : mConnectionName(connectionName), mScene(new QGraphicsScene), mView(nullptr), QObject{parent}
{
    load();
}
