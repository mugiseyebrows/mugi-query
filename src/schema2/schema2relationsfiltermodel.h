#ifndef SCHEMA2RELATIONSFILTERMODEL_H
#define SCHEMA2RELATIONSFILTERMODEL_H

#include <QObject>

#include <QSortFilterProxyModel>
#include <QRegularExpression>

class Schema2RelationsFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit Schema2RelationsFilterModel(QObject *parent = nullptr);

    void setChildTable(const QString& childTable);

    void setParentTable(const QString& parentTable);

signals:

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

    QRegularExpression mChildTable;
    QRegularExpression mParentTable;

};

#endif // SCHEMA2RELATIONSFILTERMODEL_H
