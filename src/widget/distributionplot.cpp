#include "distributionplot.h"
#include "ui_distributionplot.h"

#include <QAbstractItemModel>
#include "modelappender.h"
#include <qwt_plot_multi_barchart.h>
#include <qwt_plot_legenditem.h>
#include "distributionplotmodel.h"
#include "deleteeventfilter.h"
#include "itemdelegatewithcompleter.h"
#include "setdefaultcolors.h"
#include "modelappender.h"
#include "datautils.h"
#include "colorpalette.h"
#include "filterplotitem.h"
#include <qwt_column_symbol.h>

#include "histogram.h"
#include "distributiondataset.h"
#include "model/distributionhistogrammodel.h"
#include "doubleitemdelegate.h"
#include "copyeventfilter.h"
#include "clipboardutil.h"
#include "qwt_compat.h"
#include "tolower.h"

using namespace DataUtils;

DistributionPlot::DistributionPlot(QWidget *parent) :
    QWidget(parent),
    mAppender(new ModelAppender(this)),
    ui(new Ui::DistributionPlot)
{
    ui->setupUi(this);    
    init();
}

DistributionPlot::~DistributionPlot()
{
    delete ui;
}

QSize DistributionPlot::minimumSizeHint() const
{
    return QSize();
}

QAbstractItemModel *DistributionPlot::tableModel() const
{
    return ui->table->model();
}

void DistributionPlot::setManualRange(double vmin, double vmax)
{
    ui->options->setManualRange(vmin, vmax);
}

void DistributionPlot::init() {
    QwtPlotMultiBarChart* chart = new QwtPlotMultiBarChart();
    chart->setLayoutPolicy( QwtPlotMultiBarChart::AutoAdjustSamples );
    chart->setSpacing(3);
    chart->setMargin(3);
    chart->setLegendIconSize(QSize(10, 14));
    chart->attach(ui->plot);

    ui->plot->setAutoReplot( true );

    //ui->distributionPlot->insertLegend(new QwtLegend());

    QwtPlotLegendItem* legend = new QwtPlotLegendItem();

#if QWT_VERSION >= 0x060200
    legend->setAlignmentInCanvas(Qt::AlignTop | Qt::AlignLeft);
#else
    legend->setAlignment(Qt::AlignTop | Qt::AlignLeft);
#endif

    legend->setMaxColumns(1);
    legend->attach(ui->plot);

    DistributionPlotModel* model = new DistributionPlotModel(this);

    ui->table->setModel(model);
    connect(model,
            SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this,
            SLOT(onDataChanged(QModelIndex,QModelIndex,QVector<int>)));

    mAppender->setModel(model);

    (new DeleteEventFilter(this))->setView(ui->table);

    ItemDelegateWithCompleter* colorCompleter = new ItemDelegateWithCompleter(ColorPalette::instance()->names());

    ui->table->setItemDelegateForColumn(DistributionPlotModel::col_color,colorCompleter);

    connect(mAppender,SIGNAL(rowInserted(int)),this,SLOT(setDefaultColors()));

    connect(ui->options,SIGNAL(valuesChanged(int,double,double)),this,SLOT(onOptionsChanged(int,double,double)));

    DistributionHistogramModel* histModel = new DistributionHistogramModel(ui->histogramTable);

    ui->histogramTable->setModel(histModel);

    CopyEventFilter* filter = new CopyEventFilter(ui->histogramTable);
    filter->setView(ui->histogramTable);
    connect(filter,&CopyEventFilter::copy,[=](){
        ClipboardUtil::copyTsv(ui->histogramTable);
    });

    /*for(int c=0;c<histModel->columnCount();c++) {
        ui->histogramTable->setItemDelegateForColumn(c,new DoubleItemDelegate(ui->histogramTable));
    }*/



    QList<int> columns = {DistributionHistogramModel::col_percent1, DistributionHistogramModel::col_percent2};
    foreach(int column, columns) {
        setHistogramTableColumnPrec(column, 2);
    }

}

QStringList DistributionPlot::modelHeader() const {
    return toLower(headerData(mModel,Qt::Horizontal));
}


void DistributionPlot::setModel(QAbstractItemModel *model)
{
    mModel = model;
    QStringList header = modelHeader();
    ItemDelegateWithCompleter* xyCompleter = new ItemDelegateWithCompleter(header);
    ui->table->setItemDelegateForColumn(DistributionPlotModel::col_v,xyCompleter);
    setDefaultColors();
    updateDataset();
    updateSeries();
}

void DistributionPlot::onDataChanged(QModelIndex,QModelIndex,QVector<int>) {
    DistributionPlotModel* model = qobject_cast<DistributionPlotModel*>(ui->table->model());
    QList<DistributionPlotItem> items = model->items(modelHeader());
    if (mItems == items) {
        return;
    }
    mItems = items;
    updateDataset();
    updateSeries();
}

void DistributionPlot::setDefaultColors() {
    ::setDefaultColors(mAppender,
                     ui->table->model(),
                     {DistributionPlotModel::col_v},{DistributionPlotModel::col_color},{});
}


void DistributionPlot::onBinsValueChanged(int) {
    updateSeries();
}


void DistributionPlot::updateSeries() {
    int bins = ui->options->bins();
    bool ok1 = false;
    bool ok2 = false;
    double min = ui->options->min(&ok1);
    double max = ui->options->max(&ok2);
    if (max > min && ok1 && ok2) {
        onOptionsChanged(bins, min, max);
    }
}

void DistributionPlot::updateDataset()
{
    mDataset.update(mItems,mModel);
    if (mDataset.isEmpty()) {
        return;
    }
    ui->options->init(12,mDataset.min(),mDataset.max());
}

void DistributionPlot::setHistogramTableColumnPrec(int column, int prec) {
    DoubleItemDelegate* d = new DoubleItemDelegate(ui->histogramTable, prec);
    ui->histogramTable->setItemDelegateForColumn(column, d);
}

void DistributionPlot::onOptionsChanged(int bins, double min, double max) {

    QList<QwtPlotMultiBarChart*> barCharts = filterMultiBarCharts(ui->plot);
    QwtPlotMultiBarChart* chart = barCharts[0];

    DistributionHistogramModel* histModel = qobject_cast<DistributionHistogramModel*>(ui->histogramTable->model());


    if (mDataset.isEmpty() || bins < 2) {
        chart->setSamples(QVector<QwtSetSample>());
        chart->setBarTitles(QList<QwtText>());
        ui->plot->replot();
        return;
    }

    Histogram hist(bins, mDataset, min, max);
    chart->setBarTitles(hist.titles());

    if (hist.size() > 0) {
        ui->options->setNAuto(hist.total(0));
        if (!ui->options->autoRange()) {
            ui->options->setNManual(hist.filtered(0));
        }
    }

    histModel->setHistogram(hist);

    if (hist.size() > 0) {
        int prec = hist.prec();
        QList<int> columns = {DistributionHistogramModel::col_min, DistributionHistogramModel::col_max, DistributionHistogramModel::col_mid};
        foreach(int column, columns) {
            setHistogramTableColumnPrec(column, prec);
        }
    }

    for (int i=0; i<mItems.size(); i++)
    {
        const DistributionPlotItem& item = mItems[i];
        QwtColumnSymbol *symbol = new QwtColumnSymbol(QwtColumnSymbol::Box);
        symbol->setLineWidth(2);
        symbol->setFrameStyle(QwtColumnSymbol::NoFrame);
        symbol->setPalette(QPalette(ColorPalette::instance()->toColor(item.color())));
        chart->setSymbol(i, symbol);
    }
    chart->setSamples(hist.samples());
    ui->plot->replot();

}

