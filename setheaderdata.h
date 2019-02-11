#ifndef SETHEADERDATA_H
#define SETHEADERDATA_H

class QAbstractItemModel;
#include <qnamespace.h>
#include <QStringList>

void setHeaderData(QAbstractItemModel* model, Qt::Orientation orientation, const QStringList& values);

#endif // SETHEADERDATA_H
