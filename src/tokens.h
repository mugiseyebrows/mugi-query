#ifndef TOKENS_H
#define TOKENS_H

#include <QStringList>
#include <QSqlDatabase>
#include <QMap>
#include <QMetaType>
#include "completerdata.h"
#include "sdata.h"

class Schema2Data;

class Schema2TablesModel;

class Tokens
{
public:
    Tokens();

    //Tokens(QSqlDatabase db);

    Tokens(QSqlDatabase db, Schema2Data* data);

    QStringList functions() const;

    QStringList keywords() const;

    QStringList autocompletion(const QMap<QString, QString> &aliases = QMap<QString, QString> {}) const;

    QStringList types() const;

    QStringList tablesAndFields(bool doted) const;

    QStringList fields(bool doted) const;

    QStringList tables() const;

    QStringList fields(const QString &table, const QString &alias) const;

    QStringList fields(const QString &table, bool dotted) const;

    CompleterData completerData() const;

    QStringList sizedTypes() const;
    QStringList storedFunctions() const;
    QStringList procedures() const;

    Schema2Data* data() const;
protected:
    /*struct Table {
        QString table;
        QStringList fields;
    };*/

    QStringList commonKeywords() const;
    QStringList driverKeywords() const;

    QList<STable> mTables;
    QList<SStored> mStored;
    QString mDriverName;
    Schema2Data* mData;

};

Q_DECLARE_METATYPE(Tokens)

#endif // TOKENS_H
