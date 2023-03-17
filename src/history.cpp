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
#include "mugisql/mugisql.h"

#define PARENT_WIDGET 0

#define QUERY_EXEC(q) do { if (!q.exec()) QMessageBox::critical(PARENT_WIDGET,"Error",q.lastError().text()); } while (0)

History::History(QObject* parent)
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

void History::addQuery(const QString& connectionName, const QString& query_) {

    QSqlDatabase db = QSqlDatabase::database("_history");

    using namespace mugisql;
    using namespace mugisql::sqlite;

    insert_t q = insert(db).into(query, {query.date, query.connectionName, query.query})
            .values({datetime("now", "localtime"), connectionName, query_});

    QUERY_EXEC(q);
}

void History::addJoin(const QString& connectionName1, const QString& query1, const QStringList& columns1,
                      const QString& connectionName2, const QString& query2, const QStringList& columns2) {

    // todo implement xjoin history

}

// echo ', const QString& '{connectionName,driver,host,user,password,database,port}



void History::addDatabase(const QString &connectionName, const QString &driver, const QString &host, const QString &user, const QString &password, const QString &database_, int port)
{
    QSqlDatabase db = QSqlDatabase::database("_history");

    using namespace mugisql;
    using namespace mugisql::sqlite;

    exprlist_t fields = {
        database.date,
        database.connectionName,
        database.driver,
        database.host,
        database.user,
        database.password,
        database.database,
        database.port
    };

    exprlist_t values = {
        datetime("now", "localtime"),
        connectionName,
        driver,
        host,
        user,
        (Settings::instance()->savePasswords() ? password : QString()),
        database_,
        port
    };

    insert_t q = insert(db).into(database, fields).values(values);

    QUERY_EXEC(q);
}
