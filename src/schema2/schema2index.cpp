#include "schema2index.h"
#include "tolower.h"
#include "filterempty.h"

Schema2Index::Schema2Index()
{

}

Schema2Index::Schema2Index(const QString &name, const QStringList &columns, bool primary, bool unique, Status status)
    : mName(name), mColumns(columns), mColumnsLower(toLower(columns)), mPrimary(primary), mUnique(unique), mStatus(status)
{

}

QString Schema2Index::createQuery(const QString &tableName) const
{
    QString expr = filterEmpty({"CREATE",
                                mUnique ? "UNIQUE" : "",
                                "INDEX", mName, "ON", tableName, "(" + mColumns.join(", ") + ")",
                                mPrimary ? "WITH PRIMARY" : ""
                               }).join(" ");

    return expr;
}

QString Schema2Index::dropQuery(const QString &tableName) const
{
    QString expr = QString("DROP INDEX %1 on %2").arg(mName).arg(tableName);
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

bool Schema2Index::isIndexColumn(const QString &column)
{
    return mColumnsLower.contains(column.toLower());
}
