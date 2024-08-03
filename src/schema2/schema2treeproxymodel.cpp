#include "schema2treeproxymodel.h"

Schema2TreeProxyModel::Schema2TreeProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{}

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
