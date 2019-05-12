#include "schemaitem.h"

#include <QDebug>

SchemaItem::SchemaItem(const QString &data, SchemaItem *parent) : mData(data), mParent(parent)
{

}

SchemaItem::~SchemaItem()
{
    qDeleteAll(mChildren);
}

int SchemaItem::rowCount() const
{
    return mChildren.size();
}

int SchemaItem::columnCount() const
{
    return 1;
}

int SchemaItem::childIndex() const
{
    if (mParent) {
        return mParent->childIndex(const_cast<SchemaItem*>(this));
    }
    return 0;
}

int SchemaItem::childIndex(SchemaItem* item) const {
    return mChildren.indexOf(item);
}

SchemaItem *SchemaItem::child(int row)
{
    return mChildren.value(row);
}

QVariant SchemaItem::data() const
{
    return mData;
}

void SchemaItem::setData(const QString& data) {
    mData = data;
}

void SchemaItem::appendChild(SchemaItem *child)
{
    mChildren.append(child);
}

SchemaItem* SchemaItem::removeChild(int index) {
    return mChildren.takeAt(index);
}

bool SchemaItem::isRoot() const
{
    return type() == ItemRoot;
}

bool SchemaItem::isDatabase() const
{
    return type() == ItemDatabase;
}

bool SchemaItem::isTable() const
{
    return type() == ItemTable;
}

bool SchemaItem::isField() const
{
    return type() == ItemField;
}

SchemaItem::ItemType SchemaItem::type() const
{
    if (!mParent) {
        return ItemRoot;
    }
    if (!mParent->parent()) {
        return ItemDatabase;
    }
    if (!mParent->parent()->parent()) {
        return ItemTable;
    }
    if (!mParent->parent()->parent()->parent()) {
        return ItemField;
    }
}

SchemaItem *SchemaItem::parent() const
{
    return mParent;
}
