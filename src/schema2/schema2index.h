#ifndef SCHEMA2INDEX_H
#define SCHEMA2INDEX_H

#include <QStringList>
#include "schema2status.h"
#include "sdata.h"
class QSqlDriver;
class QSqlDatabase;

class Schema2Index
{
public:
    Schema2Index();

    Schema2Index(const QString& name, const QStringList& columns, bool primary, bool unique, Status status);

    QString createQuery(const SName& tableName, const QSqlDatabase &db) const;

    QString dropQuery(const SName& tableName, const QSqlDatabase &db) const;

    QString name() const;

    bool isIndexColumn(const QString& column);

    Status status() const;

    void setStatus(Status status);

    bool primary() const;

    QStringList columns() const;

    bool unique() const;

protected:
    QString mName;
    QStringList mColumns;
    QStringList mColumnsLower;
    bool mPrimary;
    bool mUnique;
    Status mStatus;
};

#endif // SCHEMA2INDEX_H
