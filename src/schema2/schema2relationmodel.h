#ifndef SCHEMA2RELATIONMODEL_H
#define SCHEMA2RELATIONMODEL_H

#include <QAbstractTableModel>

class Schema2RelationModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum rows {
        row_parent_table,
        row_parent_column,
        row_child_table,
        row_child_column,
        rows_count
    };

    Schema2RelationModel(const QString& parentTable, const QString& parentColumn,
                                  const QString& childTable, const QString& childColumn,
                                  QObject *parent = nullptr);

protected:
    QString mParentTable;
    QString mParentColumn;
    QString mChildTable;
    QString mChildColumn;

signals:


    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
};

#endif // SCHEMA2RELATIONMODEL_H
