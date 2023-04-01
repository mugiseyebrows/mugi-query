#ifndef SCHEMA2PARENTRELATIONSMODEL_H
#define SCHEMA2PARENTRELATIONSMODEL_H

#include <QAbstractTableModel>

class Schema2TablesModel;
class Schema2Relation;
class Schema2TableModel;

class Schema2ParentRelationsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum cols {
        col_name,
        col_child_table,
        col_child_columns,
        col_parent_table,
        col_parent_columns,
        col_constrained,
        cols_count
    };

    explicit Schema2ParentRelationsModel(Schema2TableModel* table, QObject *parent = nullptr);

    void update(Schema2TablesModel* tables);

    Schema2Relation* at(int index) const;

    QList<Schema2Relation*> values() const;

protected:
    void setItems(const QList<Schema2Relation*>& relations);

    Schema2TableModel* mTable;
    QList<Schema2Relation*> mRelations;

signals:

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void beginSetRowCount(int rows, int rowsBefore);
    void endSetRowCount(int rows, int rowsBefore);

    // QAbstractItemModel interface
public:
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
};

#endif // SCHEMA2PARENTRELATIONSMODEL_H
