#ifndef XYPLOTMODELITEM_H
#define XYPLOTMODELITEM_H
#include <QString>

class XYPlotModelItem {
public:
    XYPlotModelItem(const QString& x, const QString& y, const QString& line, const QString& marker);
    XYPlotModelItem();
    QString x() const;
    void setX(const QString& x);
    QString y() const;
    void setY(const QString& y);
    QString line() const;
    void setLine(const QString& line);
    QString marker() const;
    void setMarker(const QString& marker);
    bool operator==(const XYPlotModelItem& rhs) const;
    bool operator!=(const XYPlotModelItem& rhs) const;

protected:
    QString mX;
    QString mY;
    QString mLine;
    QString mMarker;
};
#endif // XYPLOTMODELITEM_H
