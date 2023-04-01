#include "tablestretcher.h"

#include <QTableView>
#include <QHeaderView>
#include <QResizeEvent>
#include <QDebug>

TableStretcher::TableStretcher(QObject *parent)
    : mPadding(0), QObject{parent}
{

}

void TableStretcher::setView(QTableView *view) {
    mView = view;
    mHeaderView = mView->horizontalHeader();
    mHeaderView->installEventFilter(this);
}

void TableStretcher::setRatio(const QList<double>& ratio) {
    mRatio = ratio;
}

void TableStretcher::setPadding(int value)
{
    mPadding = value;
}

TableStretcher* TableStretcher::stretch(QTableView *view, const QList<double>& ratio, int padding)
{
    TableStretcher* stretcher = new TableStretcher(view);
    stretcher->setView(view);
    stretcher->setRatio(ratio);
    stretcher->setPadding(padding);
    return stretcher;
}

bool TableStretcher::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == mHeaderView) {
        if (event->type() == QEvent::Resize) {
            QResizeEvent* resizeEvent = static_cast<QResizeEvent*>(event);
            //qDebug() << resizeEvent->size();
            double totalSize = resizeEvent->size().width();
            QAbstractItemModel* model = mView->model();
            if (!model) {
                return false;
            }
            double sum = std::accumulate(mRatio.begin(), mRatio.end(), 0.0);

            for(int column=0;column<model->columnCount();column++) {
                mView->setColumnWidth(column, mRatio.value(column) / sum * (totalSize - mPadding));
            }
        }
    }
    return false;
}
