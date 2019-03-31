#include "setdefaultcolors.h"

#include "modelappender.h"
#include <QAbstractItemModel>
#include "rowvaluegetter.h"
#include "rowvaluesetter.h"

#include "colorpalette.h"

void setDefaultColors(ModelAppender* appender, QAbstractItemModel* model,
                                const QList<int>& nonEmpty, const QList<int>& nthColor,
                                const QList<int>& noneColor) {
    appender->setActive(false);

    for(int row = 0; row < model->rowCount() - 1; row++) {
        RowValueGetter g(model,row);
        RowValueSetter s(model,row);
        bool ok = nonEmpty.size() == 0;
        foreach(int column, nonEmpty) {
            if (!g(column).isNull()) {
                ok = true;
            }
        }
        if (!ok) {
            continue;
        }
        foreach(int column, nthColor) {
            if (g(column).isNull()) {
                s(column,ColorPalette::instance()->color(row));
            }
        }
        foreach(int column, noneColor) {
            if (g(column).isNull()) {
                s(column,"none");
            }
        }
    }

    appender->setActive(true);
}
