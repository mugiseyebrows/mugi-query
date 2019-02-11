#ifndef XYPLOTMODELITEM_H
#define XYPLOTMODELITEM_H
#include <QString>

class XYPlotModelItem {
public:
    XYPlotModelItem(int x, int y, const QString& line, const QString& marker);
    XYPlotModelItem();
    int x() const;
    void setX(int x);
    int y() const;
    void setY(int y);
    QString line() const;
    void setLine(const QString& line);
    QString marker() const;
    void setMarker(const QString& marker);
    bool operator==(const XYPlotModelItem& rhs) const;
    bool operator!=(const XYPlotModelItem& rhs) const;

protected:
    int mX;
    int mY;
    QString mLine;
    QString mMarker;
};
#endif // XYPLOTMODELITEM_H
