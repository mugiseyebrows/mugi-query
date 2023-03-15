#include "schema2relation.h"

Schema2Relation::Schema2Relation()
{

}

Schema2Relation::Schema2Relation(const QString &name, const QStringList &childColumns,
                                 const QString &parentTable, const QStringList &parentColumns,
                                 bool constrained, Status status)
    : mName(name), mChildColumns(childColumns),
      mParentTable(parentTable), mParentColumns(parentColumns),
      mConstrained(constrained), mStatus(status)
{
    if (!mConstrained) {
        mStatus = StatusExisting;
    }
}
