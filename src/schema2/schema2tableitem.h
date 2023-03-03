#ifndef SCHEMA2TABLEITEM_H
#define SCHEMA2TABLEITEM_H

#include <QGraphicsItem>

class Schema2TableModel;
class Schema2RelationItem;

class Schema2TableItem : public QGraphicsItem
{
public:
    Schema2TableItem(Schema2TableModel* model, QGraphicsItem *parent = nullptr);

    void addRelation(Schema2RelationItem* relation);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
protected:
    Schema2TableModel* mModel;
    QList<Schema2RelationItem*> mRelations;

    // QGraphicsItem interface
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
};

#endif // SCHEMA2TABLEITEM_H
