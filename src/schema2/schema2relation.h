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

    QStringList createQueries(const QString& childTable) const {
        // ms access does not support ON UPDATE X ON DELETE X
        QString expr = QString("ALTER TABLE %1 ADD CONSTRAINT %2 FOREIGN KEY (%3) REFERENCES %4(%5)")
                .arg(childTable)
                .arg(mName)
                .arg(mChildColumns.join(", "))
                .arg(mParentTable)
                .arg(mParentColumns.join(", "));
        return {expr};
    }

    QStringList removeQueries(const QString& childTable) const {
        QString expr = QString("ALTER TABLE %1 DROP CONSTRAINT %2").arg(childTable).arg(mName);
        return {expr};
    }

    Status status() const {
        return mStatus;
    }

    void created() {
        mStatus = StatusExisting;
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
