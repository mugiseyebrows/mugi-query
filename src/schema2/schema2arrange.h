#ifndef SCHEMA2ARRANGE_H
#define SCHEMA2ARRANGE_H

#include <QList>
class Schema2TablesModel;
#include "hash.h"

//void arrangeInGrid(const QList<Schema2TableItem*>& items, int width, int heigth, int spacing);

enum GridType {
    GridSquare,
    GridTriangle
};

void arrangeTables(GridType type, Schema2TablesModel *tablesModel, bool all);


#endif // SCHEMA2ARRANGE_H
