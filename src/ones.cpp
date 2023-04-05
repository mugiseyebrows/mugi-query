#include "ones.h"

QList<double> ones(int size) {
    QList<double> res;
    for(int i=0;i<size;i++) {
        res.append(1);
    }
    return res;
}
