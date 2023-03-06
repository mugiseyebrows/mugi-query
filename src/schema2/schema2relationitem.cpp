#include "schema2relationitem.h"

#include <QPainter>

#include "schema2tableitem.h"

Schema2RelationItem::Schema2RelationItem(Schema2RelationModel *model, Schema2TableItem *childTable, Schema2TableItem *parentTable, QGraphicsItem *parent)
    : mModel(model), mChildTable(childTable), mParentTable(parentTable), QGraphicsItem(parent)
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

static void line_kb(const QLineF& line, double* k, double* b, bool* err) {
    double x1 = line.x1();
    double y1 = line.y1();
    double x2 = line.x2();
    double y2 = line.y2();
    *k = 0;
    *b = 0;
    if (x1 != x2) {
        // k * x1 + b = y1
        // k * x2 + b = y2
        *k = (y1 - y2) / (x1 - x2);
        *b = (y1 - *k * x1);
        *err = false;
    } else {
        *err = true;
    }
}

static bool within(double x, double x1, double x2) {
    return x == x1 || x == x2 || ((x1 < x) == (x < x2));
}

static QPointF intersection(const QLineF& line1, const QLineF& line2, bool* ok) {

    double k1, b1, k2, b2;
    bool err1, err2;
    line_kb(line1, &k1, &b1, &err1);
    line_kb(line2, &k2, &b2, &err2);

    if (err1 && err2) {
        // both lines are vertical
        *ok = false;
        return QPointF();
    }

    if (err2) {
        return intersection(line2, line1, ok);
    }

    if (err1) {
        // line1 is vertical
        double x = line1.x1();
        double y = k2 * x + b2;
        *ok = within(x, line2.x1(), line2.x2())
                && within(y, line1.y1(), line1.y2())
                && within(y, line2.y1(), line2.y2());
        return QPointF(x, y);
    }

    if (err1 || err2) {
        // todo intersection with vertical line
        *ok = false;
        return QPointF();
    }

    if (k1 == k2) {
        *ok = false;
        return QPointF();
    }
    // k1 * x + b1 = y
    // k2 * x + b2 = y
    double x = (b2 - b1) / (k1 - k2);
    double y = k1 * x + b1;

    *ok = within(x, line1.x1(), line1.x2()) && within(x, line2.x1(), line2.x2());
    return QPointF(x, y);
}

static QPointF intersection(const QLineF& line1, const QRectF& rect, bool* ok) {
    QList<QPointF> points = {rect.topLeft(), rect.topRight(), rect.bottomRight(), rect.bottomLeft()};
    for(int i=0;i<points.size();i++) {
        QPointF p1 = points[i];
        QPointF p2 = points[(i + 1) % points.size()];
        bool ok_;
        QPointF p = intersection(line1, QLineF(p1, p2), &ok_);
        if (ok_) {
            *ok = true;
            return p;
        }
    }
    *ok = false;
    return QPointF();
}

#include <math.h>

void drawArrow(QPainter *painter, const QLineF& line, double size, double angle) {

    QPointF p = line.p2() - line.p1();
    double angle0 = atan2(p.y(), p.x());

    double angle1 = angle0 - angle / 2;
    double angle2 = angle0 + angle / 2;

    QPointF p2;




    QPointF p3 = line.p2() + QPointF(-cos(angle1) * size, -sin(angle1) * size);
    //painter->drawLine(line.p2(), p2);
    QPointF p4 = line.p2() + QPointF(-cos(angle2) * size, -sin(angle2) * size);
    //painter->drawLine(line.p2(), p2);

    painter->setBrush(Qt::black);

    painter->drawPolygon(QVector<QPointF> {line.p2(), p3, p4});

    painter->drawLine(line);

}

void Schema2RelationItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF rect = boundingRect();
    //painter->drawRect(rect);

    QRectF rect1 = mParentTable->sceneBoundingRect();
    QRectF rect2 = mChildTable->sceneBoundingRect();

    QPointF c1 = rect1.center();
    QPointF c2 = rect2.center();
    QLineF line(c1, c2);

    bool ok1;
    bool ok2;

    QPointF p1 = intersection(line, rect1, &ok1);
    QPointF p2 = intersection(line, rect2, &ok2);

    if (ok1 && ok2) {
        //painter->drawLine(p1, p2);
        drawArrow(painter, QLineF(p2, p1), 15, M_PI / 5);
    } else {
        //painter->drawLine(c1, c2);
        drawArrow(painter, QLineF(c2, c1), 15, M_PI / 5);
    }

    /*QPointF p1 = mParentTable->sceneBoundingRect().center();
    QPointF p2 = mChildTable->sceneBoundingRect().center();*/


}
