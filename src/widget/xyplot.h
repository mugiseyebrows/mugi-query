#ifndef XYPLOT_H
#define XYPLOT_H

#include <QWidget>

class QAbstractItemModel;
class ModelAppender;
class QwtPlotZoomer;
class PlotPicker;
#include "xyplotmodelitem.h"
#include <QModelIndex>

namespace Ui {
class XYPlot;
}

class XYPlot : public QWidget
{
    Q_OBJECT
public:
    explicit XYPlot(QWidget *parent = nullptr);
    ~XYPlot();
    void setModel(QAbstractItemModel* model);

    QSize minimumSizeHint() const override;

    QAbstractItemModel *tableModel() const;
protected:
    QStringList modelHeader();
    void init();

    QAbstractItemModel* mModel;
    ModelAppender* mAppender;
    QList<XYPlotModelItem> mItems;
    bool mSplitterAdjusted;
    QwtPlotZoomer* mZoomer;
    PlotPicker* mPicker;

protected slots:
    void onDataChanged(QModelIndex, QModelIndex, QVector<int>);
    void setDefaultColors();
private:
    Ui::XYPlot *ui;
};

#endif // XYPLOT_H
