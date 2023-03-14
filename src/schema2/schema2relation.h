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

    QString createQuery(const QString& childTable) const {
        // ms access does not support ON UPDATE X ON DELETE X
        return QString("ALTER TABLE %1 ADD CONSTRAINT %2 FOREIGN KEY (%3) REFERENCES %4(%5)")
                .arg(childTable)
                .arg(mName)
                .arg(mChildColumns.join(", "))
                .arg(mParentTable)
                .arg(mParentColumns.join(", "));
    }

    QString removeQuery(const QString& childTable) const {
        return QString("ALTER TABLE %1 DROP CONSTRAINT %2").arg(childTable).arg(mName);
    }

    Status status() const {
        return mStatus;
    }

protected:
    QString mName;
    QStringList mChildColumns;
    QString mParentTable;
    QStringList mParentColumns;
    bool mConstrained;
    Status mStatus;
};

#endif // SCHEMA2RELATION_H
