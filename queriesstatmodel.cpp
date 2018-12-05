#include "queriesstatmodel.h"

#include "rowvaluesetter.h"

QueriesStatModel::QueriesStatModel(const QStringList& queries,
                                   const QStringList errors,
                                   const QList<int> &perf,
                                   const QList<int> &rowsAffected,
                                   QObject* parent) :
    QStandardItemModel(queries.size(),4,parent)
{

    setHeaderData(0,Qt::Horizontal,"query");
    setHeaderData(1,Qt::Horizontal,"ms");
    setHeaderData(2,Qt::Horizontal,"rows");
    setHeaderData(3,Qt::Horizontal,"error");

    for(int i=0;i<queries.size();i++) {
        RowValueSetter s(this,i);
        s(0,queries[i].trimmed());
        s(1,perf[i]);
        s(2,rowsAffected[i]);
        s(3,errors[i]);
    }

    mHasErrors = false;
    foreach(const QString error, errors) {
        if (!error.isEmpty()) {
            mHasErrors = true;
        }
    }
}

bool QueriesStatModel::hasErrors() const
{
    return mHasErrors;
}

