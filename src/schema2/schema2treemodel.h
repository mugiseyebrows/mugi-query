#ifndef SCHEMA2TREEMODEL_H
#define SCHEMA2TREEMODEL_H

#include <QObject>

#include <QStandardItemModel>

class Schema2TableModel;
class TableItem;

class Schema2TreeModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit Schema2TreeModel(QObject *parent = nullptr);

    void updateTable(Schema2TableModel* table);

    //void updateColumn(const QString& tableName, const QString& name);

    void updateColumns(const QString& tableName);

signals:



protected:
    TableItem *findTable(const QString &name);
};

#endif // SCHEMA2TREEMODEL_H
