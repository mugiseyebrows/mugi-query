#include "schema2treemodel.h"

#include <QStandardItem>
#include "schema2tablemodel.h"
#include "sdata.h"
#include "treedepth.h"

class SchemaModelItem : public QStandardItem {
public:
    SchemaModelItem(const QString& name) : mName(name), QStandardItem(name) {

    }
    QString name() const {
        return mName;
    }
protected:
    QString mName;
};

class TableItem : public QStandardItem {
public:
    TableItem(Schema2TableModel* model) : mModel(model), QStandardItem(model->tableName().name) {

    }
    Schema2TableModel* model() const {
        return mModel;
    }
    SName tableName() const {
        return mModel->tableName();
    }
    QVariant data(int role) const override {
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            return tableName().name;
        }
        return QStandardItem::data(role);
    }

protected:
    Schema2TableModel* mModel;
};

class ColumnNameItem : public QStandardItem {
public:
    ColumnNameItem(Schema2TableModel* model, int row) : mModel(model), mRow(row) {

    }
    QVariant data(int role) const override {
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            return mModel->name(mRow);
        }
        return QStandardItem::data(role);
    }
protected:
    Schema2TableModel* mModel;
    int mRow;
};

class ColumnTypeItem : public QStandardItem {
public:
    ColumnTypeItem(Schema2TableModel* model, int row) : mModel(model), mRow(row) {

    }
    QVariant data(int role) const override {
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            return mModel->type(mRow);
        }
        return QStandardItem::data(role);
    }
protected:
    Schema2TableModel* mModel;
    int mRow;
};

Schema2TreeModel::Schema2TreeModel(QObject *parent)
    : QStandardItemModel{0, 2, parent}
{
    setHorizontalHeaderLabels({"Name", "Type"});
}

void Schema2TreeModel::tableCreated(Schema2TableModel *table)
{
    /*auto* item = new TableItem(table);
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    appendRow(item);*/

    QModelIndex schemaIndex = indexOfSchema(table->tableName().schema);

    auto* root = invisibleRootItem();

    SchemaModelItem* schemaItem;
    if (schemaIndex.isValid()) {
        schemaItem = static_cast<SchemaModelItem*>(root->child(schemaIndex.row()));
    } else {
        schemaItem = new SchemaModelItem(table->tableName().schema);
        appendRow(schemaItem);
    }

    TableItem* tableItem = new TableItem(table);
    schemaItem->appendRow(tableItem);
}



bool Schema2TreeModel::isSchema(const QModelIndex &index) const
{
    return treeDepth(index) == 0;
}

bool Schema2TreeModel::isTable(const QModelIndex &index) const
{
    return treeDepth(index) == 1;
}

bool Schema2TreeModel::isColumn(const QModelIndex &index) const
{
    return treeDepth(index) == 2;
}

SName Schema2TreeModel::tableName(const QModelIndex &index) const
{
    if (!isTable(index)) {
        return QString();
    }

    QModelIndex schemaIndex = index.parent();
    auto* root = invisibleRootItem();
    SchemaModelItem* schema = static_cast<SchemaModelItem*>(root->child(schemaIndex.row()));
    TableItem* item = static_cast<TableItem*>(schema->child(index.row()));
    return item->tableName();
}

void Schema2TreeModel::tableDropped(const SName &name)
{
    QModelIndex tableIndex = indexOfTable(name);
    if (!tableIndex.isValid()) {
        qDebug() << "!tableIndex.isValid()" << __FILE__ << __LINE__;
        return;
    }
    removeRow(tableIndex.row(), tableIndex.parent());
}

void Schema2TreeModel::emitRowChanged(int row) {
    emit dataChanged(this->index(row, 0), this->index(row, 1));
}

void Schema2TreeModel::tableRenamed(const SRenamed &table)
{
    QModelIndex tableIndex = indexOfTable(table.newName);
    if (tableIndex.isValid()) {
        emitRowChanged(tableIndex.row());
    } else {
        qDebug() << "!tableIndex.isValid()" << __FILE__ << __LINE__;
    }
}

void Schema2TreeModel::tableAltered(Schema2TableModel* tableModel)
{
    QModelIndex tableIndex = indexOfTable(tableModel->tableName());
    QModelIndex schemaIndex = tableIndex.parent();

    auto* root = invisibleRootItem();
    SchemaModelItem* schemaItem = static_cast<SchemaModelItem*>(root->child(schemaIndex.row()));
    TableItem* tableItem = static_cast<TableItem*>(schemaItem->child(tableIndex.row()));

    int size = tableItem->rowCount();
    int newSize = tableModel->rowCount();
    if (size > newSize) {
        int row = newSize;
        int count = size - newSize;
        removeRows(row, count, tableIndex);
    }
    if (size < newSize) {
        for(int row=size;row<newSize;row++) {
            ColumnNameItem* nameItem = new ColumnNameItem(tableModel, row);
            ColumnTypeItem* typeItem = new ColumnTypeItem(tableModel, row);
            tableItem->appendRow({nameItem, typeItem});
        }
    }


}

QModelIndex Schema2TreeModel::indexOfSchema(const QString &name) const
{
    auto* root = invisibleRootItem();
    for(int row=0;row<rowCount();row++) {
        SchemaModelItem* item = static_cast<SchemaModelItem*>(root->child(row));
        if (item->name() == name) {
            return this->index(row, 0);
        }
    }
    return {};
}

QModelIndex Schema2TreeModel::indexOfTable(const SName& name) const {
    QModelIndex schemaIndex = indexOfSchema(name.schema);
    auto* root = invisibleRootItem();
    if (!schemaIndex.isValid()) {
        qDebug() << "!schemaIndex.isValid()" << __FILE__ << __LINE__;
        return {};
    }
    SchemaModelItem* schema = static_cast<SchemaModelItem*>(root->child(schemaIndex.row()));
    for(int row=0;row<rowCount(schemaIndex);row++) {
        TableItem* item = static_cast<TableItem*>(schema->child(row));
        if (item->tableName() == name) {
            return this->index(row, 0, schemaIndex);
        }
    }
    return {};
}

SNames Schema2TreeModel::tableNames() const {
    SNames res;
    auto* root = invisibleRootItem();
    for(int row=0;row<rowCount();row++) {
        QModelIndex schemaIndex = this->index(row, 0);
        SchemaModelItem* schema = static_cast<SchemaModelItem*>(root->child(row));
        //res.append(item->tableName().name);
        for(int row2=0;row2<rowCount(schemaIndex);row2++) {
            TableItem* item = static_cast<TableItem*>(schema->child(row));
            res.append(item->tableName());
        }
    }
    return res;
}
