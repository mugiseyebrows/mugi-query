#include "distributionplotitem.h"

DistributionPlotItem::DistributionPlotItem(const QString& v, const QString& color)
    : mV(v), mColor(color) {
}

DistributionPlotItem::DistributionPlotItem() {
}

QString DistributionPlotItem::v() const {
    return mV;
}

void DistributionPlotItem::setV(const QString& v) {
    mV = v;
}

QString DistributionPlotItem::color() const {
    return mColor;
}

void DistributionPlotItem::setColor(const QString& color) {
    mColor = color;
}

bool DistributionPlotItem::operator==(const DistributionPlotItem& rhs) const {
    return mV == rhs.v() && mColor == rhs.color();
}

bool DistributionPlotItem::operator!=(const DistributionPlotItem& rhs) const {
    return !operator==(rhs);
}
