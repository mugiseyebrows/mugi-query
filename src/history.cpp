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

#define PARENT_WIDGET qobject_cast<QWidget*>(this->parent())

#define QUERY_EXEC(q) do { if (!q.exec()) QMessageBox::critical(PARENT_WIDGET,"Error",q.lastError().text()); } while (0)

History::History(QObject* parent) : QObject(parent)
{

    QDir d(Settings::instance()->dir());

    QSqlDatabase db = QSqlDatabase::addDatabase(DRIVER_SQLITE,"_history");
    db.setDatabaseName(d.filePath("history.sqlite"));
    if (!db.open()) {
        QMessageBox::critical(PARENT_WIDGET,"error",db.lastError().text());
        return;
    }

    db.exec("create table if not exists database(date datetime, connectionName text, driver text, host text, user text, password text, database text, port int)");
    db.exec("create table if not exists query(date datetime, connectionName text, query text)");
}

void History::addQuery(const QString& connectionName, const QString& query) {

    QSqlQuery q(QSqlDatabase::database("_history"));
    q.prepare("insert into query(date,connectionName,query) values(datetime('now','localtime'),?,?)");
    q.addBindValue(connectionName);
    q.addBindValue(query);
    QUERY_EXEC(q);
}

// echo ', const QString& '{connectionName,driver,host,user,password,database,port}

void History::addDatabase(const QString &connectionName,const QString &driver, const QString &host, const QString &user, const QString &password, const QString &database, int port)
{
    QSqlQuery q(QSqlDatabase::database("_history"));
    q.prepare("insert into database(date,connectionName,driver,host,user,password,database,port) values(datetime('now','localtime'),?,?,?,?,?,?,?)");
    q.addBindValue(connectionName);
    q.addBindValue(driver);
    q.addBindValue(host);
    q.addBindValue(user);
    q.addBindValue(Settings::instance()->savePasswords() ? password : QString());    q.addBindValue(database);
    q.addBindValue(port);
    QUERY_EXEC(q);
}
