#ifndef SQLESCAPER_H
#define SQLESCAPER_H

class QSqlDriver;
#include <QSqlDatabase>
#include <QStringList>
#include "sdata.h"

class SqlEscaper {
public:
    SqlEscaper(QSqlDriver* driver);

    SqlEscaper(const QSqlDatabase& db);

    QString table(const QString &table) const;
    QString table(const SName &table) const;
    QString field(const QString& field) const;
    QStringList columns(const QStringList& columns) const;

    QString driverName() const;

protected:
    QSqlDriver* mDriver;
    QSqlDatabase mDb;

    bool mMysql = false;
    QString mSchema;
};

#endif // SQLESCAPER_H
