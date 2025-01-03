#include "relations.h"

#include "rowvaluegetter.h"
#include <QAbstractItemModel>
#include <QSet>
#include <QDebug>

Relations::Relations(QAbstractItemModel* model)
{
    initRelations(model);
    //initLinks();
}

QList<int> concat(int head, const std::vector<int>& tail) {
    QList<int> res;
    res << head;
    for(int i: tail) {
        res << i;
    }
    return res;
}

QList<int> Relations::tablesToIndexes(const QStringList &tables) {
    Path indexes;
    foreach(const QString& table, tables) {
        int index = mTables.indexOf(table);
        if (index < 0) {
            return QList<int>();
        }
        indexes << index;
    }
    return indexes;
}

int Relations::length(const PathList& pathList) {
    int res = 0;
    foreach(const Path path, pathList) {
        res += path.size() - 1;
    }
    return res + 1;
}

// todo refactor
Relations::PathList Relations::shortest(const QList<PathList> & pathLists) {
    if (pathLists.isEmpty()) {
        return PathList();
    }
    QList<int> lengths;
    foreach(const PathList& pathList, pathLists) {
        lengths << length(pathList);
    }
    int minLength = *std::min_element(lengths.begin(), lengths.end());
    for(int i=0;i<lengths.size();i++) {
        if (lengths[i] == minLength) {
            return pathLists[i];
        }
    }
    return PathList();
}

/*
QList<Relations::PathList> Relations::filterDoubleJoin(const QList<Relations::PathList>& pathLists) {
    QList<Relations::PathList> res;
    foreach(const PathList& pathList, pathLists) {
        QSet<int> pool = pathList[0].toSet();
        bool ok = true;
        for(int i=1;i<pathList.size();i++) {
            QList<int> tail = pathList[i].mid(1);
            foreach(int table, tail) {
                if (pool.contains(table)) {
                    ok = false;
                    break;
                } else {
                    pool.insert(table);
                }
            }
            if (!ok) {
                break;
            }
        }
        if (ok) {
            res << pathList;
        }
    }
    return res;
}
*/

/*
int Relations::indexOfShortestNotEmpty(const PathList& paths) {

    std::vector<int> length;
    std::transform(paths.begin(),paths.end(),
                   std::back_inserter(length),
                   [](const Path& path){return path.size() == 0 ? INT_MAX : path.size();});
    int index = std::min_element(std::begin(length),std::end(length)) - std::begin(length);
    return index;
}
*/

int Relations::indexOfShortest(const PathList& paths) {

    std::vector<int> length;
    std::transform(paths.begin(),paths.end(),
                   std::back_inserter(length),
                   [](const Path& path){return path.size();});
    int index = std::min_element(std::begin(length),std::end(length)) - std::begin(length);
    return index;
}

Relations::PathList Relations::findPath(const QStringList &tables)
{
    QList<int> indexes = tablesToIndexes(tables);
    QList<int> head = indexes.mid(0,1);
    QList<int> tail = indexes.mid(1);

    //QStringList errors;

    PathList result;

    typedef QPair<int,int> IntPair;

    QMap<int, QList<int> > links = Relations::buildLinks(mRelations,errors);

    QMap<int, PathList> completePaths = Relations::completePaths(indexes, links);

    while (tail.size() > 0) {

        QList<IntPair> keys;
        PathList paths;

        foreach(int table1, head) {
            foreach(int table2, tail) {
                Path path = Relations::shortestPathTo(completePaths,table1,table2);

                /*if (path.isEmpty()) {
                    qDebug() << "no path from " << mTables[table1] << mTables[table2];
                } else {
                    qDebug() << "path from " << mTables[table1] << mTables[table2] << "is" << (path.size() - 1) << "hops";
                }*/

                if (!path.isEmpty()) {
                    keys << IntPair(table1,table2);
                    paths << path;
                }
            }
        }

        if (paths.isEmpty()) {
            return PathList();
        }
        int index = indexOfShortest(paths);
        result << paths[index];
        IntPair key = keys[index];
        head << key.second;
        tail.removeAll(key.second);
    }

    return result;
}

const Relation& Relations::findRelation(int table1, int table2, bool* reverse) {
    foreach(const Relation& relation, mRelations) {
        bool t1 = (relation.primary() == table1 &&
                   relation.foreign() == table2);
        bool t2 = (relation.primary() == table2 &&
                   relation.foreign() == table1);
        if (t1 || t2) {
            if (t1) {
                *reverse = false;
            } else {
                *reverse = true;
            }
            return relation;
        }
    }
    return mRelations[0];
}

const QString Relations::joinExpression(const Relation &relation, bool reverse)
{
    if (!reverse) {
        return QString("%1.%2=%3.%4")
                 .arg(mTables[relation.primary()])
                 .arg(relation.primaryKey())
                 .arg(mTables[relation.foreign()])
                 .arg(relation.foreignKey());
    }
    return  QString("%1.%2=%3.%4")
            .arg(mTables[relation.foreign()])
            .arg(relation.foreignKey())
            .arg(mTables[relation.primary()])
            .arg(relation.primaryKey());
}

QString repeat(QChar c, int n) {
    QString s;
    s.fill(c,n);
    return s;
}

QString Relations::expression(const Relations::PathList &path, bool leftJoin, bool mssql)
{

    QStringList tables;
    QStringList joins;
    for(int i=0;i<path.size();i++) {
        const Path& subpath = path[i];
        for(int j = (i == 0) ? 0 : 1; j<subpath.size();j++) {
            tables << mTables[subpath[j]];
        }
        for(int j=1;j<subpath.size();j++) {
            int table1 = subpath[j-1];
            int table2 = subpath[j];
            bool reverse;
            const Relation& relation = findRelation(table1,table2,&reverse);
            joins << joinExpression(relation,reverse);
        }
    }

    QString expression;

    if (leftJoin) {
        if (mssql) {
            QStringList expression_;
            QString commans;
            expression_ << QString("select * from %2%1").arg(tables[0]).arg(repeat('(',joins.size()-1));
            for(int i=0;i<joins.size();i++) {
                expression_ << QString("left join %1 on %2%3")
                               .arg(tables[i+1])
                        .arg(joins[i])
                        .arg(i+1<joins.size() ? ")" : "");
            }
            expression = expression_.join("\n");
        } else {
            QStringList expression_;
            expression_ << QString("select * from %1").arg(tables[0]);
            for(int i=0;i<joins.size();i++) {
                expression_ << QString("left join %1 on %2").arg(tables[i+1]).arg(joins[i]);
            }
            expression = expression_.join("\n");
        }
    } else {
        expression = QString("select * from %1\nwhere %2")
                    .arg(tables.join(", "))
                    .arg(joins.join("\nand "));
    }

    return expression;
}

// todo: optimize
static QStringList uniq(const QStringList& values) {
    QStringList res;
    for(const QString& value: values) {
        if (!res.contains(value)) {
            res.append(value);
        }
    }
    return res;
}

void Relations::initRelations(QAbstractItemModel *model)
{
    for(int row=0;row<model->rowCount();row++) {
        RowValueGetter g(model,row);
        Relation relation(g(0).toString(),g(1).toString());
        if (!relation.isEmpty()) {
            mRelations.append(relation);
        }
    }

    foreach(const Relation& relation, mRelations) {
        mTables << relation.primaryTable() << relation.foreignTable();
    }
    mTables = uniq(mTables);
    for(int i=0;i<mRelations.size();i++) {
        Relation& relation = mRelations[i];
        relation.setPrimary(mTables.indexOf(relation.primaryTable()));
        relation.setForeign(mTables.indexOf(relation.foreignTable()));
    }
}

QMap<int, QList<int> > Relations::buildLinks(const QList<Relation>& relations, QStringList& errors)
{
    QMap<int, QList<int> > links;
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
    return links;
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

Relations::Path Relations::shortestPathTo(const PathList &paths, int table) {

    if (paths.isEmpty()) {
        return Path();
    }

    /*QList<int> indexes;
    foreach(const Path& path, paths) {
        indexes << path.indexOf(table);
    }
    int minIndex = *std::min_element(indexes.begin(),indexes.end());
    for(int i=0;i<indexes.size();i++) {
        if (indexes[i] == minIndex) {
            return paths[i].mid(0,minIndex+1);
        }
    }
    return Relations::Path();*/

    std::vector<int> indexes;
    std::transform(paths.begin(),paths.end(),
                   std::back_inserter(indexes),
                   [table](const Path& path){
        int index = path.indexOf(table);
        return index > -1 ? index : INT_MAX;
    });
    int index = std::min_element(std::begin(indexes),std::end(indexes)) - std::begin(indexes);
    if (indexes[index] == INT_MAX) {
        return Path();
    }
    return paths[index].mid(0,indexes[index]+1);
}

/*
Relations::Path Relations::shortest(int table1, int table2)
{
    Relations::PathList paths = pathList(table1);
    return shortest(filterContains(paths, table2), table2);
}
*/

Relations::Path Relations::pathTo(const Relations::Path& path, int table) {
    int index = path.indexOf(table);
    if (index < 0) {
        return Relations::Path();
    }
    return path.mid(0,index-1);
}

/*
Relations::Path Relations::shortestPathTo(const PathList &paths, int table2) {
    return pathTo(shortest(filterContains(paths, table2), table2));
}*/

Relations::Path Relations::shortestPathTo(const QMap<int, Relations::PathList>& paths, int table1, int table2) {
    if (!paths.contains(table1)) {
        return Relations::Path();
    }
    return shortestPathTo(paths[table1],table2);
}

QMap<int, Relations::PathList> Relations::completePaths(const QList<int>& heads, const QMap<int,QList<int> >& links) {
    int head;
    QMap<int, Relations::PathList> result;
    foreach(head,heads) {
        result[head] = completePaths(head, links);
    }
    return result;
}

Relations::PathList Relations::completePaths(int head, const QMap<int,QList<int> >& links) {
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
    return complete;
}

/*
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
*/
