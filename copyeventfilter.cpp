#include "copyeventfilter.h"

#include <QKeyEvent>
#include <QTableView>
#include <QMessageBox>
#include <QClipboard>
#include <QApplication>
#include <QDebug>

#include "datastreamer.h"

#include "dataformat.h"
#include "rowvaluegetter.h"


CopyEventFilter::CopyEventFilter(QObject *parent) :
    QObject(parent), mView(0)
{

}

void CopyEventFilter::setView(QTableView* view) {
    mView = view;
    view->installEventFilter(this);
}

bool CopyEventFilter::eventFilter(QObject * object, QEvent * event) {
    if (object == mView && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if(keyEvent->matches(QKeySequence::Copy)) {
            QAbstractItemModel* model = mView->model();
            if (!model) {
                return false;
            }
            QItemSelection selection = mView->selectionModel()->selection();
            copySelected(model, selection);
            return true;
        }
    }
    return false;
}

void CopyEventFilter::streamRange(QTextStream& stream, const QItemSelectionRange &rng) {
    DataFormat::Format format = DataFormat::Csv;
    QAbstractItemModel* model = const_cast<QAbstractItemModel*>(rng.model());
    for(int row = rng.topLeft().row(); row <= rng.bottomRight().row(); row++) {
        RowValueGetter g(model,row);
        int column = rng.topLeft().column();
        stream << DataStreamer::variantToString(g(column++),format);
        for(;column <= rng.bottomRight().column(); column++) {
            stream << "\t" << DataStreamer::variantToString(g(column),format);
        }
        stream << "\n";
    }
}

namespace {

QModelIndex topLeft(QAbstractItemModel* model) {
    return model->index(0,0);
}

QModelIndex bottomRight(QAbstractItemModel* model) {
    return model->index(model->rowCount()-1,model->columnCount()-1);
}

}


void CopyEventFilter::copyAll(QAbstractItemModel* model) {
    QString data;
    QTextStream stream(&data);

    QItemSelectionRange rng(topLeft(model),bottomRight(model));
    streamRange(stream,rng);
    stream.flush();
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(data);
}

void CopyEventFilter::copySelected(QAbstractItemModel *model, const QItemSelection& selection) {

    DataFormat::Format format = DataFormat::Csv;

    // one range of one index
    if (selection.size() == 1 && selection[0].topLeft() == selection[0].bottomRight()) {
        QClipboard *clipboard = QApplication::clipboard();
        QString data = DataStreamer::variantToString(model->data(selection[0].topLeft()),format);
        clipboard->setText(data);
        return;
    }

    // many ranges
    QString data;
    QTextStream stream(&data);
    foreach(const QItemSelectionRange& rng, selection) {
        streamRange(stream,rng);
    }
    stream.flush();
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(data);

}
