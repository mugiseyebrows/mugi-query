#include "filterempty.h"

QStringList filterEmpty(const QStringList& items) {
    QStringList res;
    for(const QString& item: qAsConst(items)) {
        if (!item.isEmpty()) {
            res.append(item);
        }
    }
    return res;
}
