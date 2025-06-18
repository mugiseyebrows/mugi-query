#include "copytoclipboard.h"

#include <QAbstractItemModel>
#include <QItemSelection>
#include "copyformat2.h"
#include <QApplication>
#include <QClipboard>
#include <QSqlDriver>
#include <QSqlQueryModel>
#include <QSqlField>
#include <QSqlRecord>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "fieldnames.h"
#include <QSet>
#include "tolist.h"

class CsvFormatter {
public:
    CsvFormatter(bool comma) : comma(comma) {

    }

    QString operator()(const QSqlField& f) {
        QVariant v = f.value();
        QString s = v.toString();
        if (v.typeId() == QMetaType::Double && comma) {
            s.replace(".",",");
        }
        return s;
    }
protected:
    bool comma = false;
};

class SqlFormatter {
public:
    SqlFormatter(QSqlDriver* driver) : driver(driver) {

    }
    QString operator()(const QSqlField& field) {
        return driver->formatValue(field);
    }
protected:
    QSqlDriver* driver;
};

class NoFormatter {
public:
    QString operator()(const QSqlField& f) {
        return f.value().toString();
    }
};

class ListFormatter {
public:
    QString operator()(const QSqlField& f) {
        QString s = f.value().toString();
        if (f.value().typeId() == QMetaType::QString) {
            // todo fix escaping
            return "\"" + s.replace("\"", "\\\"") + "\"";
        }
        return s;
    }
};


QList<QStringList> transpose(const QList<QStringList>& data) {
    int rows = data.size();
    int columns = data[0].size();
    QList<QStringList> res;
    for(int column=0;column<columns;column++) {
        QStringList dataRow;
        for(int row=0;row<rows;row++) {
            dataRow.append(data[row][column]);
        }
        res.append(dataRow);
    }
    return res;
}

static QList<int> sorted(const QSet<int>& values) {
    QList<int> res = toList(values);
    std::sort(res.begin(), res.end());
    return res;
}

static QList<QStringList> zip(const QList<QStringList>& keys, const QList<QStringList>& values, CopyFormat2::Lang lang) {

    QString op;
    QString cl;
    if (lang == CopyFormat2::Py) {
        op = "(";
        cl = ")";
    } else if (lang == CopyFormat2::Cpp || lang == CopyFormat2::Js) {
        op = "{";
        cl = "}";
    }
    auto toOne = [=](const QStringList& items) {
        if (items.size() == 1) {
            return items[0];
        }
        return op + items.join(", ") + cl;
    };
    QList<QStringList> res;
    for(int row=0;row<keys.size();row++) {
        QStringList item = {toOne(keys[row]), toOne(values[row])};
        res.append(item);
    }
    return res;
}

void copyToClipboard(QAbstractItemModel *model, const QItemSelection &selection, const CopyFormat2 &fmt, QSqlDriver* driver) {

    if (selection.isEmpty()) {
        return;
    }

    QList<int> tops, lefts, bottoms, rights;

    QSqlQueryModel* sqlModel = qobject_cast<QSqlQueryModel*>(model);


    for(const QItemSelectionRange& rng: selection) {
        tops.append(rng.top());
        lefts.append(rng.left());
        bottoms.append(rng.bottom());
        rights.append(rng.right());
    }

    int top = *std::min_element(tops.begin(), tops.end());
    int left = *std::min_element(lefts.begin(), lefts.end());
    int bottom = *std::max_element(bottoms.begin(), bottoms.end());
    int right = *std::max_element(rights.begin(), rights.end());

    QSet<int> selectedRows;
    QSet<int> selectedColumns;

    for(int row=top; row<=bottom; row++) {
        for(int column=left; column<=right; column++) {
            if (selection.contains(model->index(row, column))) {
                selectedRows.insert(row);
                selectedColumns.insert(column);
            }
        }
    }

    if (fmt.format() == CopyFormat2::Json) {
        QJsonArray data;
        for(int row=top; row<=bottom;row++) {
            if (!selectedRows.contains(row)) {
                continue;
            }
            const QSqlRecord& record = sqlModel->record(row);
            QJsonObject dataRow;
            for(int column=left; column<=right; column++) {
                if (!selectedColumns.contains(column)) {
                    continue;
                }
                QModelIndex index = model->index(row, column);
                const QSqlField field = record.field(column);
                if (selection.contains(index)) {
                    dataRow[field.name()] = QJsonValue::fromVariant(field.value());
                } else {
                    dataRow[field.name()] = {};
                }
            }
            data.append(dataRow);
        }
        QJsonDocument doc(data);
        QString text = QString::fromUtf8(doc.toJson());
        QClipboard *clipboard = qApp->clipboard();
        clipboard->setText(text);
        return;
    }

    if (fmt.format() == CopyFormat2::Dict) {

        std::function<QString(const QSqlField&)> formatter = ListFormatter{};

        QStringList columnNames = fieldNames(sqlModel->record());

        QStringList key = fmt.key();
        bool keyIsValid = !key.isEmpty();
        for(const QString& name: key) {
            if (!columnNames.contains(name)) {
                keyIsValid = false;
            }
        }

        QList<int> keyColumns;
        QList<int> valueColumns;

        if (keyIsValid) {
            for(const QString& name: key) {
                keyColumns.append(columnNames.indexOf(name));
            }
            for(int column: selectedColumns) {
                if (!keyColumns.contains(column)) {
                    valueColumns.append(column);
                }
            }
        } else {
            valueColumns = sorted(selectedColumns);
            int column = valueColumns.takeAt(0);
            keyColumns.append(column);
        }

        if (keyColumns.isEmpty() || valueColumns.isEmpty()) {
            qDebug() << "keyColumns.isEmpty()" << keyColumns.isEmpty()
                     << "valueColumns.isEmpty()" << valueColumns.isEmpty();
            return;
        }

        QList<QStringList> dataKeys;
        QList<QStringList> dataValues;

        for(int row=top; row<=bottom;row++) {
            if (!selectedRows.contains(row)) {
                continue;
            }
            const QSqlRecord& record = sqlModel->record(row);

            QStringList dataRowKeys;
            QStringList dataRowValues;

            for(int column: keyColumns) {
                QModelIndex index = model->index(row, column);
                QSqlField field;
                if (selection.contains(index)) {
                    field = record.field(column);
                }
                dataRowKeys.append(formatter(field));
            }
            for(int column: valueColumns) {
                QModelIndex index = model->index(row, column);
                QSqlField field;
                if (selection.contains(index)) {
                    field = record.field(column);
                }
                dataRowValues.append(formatter(field));
            }
            dataKeys.append(dataRowKeys);
            dataValues.append(dataRowValues);
        }

        QList<QStringList> items = zip(dataKeys, dataValues, fmt.lang());

        CopyFormat2::Lang lang = fmt.lang();

        QString text;

        if (lang == CopyFormat2::Cpp) {
            QStringList res;
            for(int row=0;row<items.size();row++) {
                res.append("{" + items[row].join(", ") + "}");
            }
            text = "{" + res.join(",\n") + "};";
        } else if (lang == CopyFormat2::Py || lang == CopyFormat2::Js) {
            QStringList res;
            for(int row=0;row<items.size();row++) {
                res.append(items[row].join(": "));
            }
            text = "{" + res.join(",\n") + "}";
        }

        QClipboard *clipboard = qApp->clipboard();
        clipboard->setText(text);
        return;
    }


    std::function<QString(const QSqlField&)> formatter = NoFormatter{};

    /*QStringList columnNames;
    const QSqlRecord& record = sqlModel->record(0);
    for(int i=0;i<record.count();i++) {
        columnNames.append(record.field(i).name());
    }*/

    if (fmt.format() == CopyFormat2::Tsv) {
        formatter = CsvFormatter(fmt.sep() == CopyFormat2::Comma);
    } else if (fmt.format() == CopyFormat2::Sql) {
        formatter = SqlFormatter(driver);
    } else if (fmt.format() == CopyFormat2::List) {
        formatter = ListFormatter();
    }

    QList<QStringList> data;

    for(int row=top; row<=bottom;row++) {
        if (!selectedRows.contains(row)) {
            continue;
        }
        const QSqlRecord& record = sqlModel->record(row);
        QStringList dataRow;
        for(int column=left; column<=right; column++) {
            if (!selectedColumns.contains(column)) {
                continue;
            }
            QModelIndex index = model->index(row, column);
            QSqlField field;
            if (selection.contains(index)) {
                field = record.field(column);
            }
            dataRow.append(formatter(field));
        }
        data.append(dataRow);
    }

    if (CopyFormat2::axisRelevant(fmt.format()) && fmt.axis() == CopyFormat2::Cols) {
        data = transpose(data);
    }

    QString text;
    if (fmt.format() == CopyFormat2::Tsv) {
        QStringList res;
        for(const QStringList& row: data) {
            res.append(row.join("\t"));
        }
        text = res.join("\n");
    } else if (fmt.format() == CopyFormat2::Sql) {
        QStringList res;
        for(const QStringList& dataRow: data) {
            res.append(dataRow);
        }
        if (res.size() == 1) {
            text = res[0];
        } else {
            text = "(" + res.join(", ") + ")";
        }
    } else if (fmt.format() == CopyFormat2::List) {

        QString op;
        QString cl;
        if (fmt.lang() == CopyFormat2::Py) {
            op = "[";
            cl = "]";
        } else if (fmt.lang() == CopyFormat2::Cpp || fmt.lang() == CopyFormat2::Js) {
            op = "{";
            cl = "}";
        }

        QStringList res;
        if (selectedColumns.size() > 1) {
            for(const QStringList& dataRow: data) {
                QString item = op + dataRow.join(", ") + cl;
                res.append(item);
            }
        } else {
            for(const QStringList& dataRow: data) {
                res.append(dataRow);
            }
        }

        QString glue = ", ";
        if (selectedColumns.size() > 1) {
            glue = ",\n";
        }

        text = op + res.join(glue) + cl;

    }
    if (text.isEmpty()) {
        return;
    }

    QClipboard *clipboard = qApp->clipboard();
    clipboard->setText(text);
}
