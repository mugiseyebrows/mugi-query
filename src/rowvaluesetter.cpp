#include "rowvaluesetter.h"

#include <QAbstractItemModel>

RowValueSetter::RowValueSetter(QAbstractItemModel *model, int row) : mModel(model), mRow(row) {}

bool RowValueSetter::operator()(int col, const QVariant& value, int role) {
    QModelIndex index = mModel->index(mRow,col);
    if (!index.isValid()) {
        return false;
    }
    return mModel->setData(index,value,role);
}

RowValueNotEmptySetter::RowValueNotEmptySetter(QAbstractItemModel *model, int row) : mModel(model), mRow(row)
{

}

bool RowValueNotEmptySetter::operator()(int col, const QString &value, int role)
{
    if (!value.isEmpty()) {
        return mModel->setData(mModel->index(mRow,col),value,role);
    }
    return true;
}
