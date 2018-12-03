#include "rowvaluegetter.h"

#include <QAbstractItemModel>

RowValueGetter::RowValueGetter(QAbstractItemModel *model, int row) : mModel(model), mRow(row) {}

QVariant RowValueGetter::operator ()(int col)
{
    return mModel->data(mModel->index(mRow,col));
}
