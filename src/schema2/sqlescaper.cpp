#include "sqlescaper.h"

#include <QSqlDriver>

SqlEscaper::SqlEscaper(QSqlDriver *driver) : mDriver(driver) {

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
