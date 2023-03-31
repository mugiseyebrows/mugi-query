#include "schema2relation.h"

#include "tolower.h"
#include "sqlescaper.h"

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

#if 0
static QStringList escaped(const QStringList& items, QSqlDriver* driver) {
    QStringList res;
    for(const QString& item: items) {
        res.append(driver->escapeIdentifier(item, QSqlDriver::FieldName));
    }
    return res;
}
#endif

QStringList Schema2Relation::createQueries(const QString &childTable, const QString& driverName, QSqlDriver* driver) const {
    // ms access does not support ON UPDATE X ON DELETE X
    SqlEscaper es(driver);
    QString expr = QString("ALTER TABLE %1 ADD CONSTRAINT %2 FOREIGN KEY (%3) REFERENCES %4(%5)")
            .arg(es.table(childTable))
            .arg(es.field(mName))
            .arg(es.columns(mChildColumns).join(", "))
            .arg(es.table(mParentTable))
            .arg(es.columns(mParentColumns).join(", "));
    return {expr};
}

QStringList Schema2Relation::dropQueries(const QString &childTable, const QString& driverName, QSqlDriver* driver) const {
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

QStringList Schema2Relation::modifyQueries(const QString &childTable, const QString &driverName, QSqlDriver *driver) const {
    return dropQueries(childTable, driverName, driver) + createQueries(childTable, driverName, driver);
}
