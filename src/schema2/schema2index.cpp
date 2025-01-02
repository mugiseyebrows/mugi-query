#include "schema2index.h"
#include "tolower.h"
#include "filterempty.h"
#include "drivernames.h"
#include "sqlescaper.h"


Schema2Index::Schema2Index()
{

}

Schema2Index::Schema2Index(const QString &name, const QStringList &columns, bool primary, bool unique, Status status)
    : mName(name), mColumns(columns), mColumnsLower(toLower(columns)), mPrimary(primary), mUnique(unique), mStatus(status)
{

}

QString Schema2Index::createQuery(const SName &tableName, const QSqlDatabase& db) const
{

    SqlEscaper es(db);
    QString driverName = es.driverName();

    if (driverName == DRIVER_ODBC) {

        QString expr = filterEmpty({"CREATE",
                                    (mUnique && !mPrimary) ? "UNIQUE" : "",
                                    "INDEX", es.field(mName),
                                    "ON", es.table(tableName),
                                    "(" + es.columns(mColumns).join(", ") + ")",
                                    mPrimary ? "WITH PRIMARY" : ""
                                   }).join(" ");

        return expr;

    } else if (driverName == DRIVER_MYSQL || driverName == DRIVER_MARIADB) {

        QString expr = filterEmpty({"ALTER TABLE", tableName.name, "ADD",
                                    (mUnique && !mPrimary) ? "UNIQUE" : "",
                                    mPrimary ? "PRIMARY": "",
                                    mPrimary ? "KEY" : "INDEX",
                                    mPrimary ? "" : es.field(mName),
                                    "(" + es.columns(mColumns).join(", ") + ")"}).join(" ");
        return expr;

    } else {
        // todo implement create index for other drivers
    }

    return QString();
}

QString Schema2Index::dropQuery(const SName &tableName, const QSqlDatabase& db) const
{
    SqlEscaper es(db);
    QString driverName = db.driverName();
    QString name = mName;
    if (driverName == DRIVER_MYSQL || driverName == DRIVER_MARIADB) {
        name = mPrimary ? "PRIMARY" : mName;
    }
    QString expr = QString("DROP INDEX %1 on %2").arg(es.field(name)).arg(es.table(tableName));
    return expr;
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
