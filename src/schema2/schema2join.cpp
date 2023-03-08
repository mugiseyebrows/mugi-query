#include "schema2join.h"
#include <QDebug>

#include "schema2relationmodel.h"

static bool containsAll(const QList<int>& item, const QList<int> join) {
    for(int key: join) {
        if (!item.contains(key)) {
            return false;
        }
    }
    return true;
}

QStringList toLower(const QStringList& items) {
    QStringList res;
    for(const QString& item: items) {
        res.append(item.toLower());
    }
    return res;
}

static QStringList childColumnParentColumn(const QHash<QStringList, Schema2RelationModel *>& relationModels,
                               const QStringList& key, bool reverse = false) {
    if (relationModels.contains(key)) {
        QString childColumn = relationModels[key]->childColumn();
        QString parentColumn = relationModels[key]->parentColumn();
        if (reverse) {
            return {parentColumn, childColumn};
        } else {
            return {childColumn, parentColumn};
        }
    }
    return {};
}

static QStringList findChildColumnParentColumn(const QString &childTable, const QString &parentTable,
                           const QHash<QStringList, Schema2RelationModel *>& relationModels) {

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

QList<Schema2Join> findJoinImpl(const QStringList &join_,
                 const QHash<QString, Schema2TableModel *>& tableModels,
                 const QHash<QStringList, Schema2RelationModel *>& relationModels)
{
    QStringList tables_ = tableModels.keys();

    QStringList tables = toLower(tables_);

    QHash<int, QList<int>> relations;

    QList<QStringList> relations_ = relationModels.keys();

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
        QStringList columns = findChildColumnParentColumn(childTable, parentTable, relationModels);
        QString childColumn = columns[0];
        QString parentColumn = columns[1];
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
            QString item = QString("left join %1 on %2.%3 = %4.%5")
                    .arg(join.childTable)
                    .arg(join.childTable)
                    .arg(join.childColumn)
                    .arg(join.parentTable)
                    .arg(join.parentColumn);
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
            QString item = QString("%2.%3 = %4.%5")
                    .arg(join.childTable)
                    .arg(join.childColumn)
                    .arg(join.parentTable)
                    .arg(join.parentColumn);
            cond.append(item);
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
