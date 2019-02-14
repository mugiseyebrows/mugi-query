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

namespace {

void resizeColumnsToContents(QTableView* view, int maxWidth) {
    QAbstractItemModel* model = view->model();
    if (!model) {
        return;
    }
    view->resizeColumnsToContents();
    for(int c=0; c<model->columnCount(); c++) {
        if (view->columnWidth(c) > maxWidth)
            view->setColumnWidth(c,maxWidth);
    }
}

QWidget* topWidget(QWidget* w) {
    while (true) {
        if (QWidget* p = qobject_cast<QWidget*>(w->parent())) {
            w = p;
        } else {
            return w;
        }
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

    ItemDelegate* delegate = new ItemDelegate(view);
    view->setItemDelegate(delegate);

    connect(ui->table,SIGNAL(customContextMenuRequested(QPoint)),
            this,SLOT(onTableCustomContextMenuRequested(QPoint)));

    QTimer::singleShot(0,[=](){
        on_tabs_currentChanged(0);
    });
}

QueryModelView::~QueryModelView()
{
    delete ui;
}

void QueryModelView::setModel(QAbstractItemModel *model)
{
    ui->table->setModel(model);
    resizeColumnsToContents(ui->table,this->width()/2);
    ui->xy->setModel(model);
    ui->distribution->setModel(model);
}

QAbstractItemModel *QueryModelView::model() const
{
    return ui->table->model();
}

void QueryModelView::on_tabs_currentChanged(int index)
{

    //qDebug() << "on_tabs_currentChanged" << this << index;

    QSplitter* splitter = ui->splitter;

    //height = 10;

    /*qDebug() << "height" << height;

    qDebug() << ui->tab->minimumSizeHint()
             << ui->tab_2->minimumSizeHint()
             << ui->tab_3->minimumSizeHint();*/

    if (index == 0) {
        int height = ui->tabs->tabBar()->height();
        //qDebug() << height;
        if (height > 0) {
            SplitterUtil::setFixed(splitter,-1,height);
        }
    } else {
        QList<int> sizes = splitter->sizes();
        //qDebug() << sizes;
        if (sizes[1] < 100) {
            SplitterUtil::setRatio(splitter, 1.0, 1.0);
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
