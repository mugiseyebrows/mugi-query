#include "deleteeventfilter.h"

#include <QKeyEvent>


DeleteEventFilter::DeleteEventFilter(QObject *parent) : QObject(parent)
{

}

bool DeleteEventFilter::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == mView && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (!keyEvent->matches(QKeySequence::Delete)) {
            return false;
        }
        QModelIndexList indexes = mView->selectionModel()->selectedIndexes();
        QAbstractItemModel* model = mView->model();
        foreach(const QModelIndex& index, indexes) {
            model->setData(index,QVariant());
        }
    }
    return false;
}

void DeleteEventFilter::setView(QTableView *view)
{
    mView = view;
    mView->installEventFilter(this);
}
