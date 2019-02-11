#include "rowvaluegetter.h"

#include <QAbstractItemModel>

RowValueGetter::RowValueGetter(const QAbstractItemModel *model, int row) : mModel(model), mRow(row) {}

RowValueGetter::~RowValueGetter()
{

}

QVariant RowValueGetter::operator ()(int col) const
{
    return mModel->data(mModel->index(mRow,col));
}
