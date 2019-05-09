#ifndef QUERYMODELVIEW_H
#define QUERYMODELVIEW_H

#include <QWidget>

class QAbstractItemModel;
class QItemSelectionModel;
class DistributionPlot;
class XYPlot;

namespace Ui {
class QueryModelView;
}

class QueryModelView : public QWidget
{
    Q_OBJECT

public:
    enum TabIndex {
        TabIndexHide,
        TabIndexXY,
        TabIndexDistribution
    };

    explicit QueryModelView(QWidget *parent = nullptr);
    ~QueryModelView();

    void setModel(QAbstractItemModel* model);

    QAbstractItemModel* model() const;

    QItemSelectionModel *selectionModel() const;
    XYPlot *xyPlot() const;
    DistributionPlot *distributionPlot() const;
public slots:
    void updateSplitter();
    void showDistributionPlot();
    void showXYPlot();

protected slots:

    void on_tabs_currentChanged(int index);
    void onTableCustomContextMenuRequested(const QPoint &pos);

private slots:
    void onCopy();
private:
    Ui::QueryModelView *ui;
    int mTabHeight;
    bool mSplitterUpdated;
};

#endif // QUERYMODELVIEW_H
