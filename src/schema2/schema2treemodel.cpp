#include "schema2treemodel.h"

#include <QStandardItem>
#include "schema2tablemodel.h"

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
protected:
    Schema2TableModel* mModel;
};

Schema2TreeModel::Schema2TreeModel(QObject *parent)
    : QStandardItemModel{0, 2, parent}
{
    setHorizontalHeaderLabels({"Name", "Type"});
}

void Schema2TreeModel::updateTable(Schema2TableModel *table)
{
    auto* item = new TableItem(table);
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    appendRow(item);
    //qDebug() << "appendRow(item)" << item;
}

// todo remove
void Schema2TreeModel::updateColumns(const QString &tableName)
{
    TableItem* table = findTable(tableName);
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

TableItem* Schema2TreeModel::findTable(const QString& name) {
    auto* root = invisibleRootItem();
    for(int row=0;row<rowCount();row++) {
        TableItem* item = static_cast<TableItem*>(root->child(row));
        if (item->tableName() == name) {
            return item;
        }
    }
    return nullptr;
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
