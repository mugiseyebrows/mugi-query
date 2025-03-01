#ifndef SCHEMA2TREEPROXYMODEL_H
#define SCHEMA2TREEPROXYMODEL_H

#include <QSortFilterProxyModel>

class Schema2TreeProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit Schema2TreeProxyModel(QObject *parent = nullptr);

    bool filterAcceptsRowItself(int sourceRow, const QModelIndex &sourceParent) const;
signals:

    // QSortFilterProxyModel interface
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

    // QSortFilterProxyModel interface
protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const;
};

#endif // SCHEMA2TREEPROXYMODEL_H
