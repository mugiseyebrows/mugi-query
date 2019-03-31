#ifndef PLOTPICKER_H
#define PLOTPICKER_H

#include <qwt_plot_picker.h>

class PlotPicker : public QwtPlotPicker
{
public:
    PlotPicker(QWidget *canvas);
    QRectF scaleRect() const;
};

#endif // PLOTPICKER_H
