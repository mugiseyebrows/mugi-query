#include "schema2tableitem.h"

#include "schema2tablemodel.h"
#include "schema2relationitem.h"
#include <QPainter>
#include <QDebug>


Schema2TableItem::Schema2TableItem(Schema2TableModel *model, QGraphicsItem *parent)
    : mModel(model), QGraphicsItem(parent)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

void Schema2TableItem::addRelation(Schema2RelationItem *relation)
{
    mRelations.append(relation);
}

QString Schema2TableItem::tableName() const
{
    return mModel->tableName();
}

QRectF Schema2TableItem::boundingRect() const
{
    int w = 200;
    int s = 25;
    return QRectF(0, 0, w, s * (mModel->rowCount() + 1));
}


void Schema2TableItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    int w = 200;
    int s = 25;

    painter->setFont(QFont("Liberation Sans", 11));

    {
        QRectF rect = boundingRect();
        //rect.moveTo(0,0);

        painter->save();

        painter->setBrush(QColor("#FFFBAC"));
        painter->drawRect(rect);
        //painter->drawLine(0,s,w,s);


        rect = QRectF(0,0,w,s);
        painter->setBrush(QColor("#FFD495"));
        //painter->setPen(Qt::NoPen);
        painter->drawRect(rect);
        painter->restore();

    }

    /*{
        QRectF rect(0,0,s,s);
        painter->drawRect(rect);
    }*/

    QRectF rect(0,0,w,s);

    QTextOption opt(Qt::AlignCenter);

    painter->drawText(rect, mModel->tableName(), opt);
    for(int row=0;row<mModel->rowCount();row++) {
        QRectF rect(0,(row + 1)*s,w,s);
        QString name = mModel->newName(row);
        painter->drawText(rect, name, opt);
    }

    painter->setPen(Qt::white);

    /*rect = insertRect();
    painter->drawText(rect, "i", opt);

    rect = updateRect();
    painter->drawText(rect, "u", opt);*/

}

void Schema2TableItem::setCenterPos(const QPointF &point)
{
    setPos(point - boundingRect().center());
}

QVariant Schema2TableItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionHasChanged) {
        for(Schema2RelationItem* relation: mRelations) {
            relation->update();
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

void Schema2TableItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit mModel->tableClicked(mModel->tableName());
}
