#include "history.h"

#include <QString>

#include <QSqlDatabase>
#include <QDir>

#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlRecord>
#include <QDebug>
#include "settings.h"
#include "drivernames.h"
#include "query_exec.h"

History::History(QObject* parent)
{

    QDir d(Settings::instance()->dir());

    QSqlDatabase db = QSqlDatabase::addDatabase(DRIVER_SQLITE,"_history");
    db.setDatabaseName(d.filePath("history.sqlite"));
    if (!db.open()) {
        QMessageBox::critical(0, "error",db.lastError().text());
        return;
    }

    db.exec("create table if not exists database(date datetime, connectionName text, driver text, host text, user text, password text, database text, port int)");
    db.exec("create table if not exists query(date datetime, connectionName text, query text)");
    db.exec("create table if not exists relations(name text, value text)");
}

History *History::mInstance = 0;

History *History::instance()
{
    if (!mInstance) {
        mInstance = new History();
    }
    return mInstance;
}

// python insert-record.py -o tmp.txt query -c date connectionName query
static void insert_into_query(QSqlDatabase db, const QVariant& date, const QVariant& connectionName, const QVariant& query) {
    QSqlQuery q(db);
    q.prepare("INSERT INTO query(date, connectionName, query) VALUES(?, ?, ?)");
    q.addBindValue(date);
    q.addBindValue(connectionName);
    q.addBindValue(query);
    QUERY_EXEC(q);
}

void History::addQuery(const QString& connectionName, const QString& query) {
    QSqlDatabase db = QSqlDatabase::database("_history");
    insert_into_query(db, QDateTime::currentDateTime(), connectionName, query);
}

void History::addJoin(const QString& connectionName1, const QString& query1, const QStringList& columns1,
                      const QString& connectionName2, const QString& query2, const QStringList& columns2) {

    // todo implement xjoin history

}

// python insert-record.py -o tmp.txt database --columns date connectionName driver host user password database port

static void insert_into_database(QSqlDatabase db, const QVariant& date, const QVariant& connectionName, const QVariant& driver, const QVariant& host, const QVariant& user, const QVariant& password, const QVariant& database, const QVariant& port) {
    QSqlQuery q(db);
    q.prepare("INSERT INTO database(date, connectionName, driver, host, user, password, database, port) VALUES(?, ?, ?, ?, ?, ?, ?, ?)");
    q.addBindValue(date);
    q.addBindValue(connectionName);
    q.addBindValue(driver);
    q.addBindValue(host);
    q.addBindValue(user);
    q.addBindValue(password);
    q.addBindValue(database);
    q.addBindValue(port);
    QUERY_EXEC(q);
}


void History::addDatabase(const QString &connectionName, const QString &driver, const QString &host, const QString &user, const QString &password, const QString &database, int port)
{
    QSqlDatabase db = QSqlDatabase::database("_history");
    insert_into_database(db, QDateTime::currentDateTime(), connectionName, driver, host, user, (Settings::instance()->savePasswords() ? password : QString()), database, port);
}
