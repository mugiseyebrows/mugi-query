#ifndef SCHEMAITEM_H
#define SCHEMAITEM_H

#include <QList>
#include <QVariant>

class SchemaItem
{
public:
    enum ItemType {
        ItemUndefined,
        ItemRoot,
        ItemDatabase,
        ItemTable,
        ItemField
    };

    SchemaItem(const QString& data, SchemaItem* parent = 0);

    ~SchemaItem();

    int rowCount() const;

    int columnCount() const;

    SchemaItem* child(int row);

    QVariant data() const;

    void appendChild(SchemaItem* child);

    SchemaItem* parent() const;

    int childIndex(SchemaItem *item) const;
    int childIndex() const;
    void setData(const QString &data);
    SchemaItem *removeChild(int index);

    bool isRoot() const;
    bool isDatabase() const;
    bool isTable() const;
    bool isField() const;

    ItemType type() const;

protected:
    SchemaItem* mParent;
    QList<SchemaItem*> mChildren;
    QString mData;
};

#endif // SCHEMAITEM_H
