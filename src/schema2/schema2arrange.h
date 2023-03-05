#ifndef SCHEMA2ARRANGE_H
#define SCHEMA2ARRANGE_H

#include <QList>
class Schema2TableItem;

void arrangeInGrid(const QList<Schema2TableItem*>& items, int width, int heigth, int spacing);

#endif // SCHEMA2ARRANGE_H
