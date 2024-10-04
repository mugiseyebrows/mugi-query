#include "schema2treemodel.h"

#include <QStandardItem>
#include "schema2tablemodel.h"
#include "sdata.h"

class TableItem : public QStandardItem {
public:
    TableItem(Schema2TableModel* model) : mModel(model), QStandardItem(model->tableName()) {

    }
    Schema2TableModel* model() const {
        return mModel;
    }
    QString tableName() const {
        return mModel->tableName();
    }
    QVariant data(int role) const override {
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            return tableName();
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
    auto* item = new TableItem(table);
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    appendRow(item);
}

#if 0
static TableItem* toTableItem(const QModelIndex &index) {
    void* internalPointer = index.internalPointer();
    if (!internalPointer) {
        return nullptr;
    }
    BaseItem* item = static_cast<BaseItem*>(internalPointer);
    if (item->type() == BaseItem::TypeTable) {
        TableItem* tableItem = static_cast<TableItem*>(item);
        return tableItem;
    }
    return nullptr;
}
#endif

bool Schema2TreeModel::isTable(const QModelIndex &index) const
{
    if (index.column() != 0) {
        return false;
    }
    return index.isValid() && !index.parent().isValid();
}

QString Schema2TreeModel::tableName(const QModelIndex &index) const
{
    if (!isTable(index)) {
        return QString();
    }

    TableItem* item = static_cast<TableItem*>(invisibleRootItem()->child(index.row()));
    return item->tableName();
}

#if 0
void Schema2TreeModel::updateColumns(const QString &tableName)
{
    TableItem* table = indexOf(tableName);
    auto* model = table->model();
    int rowCount = table->rowCount();
    for(int row=0; row<rowCount; row++) {
        QStandardItem* col0 = table->child(row, 0);
        QStandardItem* col1 = table->child(row, 1);
        col0->setText(model->name(row));
        col1->setText(model->type(row));
    }
    for(int row=rowCount;row<model->rowCount();row++) {
        QStandardItem* col0 = new QStandardItem(model->name(row));
        QStandardItem* col1 = new QStandardItem(model->type(row));
        col0->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        col1->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        table->appendRow({col0, col1});
    }
}
#endif

void Schema2TreeModel::tableDropped(const QString &name)
{
    int index = indexOf(name);
    if (index > -1) {
        removeRow(index, QModelIndex());
    }
}

void Schema2TreeModel::emitRowChanged(int row) {
    emit dataChanged(this->index(row, 0), this->index(row, 1));
}

void Schema2TreeModel::tableRenamed(const SRenamed &table)
{
    int index = indexOf(table.newName);
    if (index > -1) {
        emitRowChanged(index);
    } else {
        qDebug() << "Schema2TreeModel::tableRenamed error indexOf(table.oldName) < 0"
                 << table.newName << tableNames();
    }
}

void Schema2TreeModel::tableAltered(Schema2TableModel* tableModel)
{
    //auto* tableItem = findTable(table.name);
    int index = indexOf(tableModel->tableName());
    if (index < 0) {
        qDebug() << "Schema2TreeModel::tableAltered error index < 0"
                 << tableModel->tableName() << tableNames();
        return;
    }

    auto* tableItem = static_cast<TableItem*>(invisibleRootItem()->child(index));
    auto modelIndex = this->index(index, 0);

    Q_ASSERT(tableItem->model() == tableModel);

    int size = tableItem->rowCount();
    int newSize = tableModel->rowCount();
    if (size > newSize) {
        int row = newSize;
        int count = size - newSize;
        removeRows(row, count, modelIndex);
    }
    if (size < newSize) {
        for(int row=size;row<newSize;row++) {
            ColumnNameItem* nameItem = new ColumnNameItem(tableModel, row);
            ColumnTypeItem* typeItem = new ColumnTypeItem(tableModel, row);
            tableItem->appendRow({nameItem, typeItem});
        }
    }
}

int Schema2TreeModel::indexOf(const QString& name) const {
    auto* root = invisibleRootItem();
    for(int row=0;row<rowCount();row++) {
        TableItem* item = static_cast<TableItem*>(root->child(row));
        if (item->tableName() == name) {
            return row;
        }
    }
    return -1;
}

TableItem* Schema2TreeModel::findTable(const QString& name) const {
    auto* root = invisibleRootItem();
    for(int row=0;row<rowCount();row++) {
        TableItem* item = static_cast<TableItem*>(root->child(row));
        if (item->tableName() == name) {
            return item;
        }
    }
    return nullptr;
}

QStringList Schema2TreeModel::tableNames() const {
    auto* root = invisibleRootItem();
    QStringList res;
    for(int row=0;row<rowCount();row++) {
        TableItem* item = static_cast<TableItem*>(root->child(row));
        res.append(item->tableName());
    }
    return res;
}

#if 0
void Schema2TreeModel::updateColumn(const QString &tableName, const QString &name)
{
    TableItem* table = findTable(tableName);
    if (!table) {
        qDebug() << "table is null" << __FILE__ << __LINE__;
        return;
    }
    ColumnItem* item = findColumn(table, name);
    if (!item) {
        item = new ColumnItem();
    }
}
#endif
