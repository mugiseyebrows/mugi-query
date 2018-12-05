#include "setsplitersizesratio.h"

#include <QSplitter>
#include <numeric>
#include <QDebug>

void setSpliterSizesRatio(QSplitter* splitter, int r1, int r2) {

    QList<int> sizes = splitter->sizes();
    if (sizes.size() != 2) {
        qDebug() << "setSpliterSizesRatio error" << sizes.size() << "widgets istead of two";
        return;
    }

    int sum = std::accumulate(sizes.begin(),sizes.end(),0);
    int r1_ = sum * r1 / (r1 + r2);
    int r2_ = sum - r1_;

    //qDebug() << "total size" << sum << r1 << "/" << r2 << "is" << r1_ << "/" << r2_;

    QList<int> sizes_;
    sizes_ << r1_ << r2_;
    splitter->setSizes(sizes_);

}
