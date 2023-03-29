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

#if 0
    Automation* automation = Automation::instance(widget);
    //QString database = 0 ? "testaccess" : "test2";
    //QString database = "mysql1";
    QString database = "testaccess";
    automation->connectToDatabaseFromHistory(database);
    automation->schemaEdit();
    //automation->showJoinHelper();
    automation->start();
#endif

#if 0
    Automation* automation = Automation::instance(widget);
    //QString database = 0 ? "testaccess" : "test2";
    QString database = "mysql1";
    automation->connectToDatabaseFromHistory(database);
    automation->schemaEdit();
    automation->createTable("foo", {{"foo_id", "integer"}, {"b", "integer"}});
    automation->createPrimaryKey("pk_foo", "foo", {"foo_id"});
    automation->createTable("bar", {{"bar_id", "integer"}, {"foo_id", "integer"}});
    automation->createPrimaryKey("pk_bar", "bar", {"bar_id"});
    automation->createRelation("fk_foo_bar", "bar", {"foo_id"}, "foo", {"foo_id"}, true);
    automation->pushSchema();
    automation->start();
#endif


}
