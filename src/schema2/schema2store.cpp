#include "schema2store.h"

#include <QDebug>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSet>
#include <QSqlRecord>
#include <QPointF>

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

static QList<QVariantList> sql_select_where(QSqlDatabase db, const QString& table, const QStringList& columns, const QString& whereExrp, const QVariantList& values) {
    QString expr = QString("select %1 from %2 where %3").arg(columns.join(", ")).arg(table).arg(whereExrp);
    QSqlQuery q(db);
    q.prepare(expr);
    for(int i=0;i<values.size();i++) {
        q.addBindValue(values[i]);
    }    if (!q.exec()) {
        qDebug() << q.lastError().text() << __FILE__ << __LINE__;
        return {};
    }
    QList<QVariantList> res;
    while(q.next()) {
        QVariantList row;
        for(int i=0;i<columns.size();i++) {
            row.append(q.value(i));
        }
        res.append(row);
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
    QList<QVariantList> rows = sql_select_where(db, "schema2_pos", {"name", "x", "y"}, "db = ?", {connectionKey(connectionName)});
    QHash<QString, QPointF> res;
    for(const QVariantList& row: rows) {
        QString name = row[0].toString();
        double x = row[1].toDouble();
        double y = row[2].toDouble();
        res[name] = QPointF(x, y);
    }
    return res;
}


static void sql_delete_where(QSqlDatabase db, const QString& table, const QString& whereExpr, const QVariantList& binds) {

    QSqlQuery q(db);
    QString expr = QString("delete from %1 where %2").arg(table).arg(whereExpr);
    q.prepare(expr);
    for(int i=0;i<binds.size();i++) {
        q.addBindValue(binds[i]);
    }
    if (!q.exec()) {
        qDebug() << q.lastError().text() << __FILE__ << __LINE__;
    }
}

static void sql_insert(QSqlDatabase db, const QString& table, const QStringList& columns, const QVariantList& values) {
    QStringList questions;
    for(int i=0;i<columns.size();i++) {
        questions.append("?");
    }
    QString expr = QString("insert into %1 (%2) values (%3)").arg(table).arg(columns.join(", ")).arg(questions.join(", "));
    QSqlQuery q(db);
    q.prepare(expr);
    for(int i=0;i<values.size();i++) {
        q.addBindValue(values[i]);
    }
    if (!q.exec()) {
        qDebug() << q.lastError().text() << __FILE__ << __LINE__;
    }
}



void Schema2Store::savePos(const QString &connectionName, const QHash<QString, QPointF> &pos)
{
    QSqlDatabase db = QSqlDatabase::database("_history");
    QString key = connectionKey(connectionName);
    sql_delete_where(db, "schema2_pos", "db = ?", {key});
    QStringList names = pos.keys();
    for(const QString name: names) {
        QPointF v = pos[name];
        sql_insert(db, "schema2_pos", {"db", "name", "x", "y"}, {key, name, v.x(), v.y()});
    }
}

static QString columns_def(const QStringList& columns, const QString& primaryKey) {
    QStringList res;
    for(const QString& column: columns) {
        QString def = column == primaryKey ? QString("%1 PRIMARY KEY").arg(column) : column;
        res.append(def);
    }
    return res.join(", ");
}

static void sql_create_if_not_exist(QSqlDatabase db,
                                const QString& table,
                                const QStringList& columns,
                                const QString& primaryKey = QString()) {
    QStringList tables = db.tables();

    if (!db.open()) {
        qDebug() << "!db.open()" << __FILE__ << __LINE__;
        return;
    }

    if (!tables.contains(table)) {
        QSqlQuery q(db);
        QString expr = QString("create table %1(%2)").arg(table).arg(columns_def(columns, primaryKey));
        qDebug() << expr;
        q.prepare(expr);
        if (!q.exec()) {
            qDebug() << q.lastError().text() << __FILE__ << __LINE__;
        }
    } else {
        QSqlRecord r = db.record(table);
        QSet<QString> existing;
        for(int i=0;i<r.count();i++) {
            existing.insert(r.fieldName(i));
        }
        for(const QString& column: columns) {
            if (existing.contains(column)) {
                continue;
            }
            QString expr = QString("alter table %1 add column %2").arg(table).arg(column);
            qDebug() << expr;
            QSqlQuery q(db);
            q.prepare(expr);
            if (!q.exec()) {
                qDebug() << q.lastError().text() << __FILE__ << __LINE__;
            }
        }

    }
}




Schema2Store::Schema2Store(QObject *parent)
    : QObject{parent}
{
    QSqlDatabase db = QSqlDatabase::database("_history");
    sql_create_if_not_exist(db, "schema2_pos", {"db", "name", "x", "y"});
}

