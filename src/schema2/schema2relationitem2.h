#ifndef SCHEMA2RELATIONITEM2_H
#define SCHEMA2RELATIONITEM2_H

#include <QGraphicsItem>

class Schema2TableItem;

class Schema2RelationItem2: public QGraphicsItem
{
public:
    enum { Type = UserType + 2 };

    int type() const override;

    Schema2RelationItem2(Schema2TableItem* childTable,
                         Schema2TableItem* parentTable,
                         QGraphicsItem *parent = nullptr);

    Schema2TableItem* childTable() const {
        return mChildTable;
    }
    Schema2TableItem* parentTable() const {
        return mParentTable;
    }

protected:
    Schema2TableItem* mChildTable;
    Schema2TableItem* mParentTable;

    // QGraphicsItem interface
public:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // SCHEMA2RELATIONITEM2_H
