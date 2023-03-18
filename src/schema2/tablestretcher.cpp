#include "tablestretcher.h"

#include <QTableView>
#include <QHeaderView>
#include <QResizeEvent>
#include <QDebug>

TableStretcher::TableStretcher(QObject *parent)
    : QObject{parent}
{

}

void TableStretcher::setView(QTableView *view) {
    mView = view;
    mHeaderView = mView->horizontalHeader();
    mHeaderView->installEventFilter(this);
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
            for(int column=0;column<model->columnCount()-1;column++) {
                mView->setColumnWidth(column, mRatio.value(column) / sum * totalSize);
            }
        }
    }
    return false;
}
