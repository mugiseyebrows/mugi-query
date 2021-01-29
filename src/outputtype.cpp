#include "outputtype.h"

void OutputType::initComboBox(QComboBox *comboBox)
{
    static QStringList items = {"File", "Session", "Clipboard"};
    comboBox->clear();
    comboBox->addItems(items);
}

OutputType::Type OutputType::value(QComboBox *comboBox)
{
    return static_cast<OutputType::Type>(comboBox->currentIndex());
}

QString OutputType::name(OutputType::Type type)
{
    static QStringList items = {"File", "Session", "Clipboard"};
    return items[type];
}
