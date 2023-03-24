#include "schema2join.h"
#include <QDebug>

#include "schema2relationmodel.h"
#include "schema2relation.h"
#include "schema2tablemodel.h"

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

static QStringList childColumnParentColumn(const StringListHash<Schema2RelationModel *>& relationModels,
                               const QStringList& key, bool reverse = false) {
    if (relationModels.contains(key)) {
        QString childColumn = relationModels.get(key)->childColumn();
        QString parentColumn = relationModels.get(key)->parentColumn();
        if (reverse) {
            return {parentColumn, childColumn};
        } else {
            return {childColumn, parentColumn};
        }
    }
    return {};
}

static QList<QStringList> findChildColumnsParentColumns(
        const QString &childTable, const QString &parentTable,
        const StringHash<Schema2TableModel *>& tableModels) {
    Schema2Relation* relation = tableModels.get(childTable)->relationTo(parentTable);
    if (relation) {
        return {relation->childColumns(), relation->parentColumns()};
    }
    relation = tableModels.get(parentTable)->relationTo(childTable);
    if (relation) {
        return {relation->parentColumns(), relation->childColumns()};
    }
    return {};
}

static QStringList findChildColumnParentColumn(const QString &childTable, const QString &parentTable,
                           const StringListHash<Schema2RelationModel *>& relationModels) {

    QStringList key1 = {childTable, parentTable};
    if (relationModels.contains(key1)) {
        return childColumnParentColumn(relationModels, key1, false);
    }
    QStringList key2 = {parentTable, childTable};
    if (relationModels.contains(key2)) {
        return childColumnParentColumn(relationModels, key2, true);
    }

    QStringList key3 = toLower(key1);
    QStringList key4 = toLower(key2);

    QList<QStringList> keys = relationModels.keys();
    for(const QStringList& key: keys) {
        QStringList other = toLower(key);
        if (other == key3) {
            return childColumnParentColumn(relationModels, other, false);
        } else if (other == key4) {
            return childColumnParentColumn(relationModels, other, true);
        }
    }
    return {};
}

#include "schema2tablemodel.h"
#include "schema2relationsmodel.h"

QList<Schema2Join> findJoinImpl(const QStringList &join_,
                 const StringHash<Schema2TableModel *>& tableModels)
{
    QStringList tables_ = tableModels.keys();

    QStringList tables = toLower(tables_);

    QHash<int, QList<int>> relations;

    //QList<QStringList> relations_ = relationModels.keys();

    QList<Schema2TableModel*> tableModelValues = tableModels.values();

    for(Schema2TableModel* tableModel: tableModelValues) {
        QList<Schema2Relation *> tableRelations = tableModel->relations()->values();
        for(Schema2Relation * tableRelation: tableRelations) {
            int childTable = tables.indexOf(tableModel->tableName().toLower());
            int parentTable = tables.indexOf(tableRelation->parentTable().toLower());
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

#if 0
    for(const QStringList& item: qAsConst(relations_)) {
        int childTable = tables.indexOf(item[0].toLower());
        int parentTable = tables.indexOf(item[1].toLower());
        if (!relations.contains(childTable)) {
            relations[childTable] = {};
        }
        if (!relations.contains(parentTable)) {
            relations[parentTable] = {};
        }
        relations[childTable].append(parentTable);
        relations[parentTable].append(childTable);
    }
#endif

    QList<int> join;
    for(const QString& name: qAsConst(join_)) {
        int index = tables.indexOf(name.toLower());
        if (index < 0) {
            return {};
        }
        join.append(index);
    }

    for(int key: qAsConst(join)) {
        if (!relations.contains(key)) {
            qDebug() << "!relations.contains(key)";
            return {};
        }
    }


    QList<QList<int>> queue;
    /*QList<int> keys = relations.keys();
    for(int key: qAsConst(keys)) {
        queue.append({key});
    }*/
    for(int key: qAsConst(join)) {
        queue.append({key});
    }

    QList<int> found;

    while(!queue.isEmpty()) {
        QList<int> item = queue.takeFirst();

        if (containsAll(item, join)) {
            found = item;
            break;
        }

        QList<int> related = relations[item[item.size()-1]];
        for(int key: qAsConst(related)) {
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
    res.append(Schema2Join(tables_[found[0]]));
    for(int i=1;i<found.size();i++) {
        QString childTable = tables_[found[i]];
        QString parentTable = tables_[found[i-1]];
        //QStringList columns = findChildColumnParentColumn(childTable, parentTable, relationModels);
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
                .arg(join.childTable);
        res.append(head);
        for(int i=1;i<expr.size();i++) {
            Schema2Join join = expr[i];
            QString item = QString("left join %1 on %2")
                    .arg(join.childTable)
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
            tables.append(expr[i].childTable);
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
