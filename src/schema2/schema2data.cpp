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


/*static*/ QHash<QString, Schema2Data*> Schema2Data::mData = {};

Schema2Data *Schema2Data::instance(const QString &connectionName, QObject *parent)
{
    if (!mData.contains(connectionName)) {
        Schema2Data* data = new Schema2Data(connectionName, parent);
        mData[connectionName] = data;
    }
    return mData[connectionName];
}

void Schema2Data::pullTables() {
    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    QStringList tables = db.tables();

    for(const QString& table: tables) {
        if (!mTables.contains(table)) {
            mTables[table] = new Schema2TableModel(table);
        }
        Schema2TableModel* model = mTables[table];

        if (!mTableItems.contains(table)) {
            Schema2TableItem* item = new Schema2TableItem(model);
            if (mTablePos.contains(table)) {
                item->setPos(mTablePos[table]);
            } else {
                item->setPos(qreal(rand() % 800), qreal(rand() % 600));
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

            QStringList key = {parentTable, childTable};

            if (!mRelationModels.contains(key)) {
                Schema2RelationModel* relationModel = new Schema2RelationModel(parentTable, parentColumn, childTable, childColumn);
                mRelationModels[key] = relationModel;
            }

            if (!mRelationItems.contains(key)) {

                Schema2TableItem* parentTableItem = mTableItems[parentTable];
                Schema2TableItem* childTableItem = mTableItems[childTable];

                Schema2RelationItem* item = new Schema2RelationItem(mRelationModels[key], parentTableItem, childTableItem);
                mRelationItems[key] = item;

                parentTableItem->addRelation(item);
                childTableItem->addRelation(item);

                mScene->addItem(item);
            }


        }
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
}

void Schema2Data::push()
{

}

void Schema2Data::save()
{

}

void Schema2Data::load()
{
    pull();
}

bool Schema2Data::hasPendingChanges() const
{
    return false;
}

Schema2View *Schema2Data::view()
{
    if (!mView) {
        mView = new Schema2View();
        mView->setScene(mScene);
    }
    return mView;
}

Schema2Data::Schema2Data(const QString &connectionName, QObject *parent)
    : mConnectionName(connectionName), mScene(new QGraphicsScene), mView(nullptr), QObject{parent}
{
    load();
}
