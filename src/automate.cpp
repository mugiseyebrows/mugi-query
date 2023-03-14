#include "automate.h"

#include "automation.h"
#include <QWidget>

void automate(QWidget *widget)
{
#if 1
    Automation* automation = Automation::instance(widget);
    QString database = 0 ? "testaccess" : "test2";
    automation->connectToDatabaseFromHistory(database);
    automation->schemaEdit();
    automation->showJoinHelper();
    automation->start();
#endif
}
