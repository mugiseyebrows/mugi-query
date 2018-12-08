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

    QStringList tablesAndFields(bool doted) const;

protected:
    struct Table {
        QString table;
        QStringList fields;
    };

    QStringList commonKeywords() const;
    QStringList driverKeywords(const QString &driverName) const;

    QList<Table> mTables;
    QString mDriverName;

};

#endif // TOKENS_H
