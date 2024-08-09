#include "copyeventfilter.h"

#include <QKeyEvent>
#include <QTableView>
#include <QMessageBox>
#include <QClipboard>
#include <QApplication>
#include <QDebug>
#include "datastreamer.h"
#include "settings.h"
#include "rowvaluegetter.h"
#include "clipboardutil.h"

CopyEventFilter::CopyEventFilter(QObject *parent) :
    QObject(parent), mView(0)
{

}

void CopyEventFilter::setView(QTableView* view) {
    mView = view;
    view->installEventFilter(this);
}

void CopyEventFilter::copyTsv(QTableView *view)
{
    CopyEventFilter* filter = new CopyEventFilter(view);
    filter->setView(view);
    connect(filter,&CopyEventFilter::copy,[=](){
        ClipboardUtil::copyTsv(view);
    });
}

bool CopyEventFilter::eventFilter(QObject * object, QEvent * event) {
    if (object == mView && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if(keyEvent->matches(QKeySequence::Copy)) {
            emit copy();
            return true;
        }
        if (keyEvent->matches(QKeySequence::Paste)) {
            emit paste();
            return true;
        }
        if (keyEvent->matches(QKeySequence::Delete)) {
            emit delete_();
            return true;
        }
        if (keyEvent->matches(QKeySequence::Cut)) {
            emit cut();
            return true;
        }
    }
    return false;
}

void CopyEventFilter::onDeleteSelected()
{
    QModelIndexList indexes = mView->selectionModel()->selectedIndexes();
    QAbstractItemModel* model = mView->model();
    foreach (const QModelIndex& index, indexes) {
        model->setData(index,QVariant());
    }
}

