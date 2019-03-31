#include "plotpicker.h"

PlotPicker::PlotPicker(QWidget *canvas) : QwtPlotPicker(canvas)
{

}

QRectF PlotPicker::scaleRect() const
{
    return QwtPlotPicker::scaleRect();
}
