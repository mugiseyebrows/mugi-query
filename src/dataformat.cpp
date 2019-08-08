#include "dataformat.h"

#include <QStringList>
#include <QComboBox>
#include <QMap>

void DataFormat::initComboBox(QComboBox *comboBox, bool onlySql)
{
    static QStringList items = {"Csv", "Tsv", "Sql insert", "Sql update"};
    comboBox->clear();
    comboBox->addItems(onlySql ? items.mid(2) : items);
}

DataFormat::Format DataFormat::value(QComboBox *comboBox)
{
    static QStringList items = {"Csv", "Tsv", "Sql insert", "Sql update"};
    QString text = comboBox->currentText();
    return static_cast<DataFormat::Format>(items.indexOf(text));
}

QString DataFormat::extension(DataFormat::Format format) {
    static QMap<DataFormat::Format,QString> exts = {
        {Csv, ".csv"},
        {Tsv, ".txt"},
        {SqlInsert, ".sql"},
        {SqlUpdate, ".sql"},
    };
    return exts[format];
}

QString DataFormat::extensionsRegExp(){
    return "[.](csv|txt|sql)$";
}

QString DataFormat::fileDialogFilter(Format format)
{
    QString filter;
    if (format == DataFormat::Csv) {
        filter = "csv files (*.csv)";
    } else if (format == DataFormat::Tsv) {
        filter = "txt files (*.txt)";
    } else {
        filter = "sql files (*.sql)";
    }
    return filter;
}
