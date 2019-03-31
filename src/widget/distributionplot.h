#ifndef DISTRIBUTIONPLOT_H
#define DISTRIBUTIONPLOT_H

#include <QWidget>
class QAbstractItemModel;
class ModelAppender;

namespace Ui {
class DistributionPlot;
}

#include "distributionplotitem.h"
#include <QModelIndex>

class DistributionPlot : public QWidget
{
    Q_OBJECT
public:
    explicit DistributionPlot(QWidget *parent = nullptr);
    ~DistributionPlot();

    void setModel(QAbstractItemModel *model);

    QSize minimumSizeHint() const override;

protected:
    void init();
    ModelAppender* mAppender;
    QList<DistributionPlotItem> mItems;
    QAbstractItemModel* mModel;
    QStringList modelHeader();
protected slots:
    void onDataChanged(QModelIndex, QModelIndex, QVector<int>);
    void setDefaultColors();
    void onBinsValueChanged(int);
    void updateSeries();
private:
    Ui::DistributionPlot *ui;
};


#endif // DISTRIBUTIONPLOT_H
