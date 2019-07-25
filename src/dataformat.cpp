#include "dataformat.h"

#include <QStringList>
#include <QComboBox>

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
