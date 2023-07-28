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
    QString database = "mysql1";
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

#if 0

    Automation* automation = Automation::instance(widget);
    QString database = "mysql1";
    automation->connectToDatabaseFromHistory(database);
    automation->schemaEdit();
    automation->selectTablesNone();
    automation->selectTables({"foo"});

    QList<int> formats = {0, 1, 2, 3};
    QList<bool> cropAlls = {true, false};
    QList<bool> itemsAlls = {true, false};

    for(int format: formats) {
        for(bool cropAll: cropAlls) {
            for(bool itemsAll: itemsAlls) {
                bool clipboard = false;

                QString path = QString("D:\\w\\export-%1-crop-%2-items-%3")
                        .arg(format)
                        .arg(cropAll ? 1 : 0)
                        .arg(itemsAll ? 1 : 0);
                
                automation->exportTo(format, cropAll, itemsAll, clipboard, path);
            }
        }
    }
    /*int format = 0;
    bool cropAll = true;
    bool itemsAll = true;*/
    automation->start();
#endif

}
