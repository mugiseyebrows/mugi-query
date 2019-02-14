#ifndef DISTRIBUTIONPLOTITEM_H
#define DISTRIBUTIONPLOTITEM_H
#include <QString>

class DistributionPlotItem {
public:
    DistributionPlotItem(const QString& v, const QString& color);
    DistributionPlotItem();
    QString v() const;
    void setV(const QString& v);
    QString color() const;
    void setColor(const QString& color);
    bool operator==(const DistributionPlotItem& rhs) const;
    bool operator!=(const DistributionPlotItem& rhs) const;

protected:
    QString mV;
    QString mColor;
};
#endif // DISTRIBUTIONPLOTITEM_H
