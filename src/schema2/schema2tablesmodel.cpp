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
#include "schema2treemodel.h"
#include "schema2treeproxymodel.h"

Schema2TablesModel::Schema2TablesModel(const QString& connectionName, QGraphicsScene *scene, QObject *parent)
    : mConnectionName(connectionName), mScene(scene), mTreeModel(new Schema2TreeModel(this)), mTreeProxyModel(new Schema2TreeProxyModel(this)), QAbstractTableModel{parent}
{
    mTreeProxyModel->setSourceModel(mTreeModel);
}

int Schema2TablesModel::indexOf(const QString& name) const {

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



#if 0
void Schema2TablesModel::updateColumns(const QString& tableName) {

    mTreeModel->updateColumns(tableName);

}
#endif

#if 0
// todo updateColumn(const QString& tableName, const QList<ColumnData>& data)
void Schema2TablesModel::updateColumn(const QString& tableName, const QString &name, const QString &type, bool notNull,
                                                   const QString& default_,
                                                   bool autoIncrement, const QString& prev) {


    Schema2TableModel* model = this->table(tableName);
    if (model->updateColumn(name, type, notNull, default_, autoIncrement, prev)) {
        //mTreeModel->updateColumn(tableName, name);
    }

}
#endif

Schema2TreeModel *Schema2TablesModel::tree() const
{
    return mTreeModel;
}

Schema2TreeProxyModel *Schema2TablesModel::treeProxy() const
{
    return mTreeProxyModel;
}

QList<STable> Schema2TablesModel::tablesState() const
{
    QList<STable> res;
    for(auto* table: mTableModels) {
        QList<SColumn> columns;
        QString tableName = table->tableName();
        for(int row=0;row<table->rowCount();row++) {
#if 0
            QString name = table->name(row);
            QString type = table->type(row);
            bool notNull = table->notNull(row);
            QString default_ = table->default_(row);
            auto autoincrement = table->autoincrement(row);
            columns.append(SColumn(name, type, notNull, default_, autoincrement));
#endif
            columns.append(table->at(row));
        }
        res.append(STable(tableName, columns));
    }
    return res;
}

QList<SRelation> Schema2TablesModel::relationsState() const
{
    QList<SRelation> res;
    for(auto* model: mTableModels) {
        auto* relationsModel = model->relations();
        for(int row=0;row<relationsModel->rowCount();row++) {
            SRelation relation = relationsModel->at(row)->asRelation();
            res.append(relation);
        }
    }
    return res;
}

Schema2TableModel* Schema2TablesModel::tableCreated(const QString& table, Status status) {

    if (contains(table)) {
        return 0;
    }
    int row = rowCount();

    Schema2TableModel* model = new Schema2TableModel(table, status);

    connect(model, SIGNAL(tableClicked(QString, QPointF)), this, SIGNAL(tableClicked(QString, QPointF)));
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
    mTreeModel->tableCreated(model);
    mTreeProxyModel->sort(0);
    return model;
}

void Schema2TablesModel::tableDropped(const QString& name) {
    /*auto* table = this->table(name);
    auto* tableItem = this->tableItem(name);
*/

    int index = indexOf(name);
    if (index < 0) {
        qDebug() << "tableDropped error index < 0" << name << __FILE__ << __LINE__;
        return;
    }
    beginRemoveRows(QModelIndex(), index, index);

    auto* table = mTableModels.takeAt(index);
    auto* tableItem = mTableItems.takeAt(index);

    mScene->removeItem(tableItem);

    for(auto* item: mRelationItems) {
        if (item->parentTable() == tableItem) {
            //toRemove.append(item);
            mScene->removeItem(item);
            delete item;
        } else if (item->childTable() == tableItem) {
            //toRemove.append(item);
            mScene->removeItem(item);
            delete item;
        }
    }

    for(auto* item: mTableModels) {
        QList<Schema2Relation*> relations = item->relationsTo(name);
        for(auto* relation: relations) {
            item->relations()->remove(relation);
        }
    }

    delete tableItem;

    // todo: tree item uses this pointer
    //delete table;

    endRemoveRows();

    mTreeModel->tableDropped(name);
}

void Schema2TablesModel::tableRenamed(const SRenamed& renamed) {
    Schema2TableModel * table = this->table(renamed.oldName);
    table->setTableName(renamed.newName);
    mTreeModel->tableRenamed(renamed);
}

void Schema2TablesModel::tableAltered(const STable& table) {
    Schema2TableModel* model = this->table(table.name);
    model->tableAltered(table);
    mTreeModel->tableAltered(model);
}

void Schema2TablesModel::merge(const STablesDiff &diff)
{
    QList<STable> created = diff.created;
    QStringList dropped = diff.dropped;
    QList<SRenamed> renamed = diff.renamed;
    QList<STable> altered = diff.altered;

    for(const auto& table: created) {
        tableCreated(table.name, StatusExisting);
        tableAltered(table);
    }
    for(const auto& table: dropped) {
        tableDropped(table);
    }
    for(const auto& item: renamed) {
        tableRenamed(item);
        tableAltered(item.table);
    }
    for(const auto& table: altered) {
        tableAltered(table);
    }
    mTreeProxyModel->sort(0);
}

void Schema2TablesModel::merge(const SRelationsDiff &diff) {

    for(const auto& item: diff.dropped) {
        relationDropped(item);
    }
    for(const auto& item: diff.created) {
        relationCreated(item, true, StatusExisting);
    }

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

QList<Schema2TableItem *> Schema2TablesModel::tableItems() const
{
    return mTableItems;
}

Schema2TableItem *Schema2TablesModel::tableItem(const QString &name) const {
    int index = indexOf(name);
    if (index < 0) {
        return 0;
    }
    return mTableItems[index];
}

void Schema2TablesModel::setChecked(const QString &name, bool value) {
    auto* table = tableItem(name);
    if (!table) {
        qDebug() << "!table" << __FILE__ << __LINE__;
        return;
    }
    table->setChecked(value);
}

void Schema2TablesModel::setChecked(const QStringList& names, bool value) {
    for(const QString& name: names) {
        setChecked(name, value);
    }
}



QStringList Schema2TablesModel::checked(bool value) const {
    QStringList res;
    for(auto* item: mTableItems) {
        if (item->checked() == value) {
            res.append(item->tableName());
        }
    }
    return res;
}

void Schema2TablesModel::setChecked(bool value)
{
    for(auto* item: mTableItems) {
        item->setChecked(value);
    }
    emit dataChanged(index(0,0), index(rowCount() - 1, columnCount() - 1));
}

void Schema2TablesModel::loadPos() {
    mTablePos = Schema2Store::instance(this)->loadPos(mConnectionName);
}

void Schema2TablesModel::savePos()
{
    QHash<QString, QPointF> pos;
    for(Schema2TableItem* item: std::as_const(mTableItems)) {
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
    mSetPosQueue.clear();
}

QList<Schema2TableModel *> Schema2TablesModel::tables() const
{
    return mTableModels;
}

QStringList Schema2TablesModel::tableNames() const
{
    QStringList res;
    for(Schema2TableModel* model: std::as_const(mTableModels)) {
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
    for(Schema2TableModel* table: std::as_const(mTableModels)) {
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
    for(Schema2TableModel* table: std::as_const(mTableModels)) {
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

void Schema2TablesModel::setUncheckedMode(UncheckedMode mode)
{
    for(auto* item: mTableItems) {
        item->setUncheckedMode(mode);
    }
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
            return mTableItems[index.row()]->checked() ? Qt::Checked : Qt::Unchecked;
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

        auto* item = mTableItems[index.row()];

        item->setChecked(checked);

        if (!checked) {
            mScene->removeItem(item);
            mScene->addItem(item);
        }

        emit dataChanged(index, index);
    }
    return false;
}

void Schema2TablesModel::relationCreated(const QString& constraintName, const QString& childTable, const QStringList& childColumns,
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

void Schema2TablesModel::relationCreated(const SRelation& relation, bool constrained, Status status) {
    Schema2TableItem* childItem = tableItem(relation.childTable);
    Schema2TableItem* parentItem = tableItem(relation.parentTable);

    Schema2TableModel* childModel = table(relation.childTable);
    Schema2TableModel* parentModel = table(relation.parentTable);

    if (!childItem || !parentItem || !childModel || !parentModel) {
        qDebug() << "Schema2TablesModel::relationCreated error"
                 << relation.childTable << childModel << childItem
                 << relation.parentTable << parentModel << parentItem << __FILE__ << __LINE__;
        return;
    }

    Schema2Relation* relation_ = childModel->insertRelation(relation.name, relation.childColumns, relation.parentTable, relation.parentColumns, constrained, status);
    if (relation_) {
        Schema2RelationItem2* relationItem =
            new Schema2RelationItem2(childItem, parentItem);

        mRelationItems.append(relationItem);
        parentItem->addRelation(relationItem);
        childItem->addRelation(relationItem);

        mScene->addItem(relationItem);
    }

    parentModel->updateParentRelations(this);
}

void Schema2TablesModel::relationDropped(const SRelation& relation) {
    auto* childTable = table(relation.childTable);
    auto* parentTable = table(relation.parentTable);
    if (!childTable || !parentTable) {
        qDebug() << "Schema2TablesModel::relationDropped error" << __FILE__ << __LINE__;
        return;
    }
    Schema2Relation* relation_ = childTable->removeRelation(relation.name);
    Schema2TableItem * childTableItem = findItem(childTable);
    Schema2TableItem * parentTableItem = findItem(parentTable);
    Schema2RelationItem2* relationItem = findItem(relation_, childTableItem, parentTableItem);
    childTableItem->removeRelation(relationItem);
    parentTableItem->removeRelation(relationItem);
    mScene->removeItem(relationItem);
    delete relationItem;
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
