#ifndef RELATIONS_H
#define RELATIONS_H

class QAbstractItemModel;

#include "relation.h"
#include <QMap>

class Relations
{
public:
    typedef QList<int> Path;
    typedef QList<Path> PathList;

    Relations(QAbstractItemModel* model);

    PathList findPath(const QStringList& tables);

    QString expression(const PathList& path, bool leftJoin, bool mssql);

    void initRelations(QAbstractItemModel* model);
    //void initLinks();

    //PathList pathList(int head);
    PathList filterContains(const QList<Path> &paths, int table);

    Path shortest(int table1, int table2);
    Path shortest(const PathList &paths, int table);

    const Relation &findRelation(int table1, int table2, bool *reverse);

    const QString joinExpression(const Relation& relation, bool reverse);

    QList<int> tablesToIndexes(const QStringList &tables);
    Relations::PathList shortest(const QList<PathList> &pathList);
    int length(const PathList &pathList);
    //QList<Relations::PathList> filterDoubleJoin(const QList<Relations::PathList> &pathLists);
    //int indexOfShortestNotEmpty(const PathList &paths);
    int indexOfShortest(const PathList &paths);

    static Relations::Path shortestPathTo(const Relations::PathList &paths, int table2);
    static Relations::Path shortestPathTo(const QMap<int, Relations::PathList> &paths, int table1, int table2);
    static QMap<int, Relations::PathList> completePaths(const QList<int> &heads, const QMap<int, QList<int> > &links);
    static Relations::PathList completePaths(int head, const QMap<int, QList<int> > &links);
    static Relations::Path pathTo(const Relations::Path &path, int table);
    static QMap<int, QList<int> > buildLinks(const QList<Relation>& relations, QStringList& errors);
protected:
    QList<Relation> mRelations;
    //QMap<int, QList<int> > links;
    QStringList errors;
    //QMap<int,PathList> mPathLists;
    QStringList mTables;
};

#endif // RELATIONS_H
