#include "automate.h"

#include "automation.h"
#include <QWidget>

void automate(QWidget *widget)
{
#if 1
    Automation* automation = Automation::instance(widget);
    automation->connectToDatabaseFromHistory("testaccess");
    automation->schemaEdit();
    //automation->showJoinHelper();
    automation->start();
#endif
}
