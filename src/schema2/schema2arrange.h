#ifndef SCHEMA2ARRANGE_H
#define SCHEMA2ARRANGE_H

#include <QList>
class Schema2TablesModel;
#include "hash.h"

//void arrangeInGrid(const QList<Schema2TableItem*>& items, int width, int heigth, int spacing);

#if 0
void squareArrange(const QStringList tables, const QHash<QStringList, Schema2RelationModel*>& relationModels,
                      const QHash<QString, Schema2TableItem *> &tableItems);
#endif

enum GridType {
    GridSquare,
    GridTriangle
};

void arrangeTables(GridType type, Schema2TablesModel *tablesModel);


#endif // SCHEMA2ARRANGE_H
