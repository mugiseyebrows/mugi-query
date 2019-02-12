#include "dataplot.h"
#include "ui_dataplot.h"

#include "modelappender.h"
#include <QStringListModel>
#include <QStandardItemModel>
#include "itemdelegatewithcompleter.h"
#include <QSqlQueryModel>
#include "rowvaluegetter.h"
#include <qwt_plot_curve.h>
#include <QPolygonF>
#include "zipunzip.h"
#include "rowvaluesetter.h"
#include <qwt_symbol.h>
#include <qwt_plot_item.h>
#include "deleteeventfilter.h"
#include "lit.h"
#include <qwt_plot_multi_barchart.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_layout.h>
#include "xyplotmodel.h"
#include "distributionplotitem.h"
#include <qwt_column_symbol.h>
#include <qwt_legend.h>
#include "setdefaultcolors.h"

#include "qisnumerictype.h"

using namespace Lit;

namespace  {

template <class T>
QList<T*> filterItems(QwtPlot* plot, int rtti) {
    QList<T*> result;
    QwtPlotItemList items = plot->itemList();
    foreach (QwtPlotItem* item, items) {
        if (item->rtti() == rtti) {
            T* curve = static_cast<T*>(item);
            result.append(curve);
        }
    }
    return result;
}
#if 0
QList<QwtPlotMultiBarChart*> multibarCharts(QwtPlot* plot) {
    return filterItems<QwtPlotMultiBarChart>(plot, QwtPlotItem::Rtti_PlotMultiBarChart);
}

QList<QwtPlotCurve*> curves(QwtPlot* plot) {
    return filterItems<QwtPlotCurve>(plot, QwtPlotItem::Rtti_PlotCurve);
}

QList<QwtPlotMultiBarChart*> mutibarCharts(QwtPlot* plot) {
    QList<QwtPlotMultiBarChart*> result;
    QwtPlotItemList items = plot->itemList();
    foreach (QwtPlotItem* item, items) {
        if (item->rtti() == QwtPlotItem::Rtti_PlotMultiBarChart) {
            QwtPlotCurve* curve = static_cast<QwtPlotCurve*>(item);
            result.append(curve);
        }
    }
    return result;
}


QList<QwtPlotCurve*> curves(QwtPlot* plot) {
    QList<QwtPlotCurve*> result;
    QwtPlotItemList items = plot->itemList();
    foreach (QwtPlotItem* item, items) {
        if (item->rtti() == QwtPlotItem::Rtti_PlotCurve) {
            QwtPlotCurve* curve = static_cast<QwtPlotCurve*>(item);
            result.append(curve);
        }
    }
    return result;
}
#endif

QStringList headerData(const QAbstractItemModel* model, Qt::Orientation orientation) {
    QStringList result;
    int count = orientation == Qt::Horizontal ? model->columnCount() : model->rowCount();
    for(int section=0;section<count;section++) {
        result.append(model->headerData(section,orientation).toString());
    }
    return result;
}

QStringList toLower(const QStringList& vs) {
    QStringList result;
    foreach(const QString& v, vs) {
        result << v.toLower();
    }
    return result;
}



#if 0
QVariantList columnData(const QAbstractItemModel* model,int column) {
    QVariantList result;
    if (column < 0) {
        for(int row = 0; row < model->rowCount(); row++) {
            result << row;
        }
    } else {
        for(int row = 0; row < model->rowCount(); row++) {
            result << model->data(model->index(row,column));
        }
    }
    return result;
}

QVariantList filterNumeric(const QVariantList& data) {
    QVariantList result;
    foreach (const QVariant& v, data) {
        if (!v.isNull() && qIsNumericType(v.type())) {
            result << v;
        }
    }
    return result;
}

QList<QPair<QVariant,QVariant> > filterNull(const QList<QPair<QVariant,QVariant> >& data) {
    QList<QPair<QVariant,QVariant> > result;
    for(int i=0;i<data.size();i++) {
        if (data[i].first.isNull() || data[i].second.isNull()) {
            continue;
        }
        result.append(data[i]);
    }
    return result;
}

QPolygonF toPolygon(const QList<QPair<QVariant,QVariant> >& data) {
    QPolygonF result;
    for(int i=0;i<data.size();i++) {
        result.append(QPointF(data[i].first.toDouble(),data[i].second.toDouble()));
    }
    return result;
}
#endif

#if 0
bool equal(const QList<QStringList>& vs1, const QList<QStringList>& vs2) {
    if (vs1.size() != vs2.size()) {
        return false;
    }
    for(int i=0;i<vs1.size();i++) {
        if (vs1[i] != vs2[i]) {
            return false;
        }
    }
    return true;
}
#endif

#if 0
QString getColor(int i) {
    static QStringList palette;
    if (palette.isEmpty()) {
        palette << "red"
               << "green"
               << "blue"
               << "cyan"
               << "magenta"
               << "yellow"
               << "darkRed"
               << "darkGreen"
               << "darkBlue"
               << "darkCyan"
               << "darkMagenta"
               << "darkYellow";
    }
    return palette[i % palette.size()];
}
#endif

QMap<QString, Qt::GlobalColor> getColors() {
    static QMap<QString, Qt::GlobalColor> colors;
    if (colors.isEmpty()) {
        colors["black"] = Qt::black;
        colors["white"] = Qt::white;
        colors["darkGray"] = Qt::darkGray;
        colors["gray"] = Qt::gray;
        colors["lightGray"] = Qt::lightGray;
        colors["red"] = Qt::red;
        colors["green"] = Qt::green;
        colors["blue"] = Qt::blue;
        colors["cyan"] = Qt::cyan;
        colors["magenta"] = Qt::magenta;
        colors["yellow"] = Qt::yellow;
        colors["darkRed"] = Qt::darkRed;
        colors["darkGreen"] = Qt::darkGreen;
        colors["darkBlue"] = Qt::darkBlue;
        colors["darkCyan"] = Qt::darkCyan;
        colors["darkMagenta"] = Qt::darkMagenta;
        colors["darkYellow"] = Qt::darkYellow;
        colors["transparent"] = Qt::transparent;
        colors["none"] = Qt::transparent;
    }
    return colors;
}

}

#include <qwt_plot_legenditem.h>

void DataPlot::initDistribution() {
    QwtPlotMultiBarChart* chart = new QwtPlotMultiBarChart();
    chart->setLayoutPolicy( QwtPlotMultiBarChart::AutoAdjustSamples );
    chart->setSpacing( 3 );
    chart->setMargin( 3 );
    chart->attach(ui->distributionPlot);

    ui->distributionPlot->setAutoReplot( true );

    //ui->distributionPlot->insertLegend(new QwtLegend());

    QwtPlotLegendItem* legend = new QwtPlotLegendItem();
    legend->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    legend->setMaxColumns(1);
    legend->attach(ui->distributionPlot);

    DistributionPlotModel* model = new DistributionPlotModel(this);

    ui->distribution->setModel(model);
    connect(model,
            SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this,
            SLOT(onDistributionPlotModelChanged(QModelIndex,QModelIndex,QVector<int>)));

    mAppenderDistribution->setModel(model);



    connect(mAppenderDistribution,SIGNAL(rowInserted(int)),this,SLOT(setDefaultColorsDistributionPlot()));
}

void DataPlot::initXY() {
    ui->xyPlot->setAxisAutoScale(QwtPlot::xBottom);
    ui->xyPlot->setAxisAutoScale(QwtPlot::yLeft);

    QStandardItemModel* xyModel = new XYPlotModel(this);
    ui->xy->setModel(xyModel);

    mAppenderXYPlot->setModel(xyModel);

    connect(xyModel,SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),this,SLOT(onXYModelDataChanged(QModelIndex,QModelIndex,QVector<int>)));

    ui->bins->setValue(12);
    connect(ui->bins,SIGNAL(valueChanged(int)),this,SLOT(onBinsValueChanged(int)));

    connect(mAppenderXYPlot,SIGNAL(rowInserted(int)),this,SLOT(setDefaultColorsXYPlot()));
}

DataPlot::DataPlot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataPlot),
    mAppenderXYPlot(new ModelAppender(this)),
    mAppenderDistribution(new ModelAppender(this))
{
    ui->setupUi(this);
    initXY();
    initDistribution();
}

void DataPlot::onDistributionPlotModelChanged(QModelIndex,QModelIndex,QVector<int>) {
    DistributionPlotModel* model = qobject_cast<DistributionPlotModel*>(ui->distribution->model());
    QList<DistributionPlotItem> items = model->items(modelHeader());
    if (mDistribution != items) {
        mDistribution = items;
    }
    updateDistribution();
}

DataPlot::~DataPlot()
{
    delete ui;
}

#if 0
void DataPlot::setDefaultColors() {
    mAppender->setActive(false);
    QAbstractItemModel* model = ui->xy->model();
    for(int row = 0; row < model->rowCount() - 1; row++) {
        qDebug() << row;
        RowValueGetter g(model,row);
        RowValueSetter s(model,row);
        if (g(col_line).isNull()) {
            s(col_line,getColor(row));
        }
        if (g(col_marker).isNull()) {
            s(col_marker,"none");
        }
    }
    mAppender->setActive(true);
}
#endif



void DataPlot::setModel(QSqlQueryModel *model)
{
    mModel = model;

    QStringList header = modelHeader();

    ItemDelegateWithCompleter* xyCompleter = new ItemDelegateWithCompleter(header);
    ItemDelegateWithCompleter* colorCompleter = new ItemDelegateWithCompleter(getColors().keys());

    ui->xy->setItemDelegateForColumn(XYPlotModel::col_x,xyCompleter);
    ui->xy->setItemDelegateForColumn(XYPlotModel::col_y,xyCompleter);
    ui->xy->setItemDelegateForColumn(XYPlotModel::col_line,colorCompleter);
    ui->xy->setItemDelegateForColumn(XYPlotModel::col_marker,colorCompleter);

    ui->distribution->setItemDelegateForColumn(DistributionPlotModel::col_color,colorCompleter);
    ui->distribution->setItemDelegateForColumn(DistributionPlotModel::col_x,xyCompleter);

    (new DeleteEventFilter(this))->setView(ui->xy);
    (new DeleteEventFilter(this))->setView(ui->distribution);

    setDefaultColorsXYPlot();
    setDefaultColorsDistributionPlot();

    //connect(ui->distribution,SIGNAL(currentIndexChanged(QString)),this,SLOT(onDistributionCurrentIndexChanged(QString)));

    /*ui->distribution->clear();
    ui->distribution->addItems(header);*/

}

QStringList DataPlot::modelHeader() {
    return toLower(headerData(mModel,Qt::Horizontal));
}


Qt::GlobalColor toGlobalColor(const QString& color) {
    auto colors = getColors();
    return colors.value(color,Qt::transparent);
}

#include "filterplotitem.h"
#include "datautils.h"
using namespace DataUtils;

void DataPlot::onXYModelDataChanged(QModelIndex,QModelIndex,QVector<int>) {

    XYPlotModel* model = qobject_cast<XYPlotModel*>(ui->xy->model());

    QStringList header = modelHeader();

    QList<XYPlotModelItem> xy = model->items(header);

    if (xy != mXy) {

        //qDebug() << "xy != mXy";

        QList<QwtPlotCurve*> curves = filterCurves(ui->xyPlot);
        int attached = curves.size();
        while(curves.size() < xy.size()) {
            curves.append(new QwtPlotCurve());
        }

        //auto colors = getColors();

        for(int i=0;i<xy.size();i++) {

            const XYPlotModelItem& item = xy[i];

            int x = item.x();
            int y = item.y();
            Qt::GlobalColor line = toGlobalColor(item.line());
            Qt::GlobalColor marker = toGlobalColor(item.marker());

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
            curves[i]->attach(ui->xyPlot);
            curves[i]->setRenderHint(QwtPlotItem::RenderAntialiased, true);
        }

        for(int i=xy.size();i<curves.size();i++) {
            curves[i]->detach();
            delete curves[i];
        }

        ui->xyPlot->replot();
        mXy = xy;
    }

}
#if 0
QVector<double> toDoubleVector(const QVariantList& vs) {
    QVector<double> res;
    QVariant v;
    foreach(v,vs) {
        res.append(v.toDouble());
    }
    return res;
}


QList<double> toDouble(const QVariantList& vs) {
    QList<double> res;
    QVariant v;
    foreach(v,vs) {
        res.append(v.toDouble());
    }
    return res;
}
#endif


void DataPlot::updateDistribution() {

    int numBars = mDistribution.size();
    int bins = ui->bins->value();
    if (bins < 1 || numBars < 1) {
        return;
    }

    QStringList header = modelHeader();

    QList<QList<double> > values;

    for(int i=0;i<numBars;i++) {

        const DistributionPlotItem& item = mDistribution[i];

        QList<double> values_ = toDouble(filterNumeric(columnData(mModel,item.column())));
        if (values_.isEmpty()) {
            values_.append(0.0);
        }
        values.append(values_);
    }

    double min = values[0][0];
    double max = values[0][0];

    for(int i=0;i<numBars;i++) {
        const QList<double> values_ = values[i];
        double min_ = *std::min_element(values_.begin(),values_.end());
        double max_ = *std::max_element(values_.begin(),values_.end());
        min = qMin(min_, min);
        max = qMax(max_, max);
    }

    double w = max - min;

    QList<QVector<double> > hist;
    while(hist.size() < bins) {
        QVector<double> zeros;
        while(zeros.size() < numBars) {
            zeros.append(0.0);
        }
        hist.append(zeros);
    }

    for(int i=0;i<numBars;i++) {
        const QList<double> values_ = values[i];
        foreach(double v, values_) {
            int index = qMin((int)((v - min) * (double) bins / w), hist.size() - 1);
            hist[index][i] += 1.0;
        }
    }

    QList<QwtPlotMultiBarChart*> barCharts = filterMultiBarCharts(ui->distributionPlot);

    QwtPlotMultiBarChart* chart = barCharts[0];

        QList<QwtText> titles;
        for ( int i = 0; i < numBars; i++ )
        {
            const DistributionPlotItem& item = mDistribution[i];
            titles.append(header.value(item.column()));
        }
        chart->setBarTitles(titles);

        chart->setLegendIconSize( QSize( 10, 14 ) );

        for ( int i = 0; i < numBars; i++ )
        {
            const DistributionPlotItem& item = mDistribution[i];
            QwtColumnSymbol *symbol = new QwtColumnSymbol( QwtColumnSymbol::Box );
            symbol->setLineWidth( 2 );
            symbol->setFrameStyle( QwtColumnSymbol::Raised );
            symbol->setPalette( QPalette( toGlobalColor(item.color()) ) );
            chart->setSymbol( i, symbol );
        }


        QVector<QwtSetSample> samples;

        for(int i=0;i<hist.size();i++) {
            samples += QwtSetSample(min + (w * ((double)i + 0.5)) / bins, hist[i]);
        }

        //samples += QwtSetSample(3.0, hist);

    chart->setSamples( samples );

    //setDistributionPlotOrientation(d_barChartItem, 0);

}


void DataPlot::onBinsValueChanged(int) {
    updateDistribution();
}

/*
void DataPlot::onDistributionCurrentIndexChanged(QString) {
    updateDistribution();
}
*/

void DataPlot::setDefaultColorsXYPlot() {
    setDefaultColors(mAppenderXYPlot,
                     ui->xy->model(),
                     il(XYPlotModel::col_x, XYPlotModel::col_y),
                     il(XYPlotModel::col_line),
                     il(XYPlotModel::col_marker));
}
void DataPlot::setDefaultColorsDistributionPlot() {
    setDefaultColors(mAppenderDistribution,
                     ui->distribution->model(),
                     il(DistributionPlotModel::col_x),
                     il(DistributionPlotModel::col_color),
                     il());
}

