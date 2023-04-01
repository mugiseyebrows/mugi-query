#include "schema2tablesmodel.h"

#include <QGraphicsScene>
#include "schema2tableitem.h"
#include <QDebug>
#include "schema2store.h"
#include "schema2tablemodel.h"
#include "schema2tableitem.h"
#include "schema2relationsmodel.h"
#include "schema2relation.h"
#include "schema2relationitem2.h"
#include "schema2parentrelationsmodel.h"
#include "schema2indexesmodel.h"
#include "schema2index.h"

Schema2TablesModel::Schema2TablesModel(const QString& connectionName, QGraphicsScene *scene, QObject *parent)
    : mConnectionName(connectionName), mScene(scene), QAbstractTableModel{parent}
{

}

int Schema2TablesModel::indexOf(const QString& name) {

    for(int i=0;i<mTableModels.size();i++) {
        if (mTableModels[i]->tableName().toLower() == name.toLower()) {
            return i;
        }
    }
    return -1;
}

Schema2TableModel *Schema2TablesModel::table(const QString &name) {
    int index = indexOf(name);
    if (index < 0) {
        return 0;
    }
    return mTableModels[index];
}

bool Schema2TablesModel::contains(const QString &table) {
    return indexOf(table) > -1;
}

Schema2TableModel* Schema2TablesModel::tablePulled(const QString& table, Status status) {

    if (contains(table)) {
        return 0;
    }
    int row = rowCount();

    Schema2TableModel* model = new Schema2TableModel(table, status);

    connect(model, SIGNAL(tableClicked(QString)), this, SIGNAL(tableClicked(QString)));
    Schema2TableItem* item = new Schema2TableItem(model);
    if (mTablePos.contains(table)) {
        item->setPos(mTablePos.get(table));
    } else {
        mSetPosQueue.append(item);
    }

    beginInsertRows(QModelIndex(), row, row);
    mTableModels.append(model);
    mTableItems.append(item);
    endInsertRows();
    mScene->addItem(item);
    return model;
}

Schema2TableModel* Schema2TablesModel::tableRemoved(const QString &tableName)
{
    int index = indexOf(tableName);
    if (index < 0) {
        return 0;
    }
    beginRemoveRows(QModelIndex(), index, index);
    auto* item = mTableItems[index];
    auto* model = mTableModels[index];
    mScene->removeItem(item);
    mTableModels.removeAt(index);
    mTableItems.removeAt(index);
    delete item;
    endRemoveRows();
    return model;
}

Schema2TableItem *Schema2TablesModel::tableItem(const QString &name) {
    int index = indexOf(name);
    if (index < 0) {
        return 0;
    }
    return mTableItems[index];
}

void Schema2TablesModel::setGrayed(const QString &name, bool value) {
    auto* table = tableItem(name);
    if (!table) {
        qDebug() << "!table" << __FILE__ << __LINE__;
        return;
    }
    table->setGrayed(value);
}

void Schema2TablesModel::loadPos() {
    mTablePos = Schema2Store::instance(this)->loadPos(mConnectionName);
}

void Schema2TablesModel::savePos()
{
    QHash<QString, QPointF> pos;
    for(Schema2TableItem* item: qAsConst(mTableItems)) {
        pos[item->tableName()] = item->pos();
    }
    Schema2Store::instance(this)->savePos(mConnectionName, pos);
}

void Schema2TablesModel::setTableItemsPos()
{
    int w = 200;
    int s = 25;
    int spacing = 40;
    int h = 900;

    int x = 0;
    int y = 0;

    for(Schema2TableItem* item: mSetPosQueue) {
        int height = item->boundingRect().height();
        if ((y + height + spacing) > h) {
            x += w + spacing;
            y = 0;
            item->setPos(x, y);
        } else {
            item->setPos(x, y);
        }
        y += height + spacing;
    }
}

QList<Schema2TableModel *> Schema2TablesModel::tables() const
{
    return mTableModels;
}

QStringList Schema2TablesModel::tableNames() const
{
    QStringList res;
    for(Schema2TableModel* model: qAsConst(mTableModels)) {
        res.append(model->tableName());
    }
    return res;
}

QList<Schema2Relation *> Schema2TablesModel::relationsFrom(const QString &tableName)
{
    Schema2TableModel* table = this->table(tableName);
    return table->relations()->values();
}



QList<Schema2Relation *> Schema2TablesModel::relationsTo(const QString &tableName)
{
#if 0
    QList<Schema2Relation *> res;
    for(Schema2TableModel* table: qAsConst(mTableModels)) {
        auto* relation = table->relationTo(tableName);
        if (relation) {
            res.append(relation);
        }
    }
    return res;
#endif
    Schema2TableModel* table = this->table(tableName);
    return table->parentRelations()->values();
}

Schema2TableModel *Schema2TablesModel::findChildTable(Schema2Relation *relation)
{
    for(Schema2TableModel* table: qAsConst(mTableModels)) {
        if (table->contains(relation)) {
            return table;
        }
    }
    return 0;
}

Schema2TableItem *Schema2TablesModel::findItem(Schema2TableModel *model)
{
    for(auto* item: mTableItems) {
        if (item->model() == model) {
            return item;
        }
    }
    return 0;
}



Schema2RelationItem2* Schema2TablesModel::findItem(Schema2Relation* relation,
                                                   Schema2TableItem* childTable,
                                                   Schema2TableItem* parentTable) {

    for(Schema2RelationItem2* item: mRelationItems) {
        if (item->childTable() == childTable && item->parentTable() == parentTable) {
            return item;
        }
    }
    return 0;
}

QStringList Schema2TablesModel::createTablesQueries(const QString& driverName, QSqlDriver *driver) const
{
    QStringList res;
    for(auto* model: mTableModels) {
        res.append(model->createQueries(driverName, driver));
    }
    return res;
}



QStringList Schema2TablesModel::createIndexesQueries(const QString& driverName, QSqlDriver *driver) const {
    QStringList res;
    for(auto* model: mTableModels) {
        auto relationNames = model->relationNames();
        auto indexNames = model->indexNames();
        for(const QString& name: indexNames) {
            if (relationNames.contains(name)) {
                continue;
            }
            auto* index = model->indexes()->get(name);
            res.append(index->createQuery(model->tableName(), driverName, driver));
        }
    }
    return res;
}

QStringList Schema2TablesModel::createRelationsQueries(const QString& driverName, QSqlDriver *driver) const {
    QStringList res;
    for(auto* model: mTableModels) {
        auto relations = model->relations()->values();
        for(auto* relation: relations) {
            res.append(relation->createQueries(model->tableName(), driverName, driver));
        }
    }
    return res;
}

int Schema2TablesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return mTableModels.size();
}

int Schema2TablesModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return 1;
}

QVariant Schema2TablesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    if (role == Qt::EditRole || role == Qt::DisplayRole) {
        if (index.column() == 0) {
            return mTableModels[index.row()]->tableName();
        }
    }
    if (role == Qt::CheckStateRole) {
        if (index.column() == 0) {
            return mTableItems[index.row()]->grayed() ? Qt::Unchecked : Qt::Checked;
        }
    }
    return QVariant();
}


bool Schema2TablesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) {
        return false;
    }
    if (role == Qt::CheckStateRole) {
        bool checked = value.toInt() == Qt::Checked;
        mTableItems[index.row()]->setGrayed(!checked);
        emit dataChanged(index, index);
    }
    return false;
}

void Schema2TablesModel::relationPulled(const QString& constraintName, const QString& childTable, const QStringList& childColumns,
               const QString& parentTable, const QStringList& parentColumns,
               bool constrained, Status status) {

    Schema2TableItem* childItem = tableItem(childTable);
    Schema2TableItem* parentItem = tableItem(parentTable);

    Schema2TableModel* childModel = table(childTable);
    Schema2TableModel* parentModel = table(parentTable);

    if (!childItem || !parentItem || !childModel || !parentModel) {
        qDebug() << childTable << childModel << childItem
                 << parentTable << parentModel << parentItem << __FILE__ << __LINE__;
        return;
    }

    Schema2Relation* newRelation = childModel->insertRelation(constraintName, childColumns, parentTable, parentColumns, constrained, status);
    if (newRelation) {
        Schema2RelationItem2* relationItem =
                new Schema2RelationItem2(childItem, parentItem);

        mRelationItems.append(relationItem);
        parentItem->addRelation(relationItem);
        childItem->addRelation(relationItem);

        mScene->addItem(relationItem);
    }

    parentModel->updateParentRelations(this);
}

void Schema2TablesModel::relationRemoved(Schema2Relation* relation) {

    auto* childTable = findChildTable(relation);
    mDropRelationsQueue.append({childTable->tableName(), relation});

    //Schema2TableModel* childTable = findChildTable(relation);
    Schema2TableModel* parentTable = table(relation->parentTable());

    childTable->removeRelation(relation);

    Schema2TableItem * childTableItem = findItem(childTable);
    Schema2TableItem * parentTableItem = findItem(parentTable);

    Schema2RelationItem2* relationItem = findItem(relation, childTableItem, parentTableItem);

    childTableItem->removeRelation(relationItem);
    parentTableItem->removeRelation(relationItem);

    mScene->removeItem(relationItem);

    delete relationItem;
}

Qt::ItemFlags Schema2TablesModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable;
}
