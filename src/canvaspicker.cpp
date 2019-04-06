#include "canvaspicker.h"

#include <qwt_plot_canvas.h>
#include <QMouseEvent>
#include "filterplotitem.h"
#include "plotmultibarchart.h"

CanvasPicker::CanvasPicker(QwtPlot *plot) : QObject(plot)
{
    QwtPlotCanvas *canvas = qobject_cast<QwtPlotCanvas *>( plot->canvas() );
    canvas->installEventFilter(this);
    canvas->setMouseTracking(true);
}

QwtPlot *CanvasPicker::plot() const
{
    return qobject_cast<QwtPlot *>( parent() );
}

bool CanvasPicker::eventFilter(QObject *object, QEvent *event)
{
    if ( plot() == 0 || object != plot()->canvas() ) {
        return false;
    }
    if (event->type() == QEvent::MouseMove) {


    }

    if (event->type() == QEvent::MouseButtonRelease) {

        return false;


        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        //qDebug() << mouseEvent->pos();

        QList<QwtPlotMultiBarChart*> charts = filterMultiBarCharts(plot());
        PlotMultiBarChart* chart = static_cast<PlotMultiBarChart*>(charts[0]);


        QwtScaleMap xMap = plot()->canvasMap(QwtPlot::xBottom);
        QPoint p = mouseEvent->pos();
        double x_ = xMap.invTransform(p.x());

        qDebug() << p.x() << x_;

        int index = chart->closestSample(x_);

        qDebug() << index << "test";

    }

    return false;
}
