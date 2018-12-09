#ifndef TOKENS_H
#define TOKENS_H

#include <QStringList>
#include <QSqlDatabase>

class Tokens
{
public:
    Tokens();

    Tokens(QSqlDatabase db);

    QStringList functions() const;

    QStringList keywords() const;

    QStringList autocompletion() const;

    QStringList types() const;

    QStringList tablesAndFields(bool doted) const;

    QStringList fields(bool doted) const;

    QStringList tables() const;

protected:
    struct Table {
        QString table;
        QStringList fields;
    };

    QStringList commonKeywords() const;
    QStringList driverKeywords() const;

    QList<Table> mTables;
    QString mDriverName;

};

#endif // TOKENS_H
