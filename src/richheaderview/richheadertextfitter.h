#ifndef RICHHEADERTEXTFITTER_H
#define RICHHEADERTEXTFITTER_H

#include <QRect>

class RichHeaderTextFitter
{
public:
    static QRect fit(const QRect &rect, int fontHeight, double angle);
    static QRect fitWidth(const QRect &rect, int fontHeight, double angle);
    static QRect fitHeight(const QRect &rect, int fontHeight, double angle);
    static double normalizeAngle(double angle);
    static QRect fitMultiline(const QRect &rect, int fontHeight, double angle, double k);
};

#endif // RICHHEADERTEXTFITTER_H
