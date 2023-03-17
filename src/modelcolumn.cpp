#include "modelcolumn.h"

#include <QAbstractItemModel>

void setModelColumn(QAbstractItemModel* model, int column, const QStringList& values) {
    if (!model) {
        return;
    }
    for(int row=0;row<values.size();row++) {
        model->setData(model->index(row, column), values[row]);
    }
}

QStringList modelColumn(QAbstractItemModel* model, int column) {
    if (!model) {
        return {};
    }
    QStringList res;
    for(int row=0;row<model->rowCount();row++) {
        QString value = model->data(model->index(row, column)).toString();
        res.append(value);
    }
    return res;
}
