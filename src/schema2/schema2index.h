#ifndef SCHEMA2INDEX_H
#define SCHEMA2INDEX_H

#include <QStringList>
#include "schema2status.h"

class Schema2Index
{
public:
    Schema2Index();

    Schema2Index(const QString& name, const QStringList& columns, bool primary, bool unique, Status status);

    QString createQuery(const QString& tableName) const;

    QString dropQuery(const QString& tableName) const;

    QString name() const;

    bool isIndexColumn(const QString& column);

    Status status() const;

    void setStatus(Status status);
protected:
    QString mName;
    QStringList mColumns;
    QStringList mColumnsLower;
    bool mPrimary;
    bool mUnique;
    Status mStatus;
};

#endif // SCHEMA2INDEX_H
