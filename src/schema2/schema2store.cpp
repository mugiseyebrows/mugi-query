#include "schema2store.h"

#include <QDebug>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSet>
#include <QSqlRecord>
#include <QPointF>
#include "sqlutil.h"
#include "sdata.h"

static QStringList filterEmpty(const QStringList& values) {
    QStringList res;
    for(const QString& value: values) {
        if (value.isEmpty()) {
            continue;
        }
        res.append(value);
    }
    return res;
}

static QString connectionKey(const QString connectionName) {
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QStringList res = filterEmpty({db.driverName(), db.databaseName(), db.hostName()});
    return res.join(" ");
}

Schema2Store* Schema2Store::mInstance = 0;

Schema2Store *Schema2Store::instance(QObject *parent)
{
    if (!mInstance) {
        mInstance = new Schema2Store(parent);
    }
    return mInstance;
}

QHash<SName, QPointF> Schema2Store::loadPos(const QString &connectionName)
{
    QSqlDatabase db = QSqlDatabase::database("_history");
    QList<QVariantList> rows = sql::select_where(db, "schema2_pos", {"schema", "name", "x", "y"}, "db = ?", {connectionKey(connectionName)});
    QHash<SName, QPointF> res;
    for(const QVariantList& row: rows) {
        QString schema = row[0].toString();
        QString name = row[1].toString();
        double x = row[2].toDouble();
        double y = row[3].toDouble();
        res[SName(schema, name)] = QPointF(x, y);
    }
    return res;
}





void Schema2Store::savePos(const QString &connectionName, const QHash<SName, QPointF> &pos)
{
    QSqlDatabase db = QSqlDatabase::database("_history");
    QString key = connectionKey(connectionName);
    sql::delete_where(db, "schema2_pos", "db = ?", {key});
    QList<SName> names = pos.keys();
    for(const SName name: names) {
        QPointF v = pos[name];
        sql::insert(db, "schema2_pos", {"db", "schema", "name", "x", "y"}, {key, name.schema, name.name, v.x(), v.y()});
    }
}




Schema2Store::Schema2Store(QObject *parent)
    : QObject{parent}
{
    QSqlDatabase db = QSqlDatabase::database("_history");
    sql::create_or_alter(db, "schema2_pos", {"db", "schema", "name", "x", "y"});
}

