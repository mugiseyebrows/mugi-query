#include "schemamodel.h"

SchemaModel::SchemaModel(QObject *parent) : QAbstractItemModel(parent)
{
    mRoot = new SchemaItem("Tables");
}

SchemaModel::~SchemaModel()
{
    delete mRoot;
}

void SchemaModel::setChildren(const QModelIndex& index, const QStringList& data) {
    SchemaItem* item = getItem(index);
    int diff = data.size() - item->rowCount();
    if (diff > 0) {
        beginInsertRows(index,item->rowCount(), item->rowCount() + diff - 1);
        for(int i=0;i<diff;i++) {
            SchemaItem* child = new SchemaItem(QString(),item);
            item->appendChild(child);
        }
        endInsertRows();
    } else if (diff < 0) {
        int diff_ = -diff;
        beginRemoveRows(index,item->rowCount() - diff_, item->rowCount() - 1);
        for(int i=0;i<diff_;i++) {
            delete item->removeChild(item->rowCount() - 1);
        }
        endRemoveRows();
    }
    for(int row=0;row<item->rowCount();row++) {
        item->child(row)->setData(data[row]);
    }
}

void SchemaModel::update(const QMap<QString, Tokens> &tokens)
{
    QStringList connectionNames = tokens.keys();
    qSort(connectionNames);

    // databases
    setChildren(QModelIndex(),connectionNames);

    // tables
    for(int row=0;row<connectionNames.size();row++) {
        const Tokens& tokens_ = tokens[connectionNames[row]];
        QStringList tables = tokens_.tables();
        QModelIndex index = this->index(row,0);
        setChildren(index, tables);
    }

    // fields
    for(int row=0;row<connectionNames.size();row++) {
        QModelIndex databaseIndex = this->index(row,0);
        const Tokens& tokens_ = tokens[connectionNames[row]];
        QStringList tables = tokens_.tables();
        for(int row_=0;row_<tables.size();row_++) {
            QString table = tables[row_];
            QStringList fields = tokens_.fields(table, false);
            QModelIndex tableIndex = this->index(row_,0,databaseIndex);
            setChildren(tableIndex, fields);
        }
    }

}

QVariant SchemaModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        SchemaItem* item = getItem(index);
        return item->data();
    }
    return QVariant();
}

SchemaItem* SchemaModel::getItem(const QModelIndex &index) const {
    if (!index.isValid() || !index.internalPointer()) {
        return mRoot;
    }
    return static_cast<SchemaItem*>(index.internalPointer());
}

QModelIndex SchemaModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    SchemaItem *childItem = getItem(index);
    SchemaItem *parentItem = childItem->parent();

    if (parentItem == mRoot) {
        return QModelIndex();
    }

    return createIndex(parentItem->childIndex(), 0, parentItem);
}

QVariant SchemaModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        if (section == 0) {
            return mRoot->data();
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

int SchemaModel::rowCount(const QModelIndex &parent) const
{
    SchemaItem* item = getItem(parent);
    return item->rowCount();
}

int SchemaModel::columnCount(const QModelIndex &parent) const
{
    return mRoot->columnCount();
}

QModelIndex SchemaModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();
    SchemaItem *parentItem = getItem(parent);
    SchemaItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}
