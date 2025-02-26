#ifndef QUERYMODELVIEW_H
#define QUERYMODELVIEW_H

#include <QWidget>

class QAbstractItemModel;
class QItemSelectionModel;
class DistributionPlot;
class XYPlot;
class HexItemDelegate;
class ItemDelegate;
class QAbstractItemDelegate;
class BinItemDelegate;

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
    enum Size {
        SizeFit,
        SizeS,
        SizeM,
        SizeL
    };

    explicit QueryModelView(QWidget *parent = nullptr);
    ~QueryModelView();

    void viewAsHex();

    void viewAsBin();

    void viewAsString();

    void setModel(QAbstractItemModel* model);

    QAbstractItemModel* model() const;

    QItemSelectionModel *selectionModel() const;
    XYPlot *xyPlot() const;
    DistributionPlot *distributionPlot() const;

    void setColumnsSize(Size size);
    void setRowsHeight(Size size);

public slots:
    void updateSplitter();
    void showDistributionPlot();
    void showXYPlot();

protected slots:

    void on_tabs_currentChanged(int index);
    void onTableCustomContextMenuRequested(const QPoint &pos);

protected:
    void updateRowsHeight();
    void updateColumnsWidth();

private slots:
    void onCopy();
protected:
    Ui::QueryModelView *ui;
    int mTabHeight;
    bool mSplitterUpdated;
    ItemDelegate* mItemDelegate;
    HexItemDelegate* mHexItemDelegate;
    BinItemDelegate* mBinItemDelegate;

    Size mColumnsWidth = SizeFit;
    Size mRowsHeight = SizeS;

    void setItemDelegateForColumns(const QList<int> columns, QAbstractItemDelegate *delegate);
};

#endif // QUERYMODELVIEW_H
