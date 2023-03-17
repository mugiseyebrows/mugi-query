#include "sqlutil.h"

#include <QSqlQuery>
#include <QSqlRecord>

#include <QDebug>
#include <QSqlError>

QList<QVariantList> sql::select_where(QSqlDatabase db, const QString& table, const QStringList& columns, const QString& whereExrp, const QVariantList& values) {
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


void sql::delete_where(QSqlDatabase db, const QString& table, const QString& whereExpr, const QVariantList& binds) {

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

void sql::insert(QSqlDatabase db, const QString& table, const QStringList& columns, const QVariantList& values) {
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


static QString columns_def(const QStringList& columns, const QString& primaryKey) {
    QStringList res;
    for(const QString& column: columns) {
        QString def = column == primaryKey ? QString("%1 PRIMARY KEY").arg(column) : column;
        res.append(def);
    }
    return res.join(", ");
}

void sql::create_or_alter(QSqlDatabase db,
                                const QString& table,
                                const QStringList& columns,
                                const QString& primaryKey) {
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
