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
    void initRelations(QAbstractItemModel* model);
    void initLinks();

    PathList pathList(int head);
    PathList filterContains(const QList<Path> &paths, int table);

    Path shortest(int table1, int table2);
    Path shortest(const PathList &paths, int table);

protected:
    QList<Relation> relations;
    QMap<int, QList<int> > links;
    QStringList errors;
    QMap<int,PathList> mPathLists;
};

#endif // RELATIONS_H
