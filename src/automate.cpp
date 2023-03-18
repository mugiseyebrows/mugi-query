#include "automate.h"

#include "automation.h"
#include <QWidget>

void automate(QWidget *widget)
{
#if 0
    // debug xjoin
    Automation* automation = Automation::instance(widget);
    automation->connectToDatabaseFromHistory("foo");
    automation->connectToDatabaseFromHistory("bar");
    automation->toolXJoin("foo", "bar");
    automation->start();
#endif

#if 1
    Automation* automation = Automation::instance(widget);
    QString database = 0 ? "testaccess" : "test2";
    automation->connectToDatabaseFromHistory(database);
    automation->schemaEdit();
    //automation->showJoinHelper();
    automation->start();
#endif
}
