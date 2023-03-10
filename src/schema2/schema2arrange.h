#ifndef SCHEMA2ARRANGE_H
#define SCHEMA2ARRANGE_H

#include <QList>
class Schema2TableItem;
class Schema2RelationModel;
class Schema2RelationItem;
#include "hash.h"

void arrangeInGrid(const QList<Schema2TableItem*>& items, int width, int heigth, int spacing);

#if 0
void squareArrange(const QStringList tables, const QHash<QStringList, Schema2RelationModel*>& relationModels,
                      const QHash<QString, Schema2TableItem *> &tableItems);
#endif

enum GridType {
    GridSquare,
    GridTriangle
};

void arrangeTables(GridType type, const QStringList tables,
             const StringListHash<Schema2RelationModel *> &relationModels,
             const StringHash<Schema2TableItem*>& tableItems);


#endif // SCHEMA2ARRANGE_H
