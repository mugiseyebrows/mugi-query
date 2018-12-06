#ifndef SCHEMAFETCHER_H
#define SCHEMAFETCHER_H

class QSqlDatabase;
#include <QStringList>

class SchemaFetcher
{
public:
    SchemaFetcher();

    static QStringList fetch(const QSqlDatabase& db);

    static QString odbcUriFileName(const QString& databaseName);
    /*static QStringList tableNames(const QSqlDatabase& db);*/
    static QStringList fieldNames(const QSqlDatabase& db, const QStringList& tables);

    static QStringList common();

    static QStringList functions(const QString& driver);

    static void test();

    static QStringList specific(const QString &driver);
};


#endif // SCHEMAFETCHER_H
