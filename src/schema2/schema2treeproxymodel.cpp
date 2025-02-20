#include "schema2treeproxymodel.h"

#include "treedepth.h"
#include "schema2treemodel.h"

Schema2TreeProxyModel::Schema2TreeProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{

}

bool Schema2TreeProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (!sourceParent.isValid()) {
        // schema
        return true;
    }

    if (sourceParent.isValid() && !sourceParent.parent().isValid()) {
        // table
        return filterAcceptsRowItself(sourceRow, sourceParent);
    }

    if (sourceParent.isValid() && sourceParent.parent().isValid()) {
        // column
        return true;
    }
    return false;
}

bool Schema2TreeProxyModel::filterAcceptsRowItself(int sourceRow, const QModelIndex &sourceParent) const
{
    return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}

bool Schema2TreeProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    if (treeDepth(source_left) == 2) {
        return source_left.row() < source_right.row(); // do not sort columns
    }
    return source_left.data().toString() < source_right.data().toString();
}
