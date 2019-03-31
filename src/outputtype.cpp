#include "outputtype.h"

#include "lit.h"

void OutputType::initComboBox(QComboBox *comboBox)
{
    comboBox->clear();
    comboBox->addItems(Lit::sl("File", "Session", "Clipboard"));
}

OutputType::Type OutputType::value(QComboBox *comboBox)
{
    return static_cast<OutputType::Type>(comboBox->currentIndex());
}
