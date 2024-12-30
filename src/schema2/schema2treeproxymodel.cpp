#include "schema2treeproxymodel.h"

#include "treedepth.h"

Schema2TreeProxyModel::Schema2TreeProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{

}

bool Schema2TreeProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (source_parent.isValid()) {
        return true;
    }
    const auto* model = sourceModel();
    QString name = model->data(model->index(source_row, 0, source_parent)).toString();
    auto rx = filterRegularExpression();
    return rx.match(name).hasMatch();
}


bool Schema2TreeProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    if (treeDepth(source_left) == 2) {
        return source_left.row() < source_right.row(); // do not sort columns
    }
    return source_left.data().toString() < source_right.data().toString();
}
