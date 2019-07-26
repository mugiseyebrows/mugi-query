#ifndef TOKENS_H
#define TOKENS_H

#include <QStringList>
#include <QSqlDatabase>
#include <QMap>
#include <QMetaType>

class Tokens
{
public:
    Tokens();

    Tokens(QSqlDatabase db);

    QStringList functions() const;

    QStringList keywords() const;

    QStringList autocompletion(const QMap<QString, QString> &aliases = QMap<QString, QString> {}) const;

    QStringList types() const;

    QStringList tablesAndFields(bool doted) const;

    QStringList fields(bool doted) const;

    QStringList tables() const;

    QStringList fields(const QString &table, const QString &alias) const;

    QStringList fields(const QString &table, bool dotted) const;

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

Q_DECLARE_METATYPE(Tokens)

#endif // TOKENS_H
