#ifndef QUERYMODELVIEW_H
#define QUERYMODELVIEW_H

#include <QWidget>

class QAbstractItemModel;
class QItemSelectionModel;
class DistributionPlot;
class XYPlot2;
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
        SizeL,
        SizeXL,
    };

    explicit QueryModelView(QWidget *parent = nullptr);
    ~QueryModelView();

    void viewAsHex();

    void viewAsBin();

    void viewAsString();

    void setModel(QAbstractItemModel* model);

#if 0
    QList<int> normalColumns() const;
#endif

    QList<int> delegatedColumns() const;

    QAbstractItemModel* model() const;

    QItemSelectionModel *selectionModel() const;
    XYPlot2 *xyPlot() const;
    DistributionPlot *distributionPlot() const;

    void setColumnsWidth(Size size);
    void setRowsHeight(Size size);

signals:
    void copySelected();

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
