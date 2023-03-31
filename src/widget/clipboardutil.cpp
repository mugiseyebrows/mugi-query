#include "clipboardutil.h"

#include "clipboard.h"
#include <QTableView>
#include <QMessageBox>
#include <QDebug>
#include "error.h"

void ClipboardUtil::copyTsv(QTableView *view)
{
    QAbstractItemModel* model = view->model();
    if (!model) {
        qDebug() << "!model" << __FILE__ << __LINE__;
        return;
    }
    QItemSelection selection = view->selectionModel()->selection();
    QString error;
    QWidget* parent = qobject_cast<QWidget*>(view->parent());
    Clipboard::copySelected(model, selection, DataFormat::Csv, "\t", false, view->locale(), error);
    Error::show(parent,error);
}
