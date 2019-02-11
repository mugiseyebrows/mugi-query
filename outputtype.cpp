#include "outputtype.h"

void OutputType::initComboBox(QComboBox *comboBox)
{
    QStringList items;
    items << "File"
          << "Session"
          << "Clipboard";
    comboBox->clear();
    comboBox->addItems(items);
}

OutputType::Type OutputType::value(QComboBox *comboBox)
{
    return static_cast<OutputType::Type>(comboBox->currentIndex());
}
