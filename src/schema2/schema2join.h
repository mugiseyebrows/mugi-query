#ifndef SCHEMA2JOIN_H
#define SCHEMA2JOIN_H

#include <QStringList>
#include <QHash>
#include "hash.h"

class Schema2TableModel;
class Schema2RelationModel;

enum JoinType {
    LeftJoin,
    CrossJoin
};

QStringList joinTypes();

class Schema2Join {
public:
    Schema2Join() {

    }
    Schema2Join(const QString& childTable) : childTable(childTable) {

    }
    Schema2Join(const QString& childTable, const QString& childColumn,
         const QString& parentTable, const QString& parentColumn)
        : childTable(childTable), childColumn(childColumn),
          parentTable(parentTable), parentColumn(parentColumn) {

    }

    QString childTable;
    QString childColumn;
    QString parentTable;
    QString parentColumn;
};

QString toString(const QList<Schema2Join>& expr, bool mssql, JoinType exprType);

QList<Schema2Join> findJoinImpl(const QStringList& join,
                                const StringHash<Schema2TableModel *>& tableModels,
                                const StringListHash<Schema2RelationModel *> &relationModels);

#endif // SCHEMA2JOIN_H
