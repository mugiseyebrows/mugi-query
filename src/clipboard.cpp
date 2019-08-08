#include "clipboard.h"

#include <QTextStream>
#include <QItemSelection>
#include <QLocale>
#include "rowvaluegetter.h"
#include "datastreamer.h"
#include <QClipboard>
#include <QApplication>
#include "rowvaluesetter.h"
#include <QSqlQueryModel>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>

void Clipboard::streamRange(QTextStream& stream, const QItemSelectionRange &rng,
                                  DataFormat::Format format,
                                  const QString &separator,
                                  DataFormat::ActionType action,
                                  const QLocale& locale, QString& error) {

    Formats formats(action);
    QAbstractItemModel* model = const_cast<QAbstractItemModel*>(rng.model());
    for(int row = rng.topLeft().row(); row <= rng.bottomRight().row(); row++) {
        RowValueGetter g(model,row);
        int column = rng.topLeft().column();
        stream << DataStreamer::variantToString(g(column++), format, formats, locale, error);
        if (!error.isEmpty()) {
            return;
        }
        for(;column <= rng.bottomRight().column(); column++) {
            stream << separator << DataStreamer::variantToString(g(column),format,formats,locale,error);
            if (!error.isEmpty()) {
                return;
            }
        }
        stream << "\n";
    }
}

QModelIndex Clipboard::pasteTsv(QAbstractItemModel *model, const QModelIndex &index,
                                bool appendRows, bool appendColumns)
{

    int column0 = index.column();
    int row0 = index.row();
    int maxColumn = column0;

    QString text = QApplication::clipboard()->text();
    QStringList lines = text.split("\n");

    while (lines.size() > 0 && lines[lines.size()-1] == "") {
        lines.takeLast();
    }

    int maxRow = row0 + lines.size() - 1;

    if (appendRows) {
        if (maxRow >= model->rowCount()) {
            model->insertRows(model->rowCount(), maxRow - model->rowCount() + 1);
        }
    }

    for(int i=0;i<lines.size();i++) {
        int row = row0 + i;
        QStringList columns = lines[i].split("\t");

        maxColumn = qMax(maxColumn,column0 + columns.size() - 1);

        if (appendColumns) {
            if (maxColumn >= model->columnCount()) {
                model->insertColumns(model->columnCount(), column0 + columns.size() - maxColumn + 1);
            }
        }

        RowValueSetter s(model,row);
        for(int column=0;column<columns.size();column++) {
            int column_ = column + column0;
            s(column_, columns[column]);
        }
    }
    if (maxColumn >= model->columnCount()) {
        maxColumn = model->columnCount() - 1;
    }
    if (maxRow >= model->rowCount()) {
        maxRow = model->rowCount() - 1;
    }
    return model->index(maxRow, maxColumn);
}

void Clipboard::copySelected(QAbstractItemModel *model, const QItemSelection& selection,
                                   DataFormat::Format format, const QString &separator,
                                   const QLocale& locale, QString& error) {

    DataFormat::ActionType action = DataFormat::ActionCopy;
    Formats formats(action);

    // one range of one index
    if (selection.size() == 1 && selection[0].topLeft() == selection[0].bottomRight()) {
        QClipboard *clipboard = QApplication::clipboard();
        QString data = DataStreamer::variantToString(model->data(selection[0].topLeft()),format,formats,locale,error);
        if (!error.isEmpty()) {
            return;
        }
        clipboard->setText(data);
        return;
    }



    // many ranges
    QString data;
    QTextStream stream(&data);
    foreach(const QItemSelectionRange& rng, selection) {
        streamRange(stream, rng, format, separator, action, locale, error);
        if (!error.isEmpty()) {
            return;
        }
    }
    stream.flush();
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(data);

}



void Clipboard::copySelectedAsList(QSqlQueryModel *model,
                                         const QItemSelection &selection)
{
    QStringList result;
    const QSqlDriver* driver = model->query().driver();

    foreach(const QItemSelectionRange& range, selection) {
        QModelIndexList indexes = range.indexes();
        foreach(const QModelIndex& index, indexes) {
            const QSqlRecord& record = model->record(index.row());
            result << driver->formatValue(record.field(index.column()));
        }
    }
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText("(" + result.join(",") + ")");
    return;
}
