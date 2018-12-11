#include "modelappender.h"

ModelAppender::ModelAppender(QObject *parent) : QObject(parent), mActive(true)
{

}

void ModelAppender::setModel(QAbstractItemModel *model)
{
    connect(model,SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this,SLOT(onDataChanged(QModelIndex,QModelIndex,QVector<int>)));
}

void ModelAppender::setActive(bool active)
{
    mActive = active;
}

void ModelAppender::onDataChanged(QModelIndex tl,QModelIndex br,QVector<int> roles) {

    if (!mActive) {
        return;
    }

    QAbstractItemModel* model = const_cast<QAbstractItemModel*>(br.model());
    if (br.row() == model->rowCount() - 1 && !model->data(br).toString().isEmpty()) {
        int row = model->rowCount();
        model->insertRow(row);
        emit rowInserted(row);
    }

}
