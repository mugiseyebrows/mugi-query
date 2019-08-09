#ifndef SPLITTERUTIL_H
#define SPLITTERUTIL_H
#include <QList>
class QSplitter;
namespace SplitterUtil {
    QList<int> proportional(const QList<int> &sizes, const QList<double> &ratios);
    bool setRatio(QSplitter *splitter, const QList<double> &ratios);
    bool setFixed(QSplitter *splitter, int r0, int r1);
    bool setRatioWithMinSize(QSplitter *splitter, double r0, double r1, int m0, int m1);
} // namespace SplitterUtil
#endif // SPLITTERUTIL_H
