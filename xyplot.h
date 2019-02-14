#ifndef XYPLOT_H
#define XYPLOT_H

#include <QWidget>

class QAbstractItemModel;
class ModelAppender;
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

protected:
    QStringList modelHeader();
    void init();

    QAbstractItemModel* mModel;
    ModelAppender* mAppender;
    QList<XYPlotModelItem> mItems;
    bool mSplitterAdjusted;


protected slots:
    void onDataChanged(QModelIndex, QModelIndex, QVector<int>);
    void setDefaultColors();
private:
    Ui::XYPlot *ui;
};

#endif // XYPLOT_H
