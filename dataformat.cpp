#include "dataformat.h"

#include <QStringList>
#include <QComboBox>

void DataFormat::initComboBox(QComboBox *comboBox)
{
    QStringList items;
    items << "Csv"
          << "Tsv"
          << "Sql insert"
          << "Sql update";
    comboBox->clear();
    comboBox->addItems(items);
}

DataFormat::Format DataFormat::value(QComboBox *comboBox)
{
    return static_cast<DataFormat::Format>(comboBox->currentIndex());
}
