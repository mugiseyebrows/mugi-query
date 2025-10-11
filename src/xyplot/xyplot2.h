#ifndef XYPLOT2_H
#define XYPLOT2_H

#include <QWidget>
#include <QwtPlot>
class QwtPlotZoomer;
class PlotPicker;
class PlotCurve;

class QAbstractItemModel;

class XYPlot2 : public QwtPlot
{
    Q_OBJECT
public:
    explicit XYPlot2(QWidget *parent = nullptr);

    void setModel(QAbstractItemModel *model);
    QSize minimumSizeHint() const override;
protected:
    QAbstractItemModel* mModel = nullptr;
    QStringList mHeader; // unsorted
    QStringList mColumns; // sorted
    QwtPlotZoomer* mZoomer = nullptr;
    PlotPicker* mPicker = nullptr;

    void setSamples(PlotCurve *curve, bool updateAxes);
    void init();
signals:
protected slots:
    void onLegendClicked(QVariant, int);

    // QWidget interface
public:
    QSize sizeHint() const override;
};

#endif // XYPLOT2_H
