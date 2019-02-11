#ifndef DISTRIBUTIONPLOTITEM_H
#define DISTRIBUTIONPLOTITEM_H
#include <QString>

class DistributionPlotItem {
public:
    DistributionPlotItem(int column, const QString& color);
    DistributionPlotItem();
    int column() const;
    void setColumn(int column);
    QString color() const;
    void setColor(const QString& color);
    bool operator==(const DistributionPlotItem& rhs) const;
    bool operator!=(const DistributionPlotItem& rhs) const;

protected:
    int mColumn;
    QString mColor;
};
#endif // DISTRIBUTIONPLOTITEM_H
