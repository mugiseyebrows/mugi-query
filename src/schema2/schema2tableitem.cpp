#include "schema2tableitem.h"

#include "schema2tablemodel.h"
#include "schema2relationitem2.h"
#include <QPainter>
#include <QDebug>
#include "style.h"
#include "schema2indexesmodel.h"

#define LINE_HEIGHT 25

Schema2TableItem::Schema2TableItem(Schema2TableModel *model, QGraphicsItem *parent)
    : mModel(model), QGraphicsItem(parent), mChecked(true)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    //setFlag(QGraphicsItem::ItemIsSelectable, true);
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
    int s = LINE_HEIGHT;
    return QRectF(0, 0, w, s * (mModel->rowCount() + 1));
}

static void drawRect(QPainter *painter, const QRectF& boundary, bool isSelected) {

    double HoveredPenWidth = 1.5;
    double PenWidth = 1.0;

    auto color = isSelected ? Style::current.SelectedBoundaryColor : Style::current.NormalBoundaryColor;

    bool isHovered = false;

    if (isHovered) {
        QPen p(color, HoveredPenWidth);
        painter->setPen(p);
    } else {
        QPen p(color, PenWidth);
        painter->setPen(p);
    }

    QSizeF size = boundary.size();

    QLinearGradient gradient(QPointF(0.0, 0.0), QPointF(0.0, size.height()));

    gradient.setColorAt(0.0, Style::current.GradientColor0);
    gradient.setColorAt(0.10, Style::current.GradientColor1);
    gradient.setColorAt(0.90, Style::current.GradientColor2);
    gradient.setColorAt(1.0, Style::current.GradientColor3);

    painter->setBrush(gradient);

    double const radius = 4.0;

    painter->drawRoundedRect(boundary, radius, radius);
}


/* 1 2
 * 4 3
 */
static QPainterPath borderRadius(const QRectF& rect, double tl, double tr, double br, double bl) {

    QPointF topLeft = rect.topLeft();
    QPointF bottomRight = rect.bottomRight();

    double x1 = topLeft.x();
    double x2 = bottomRight.x();
    double y1 = topLeft.y();
    double y2 = bottomRight.y();
    double r;

    auto rectAround = [=](double x, double y, double r){
        QRectF rect(QPointF(0,0),QSizeF(r*2,r*2));
        rect.moveCenter(QPointF(x, y));
        return rect;
    };

    QPainterPath path;
    double sweep = 90;

    // arc3
    r = br;
    if (r > 0) {
        path.moveTo(x2 - r, y2);
        path.arcTo(rectAround(x2-r, y2-r, r), -90, sweep);
    } else {
        path.moveTo(x2, y2);
    }
    // arc2
    r = tr;
    if (r > 0) {
        path.lineTo(x2, y1+r);
        path.arcTo(rectAround(x2-r, y1+r, r), 0, sweep);
    } else {
        path.lineTo(x2, y1);
    }
    // arc1
    r = tl;
    if (r > 0) {
        path.lineTo(x1+r, y1);
        path.arcTo(rectAround(x1+r, y1+r, r), 90, sweep);
    } else {
        path.lineTo(x1, y1);
    }
    // arc4
    r = bl;
    if (r > 0) {
        path.lineTo(x1, y2-r);
        path.arcTo(rectAround(x1+r, y2-r, r), 180, sweep);
    } else {
        path.lineTo(x1, y2);
    }
    // close path
    r = br;
    if (r > 0) {
        path.lineTo(x2-r, y2);
    } else {
        path.lineTo(x2, y2);
    }

    return path;
}

void Schema2TableItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    int w = 200;
    int s = LINE_HEIGHT;

    bool unchecked = !mChecked;

    if (unchecked && mMode == UncheckedInvisible) {
        return;
    }

    painter->setFont(QFont("Liberation Sans", 11));



    {
        QRectF rect = boundingRect();
        //rect.moveTo(0,0);

        painter->save();

        //QBrush bodyBrush = unchecked ? QColor("#ffffff") : QColor("#FFFBAC");

        //painter->setBrush(bodyBrush);
        //painter->drawRect(rect);

        if (mChecked) {
            drawRect(painter, rect, isSelected());
        } else {
            double const radius = 4.0;
            painter->drawRoundedRect(rect, radius, radius);
        }

    }

    double radius = 4.0;
    QRectF titleRect = QRectF(0,0,w,s);
    QBrush titleBrush = mChecked ? Style::current.TitleColor : QColor("#FFD495");

    painter->setPen(Qt::NoPen);
    if (mChecked) {
        painter->setBrush(titleBrush);
        auto path = borderRadius(titleRect, radius, radius, 0, 0);
        painter->drawPath(path);
        //painter->drawRoundedRect(titleRect, radius, radius);
    }

    painter->restore();

    /*{
        QRectF rect(0,0,s,s);
        painter->drawRect(rect);
    }*/

    QRectF rect(0,0,w,s);


    painter->setPen(mChecked ? Style::current.TextColorTitle : Style::current.TextColorFaded);

    QTextOption opt(Qt::AlignCenter);
    painter->drawText(rect, mModel->tableName(), opt);

    painter->setPen(mChecked ? Style::current.TextColor : Style::current.TextColorFaded);

    auto primaryKey = mModel->indexes()->primaryKey();
    auto relationsChildColumns = mModel->relationsChildColumns();

    for(int row=0;row<mModel->rowCount();row++) {
        QRectF rect(0,(row + 1)*s,w,s);
        QString name = mModel->name(row);

        QTextOption opt(Qt::AlignCenter);
        painter->drawText(rect, name, opt);

        if (mChecked) {
            if (primaryKey.contains(name)) {
                QTextOption opt(Qt::AlignLeft);
                QRectF rect(s, (row + 1) * s, w, s);
                painter->drawText(rect, "🔑", opt);
            }

            if (relationsChildColumns.contains(name)) {
                QTextOption opt(Qt::AlignRight);
                QRectF rect(0, (row + 1) * s, w - 20, s);
                painter->drawText(rect, "👉", opt);
            }
        }

        /*if (mModel->isIndexColumn(name)) {
            QTextOption opt(Qt::AlignRight | Qt::AlignVCenter);
            rect.setWidth(rect.width() - 10);
            painter->drawText(rect, "I", opt);
        }*/

    }


    //painter->setPen(Qt::white);

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

void Schema2TableItem::setChecked(bool value)
{
    if (mChecked == value) {
        return;
    }
    mChecked = value;
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
