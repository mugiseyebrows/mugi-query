#include "qwt_compat.h"

#include <qwt_plot_legenditem.h>

void setAlignmentInCanvas(QwtPlotLegendItem* legend, Qt::Alignment alignment) {
#if QWT_VERSION > 0x060104
   legend->setAlignmentInCanvas(alignment);
#else
   legend->setAlignment(alignment);
#endif
}
