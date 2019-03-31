#include "rowvaluesetter.h"

#include <QAbstractItemModel>

RowValueSetter::RowValueSetter(QAbstractItemModel *model, int row) : mModel(model), mRow(row) {}

bool RowValueSetter::operator()(int col, const QVariant& value, int role) {
    return mModel->setData(mModel->index(mRow,col),value,role);
}
