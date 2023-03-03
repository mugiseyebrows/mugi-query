#include "schema2relationitem.h"

#include <QPainter>

#include "schema2tableitem.h"

Schema2RelationItem::Schema2RelationItem(Schema2RelationModel *model, Schema2TableItem *parentTable, Schema2TableItem *childTable, QGraphicsItem *parent)
    : mModel(model), mParentTable(parentTable), mChildTable(childTable), QGraphicsItem(parent)
{

}

QRectF Schema2RelationItem::boundingRect() const
{
    QRectF rect1 = mParentTable->sceneBoundingRect();
    QRectF rect2 = mChildTable->sceneBoundingRect();

    int top = qMin(rect1.top(), rect2.top());
    int left = qMin(rect1.left(), rect2.left());
    int bottom = qMax(rect1.bottom(), rect2.bottom());
    int right = qMax(rect1.right(), rect2.right());

    QRectF res(QPointF(left, top), QPointF(right, bottom));
    return res;
}

void Schema2RelationItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF rect = boundingRect();
    //painter->drawRect(rect);

    QPointF p1 = mParentTable->sceneBoundingRect().center();
    QPointF p2 = mChildTable->sceneBoundingRect().center();

    painter->drawLine(p1, p2);
}
