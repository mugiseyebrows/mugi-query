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
#include <QDebug>


void Clipboard::streamHeader(QTextStream& stream, QSqlQueryModel *model, const QString &separator, const QString& end) {
    int columnCount = model->columnCount();
    if (columnCount < 1) {
        return;
    }
    stream << model->headerData(0, Qt::Horizontal).toString();
    for(int col=1; col < columnCount; col++) {
        stream << separator << model->headerData(col, Qt::Horizontal).toString();
    }
    stream << end;
}

void Clipboard::streamHeader(QTextStream& stream, const QItemSelectionRange &rng,
                             const QString &separator, const QString& end) {


    QAbstractItemModel* model = const_cast<QAbstractItemModel*>(rng.model());
    int col = rng.topLeft().column();
    stream << model->headerData(col++, Qt::Horizontal).toString();
    for(; col <= rng.bottomRight().column(); col++) {
        stream << separator << model->headerData(col, Qt::Horizontal).toString();
    }
    stream << end;
}

void Clipboard::streamRange(QTextStream& stream, const QItemSelectionRange &rng,
                                  DataFormat::Format format,
                                  const QString &separator,
                                  DataFormat::ActionType action,
                                  const QLocale& locale, QString& error) {

    Formats formats(action);
    QAbstractItemModel* model = const_cast<QAbstractItemModel*>(rng.model());

    /*qDebug() << "rng.topLeft().row()" << rng.topLeft().row()
             << "rng.bottomRight().row()" << rng.bottomRight().row()
             << "rng.topLeft().column()" << rng.topLeft().column()
             << "rng.bottomRight().column()" << rng.bottomRight().column();*/

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
                                   bool header, const QLocale& locale, QString& error) {

    QString data = selectedToString(model, selection, format, separator, header, locale, error);
    if (!error.isEmpty()) {
        return;
    }
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(data);
}

QString Clipboard::selectedToString(QAbstractItemModel *model, const QItemSelection& selection,
                                   DataFormat::Format format, const QString &separator,
                                   bool header, const QLocale& locale, QString& error) {

    DataFormat::ActionType action = DataFormat::ActionCopy;
    Formats formats(action);

    // one range of one index
    if (selection.size() == 1 && selection[0].topLeft() == selection[0].bottomRight()) {
        QString data = DataStreamer::variantToString(model->data(selection[0].topLeft()),format,formats,locale,error);
        if (!error.isEmpty()) {
            return QString();
        }
        return data;
    }

    // many ranges
    QString data;
    QTextStream stream(&data);
    foreach(const QItemSelectionRange& rng, selection) {
        if (header) {
            streamHeader(stream, rng, separator);
        }
        streamRange(stream, rng, format, separator, action, locale, error);
        if (!error.isEmpty()) {
            return QString();
        }
    }
    stream.flush();
    return data;
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
}

void Clipboard::copySelectedNames(QSqlQueryModel *model, const QItemSelection &selection)
{
    /*if (selection.size() == 0) {
        return;
    }*/
    QString text;
    QTextStream stream(&text);
    if (selection.size() == 0) {
        streamHeader(stream, model, "\t", "\t");
    } else {
        for(const QItemSelectionRange& range: selection) {
            streamHeader(stream, range, "\t", "\t");
        }
    }
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(text);
}

class ExprArg {
public:
    ExprArg(const QString name, const QString value, bool isDouble) :
        name(name),value(value),isDouble(isDouble) {

    }

    QString name;
    QString value;
    bool isDouble;
    QString toString(double eps) const {
        /*if (isDouble) {
            return QString("abs(%1-%2)<%3").arg(name).arg(value).arg(eps);
        }*/
        return QString("%1=%2").arg(name).arg(value);
    }

};

class Expr {
public:
    QList<ExprArg> args;
    QString toString() const {
        double eps=1e-6;
        if (args.size() == 1) {
            return args[0].toString(eps);
        }
        QStringList args_;
        for(const ExprArg& arg: args) {
            args_.append(arg.toString(eps));
        }
        return "(" + args_.join(" and ") + ")";
    }
};

void Clipboard::copySelectedAsCondition(QSqlQueryModel *model,
                             const QItemSelection &selection) {

    const QSqlDriver* driver = model->query().driver();

    QList<QPair<int,int> > ranges_;
    for(const QItemSelectionRange& range: selection) {
        QModelIndexList indexes = range.indexes();
        for(const QModelIndex& index: indexes) {
            ranges_.append(QPair<int,int>(index.row(),index.column()));
        }
    }
    QSet<int> rows;
    for(const QPair<int,int>& index: ranges_) {
        rows.insert(index.first);
    }
    QList<int> rows_ = rows.toList();
    qSort(rows_.begin(),rows_.end());

    QStringList exprs;

    for(int row: rows_) {
        QList<int> columns;
        for(const QPair<int,int>& index: ranges_) {
            if (index.first == row) {
                columns.append(index.second);
            }
        }
        Expr expr;
        const QSqlRecord& record = model->record(row);
        for(int column: columns) {
            QString name = record.fieldName(column);
            QSqlField field = record.field(column);
            QVariant value_ = field.value();
            QString value = driver->formatValue(field);
            expr.args.append(ExprArg(name,value,value_.type() == QVariant::Double));
        }
        exprs.append(expr.toString());
    }

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(exprs.join(" or "));
}

