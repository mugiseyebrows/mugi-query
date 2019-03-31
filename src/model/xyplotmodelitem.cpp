#include "xyplotmodelitem.h"

XYPlotModelItem::XYPlotModelItem(const QString& x, const QString& y, const QString& line,
                                 const QString& marker)
    : mX(x), mY(y), mLine(line), mMarker(marker) {
}

XYPlotModelItem::XYPlotModelItem() {
}

QString XYPlotModelItem::x() const {
    return mX;
}

void XYPlotModelItem::setX(const QString& x) {
    mX = x;
}

QString XYPlotModelItem::y() const {
    return mY;
}

void XYPlotModelItem::setY(const QString& y) {
    mY = y;
}

QString XYPlotModelItem::line() const {
    return mLine;
}

void XYPlotModelItem::setLine(const QString& line) {
    mLine = line;
}

QString XYPlotModelItem::marker() const {
    return mMarker;
}

void XYPlotModelItem::setMarker(const QString& marker) {
    mMarker = marker;
}

bool XYPlotModelItem::operator==(const XYPlotModelItem& rhs) const {
    return mX == rhs.x() && mY == rhs.y() && mLine == rhs.line() && mMarker == rhs.marker();
}

bool XYPlotModelItem::operator!=(const XYPlotModelItem& rhs) const {
    return !operator==(rhs);
}
