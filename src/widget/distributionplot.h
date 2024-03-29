#ifndef DISTRIBUTIONPLOT_H
#define DISTRIBUTIONPLOT_H

#include <QWidget>
class QAbstractItemModel;
class ModelAppender;


namespace Ui {
class DistributionPlot;
}

#include "distributionplotitem.h"
#include "distributiondataset.h"
#include <QModelIndex>

class DistributionPlot : public QWidget
{
    Q_OBJECT
public:
    explicit DistributionPlot(QWidget *parent = nullptr);
    ~DistributionPlot();

    void setModel(QAbstractItemModel *model);

    QSize minimumSizeHint() const override;

    QAbstractItemModel* tableModel() const;

    void setManualRange(double vmin, double vmax);

    void setHistogramTableColumnPrec(int column, int prec);
protected:
    void init();
    QStringList modelHeader() const;

    ModelAppender* mAppender;
    QList<DistributionPlotItem> mItems;
    QAbstractItemModel* mModel;
    DistributionDataset mDataset;


protected slots:
    void onDataChanged(QModelIndex, QModelIndex, QVector<int>);
    void setDefaultColors();
    void onBinsValueChanged(int);
    void updateSeries();
    void updateDataset();
    void onOptionsChanged(int bins, double min, double max);
private:
    Ui::DistributionPlot *ui;
};


#endif // DISTRIBUTIONPLOT_H
