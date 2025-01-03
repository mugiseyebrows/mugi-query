#ifndef SCHEMA2RELATION_H
#define SCHEMA2RELATION_H

#include <QStringList>
#include "schema2status.h"
#include <QVariant>
#include "sdata.h"
class QSqlDatabase;
class Schema2TableModel;
class QSqlDriver;

class Schema2Relation
{
public:
    enum cols {
        col_name,
        col_child_table,
        col_child_columns,
        col_parent_table,
        col_parent_columns,
        col_constrained,
        cols_count
    };

    Schema2Relation();
    Schema2Relation(Schema2TableModel* childTable, const QString& name, const QStringList& childColumns,
                    const SName &parentTable, const QStringList& parentColumns,
                    bool constrained, Status status);

    SName childTable() const;

    Schema2TableModel* childTableModel() const;

    QString name() const;

    QStringList childColumns() const;

    SName parentTable() const;

    QStringList parentColumns() const;

    void setChildColumns(const QStringList& value);

    void setParentColumns(const QStringList& value);

    QStringList createQueries(const SName &childTable, const QSqlDatabase &db) const;

    QStringList dropQueries(const SName &childTable, const QSqlDatabase &db) const;

    Status status() const;

    void pushed();

    QStringList modifyQueries(const SName &childTable, const QSqlDatabase &db) const;

    void setName(const QString &name);

    bool constrained() const;

    QVariant data(int column, int role) const;

    static QVariant headerData(int section, int orientation, int role);

    SRelation asRelation() const;

protected:

    Schema2TableModel* mChildTableModel;

    QString mName;
    QStringList mChildColumns;
    SName mParentTable;
    QStringList mParentColumns;
    bool mConstrained;
    Status mStatus;

    QString mOldName;

};

#endif // SCHEMA2RELATION_H
