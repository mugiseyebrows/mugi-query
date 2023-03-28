#include "schema2index.h"
#include "tolower.h"
#include "filterempty.h"
#include "drivernames.h"

Schema2Index::Schema2Index()
{

}

Schema2Index::Schema2Index(const QString &name, const QStringList &columns, bool primary, bool unique, Status status)
    : mName(name), mColumns(columns), mColumnsLower(toLower(columns)), mPrimary(primary), mUnique(unique), mStatus(status)
{

}

QString Schema2Index::createQuery(const QString &tableName, const QString& driverName) const
{

    if (driverName == DRIVER_ODBC) {

        QString expr = filterEmpty({"CREATE",
                                    mUnique ? "UNIQUE" : "",
                                    "INDEX", mName, "ON", tableName, "(" + mColumns.join(", ") + ")",
                                    mPrimary ? "WITH PRIMARY" : ""
                                   }).join(" ");

        return expr;

    } else if (driverName == DRIVER_MYSQL || driverName == DRIVER_MARIADB) {

        QString expr = filterEmpty({"ALTER TABLE", tableName, "ADD",
                                    (mUnique && !mPrimary) ? "UNIQUE" : "",
                                    mPrimary ? "PRIMARY": "",
                                    mPrimary ? "KEY" : "INDEX",
                                    mPrimary ? "" : mName,
                                    "(" + mColumns.join(", ") + ")"}).join(" ");
        return expr;

    } else {
        // todo implement create index for other drivers
    }

    return QString();
}

QString Schema2Index::dropQuery(const QString &tableName, const QString& driverName) const
{
    if (driverName == DRIVER_ODBC) {
        QString expr = QString("DROP INDEX %1 on %2").arg(mName).arg(tableName);
        return expr;
    } else if (driverName == DRIVER_MYSQL || driverName == DRIVER_MARIADB) {
        QString expr = QString("DROP INDEX %1 on %2").arg(mPrimary ? "`PRIMARY`" : mName).arg(tableName);
        return expr;
    }
    return QString();
}

QString Schema2Index::name() const
{
    return mName;
}

Status Schema2Index::status() const
{
    return mStatus;
}

void Schema2Index::setStatus(Status status)
{
    mStatus = status;
}

bool Schema2Index::primary() const {
    return mPrimary;
}

QStringList Schema2Index::columns() const {
    return mColumns;
}

bool Schema2Index::unique() const {
    return mUnique;
}

bool Schema2Index::isIndexColumn(const QString &column)
{
    return mColumnsLower.contains(column.toLower());
}
