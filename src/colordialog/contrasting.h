#ifndef CONTRASTING_H
#define CONTRASTING_H

#include <QColor>

double luminance(const QColor& color);
double contrast(const QColor& c1, const QColor& c2);
QColor contrasting(const QColor& background, const QColor& text1, const QColor& text2);

#endif // CONTRASTING_H
