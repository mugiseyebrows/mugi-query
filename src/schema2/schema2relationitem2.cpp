#include "schema2relationitem2.h"

#include "schema2tableitem.h"

#include <math.h>
#include <QPainter>

Schema2RelationItem2::Schema2RelationItem2(Schema2TableItem *childTable,
                                           Schema2TableItem *parentTable,
                                           QGraphicsItem *parent)
    : mChildTable(childTable), mParentTable(parentTable), QGraphicsItem(parent)
{


}


QRectF Schema2RelationItem2::boundingRect() const
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




static QPointF intersection(const QLineF& line1, const QRectF& rect, bool* ok) {
    QList<QPointF> points = {rect.topLeft(), rect.topRight(), rect.bottomRight(), rect.bottomLeft()};
    for(int i=0;i<points.size();i++) {
        QPointF p1 = points[i];
        QPointF p2 = points[(i + 1) % points.size()];
        QPointF p;
        if (line1.intersects(QLineF(p1, p2), &p) == QLineF::BoundedIntersection) {
            *ok = true;
            return p;
        }
    }
    *ok = false;
    return QPointF();
}

static void drawArrow(QPainter *painter, const QLineF& line, double size, double angle) {

    QPointF p = line.p2() - line.p1();
    double angle0 = atan2(p.y(), p.x());

    double angle1 = angle0 - angle / 2;
    double angle2 = angle0 + angle / 2;

    QPointF p2;

    QPointF p3 = line.p2() + QPointF(-cos(angle1) * size, -sin(angle1) * size);
    //painter->drawLine(line.p2(), p2);
    QPointF p4 = line.p2() + QPointF(-cos(angle2) * size, -sin(angle2) * size);
    //painter->drawLine(line.p2(), p2);

    //painter->setBrush(Qt::black);

    painter->drawPolygon(QVector<QPointF> {line.p2(), p3, p4});

    painter->drawLine(line);

}

void Schema2RelationItem2::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                                 QWidget *widget)
{

    //QRectF rect = boundingRect();
    //painter->drawRect(rect);

    QRectF rect1 = mParentTable->sceneBoundingRect();
    QRectF rect2 = mChildTable->sceneBoundingRect();

    QPointF c1 = rect1.center();
    QPointF c2 = rect2.center();
    QLineF line(c1, c2);

    bool ok1;
    bool ok2;

    bool grayed = mParentTable->grayed() || mChildTable->grayed();

    painter->setPen(grayed ? QColor("#888888") : QColor("#000000"));
    painter->setBrush(grayed ? QColor("#888888") : QColor("#000000"));

    QPointF p1 = intersection(line, rect1, &ok1);
    QPointF p2 = intersection(line, rect2, &ok2);

    if (ok1 && ok2) {
        //painter->drawLine(p1, p2);
        drawArrow(painter, QLineF(p2, p1), 15, M_PI / 5);
    } else {
        //painter->drawLine(c1, c2);
        drawArrow(painter, QLineF(c2, c1), 15, M_PI / 5);
    }

}
