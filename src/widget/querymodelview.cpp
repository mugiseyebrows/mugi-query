#include "querymodelview.h"
#include "ui_querymodelview.h"

#include <QAbstractItemModel>
#include "splitterutil.h"

#include "copyeventfilter.h"
#include "itemdelegate.h"
#include <QDebug>
#include <QTimer>
#include "xyplot.h"
#include <QMenu>
#include "mainwindow.h"
#include <QMessageBox>
#include "clipboard.h"
#include "clipboardutil.h"
#include "hexitemdelegate.h"
#include "binitemdelegate.h"

static void resizeColumnsToContents(QTableView* view, int maxWidth, const QList<int>& skipColumns) {
    int columnCount = view->horizontalHeader()->count();
    QMap<int, int> size;
    for(int c: skipColumns) {
        size[c] = view->columnWidth(c);
    }
    view->resizeColumnsToContents();
    for(int c=0; c<columnCount; c++) {
        if (view->columnWidth(c) > maxWidth) {
            view->setColumnWidth(c, maxWidth);
        }
    }
    for(int c: skipColumns) {
        view->setColumnWidth(c, size[c]);
    }
}

static void resizeColumnsToContents(QTableView* view, int maxWidth) {
    int columnCount = view->model()->columnCount();
    view->resizeColumnsToContents();
    for(int c=0; c<columnCount; c++) {
        if (view->columnWidth(c) > maxWidth) {
            view->setColumnWidth(c, maxWidth);
        }
    }
}

static void resizeRowsToContents(QTableView* view, int maxHeight) {
    int rowCount = view->model()->rowCount();
    view->resizeRowsToContents();
    for(int r=0; r<rowCount; r++) {
        if (view->rowHeight(r) > maxHeight) {
            view->setRowHeight(r, maxHeight);
        }
    }
}

static void setColumnsWidth(QTableView* view, int width, const QList<int>& skipColumns) {
    int columnCount = view->model()->columnCount();
    for(int c=0; c<columnCount; c++) {
        if (skipColumns.contains(c)) {
            continue;
        }
        view->setColumnWidth(c, width);
    }
}

static void setColumnsWidth(QTableView* view, int width) {
    QAbstractItemModel* model = view->model();
    for(int c=0; c<model->columnCount(); c++) {
        view->setColumnWidth(c, width);
    }
}

static void setRowsHeight(QTableView* view, int height) {
    QAbstractItemModel* model = view->model();
    for(int r=0; r<model->rowCount(); r++) {
        view->setRowHeight(r, height);
    }
}


static QWidget* topWidget(QWidget* w) {
    while (true) {
        if (QWidget* p = qobject_cast<QWidget*>(w->parent())) {
            w = p;
        } else {
            return w;
        }
    }
}


QItemSelectionModel* QueryModelView::selectionModel() const {
    return ui->table->selectionModel();
}

QueryModelView::QueryModelView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QueryModelView),
    mSplitterUpdated(false)
{
    ui->setupUi(this);

    QTableView* view = ui->table;

    CopyEventFilter* filter = new CopyEventFilter(view);
    filter->setView(view);
    connect(filter,SIGNAL(copy()),this,SLOT(onCopy()));

    mItemDelegate = new ItemDelegate(view);
    view->setItemDelegate(mItemDelegate);

    mHexItemDelegate = new HexItemDelegate(view);

    mBinItemDelegate = new BinItemDelegate(view);

    connect(ui->table,SIGNAL(customContextMenuRequested(QPoint)),
            this,SLOT(onTableCustomContextMenuRequested(QPoint)));

    QTimer::singleShot(0,[=](){
        on_tabs_currentChanged(0);
    });
}

QueryModelView::~QueryModelView()
{
    qDebug() << "~QueryModelView";
    delete ui;
}

template <typename T>
static QList<T> toList(const QSet<T>& qlist)
{
    return QList<T> (qlist.constBegin(), qlist.constEnd());
}

static QList<int> selectedColumns(QItemSelectionModel* selectionModel) {
    auto indexes = selectionModel->selectedIndexes();
    QSet<int> columns;
    for(auto index: indexes) {
        columns.insert(index.column());
    }
    return toList(columns);
}

void QueryModelView::setItemDelegateForColumns(const QList<int> columns, QAbstractItemDelegate* delegate) {
    QTableView* view = ui->table;
    for(int column: columns) {
        view->setItemDelegateForColumn(column, delegate);
    }
}

void QueryModelView::viewAsHex()
{
    setItemDelegateForColumns(selectedColumns(selectionModel()), mHexItemDelegate);
}

void QueryModelView::viewAsBin()
{
    setItemDelegateForColumns(selectedColumns(selectionModel()), mBinItemDelegate);
}

void QueryModelView::viewAsString()
{
    setItemDelegateForColumns(selectedColumns(selectionModel()), mItemDelegate);
}

XYPlot* QueryModelView::xyPlot() const {
    return ui->xy;
}

DistributionPlot* QueryModelView::distributionPlot() const {
    return ui->distribution;
}

void QueryModelView::setColumnsWidth(Size size)
{
    mColumnsWidth = size;
    updateColumnsWidth();
}

void QueryModelView::setRowsHeight(Size size)
{
    mRowsHeight = size;
    updateRowsHeight();
}

void QueryModelView::updateRowsHeight() {
    int height = -1;
    switch(mRowsHeight) {
    case SizeFit:
        break;
    case SizeS:
        height = 30;
        break;
    case SizeM:
        height = 60;
        break;
    case SizeL:
        height = 120;
        break;
    }

    if (height > 0) {
        ::setRowsHeight(ui->table, height);
    } else {
        resizeRowsToContents(ui->table, this->height() / 2);
    }
}

void QueryModelView::updateColumnsWidth() {
    int width = -1;
    switch(mColumnsWidth) {
    case SizeFit:
        break;
    case SizeS:
        width = 50;
        break;
    case SizeM:
        width = 100;
        break;
    case SizeL:
        width = 200;
        break;
    }
    if (width > 0) {
        ::setColumnsWidth(ui->table, width, delegatedColumns());
    } else {
        resizeColumnsToContents(ui->table, this->width() / 2, delegatedColumns());
    }
}

void QueryModelView::setModel(QAbstractItemModel *model)
{
    ui->table->setModel(model);

    updateColumnsWidth();
    updateRowsHeight();

    ui->xy->setModel(model);
    ui->distribution->setModel(model);
}

#if 0
QList<int> QueryModelView::normalColumns() const
{
    QList<int> res;
    int columnCount = ui->table->model()->columnCount();
    for(int column=0;column<columnCount;column++) {
        auto* columnDelegate = ui->table->itemDelegateForColumn(column);
        if (columnDelegate == nullptr || columnDelegate == mItemDelegate) {
            res.append(column);
        }
    }
    return res;
}
#endif

QList<int> QueryModelView::delegatedColumns() const
{
    QList<int> res;
    int columnCount = ui->table->model()->columnCount();
    for(int column=0;column<columnCount;column++) {
        auto* columnDelegate = ui->table->itemDelegateForColumn(column);
        if (!(columnDelegate == nullptr || columnDelegate == mItemDelegate)) {
            res.append(column);
        }
    }
    return res;
}

QAbstractItemModel *QueryModelView::model() const
{
    return ui->table->model();
}

void QueryModelView::on_tabs_currentChanged(int index)
{

    //qDebug() << "on_tabs_currentChanged" << this << index;

    QSplitter* splitter = ui->splitter;

    if (index == 0) {
        int height = ui->tabs->tabBar()->height();
        if (height > 0) {
            SplitterUtil::setFixed(splitter,-1,height);
        }
    } else {
        QList<int> sizes = splitter->sizes();
        //qDebug() << sizes;
        if (sizes[1] < 100) {
            SplitterUtil::setRatioWithMinSize(splitter, 1.0, 1.0, -1, 260);
        }
    }
}

void QueryModelView::updateSplitter()
{
    if (mSplitterUpdated) {
        return;
    }
    on_tabs_currentChanged(ui->tabs->currentIndex());
    mSplitterUpdated = true;
}

void QueryModelView::showXYPlot()
{
    ui->tabs->setCurrentIndex(TabIndexXY);
}

void QueryModelView::showDistributionPlot()
{
    ui->tabs->setCurrentIndex(TabIndexDistribution);
}

void QueryModelView::onTableCustomContextMenuRequested(const QPoint &)
{
    QWidget* widget = topWidget(this);

    MainWindow* mainWindow = qobject_cast<MainWindow*>(widget);
    if (!mainWindow) {
        qDebug() << "error" << __FILE__ << __LINE__;
        return;
    }

    QMenu* menu = mainWindow->selectionMenu();
    menu->exec(QCursor::pos());
}



void QueryModelView::onCopy() {
    ClipboardUtil::copyTsv(ui->table);
}
