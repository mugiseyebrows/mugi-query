#include "richheadertextfitter.h"

#include <math.h>
#include <QList>
#include <QDebug>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace {

double degToRad(double angle) {
    return angle / 180.0 * M_PI;
}

bool equals(double a, double b) {
    return fabs(a-b) < 0.00001;
}

}

QRect RichHeaderTextFitter::fitMultiline(const QRect &rect, int fontHeight, double angle, double k) {
    int n = qMin(rect.width(),rect.height()) / fontHeight;
    QList<int> widths;
    widths << 0;
    for(int i=1;i<=n;i++) {
        widths << fit(rect, fontHeight * i, angle).width();
    }
    QList<double> areas;
    for(int i=0;i<=n;i++) {
        areas << widths[i] * (double) i * k * (double) fontHeight;
    }

    int index = std::max_element(areas.begin(), areas.end()) - areas.begin();

    //qDebug() << areas << index;

    return QRect(QPoint(0,0),QSize(widths[index],index * fontHeight));
}

QRect RichHeaderTextFitter::fit(const QRect &rect, int fontHeight, double angle)
{
    angle = normalizeAngle(angle);
    if (equals(sin(degToRad(angle)),0.0)) {
        return fitWidth(rect,fontHeight,angle);
    } else if (equals(cos(degToRad(angle)),0.0)) {
        return fitHeight(rect,fontHeight,angle);
    }
    QRect r1 = fitWidth(rect,fontHeight,angle);
    QRect r2 = fitHeight(rect,fontHeight,angle);
    return r1.width() < r2.width() ? r1 : r2;
}

QRect RichHeaderTextFitter::fitWidth(const QRect &rect, int fontHeight, double angle)
{
    double width = 2.0 * ((double) rect.width() / 2.0 / cos(degToRad(angle)) - (double) fontHeight / 2.0 * tan(degToRad(angle)));
    return QRect(QPoint(0,0), QSize(width, fontHeight));
}

QRect RichHeaderTextFitter::fitHeight(const QRect &rect, int fontHeight, double angle)
{
    double width = 2.0 * ((double) rect.height() / 2.0 - (double) fontHeight / 2.0 * cos(degToRad(angle))) / sin(degToRad(angle));
    return QRect(QPoint(0,0), QSize(width, fontHeight));
}

double RichHeaderTextFitter::normalizeAngle(double angle)
{
    while (angle > 180.0) {
        angle -= 180.0;
    }
    if (angle > 90.0) {
        angle = 180.0 - angle;
    }
    return angle;
}
