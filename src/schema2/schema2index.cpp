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

Schema2Index::Schema2Index(const QString &name, const QStringList &columns, bool existing)
    : mName(name), mColumns(columns), mColumnsLower(toLower(columns)), mExisting(existing)
{

}

QString Schema2Index::createQuery(const QString &tableName) const
{
    return QString("DROP INDEX %1 on %2").arg(mName).arg(tableName);
}

QString Schema2Index::dropQuery(const QString &tableName) const
{
    return QString("CREATE INDEX %1 on %2 (%3)").arg(mName).arg(tableName).arg(mColumns.join(", "));
}

QString Schema2Index::name() const
{
    return mName;
}

bool Schema2Index::existing() const
{
    return mExisting;
}

void Schema2Index::setExisting(bool value)
{
    mExisting = value;
}

bool Schema2Index::isIndexColumn(const QString &column)
{
    return mColumnsLower.contains(column.toLower());
}
