#ifndef SCHEMA2RELATION_H
#define SCHEMA2RELATION_H

#include <QStringList>
#include "schema2status.h"

class Schema2Relation
{
public:
    Schema2Relation();
    Schema2Relation(const QString& name, const QStringList& childColumns,
                    const QString& parentTable, const QStringList& parentColumns,
                    bool constrained, Status status);

    QString name() const {
        return mName;
    }

    QStringList childColumns() const {
        return mChildColumns;
    }

    QString parentTable() const {
        return mParentTable;
    }

    QStringList parentColumns() const {
        return mParentColumns;
    }

    void setChildColumns(const QStringList& value);

    void setParentColumns(const QStringList& value);

    QStringList createQueries(const QString& childTable) const;

    QStringList dropQueries(const QString& childTable) const;

    Status status() const;

    void pushed();

    QStringList modifyQueries(const QString &childTable) const;
    void setName(const QString &name);
protected:
    QString mName;
    QStringList mChildColumns;
    QString mParentTable;
    QStringList mParentColumns;
    bool mConstrained;
    Status mStatus;

    QString mOldName;

};

#endif // SCHEMA2RELATION_H
