#include "distributionhistogrammodel.h"

#include "distributiondataset.h"
#include "histogram.h"

DistributionHistogramModel::DistributionHistogramModel(QObject *parent) :
    QStandardItemModel(0, cols_size, parent)
{
    QList<QString> header = {"Min","Max","Mid","N","% tot","% fil"};
    for(int i=0;i<header.size();i++) {
        setHeaderData(i, Qt::Horizontal, header[i]);
    }
}

void DistributionHistogramModel::setRowCount(int count_) {
    // remove rows
    if (rowCount() > count_) {
        int count = rowCount() - count_;
        removeRows(rowCount() - count, count);
    }
    // insert rows
    if (rowCount() < count_) {
        int count = count_ - rowCount();
        insertRows(rowCount(), count);
    }
}

void DistributionHistogramModel::setHistogram(const Histogram &hist)
{

    if (hist.size() == 0) {
        setRowCount(0);
        return;
    }

    QList<QList<double> > histRows = hist.histRows(0);
    setRowCount(histRows.size());

    for(int r=0;r<histRows.size();r++) {
        QList<double> histRow = histRows[r];
        for(int c=0;c<histRow.size();c++) {
            setData(index(r,c), histRow[c]);
        }
    }

}
