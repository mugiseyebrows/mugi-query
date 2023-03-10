#include "schema2relationsfiltermodel.h"

Schema2RelationsFilterModel::Schema2RelationsFilterModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{
    setChildTable(QString());
    setParentTable(QString());
}

void Schema2RelationsFilterModel::setChildTable(const QString &childTable)
{
    mChildTable = QRegularExpression(childTable, QRegularExpression::CaseInsensitiveOption);
    invalidateFilter();
}

void Schema2RelationsFilterModel::setParentTable(const QString &parentTable)
{
    mParentTable = QRegularExpression(parentTable, QRegularExpression::CaseInsensitiveOption);
    invalidateFilter();
}

bool Schema2RelationsFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QAbstractItemModel* model = sourceModel();
    if (!model) {
        return false;
    }
    QString childTable = model->data(model->index(source_row, 0)).toString();
    QString parentTable = model->data(model->index(source_row, 1)).toString();
    return mChildTable.match(childTable).hasMatch() && mParentTable.match(parentTable).hasMatch();
}
