#ifndef SCHEMA2TREEMODEL_H
#define SCHEMA2TREEMODEL_H

#include <QObject>

#include <QStandardItemModel>

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

    void tableDropped(const QString &name);

    void tableRenamed(const SRenamed &table);

    void tableAltered(Schema2TableModel *tableModel);

    void tableCreated(Schema2TableModel* table);

    bool isTable(const QModelIndex& index) const;

    QString tableName(const QModelIndex& index) const;

signals:



protected:
    int indexOf(const QString &name) const;
    TableItem *findTable(const QString &name) const;
    void emitRowChanged(int row);
    QStringList tableNames() const;
};

#endif // SCHEMA2TREEMODEL_H
