#include "automate.h"

#include "automation.h"
#include <QWidget>

void automate(QWidget *widget)
{
#if 1
    Automation* automation = Automation::instance(widget);
    automation->connectToDatabaseFromHistory("dvdrental");
    automation->schemaEdit();
    automation->start();
#endif
}
