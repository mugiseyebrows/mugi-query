#include "schema2relation.h"

#include "tolower.h"

Schema2Relation::Schema2Relation()
{

}

Schema2Relation::Schema2Relation(const QString &name, const QStringList &childColumns,
                                 const QString &parentTable, const QStringList &parentColumns,
                                 bool constrained, Status status)
    : mName(name), mChildColumns(childColumns),
      mParentTable(parentTable), mParentColumns(parentColumns),
      mConstrained(constrained), mStatus(status), mOldName(name)
{
    if (!mConstrained) {
        mStatus = StatusExisting;
    }
}

void Schema2Relation::setName(const QString& name) {
    if (name.toLower() == mName.toLower()) {
        return;
    }
    mName = name;
    if (mStatus != StatusNew) {
        mStatus = StatusModified;
    }
}

void Schema2Relation::setChildColumns(const QStringList &value) {
    if (toLower(value) == toLower(mChildColumns)) {
        return;
    }
    mChildColumns = value;
    mStatus = StatusModified;
    if (mStatus != StatusNew) {
        mStatus = StatusModified;
    }
}

void Schema2Relation::setParentColumns(const QStringList &value) {
    if (toLower(value) == toLower(mParentColumns)) {
        return;
    }
    mParentColumns = value;
    if (mStatus != StatusNew) {
        mStatus = StatusModified;
    }
}

QStringList Schema2Relation::createQueries(const QString &childTable) const {
    // ms access does not support ON UPDATE X ON DELETE X
    QString expr = QString("ALTER TABLE %1 ADD CONSTRAINT %2 FOREIGN KEY (%3) REFERENCES %4(%5)")
            .arg(childTable)
            .arg(mName)
            .arg(mChildColumns.join(", "))
            .arg(mParentTable)
            .arg(mParentColumns.join(", "));
    return {expr};
}

QStringList Schema2Relation::removeQueries(const QString &childTable) const {
    QString expr = QString("ALTER TABLE %1 DROP CONSTRAINT %2").arg(childTable).arg(mOldName);
    return {expr};
}

Status Schema2Relation::status() const {
    return mStatus;
}

void Schema2Relation::pushed() {
    mStatus = StatusExisting;
    mOldName = mName;
}

QStringList Schema2Relation::modifyQueries(const QString &childTable) const {
    return removeQueries(childTable) + createQueries(childTable);
}
