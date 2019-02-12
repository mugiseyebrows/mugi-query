#include "querymodelview.h"
#include "ui_querymodelview.h"

#include <QAbstractItemModel>
#include "splitterutil.h"

#include "copyeventfilter.h"
#include "itemdelegate.h"
#include <QDebug>
#include <QTimer>
#include "xyplot.h"

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

}

QueryModelView::QueryModelView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QueryModelView)
{
    ui->setupUi(this);

    QTableView* view = ui->table;

    CopyEventFilter* filter = new CopyEventFilter(view);
    filter->setView(view);

    ItemDelegate* delegate = new ItemDelegate(view);
    view->setItemDelegate(delegate);


    QTimer::singleShot(0,[=](){
        on_tabs_currentChanged(0);
        //mTabHeight = //ui->tabs->tab



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

bool QueryModelView::hasCurves() const
{
    return ui->xy->hasCurves();
}

void QueryModelView::on_tabs_currentChanged(int index)
{

    qDebug() << "on_tabs_currentChanged" << this << index;

    QSplitter* splitter = ui->splitter;

    int height = ui->tabs->tabBar()->height();

    //height = 10;

    /*qDebug() << "height" << height;

    qDebug() << ui->tab->minimumSizeHint()
             << ui->tab_2->minimumSizeHint()
             << ui->tab_3->minimumSizeHint();*/

    if (index == 0) {
        SplitterUtil::setFixed(splitter,-1,height);
    } else {
        QList<int> sizes = splitter->sizes();
        if (sizes[1] < 100) {
            SplitterUtil::setRatio(splitter, 1.0, 1.0);
        }
    }

}
