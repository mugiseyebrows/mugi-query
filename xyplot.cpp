#include "xyplot.h"
#include "ui_xyplot.h"

#include "deleteeventfilter.h"
#include "itemdelegatewithcompleter.h"
#include "xyplotmodel.h"
#include "colorpalette.h"
#include "lit.h"
#include "setdefaultcolors.h"
#include "xyplotmodel.h"
#include "modelappender.h"
#include <qwt_plot_curve.h>
#include "filterplotitem.h"
#include "zipunzip.h"
#include "datautils.h"
#include <qwt_symbol.h>
#include <QDebug>
#include "splitterutil.h"
#include <QTimer>
using namespace DataUtils;
using namespace Lit;

XYPlot::XYPlot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::XYPlot),
    mAppender(new ModelAppender(this)),
    mSplitterAdjusted(false)
{
    ui->setupUi(this);
    init();
}

XYPlot::~XYPlot()
{
    delete ui;
}

void XYPlot::init() {
    (new DeleteEventFilter(this))->setView(ui->table);

    ui->plot->setAxisAutoScale(QwtPlot::xBottom);
    ui->plot->setAxisAutoScale(QwtPlot::yLeft);

    QStandardItemModel* model = new XYPlotModel(this);
    ui->table->setModel(model);

    mAppender->setModel(model);

    connect(model,SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),this,SLOT(onDataChanged(QModelIndex,QModelIndex,QVector<int>)));

    connect(mAppender,SIGNAL(rowInserted(int)),this,SLOT(setDefaultColors()));

    ItemDelegateWithCompleter* colorCompleter = new ItemDelegateWithCompleter(ColorPalette::instance()->names());

    ui->table->setItemDelegateForColumn(XYPlotModel::col_line,colorCompleter);
    ui->table->setItemDelegateForColumn(XYPlotModel::col_marker,colorCompleter);
}


QStringList XYPlot::modelHeader() {
    return toLower(headerData(mModel,Qt::Horizontal));
}


void XYPlot::setModel(QAbstractItemModel *model)
{
    mModel = model;

    ItemDelegateWithCompleter* xyCompleter = new ItemDelegateWithCompleter(modelHeader());

    ui->table->setItemDelegateForColumn(XYPlotModel::col_x,xyCompleter);
    ui->table->setItemDelegateForColumn(XYPlotModel::col_y,xyCompleter);

    setDefaultColors();

    /*if (!mSplitterAdjusted) {
        QTimer::singleShot(0,[=](){
            int width = ui->table->horizontalHeader()->defaultSectionSize() * 4 + 10;
            SplitterUtil::setFixed(ui->splitter,width,-1);
        });
    }*/

    mSplitterAdjusted = true;

    mItems.clear();
    onDataChanged(QModelIndex(),QModelIndex(),QVector<int>());
}


QSize XYPlot::minimumSizeHint() const
{
    return QSize();
}

void XYPlot::setDefaultColors() {
    ::setDefaultColors(mAppender,
                     ui->table->model(),
                     il(XYPlotModel::col_x, XYPlotModel::col_y),
                     il(XYPlotModel::col_line),
                     il(XYPlotModel::col_marker));
}

void XYPlot::onDataChanged(QModelIndex,QModelIndex,QVector<int>) {

    XYPlotModel* model = qobject_cast<XYPlotModel*>(ui->table->model());

    QStringList header = modelHeader();

    QList<XYPlotModelItem> items = model->items(header);

    if (items == mItems) {
        return;
    }

    mItems = items;

    QList<QwtPlotCurve*> curves = filterCurves(ui->plot);
    int attached = curves.size();
    while(curves.size() < items.size()) {
        curves.append(new QwtPlotCurve());
    }

    if (!items.isEmpty()) {
        while (mModel->canFetchMore(QModelIndex())) {
            mModel->fetchMore(QModelIndex());
        }
    }

    qDebug() << "rowCount" << mModel->rowCount();

    for(int i=0;i<items.size();i++) {

        const XYPlotModelItem& item = items[i];

        int x = item.x();
        int y = item.y();
        Qt::GlobalColor line = ColorPalette::instance()->toGlobalColor(item.line());
        Qt::GlobalColor marker = ColorPalette::instance()->toGlobalColor(item.marker());

        QPolygonF polygon = toPolygon(filterNumeric(zipToPairList(columnData(mModel,x),columnData(mModel,y))));
        curves[i]->setSamples(polygon);

        curves[i]->setStyle(line == Qt::transparent ? QwtPlotCurve::NoCurve : QwtPlotCurve::Lines);
        curves[i]->setPen(line, 2);
        QwtSymbol *symbol = 0;
        if (marker != Qt::transparent) {
            symbol = new QwtSymbol(QwtSymbol::Ellipse, QBrush(marker), QPen(Qt::NoPen), QSize(6, 6));
        }
        curves[i]->setSymbol(symbol);
    }
    for(int i=attached;i<curves.size();i++) {
        curves[i]->attach(ui->plot);
        curves[i]->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    }

    for(int i=items.size();i<curves.size();i++) {
        curves[i]->detach();
        delete curves[i];
    }

    ui->plot->replot();

}
