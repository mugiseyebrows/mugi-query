#include "distributionplotitem.h"

DistributionPlotItem::DistributionPlotItem(int column, const QString& color)
    : mColumn(column), mColor(color) {
}

DistributionPlotItem::DistributionPlotItem() {
}

int DistributionPlotItem::column() const {
    return mColumn;
}

void DistributionPlotItem::setColumn(int column) {
    mColumn = column;
}

QString DistributionPlotItem::color() const {
    return mColor;
}

void DistributionPlotItem::setColor(const QString& color) {
    mColor = color;
}

bool DistributionPlotItem::operator==(const DistributionPlotItem& rhs) const {
    return mColumn == rhs.column() && mColor == rhs.color();
}

bool DistributionPlotItem::operator!=(const DistributionPlotItem& rhs) const {
    return !operator==(rhs);
}
