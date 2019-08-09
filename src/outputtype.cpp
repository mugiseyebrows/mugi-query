#include "outputtype.h"

void OutputType::initComboBox(QComboBox *comboBox)
{
    comboBox->clear();
    comboBox->addItems({"File", "Session", "Clipboard"});
}

OutputType::Type OutputType::value(QComboBox *comboBox)
{
    return static_cast<OutputType::Type>(comboBox->currentIndex());
}
