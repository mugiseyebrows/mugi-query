#include "relations.h"

#include "rowvaluegetter.h"
#include <QAbstractItemModel>
#include <QSet>

Relations::Relations(QAbstractItemModel* model)
{
    initRelations(model);
    initLinks();
}

void Relations::initRelations(QAbstractItemModel *model)
{
    for(int row=0;row<model->rowCount();row++) {
        RowValueGetter g(model,row);
        Relation relation(g(0).toString(),g(1).toString());
        if (!relation.isEmpty()) {
            relations.append(relation);
        }
    }
    QStringList tables;
    foreach(const Relation& relation, relations) {
        tables << relation.primaryTable() << relation.foreignTable();
    }
    tables = tables.toSet().toList();
    for(int i=0;i<relations.size();i++) {
        Relation& relation = relations[i];
        relation.setPrimary(tables.indexOf(relation.primaryTable()));
        relation.setForeign(tables.indexOf(relation.foreignTable()));
    }
}

void Relations::initLinks()
{

    foreach(const Relation& relation, relations) {
        int primary = relation.primary();
        int foreign = relation.foreign();
        if (!links.contains(primary)) {
            links[primary] = QList<int>();
        }
        if (!links.contains(foreign)) {
            links[foreign] = QList<int>();
        }
        if (primary == foreign) {
            errors << QString("recursive relations are not supported %1.%2 %3.%4")
                      .arg(relation.primaryTable())
                      .arg(relation.primaryKey())
                      .arg(relation.foreignTable())
                      .arg(relation.foreignKey());
        } else {
            links[primary].append(foreign);
            links[foreign].append(primary);
        }
    }
}

Relations::PathList Relations::filterContains(const PathList& paths, int table) {
    QList<Path> res;
    foreach(const Path& path, paths) {
        if (path.contains(table)) {
            res << path;
        }
    }
    return res;
}

Relations::Path Relations::shortest(const PathList &paths, int table) {
    QList<int> indexes;
    foreach(const Path& path, paths) {
        indexes << path.indexOf(table);
    }
    int minIndex = *std::min_element(indexes.begin(),indexes.end());
    for(int i=0;i<indexes.size();i++) {
        if (indexes[i] == minIndex) {
            return paths[i].mid(0,minIndex+1);
        }
    }
    return Relations::Path();
}

Relations::Path Relations::shortest(int table1, int table2)
{
    Relations::PathList paths = pathList(table1);
    paths = filterContains(paths, table2);
    if (paths.isEmpty()) {
        return Path();
    }
    return shortest(paths, table2);
}

Relations::PathList Relations::pathList(int head)
{
    if (mPathLists.contains(head)) {
        return mPathLists[head];
    }

    QList<QList<int> > paths;
    QList<QList<int> > paths_;
    QList<int> initial;
    initial << head;
    paths.append(initial);
    QList<QList<int> > complete;
    int iter = 0;
    while(!paths.isEmpty()) {
        iter++;
        for(int i=0;i<paths.size();i++) {
            QList<int> path = paths[i];
            int head = path[path.size()-1];
            QList<int> possibilities = links[head];
            bool stuck = true;
            foreach(int possibility, possibilities) {
                if (!path.contains(possibility)) {
                    QList<int> newPath = path;
                    newPath << possibility;
                    paths_ << newPath;
                    stuck = false;
                }
            }
            if (stuck) {
                complete << path;
            }
        }

        paths = paths_;
        paths_ = QList<QList<int> >();
    }

    mPathLists[head] = complete;
    return complete;
}
