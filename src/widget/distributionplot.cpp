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
#include "lit.h"
#include "datautils.h"
#include "colorpalette.h"
#include "filterplotitem.h"
#include <qwt_column_symbol.h>
#include "canvaspicker.h"
#include "histogram.h"

using namespace DataUtils;
using namespace Lit;

DistributionPlot::DistributionPlot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DistributionPlot),
    mAppender(new ModelAppender(this))
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

#include "plotmultibarchart.h"

void DistributionPlot::init() {
    QwtPlotMultiBarChart* chart = new QwtPlotMultiBarChart();
    chart->setLayoutPolicy( QwtPlotMultiBarChart::AutoAdjustSamples );
    chart->setSpacing( 3 );
    chart->setMargin( 3 );
    chart->attach(ui->plot);

    ui->plot->setAutoReplot( true );

    ui->bins->setValue(10);

    //ui->distributionPlot->insertLegend(new QwtLegend());

    QwtPlotLegendItem* legend = new QwtPlotLegendItem();
    legend->setAlignment(Qt::AlignTop | Qt::AlignLeft);
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
    connect(ui->bins,SIGNAL(valueChanged(int)),this,SLOT(onBinsValueChanged(int)));

    //mPicker = new CanvasPicker(ui->plot);
}


QStringList DistributionPlot::modelHeader() {
    return toLower(headerData(mModel,Qt::Horizontal));
}


void DistributionPlot::setModel(QAbstractItemModel *model)
{
    mModel = model;

    QStringList header = modelHeader();

    ItemDelegateWithCompleter* xyCompleter = new ItemDelegateWithCompleter(header);

    ui->table->setItemDelegateForColumn(DistributionPlotModel::col_v,xyCompleter);

    setDefaultColors();

    updateSeries();
}


void DistributionPlot::onDataChanged(QModelIndex,QModelIndex,QVector<int>) {
    DistributionPlotModel* model = qobject_cast<DistributionPlotModel*>(ui->table->model());
    QList<DistributionPlotItem> items = model->items(modelHeader());
    if (mItems == items) {
        return;
    }
    mItems = items;
    updateSeries();
}

void DistributionPlot::setDefaultColors() {
    ::setDefaultColors(mAppender,
                     ui->table->model(),
                     il(DistributionPlotModel::col_v),
                     il(DistributionPlotModel::col_color),
                     il());
}


void DistributionPlot::onBinsValueChanged(int) {
    updateSeries();
}


void DistributionPlot::updateSeries() {

    int numBars = mItems.size();
    int bins = ui->bins->value();
    if (bins < 1 || numBars < 1) {
        return;
    }

    bool ok;
    QPair<double,double> range = Histogram::range(mItems,modelHeader(),mModel,&ok);
    if (ok) {
        ui->options->init(12,range.first,range.second);
    }

    Histogram hist(ui->options->bins(), mItems, modelHeader(), mModel, ui->options->vmin(), ui->options->vmax());
    QList<QwtPlotMultiBarChart*> barCharts = filterMultiBarCharts(ui->plot);
    QwtPlotMultiBarChart* chart = barCharts[0];
    chart->setBarTitles(hist.titles());
    chart->setLegendIconSize(QSize(10, 14));

    for (int i = 0; i < numBars; i++)
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

