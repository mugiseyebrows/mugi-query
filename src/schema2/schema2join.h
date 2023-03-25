#ifndef SCHEMA2JOIN_H
#define SCHEMA2JOIN_H

#include <QStringList>
#include <QHash>
#include "hash.h"

class Schema2TableModel;
class Schema2RelationModel;
class Schema2TablesModel;

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
    Schema2Join(const QString& childTable, const QStringList& childColumn,
         const QString& parentTable, const QStringList& parentColumn)
        : childTable(childTable), childColumns(childColumn),
          parentTable(parentTable), parentColumns(parentColumn) {

    }

    QString onExpr() const {
        QStringList res;
        for(int i=0;i<childColumns.size();i++) {
            QString item = QString("%1.%2 = %3.%4")
                    .arg(childTable)
                    .arg(childColumns[i])
                    .arg(parentTable)
                    .arg(parentColumns[i]);
            res.append(item);
        }
        return res.join(" and ");
    }

    QString childTable;
    QStringList childColumns;
    QString parentTable;
    QStringList parentColumns;
};

QString toString(const QList<Schema2Join>& expr, bool mssql, JoinType exprType);

QList<Schema2Join> findJoinImpl(const QStringList& join,
                                Schema2TablesModel *tableModels);

#endif // SCHEMA2JOIN_H
