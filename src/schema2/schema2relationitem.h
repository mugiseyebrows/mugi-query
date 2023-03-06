#ifndef SCHEMA2RELATIONITEM_H
#define SCHEMA2RELATIONITEM_H

#include <QGraphicsItem>
class Schema2RelationModel;
class Schema2TableItem;

class Schema2RelationItem : public QGraphicsItem
{
public:
    Schema2RelationItem(Schema2RelationModel* model,
                        Schema2TableItem* childTable,
                        Schema2TableItem* parentTable,
                        QGraphicsItem *parent = nullptr);
protected:
    Schema2RelationModel* mModel;
    Schema2TableItem* mChildTable;
    Schema2TableItem* mParentTable;

    // QGraphicsItem interface
public:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // SCHEMA2RELATIONITEM_H
