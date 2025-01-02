#include "sqlescaper.h"

#include <QSqlDriver>
#include "drivernames.h"

SqlEscaper::SqlEscaper(QSqlDriver *driver) : mDriver(driver) {

}

SqlEscaper::SqlEscaper(const QSqlDatabase& db) : mDb(db), mDriver(db.driver()), mMysql(db.driverName() == DRIVER_MYSQL), mSchema(db.databaseName())
{

}

QString SqlEscaper::table(const SName &table) const {

    if (mMysql && table.schema != mSchema) {
        return mDriver->escapeIdentifier(table.fullname(), QSqlDriver::TableName);
    }
    return mDriver->escapeIdentifier(table.name, QSqlDriver::TableName);
}

QString SqlEscaper::table(const QString &table) const {
    return mDriver->escapeIdentifier(table, QSqlDriver::TableName);
}

QString SqlEscaper::field(const QString &field) const {
    return mDriver->escapeIdentifier(field, QSqlDriver::FieldName);
}

QStringList SqlEscaper::columns(const QStringList &columns) const {
    QStringList res;
    for(const QString& item: columns) {
        res.append(mDriver->escapeIdentifier(item, QSqlDriver::FieldName));
    }
    return res;
}

QString SqlEscaper::driverName() const
{
    return mDb.driverName();
}
