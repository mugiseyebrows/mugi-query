#include "schema2index.h"

Schema2Index::Schema2Index()
{

}

static QStringList toLower(const QStringList& items) {
    QStringList res;
    for(const QString& item: items) {
        res.append(item.toLower());
    }
    return res;
}

Schema2Index::Schema2Index(const QString &name, const QStringList &columns, bool primary, Status status)
    : mName(name), mColumns(columns), mColumnsLower(toLower(columns)), mPrimary(primary), mStatus(status)
{

}

QString Schema2Index::createQuery(const QString &tableName) const
{
    QString expr = QString("CREATE INDEX %1 on %2 (%3)%4")
            .arg(mName)
            .arg(tableName)
            .arg(mColumns.join(", "))
            .arg(mPrimary ? " WITH PRIMARY" : "");
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
