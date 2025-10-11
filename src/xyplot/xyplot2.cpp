#include "xyplot2.h"

#include <QAbstractItemModel>
#include "plotpicker.h"
#include <QwtEventPattern>
#include <QwtPlotZoomer>
#include <QwtPlotPanner>
#include "filterplotitem.h"
#include <QwtLegend>
#include "datautils.h"
#include "plotcurve.h"
#include "plotcurvedialog.h"
#include "zipunzip.h"
#include <QPen>

XYPlot2::XYPlot2(QWidget *parent)
    : QwtPlot{parent}
{
    init();
}

void XYPlot2::setModel(QAbstractItemModel *model) {
    mModel = model;
    mHeader = headerData(mModel,Qt::Horizontal);
    mColumns = mHeader;
    std::sort(mColumns.begin(), mColumns.end());

    QList<QwtPlotCurve*> curves = filterCurves(this);

    QwtPlotCurve* last = nullptr;
    if (curves.size() > 0) {
        last = curves[curves.size()-1];
    }

    for(QwtPlotCurve* curve: curves) {
        bool updateAxes = curve == last;
        setSamples(static_cast<PlotCurve*>(curve), updateAxes);
    }

}

static int indexOf(const QString& needle, const QStringList& haystack, Qt::CaseSensitivity cs) {
    if (cs == Qt::CaseSensitive) {
        return haystack.indexOf(needle);
    }
    for(int i=0;i<haystack.size();i++) {
        if (needle.compare(haystack[i], Qt::CaseInsensitive) == 0) {
            return i;
        }
    }
    return -1;
}

void XYPlot2::setSamples(PlotCurve* curve, bool updateAxes) {
    if (mModel == nullptr) {
        qDebug() << "mModel == nullptr";
        return;
    }

    QString xs = curve->x();
    QString ys = curve->y();
    if (ys.isEmpty()) {
        return;
    }
    int x = indexOf(xs, mHeader, Qt::CaseInsensitive);
    int y = indexOf(ys, mHeader, Qt::CaseInsensitive);
    QPolygonF samples = toSamples(filterNumeric(zipToPairList(columnData(mModel,x),columnData(mModel,y))));
    curve->setSamples(samples);

    if (updateAxes) {
        setAxisAutoScale(QwtPlot::xBottom);
        setAxisAutoScale(QwtPlot::yLeft);
        this->updateAxes();
        mZoomer->setZoomBase(true);
    }
}

void XYPlot2::init() {

    QwtPlot* plot = this;

    //plot->setAxisAutoScale(QwtPlot::xBottom);
    //plot->setAxisAutoScale(QwtPlot::yLeft);

    mZoomer = new QwtPlotZoomer(plot->canvas());
    mZoomer->setRubberBandPen( QPen( Qt::black ) );
    mZoomer->setTrackerPen( QPen( Qt::black ) );
    mZoomer->setMousePattern( QwtEventPattern::MouseSelect2,
                             Qt::RightButton, Qt::ControlModifier );
    mZoomer->setMousePattern( QwtEventPattern::MouseSelect3,
                             Qt::RightButton );

    mPicker = new PlotPicker(plot->canvas());

    QwtPlotPanner *panner = new QwtPlotPanner(plot->canvas());
    panner->setMouseButton( Qt::MiddleButton );

    QwtLegend* legend = new QwtLegend();
    legend->setDefaultItemMode(QwtLegendData::Clickable);
    plot->insertLegend(legend);

    connect(legend, SIGNAL(clicked(QVariant,int)), this, SLOT(onLegendClicked(QVariant,int)));

    PlotCurve* curveAdd = new PlotCurve("Add", true);
    curveAdd->setStyle(QwtPlotCurve::NoCurve);
    curveAdd->attach(plot);
}


void XYPlot2::onLegendClicked(QVariant itemInfo, int index) {



    //qDebug() << "onClicked"  << itemInfo;

    QwtPlotItem* plotItem = infoToItem( itemInfo );

    //QwtPlotCurve* plotCurve = static_cast<QwtPlotCurve*>(plotItem);

    //qDebug() << plotItem;

    if (plotItem->rtti() != QwtPlotItem::Rtti_PlotCurve) {
        qDebug() << "!QwtPlotItem::Rtti_PlotCurve" << __FILE__ << __LINE__;
        return;
    }

    PlotCurve* curve = static_cast<PlotCurve*>(plotItem);
    if (!curve) {
        qDebug() << "!curve";
        return;
    }

    if (curve->addButton()) {
        curve = new PlotCurve();
        curve->attach(this);
    }

    PlotCurveDialog dialog(mColumns, curve, this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    if (dialog.remove()) {
        curve->detach();
        updateLegend();
        delete curve;
        curve = nullptr;
        //});
    } else {

        dialog.updateCurve();
    }

    if (curve) {
        setSamples(curve, true);
    }

}

QSize XYPlot2::sizeHint() const
{
    return {0, 0};
}

QSize XYPlot2::minimumSizeHint() const
{
    return {0, 0};
}

