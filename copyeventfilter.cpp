#include "copyeventfilter.h"

#include <QKeyEvent>
#include <QTableView>
#include <QMessageBox>
#include <QClipboard>
#include <QApplication>
#include <QDebug>

#include "datastreamer.h"


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
            copySelected(model, selection, DataFormat::Csv, "\t", mView->locale());
            return true;
        }
    }
    return false;
}

#include "settings.h"

void CopyEventFilter::streamRange(QTextStream& stream, const QItemSelectionRange &rng,
                                  DataFormat::Format format,
                                  const QString &separator,
                                  DataFormat::ActionType action,
                                  const QLocale& locale) {

    Formats formats(action);
    QAbstractItemModel* model = const_cast<QAbstractItemModel*>(rng.model());
    for(int row = rng.topLeft().row(); row <= rng.bottomRight().row(); row++) {
        RowValueGetter g(model,row);
        int column = rng.topLeft().column();
        stream << DataStreamer::variantToString(g(column++),format,formats,locale);
        for(;column <= rng.bottomRight().column(); column++) {
            stream << separator << DataStreamer::variantToString(g(column),format,formats,locale);
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

#if 0
void CopyEventFilter::copyAll(QAbstractItemModel* model) {
    QString data;
    QTextStream stream(&data);

    QItemSelectionRange rng(topLeft(model),bottomRight(model));
    streamRange(stream,rng);
    stream.flush();
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(data);
}
#endif

void CopyEventFilter::copySelected(QAbstractItemModel *model, const QItemSelection& selection,
                                   DataFormat::Format format, const QString &separator, const QLocale& locale) {

    DataFormat::ActionType action = DataFormat::ActionCopy;
    Formats formats(action);

    // one range of one index
    if (selection.size() == 1 && selection[0].topLeft() == selection[0].bottomRight()) {
        QClipboard *clipboard = QApplication::clipboard();
        QString data = DataStreamer::variantToString(model->data(selection[0].topLeft()),format,formats,locale);
        clipboard->setText(data);
        return;
    }

    // many ranges
    QString data;
    QTextStream stream(&data);
    foreach(const QItemSelectionRange& rng, selection) {
        streamRange(stream, rng, format, separator, action, locale);
    }
    stream.flush();
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(data);

}

void CopyEventFilter::copySelectedAsList(QAbstractItemModel *model,
                                         const QItemSelection &selection,
                                         const QLocale& locale)
{
    QVariantList vs;
    foreach(const QItemSelectionRange& rng, selection) {
        for(int row = rng.topLeft().row(); row <= rng.bottomRight().row(); row++) {
            for (int column = rng.topLeft().column(); column <= rng.bottomRight().column(); column++) {
                vs << model->data(model->index(row,column));
            }
        }
    }
    Formats formats(DataFormat::ActionCopy);
    QStringList ss = DataStreamer::variantListToStringList(vs,DataFormat::SqlInsert,formats,locale);
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText("(" + ss.join(",") + ")");
    return;
}
