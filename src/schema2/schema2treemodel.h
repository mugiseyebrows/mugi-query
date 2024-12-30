#ifndef SCHEMA2TREEMODEL_H
#define SCHEMA2TREEMODEL_H

#include <QObject>

#include <QStandardItemModel>
#include "sdata.h"

class Schema2TableModel;
class TableItem;
class SRenamed;
class STable;

class Schema2TreeModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit Schema2TreeModel(QObject *parent = nullptr);

    void updateTable(Schema2TableModel* table);

    //void updateColumn(const QString& tableName, const QString& name);

    //void updateColumns(const QString& tableName);

    void tableDropped(const SName &name);

    void tableRenamed(const SRenamed &table);

    void tableAltered(Schema2TableModel *tableModel);

    void tableCreated(Schema2TableModel* table);

    bool isTable(const QModelIndex& index) const;

    bool isSchema(const QModelIndex& index) const;

    bool isColumn(const QModelIndex& index) const;

    SName tableName(const QModelIndex& index) const;

    //SchemaItem *findSchema(const QString &name) const;
signals:


protected:
    QModelIndex indexOfTable(const SName& sname) const;

    QModelIndex indexOfSchema(const QString &name) const;

    //TableItem *findTable(const SName &name) const;
    void emitRowChanged(int row);
    SNames tableNames() const;
};

#endif // SCHEMA2TREEMODEL_H
