#include "treedepth.h"

#include <QModelIndex>

int treeDepth(const QModelIndex &index) {
    int count = 0;
    QModelIndex index_ = index;
    while (index_.parent().isValid()) {
        count += 1;
        index_ = index_.parent();
    }
    return count;
}
