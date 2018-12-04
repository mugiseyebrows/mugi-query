#include "history.h"

#include <QString>

#include <QSqlDatabase>
#include <QDir>
#include <QStandardPaths>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlRecord>
#include <QDebug>

#define PARENT_WIDGET qobject_cast<QWidget*>(this->parent())

#define QUERY_EXEC(q) do { if (!q.exec()) QMessageBox::critical(PARENT_WIDGET,"Error",q.lastError().text()); } while (0)

History::History(QObject* parent) : QObject(parent)
{
#if QT_VERSION >= 0x050000
    QString home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#else
    QString home = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
#endif

    QString name = ".mugi-query";

    QDir d(QDir(home).filePath(name));
    if (!d.exists()) {
        d.cdUp();
        d.mkdir(name);
        d.cd(name);
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","_history");
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
    q.prepare("insert into query(date,connectionName,query) values(datetime('now'),?,?)");
    q.addBindValue(connectionName);
    q.addBindValue(query);
    QUERY_EXEC(q);
}

// echo ', const QString& '{connectionName,driver,host,user,password,database,port}

void History::addDatabase(const QString &connectionName,const QString &driver, const QString &host, const QString &user, const QString &password, const QString &database, int port)
{
    QSqlQuery q(QSqlDatabase::database("_history"));
    q.prepare("insert into database(date,connectionName,driver,host,user,password,database,port) values(datetime('now'),?,?,?,?,?,?,?)");
    q.addBindValue(connectionName);
    q.addBindValue(driver);
    q.addBindValue(host);
    q.addBindValue(user);
    q.addBindValue(password);
    q.addBindValue(database);
    q.addBindValue(port);
    QUERY_EXEC(q);
}
