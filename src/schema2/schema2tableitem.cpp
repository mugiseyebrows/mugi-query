#include "schema2tableitem.h"

#include "schema2tablemodel.h"
#include "schema2relationitem2.h"
#include <QPainter>
#include <QDebug>


Schema2TableItem::Schema2TableItem(Schema2TableModel *model, QGraphicsItem *parent)
    : mModel(model), QGraphicsItem(parent), mGrayed(false)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

void Schema2TableItem::addRelation(Schema2RelationItem2 *relation)
{
    mRelations.append(relation);
}

void Schema2TableItem::removeRelation(Schema2RelationItem2 *relation) {
    mRelations.removeOne(relation);
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

    if (mGrayed && mMode == UncheckedInvisible) {
        return;
    }

    painter->setFont(QFont("Liberation Sans", 11));

    painter->setPen(mGrayed ? QColor("#888888") : QColor("#000000"));

    {
        QRectF rect = boundingRect();
        //rect.moveTo(0,0);

        painter->save();

        QBrush bodyBrush = mGrayed ? QColor("#ffffff") : QColor("#FFFBAC");

        painter->setBrush(bodyBrush);
        painter->drawRect(rect);
        //painter->drawLine(0,s,w,s);

        rect = QRectF(0,0,w,s);

        QBrush titleBrush = mGrayed ? QColor("#ffffff") : QColor("#FFD495");

        painter->setBrush(titleBrush);
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
        QString name = mModel->name(row);
        QTextOption opt(Qt::AlignCenter);
        painter->drawText(rect, name, opt);
        /*if (mModel->isIndexColumn(name)) {
            QTextOption opt(Qt::AlignRight | Qt::AlignVCenter);
            rect.setWidth(rect.width() - 10);
            painter->drawText(rect, "I", opt);
        }*/
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

QPointF Schema2TableItem::centerPos() const
{
    return this->pos() + boundingRect().center();
}

void Schema2TableItem::setGrayed(bool value)
{
    if (mGrayed == value) {
        return;
    }
    mGrayed = value;
    update();
    for(Schema2RelationItem2* item: mRelations) {
        item->update();
    }
}

void Schema2TableItem::setUncheckedMode(UncheckedMode mode) {
    if (mMode == mode) {
        return;
    }
    mMode = mode;
    update();
    for(Schema2RelationItem2* item: mRelations) {
        item->update();
    }
}

#if 0
bool Schema2TableItem::isIndexColumn(const QString &column) const
{
    return mModel->isIndexColumn(column);
}
#endif

QVariant Schema2TableItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionHasChanged) {
        for(Schema2RelationItem2* relation: mRelations) {
            relation->update();
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

#include <QGraphicsSceneMouseEvent>

void Schema2TableItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    /*qDebug() << "mousePressEvent" << mModel->tableName()
             << "pos" << event->pos()
             << "scenePos" << event->scenePos();*/
    emit mModel->tableClicked(mModel->tableName(), event->scenePos());
}

int Schema2TableItem::type() const
{
    return Type;
}
