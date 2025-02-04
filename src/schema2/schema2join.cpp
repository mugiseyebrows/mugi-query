#include "schema2join.h"
#include <QDebug>


#include "schema2relation.h"
#include "schema2tablemodel.h"
#include "schema2relationsmodel.h"
#include "schema2tablesmodel.h"

static bool containsAll(const QList<int>& item, const QList<int> join) {
    for(int key: join) {
        if (!item.contains(key)) {
            return false;
        }
    }
    return true;
}

static QStringList toLower(const QStringList& items) {
    QStringList res;
    for(const QString& item: items) {
        res.append(item.toLower());
    }
    return res;
}

static QList<QStringList> findChildColumnsParentColumns(
        const SName &childTable, const SName &parentTable,
        Schema2TablesModel* tableModels) {
    auto relations = tableModels->table(childTable)->relationsTo(parentTable);
    for(auto* relation: relations) {
        return {relation->childColumns(), relation->parentColumns()};
    }
    relations = tableModels->table(parentTable)->relationsTo(childTable);
    for(auto* relation: relations) {
        return {relation->parentColumns(), relation->childColumns()};
    }
    return {};
}

static int findTable(const SNames& tables, const QString& table) {

    QString table_ = table.toLower();
    if (table.contains(".")) {
        for(int i=0;i<tables.size();i++) {
            if (tables[i].fullname().toLower() == table_) {
                return i;
            }
        }
    } else {
        for(int i=0;i<tables.size();i++) {
            if (tables[i].name.toLower() == table_) {
                return i;
            }
        }
    }
    return -1;
}

QList<Schema2Join> findJoinImpl(const QStringList &join_,
                 Schema2TablesModel* tableModels)
{
    SNames tables = tableModels->tableNames();

    //QStringList tables = toLower(tables_);

    QHash<int, QList<int>> relations;

    //QList<QStringList> relations_ = relationModels.keys();

    QList<Schema2TableModel*> tableModelValues = tableModels->tables();

    for(Schema2TableModel* tableModel: tableModelValues) {
        QList<Schema2Relation *> tableRelations = tableModel->relations()->values();
        for(Schema2Relation * tableRelation: tableRelations) {
            int childTable = tables.indexOf(tableModel->tableName());
            int parentTable = tables.indexOf(tableRelation->parentTable());
            if (!relations.contains(childTable)) {
                relations[childTable] = {};
            }
            if (!relations.contains(parentTable)) {
                relations[parentTable] = {};
            }
            relations[childTable].append(parentTable);
            relations[parentTable].append(childTable);
        }
    }

    QList<int> join;
    for(const QString& name: std::as_const(join_)) {
        int index = findTable(tables, name);
        if (index < 0) {
            return {};
        }
        join.append(index);
    }

    for(int key: std::as_const(join)) {
        if (!relations.contains(key)) {
            qDebug() << "!relations.contains(key)";
            return {};
        }
    }


    QList<QList<int>> queue;
    /*QList<int> keys = relations.keys();
    for(int key: std::as_const(keys)) {
        queue.append({key});
    }*/
    for(int key: std::as_const(join)) {
        queue.append(QList<int>{key});
    }

    QList<int> found;

    while(!queue.isEmpty()) {
        QList<int> item = queue.takeFirst();

        if (containsAll(item, join)) {
            found = item;
            break;
        }

        QList<int> related = relations[item[item.size()-1]];
        for(int key: std::as_const(related)) {
            if (item.contains(key)) {
                continue;
            }
            QList<int> newItem = item;
            newItem.append(key);
            queue.append(newItem);
        }

        //qDebug() << "queue.size" << queue.size();
    }

    if (found.isEmpty()) {
        return {};
    }

    QList<Schema2Join> res;
    res.append(Schema2Join(tables[found[0]]));
    for(int i=1;i<found.size();i++) {
        SName childTable = tables[found[i]];
        SName parentTable = tables[found[i-1]];
        QList<QStringList> columns = findChildColumnsParentColumns(childTable, parentTable, tableModels);
        QStringList childColumn = columns[0];
        QStringList parentColumn = columns[1];
        Schema2Join item(childTable, childColumn, parentTable, parentColumn);
        res.append(item);
    }

    //qDebug() << found << found.size();

    return res;
}

static QString repeat(const QString& s, int n) {
    QStringList res;
    for(int i=0;i<n;i++) {
        res.append(s);
    }
    return res.join("");
}

QString toString(const QList<Schema2Join> &expr, bool mssql, JoinType joinType)
{

    if (joinType == LeftJoin) {

        QStringList res;
        Schema2Join join = expr[0];
        QString head = QString("select * from %1%2")
                .arg(mssql ? repeat( "(", expr.size()-2) : "")
                .arg(join.childTable.name);
        res.append(head);
        for(int i=1;i<expr.size();i++) {
            Schema2Join join = expr[i];
            QString item = QString("left join %1 on %2")
                    .arg(join.childTable.name)
                    .arg(join.onExpr());
            if (mssql && i<expr.size()-1) {
                item.append(")");
            }
            res.append(item);
        }
        return res.join("\n");

    } else if (joinType == CrossJoin) {

        QStringList tables;
        for(int i=0;i<expr.size();i++) {
            tables.append(expr[i].childTable.name);
        }
        QStringList cond;
        for(int i=1;i<expr.size();i++) {
            Schema2Join join = expr[i];
            cond.append(join.onExpr());
        }
        QString res = QString("select * from %1\nwhere %2")
                .arg(tables.join(", "))
                .arg(cond.join("\nand "));

        return res;
    }

    return QString();
}

QStringList joinTypes() {
    return {"Left Join", "Cross Join"};
}
