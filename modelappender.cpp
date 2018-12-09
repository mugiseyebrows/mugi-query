#include "modelappender.h"

ModelAppender::ModelAppender(QObject *parent) : QObject(parent)
{

}

void ModelAppender::setModel(QAbstractItemModel *model)
{
    connect(model,SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this,SLOT(onDataChanged(QModelIndex,QModelIndex,QVector<int>)));
}


void ModelAppender::onDataChanged(QModelIndex tl,QModelIndex br,QVector<int> roles) {


    QAbstractItemModel* model = const_cast<QAbstractItemModel*>(br.model());
    if (br.row() == model->rowCount() - 1 && !model->data(br).toString().isEmpty()) {
        model->insertRow(model->rowCount());
    }


}
