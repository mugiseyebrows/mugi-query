#include "schema2store.h"

#include <QDebug>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSet>
#include <QSqlRecord>
#include <QPointF>
#include "sqlutil.h"

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

QHash<QString, QPointF> Schema2Store::loadPos(const QString &connectionName)
{
    QSqlDatabase db = QSqlDatabase::database("_history");
    QList<QVariantList> rows = sql::select_where(db, "schema2_pos", {"name", "x", "y"}, "db = ?", {connectionKey(connectionName)});
    QHash<QString, QPointF> res;
    for(const QVariantList& row: rows) {
        QString name = row[0].toString();
        double x = row[1].toDouble();
        double y = row[2].toDouble();
        res[name] = QPointF(x, y);
    }
    return res;
}





void Schema2Store::savePos(const QString &connectionName, const QHash<QString, QPointF> &pos)
{
    QSqlDatabase db = QSqlDatabase::database("_history");
    QString key = connectionKey(connectionName);
    sql::delete_where(db, "schema2_pos", "db = ?", {key});
    QStringList names = pos.keys();
    for(const QString name: names) {
        QPointF v = pos[name];
        sql::insert(db, "schema2_pos", {"db", "name", "x", "y"}, {key, name, v.x(), v.y()});
    }
}




Schema2Store::Schema2Store(QObject *parent)
    : QObject{parent}
{
    QSqlDatabase db = QSqlDatabase::database("_history");
    sql::create_or_alter(db, "schema2_pos", {"db", "name", "x", "y"});
}

