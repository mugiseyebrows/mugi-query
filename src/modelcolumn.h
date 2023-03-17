#ifndef MODELCOLUMN_H
#define MODELCOLUMN_H

class QAbstractItemModel;
class QStringList;

void setModelColumn(QAbstractItemModel* model, int column, const QStringList& values);

QStringList modelColumn(QAbstractItemModel* model, int column);

#endif // MODELCOLUMN_H
