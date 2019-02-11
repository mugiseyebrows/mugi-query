#ifndef DATAPLOT_H
#define DATAPLOT_H

#include <QWidget>

#include "tokens.h"
#include <QModelIndex>
#include "distributionplotmodel.h"
#include "xyplotmodelitem.h"

class ModelAppender;
class QSqlQueryModel;
class QwtPlotMultiBarChart;

namespace Ui {
class DataPlot;
}

class DataPlot : public QWidget
{
    Q_OBJECT

public:


    explicit DataPlot(QWidget *parent = nullptr);
    ~DataPlot();

    void setModel(QSqlQueryModel* model);

protected:
    void updateDistribution();
    QStringList modelHeader();

    Ui::DataPlot *ui;
    QSqlQueryModel* mModel;

    ModelAppender* mAppenderXYPlot;
    ModelAppender* mAppenderDistribution;

    QList<DistributionPlotItem> mDistribution;
    QList<XYPlotModelItem> mXy;

    void setDistributionPlotOrientation(QwtPlotMultiBarChart *d_barChartItem, int orientation);
    void initDistribution();
    void initXY();
protected slots:
    void onXYModelDataChanged(QModelIndex, QModelIndex, QVector<int>);
    void setDefaultColorsXYPlot();
    void setDefaultColorsDistributionPlot();
    void onBinsValueChanged(int bins);
    void onDistributionCurrentIndexChanged(QString);
    void onDistributionPlotModelChanged(QModelIndex, QModelIndex, QVector<int>);
};

#endif // DATAPLOT_H
