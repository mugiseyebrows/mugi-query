#include "plotpicker.h"
#include <QRectF>

PlotPicker::PlotPicker(QWidget *canvas) : QwtPlotPicker(canvas)
{

}

QRectF PlotPicker::scaleRect() const
{
    return QwtPlotPicker::scaleRect();
}
