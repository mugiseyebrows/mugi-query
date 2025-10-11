#ifndef PLOTCURVE_H
#define PLOTCURVE_H

#include <QwtPlotCurve>
#include <QwtLegendData>

class PlotCurve : public QwtPlotCurve
{
public:
    PlotCurve(const QString& title = {}, bool addButton = false);

    bool addButton() const;

    QString x() const;

    QString y() const;

    void setX(const QString& value);

    void setY(const QString& value);

    void updateTitle();

protected:
    bool mAddButton = false;
    QString mX;
    QString mY;

public:
    //QList<QwtLegendData> legendData() const;
    void updateLegendAttributes();
    QwtGraphic legendIcon(int index, const QSizeF &size) const override;
    //QList<QwtLegendData> legendData() const;
protected:
    //virtual QwtGraphic drawLegendIcon() const;
};

#endif // PLOTCURVE_H
