#ifndef CANVASPICKER_H
#define CANVASPICKER_H

#include <qwt_plot.h>
#include <QObject>

class CanvasPicker : public QObject
{
public:
    CanvasPicker(QwtPlot *plot);

protected:
    QwtPlot *plot() const;
    bool eventFilter(QObject *watched, QEvent *event);
public slots:

};

#endif // CANVASPICKER_H
