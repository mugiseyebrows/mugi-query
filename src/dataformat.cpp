#include "dataformat.h"

#include <QStringList>
#include <QComboBox>
#include <QMap>

void DataFormat::initComboBox(QComboBox *comboBox, bool onlySql)
{
    static QStringList items = {"Csv", "Tsv", "Xlsx", "Json", "Sql insert", "Sql update"};
    comboBox->clear();
    comboBox->addItems(onlySql ? items.mid(3) : items);
}

DataFormat::Format DataFormat::value(const QString& text) {
    static QStringList items = {"Csv", "Tsv", "Xlsx", "Json", "Sql insert", "Sql update"};
    return static_cast<DataFormat::Format>(items.indexOf(text));
}

DataFormat::Format DataFormat::value(QComboBox *comboBox)
{
    return value(comboBox->currentText());
}

QString DataFormat::extension(DataFormat::Format format) {
    static QMap<DataFormat::Format,QString> exts = {
        {Csv, ".csv"},
        {Tsv, ".txt"},
        {Json, ".json"},
        {Xlsx, ".xlsx"},
        {SqlInsert, ".sql"},
        {SqlUpdate, ".sql"},
    };
    return exts[format];
}

QString DataFormat::extensionsRegExp(){
    return "[.](csv|txt|sql|json)$";
}

QString DataFormat::fileDialogFilter(Format format)
{
    static QMap<DataFormat::Format,QString> filter = {
        {Csv, "csv files (*.csv)"},
        {Tsv, "txt files (*.txt)"},
        {Xlsx, "xlsx files (*.xlsx)"},
        {Json, "json files (*.json)"},
        {SqlInsert, "sql files (*.sql)"},
        {SqlUpdate, "sql files (*.sql)"},
    };
    return filter[format];
}

QString DataFormat::name(DataFormat::Format format)
{
    static QStringList items = {"Csv", "Tsv", "Xlsx", "Json", "Sql insert", "Sql update"};
    return items[format];
}
